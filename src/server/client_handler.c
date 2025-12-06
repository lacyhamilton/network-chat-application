// header files
#include "client_handler.h"
#include "../network.h"

#define DBG
#include "../dbg.h"

// #include <signal.h>

// function definitions

// ############################################ utility function prototypes #############################################

static void broadcast_message(NodeList *client_list, Message *message);

static void handle_join(NodeList *client_list, Message *message);

static void handle_leave(NodeList *client_list, Message *message);

static void handle_shutdown(NodeList *client_list, Message *message);

static void handle_shutdown_all(NodeList *client_list, Message *message);

// ############################################ library function definitions ############################################

void *talk_to_client(void* arg)
{
	ClientThreadArgs *local_args = (ClientThreadArgs *)arg;

	// buffer for receiving a message from connection
	Message msg;

	read_message(local_args->client_socket, &msg);

	// This is a single switch case because we are not doing persistent connections.
	switch (msg.type)
	{
	case JOIN:
		// pass join message to all nodes in session
		broadcast_message(local_args->client_list, &msg);

		handle_join(local_args->client_list, &msg);
		break;

	case POST:
		broadcast_message(local_args->client_list, &msg);
		break;

	case LEAVE:
		// call handler to remove node from list and send message to source
		handle_leave(local_args->client_list, &msg);

		broadcast_message(local_args->client_list, &msg);
		break;

	// ############### TODO - SHOULD ALSO SEND SHUTDOWN MESSAGE TO CLIENT'S LISTENING COMPONENT ??? ##################
	case SHUTDOWN: // This works similarly to LEAVE but terminates the chat client
		// 1. Remove the client from the list of ChatNodes
		// 2. Notify all other clients that the user has left
		// 3. Close the client socket and terminate the thread
		// 4. Terminate the chat client application

		handle_shutdown(local_args->client_list, &msg);
		broadcast_message(local_args->client_list, &msg);
		break;

	case SHUTDOWN_ALL:
		// terminate each client individually - no broadcast_message call
		handle_shutdown_all(local_args->client_list, &msg);

		break;
	}

	// deallocate dynamically allocated memory
	free(arg);

	return NULL;
}

// ############################################ utility function definitions ############################################

// iterate through a list of client nodes and pass a message to all nodes but sender
// static void broadcast_message(NodeList *client_list, ChatNode *source)
static void broadcast_message(NodeList *client_list, Message *message)
{
	// pointer for read-only list traversal
	ChatNode *curr_node = NULL;
	// hold a copy of the list in a local variable - reduce contention
	ChatNode *copy_head = NULL;
	// used to hold per-node copy
	ChatNode *node_copy = NULL;

	// aquire lock on entrance
	pthread_mutex_lock(&client_list->mutex);

	curr_node = client_list->head;

	// copy-over loop for the nodes in the list
	while (curr_node)
	{
		// create a copy of the current node
		node_copy = create_node(curr_node->logical_name,
											curr_node->ip,
											curr_node->port);
		// add to list at head - NOT STABLE, changes order
		node_copy->next = copy_head;
		copy_head = node_copy;

		curr_node = curr_node->next;
	}
	// release lock after copy
	pthread_mutex_unlock(&client_list->mutex);

	// loop for the copy of nodes - avoid socket overhead in critical section
	curr_node = copy_head;

	while (curr_node)
	{
		// check if current node is source
		if (same_node(curr_node, &message->chat_node))
		{
			// skip current connection iteration
			curr_node = curr_node->next;
			continue;
		}

		// try for connection with node
		int sender_socket = open_connection(curr_node);

		// check for successful connection made
		if (sender_socket >= 0)
		{
			send_message(sender_socket, message);
			close(sender_socket);
		}

		// move to next node
		curr_node = curr_node->next;
	}

	// loop to free nodes
	while (copy_head)
	{
		curr_node = copy_head;
		copy_head = copy_head->next;
		free(curr_node);
	}
}

// utility function to add a node to a list and pass the message back to the client's listener
static void handle_join(NodeList *client_list, Message *message)
{
	// node for insertion
	ChatNode *new_node = NULL;

	new_node = create_node(message->chat_node.logical_name,
							message->chat_node.ip,
							message->chat_node.port);

	// insert node to list
	add_node(client_list, new_node);

	// open connection to send message to caller
	int sender_socket = open_connection(&message->chat_node);

	// pass message through current connection
	if (sender_socket >= 0)
	{
		send_message(sender_socket, message);
		close(sender_socket);
	}
}

// #################### handle_leave and handle_shutdown are currently identical ######################
	// maybe will need separate logic
	// IF NOT COMBINE INTO THE SAME FUNCTION

// utility function to remove a node from a list and pass the message back to the client's listener
static void handle_leave(NodeList *client_list, Message *message)
{
	// open connection to send message to caller
	int sender_socket = open_connection(&message->chat_node);

	// check for successful socket creation
	if (sender_socket >= 0)
	{
		send_message(sender_socket, message);
		close(sender_socket);
	}

	// clear the node
	safe_remove_node(client_list, &message->chat_node);
}

// utility function to pass a shutdown message from a client's sender to their listener
	// removes the node from the list
static void handle_shutdown(NodeList *client_list, Message *message)
{
	int sender_socket = open_connection(&message->chat_node);

	// check for successful connection
	if (sender_socket >= 0)
	{
		send_message(sender_socket, message);
		close(sender_socket);
	}

	// remove node from session
	safe_remove_node(client_list, &message->chat_node);
}

static void handle_shutdown_all(NodeList *client_list, Message *message)
{
	ChatNode *head = NULL;

	// critical section access - destructive - selfish hold, no list copies
	pthread_mutex_lock(&client_list->mutex);

	// assign head
	head = client_list->head;
	// unset outside access
	client_list->head = NULL;

	pthread_mutex_unlock(&client_list->mutex);

	// loop while nodes in list
	while (head)
	{
		// per-iteration socket for message passing
		int socket = open_connection(head);

		// check for successful connection
		if (socket >= 0)
		{

			send_message(socket, message);
			close(socket);
		}

		// remove node from session - increments head
		unsafe_remove_node(&head, head);
	}
}