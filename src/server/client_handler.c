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

static void handle_shutdown_all(NodeList *client_list,
									Message *message,
									ChatNode *server_node,
									atomic_bool *enter_shutdown,
									atomic_bool *session_end);

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

	case SHUTDOWN: // This works similarly to LEAVE but terminates the chat client
		handle_shutdown(local_args->client_list, &msg);
		broadcast_message(local_args->client_list, &msg);
		break;

	case SHUTDOWN_ALL:
		// terminate each client individually - no broadcast_message call
		handle_shutdown_all(local_args->client_list,
											&msg,
											local_args->node_self,
											local_args->enter_shutdown,
											local_args->session_end);
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

static void handle_shutdown_all(NodeList *client_list,
									Message *message,
									ChatNode *server_node,
									atomic_bool *enter_shutdown,
									atomic_bool *session_end)
{
	ChatNode *head = NULL;
	// generic socket used for reaching client nodes, open/close per client
	int socket;
	// trace if source node was joined
	bool source_joined = false;

	// test-and-set for another client running shutdown_all for termiantion
	if (atomic_exchange(enter_shutdown, true)) return;

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
		socket = open_connection(head);

		// check for successful connection
		if (socket >= 0)
		{

			send_message(socket, message);
			close(socket);
		}

		// check if node was source
		if (same_node(head, &message->chat_node)) source_joined = true;

		// remove node from session - increments head
		unsafe_remove_node(&head, head);
	}

	// check if need pass message to source
	if (!source_joined)
	{
		// pass message back to caller
		socket = open_connection(&message->chat_node);

		if (socket >= 0)
		{
			send_message(socket, message);
			close(socket);
		}
	}

	// update state
	atomic_store(session_end, true);

	// pass message to waiting accept() in main - force exit
	socket = open_connection(server_node);

	if (socket >= 0)
	{
		send_message(socket, message);
		close(socket);
	}
}