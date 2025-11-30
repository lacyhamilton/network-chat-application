// header files
#include "client_handler.h"

// function definitions

/*
  here, the function is OUTLINED, that is going to implement the code that is being executed
  in a thread that handles a client request, aka void talk_to_client(void* arg) 
  (don't put that code into main.c). This function needs to have a big switch statement, its
  entry points pertain to the different kinds of messages a server may receive from a client.
  The different entry points need to be sketched out and comments need to be added that
  reflect your understanding of what a certain entry point's job.
*/

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
		// if (!strcmp(curr_node->ip, message->chat_node.ip)
		// 	&& curr_node->port == message->chat_node.port)
		if (same_node(curr_node, &message->chat_node))
		{
			// skip current connection iteration
			curr_node = curr_node->next;
			continue;
		}

		// per-node socket creation
		int sender_socket;
		struct sockaddr_in sender_address;

		// reset memory occupied
		memset(&sender_address, 0, sizeof(sender_address));

		// create addr struct
		sender_address.sin_family = AF_INET;
		sender_address.sin_addr.s_addr = inet_addr(curr_node->ip);
		sender_address.sin_port = htons(curr_node->port);

		// ignore SIGPIPE on connection closed
		// signal(SIGPIPE, SIG_IGN);

		// assign socket
		sender_socket = socket(AF_INET, SOCK_STREAM, 0);

		// connect to client's listener - check for failure
		if (connect(sender_socket, (struct sockaddr *)&sender_address, sizeof(sender_address)) == -1)
		{
			perror("Failed connecting to client at host");
			close(sender_socket);
			// pass node
			curr_node = curr_node->next;
			continue;
		}

		// pass message through current connection
		send_message(sender_socket, message);

		// close connection
		close(sender_socket);

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

void *talk_to_client(void* arg)
{
	// cast the arg parameter to a socket descriptor
		// ################## SHOULD BE CAST TO ClientThreadArgs * TYPE STRUCT ??? ################
	int client_socket = *((int *)arg); // assuming arg is a pointer to an int representing the socket descriptor

	Message msg;

	// This is a single switch case because we are not doing persistent connections.
	switch (msg.type)
	{
	case JOIN:
		// 0. Check that the client is not already in the list of clients (avoid repeated JOIN commands)
		// 1. Add the client's ChatNode to the linked list of clients
		// 2. Send a notification to all other clients that the user has joined
		// 3. Send acknowledgment to the joining client
		break;

	case POST:
		// 1. Validate that the client is in the linked list of clients
		// 2. Send the message to all other clients except the sender
		break;

	case LEAVE:
		// 1. Remove the client from the linked list of ChatNodes
		// 2. Notify all other clients that the user has left
		// 3. Close the client socket and terminate the thread
		break;

	// ############### TODO - SHOULD ALSO SEND SHUTDOWN MESSAGE TO CLIENT'S LISTENING COMPONENT ??? ##################
	case SHUTDOWN: // This works similarly to LEAVE but terminates the chat client
		// 1. Remove the client from the list of ChatNodes
		// 2. Notify all other clients that the user has left
		// 3. Close the client socket and terminate the thread
		// 4. Terminate the chat client application
		break;

	case SHUTDOWN_ALL:
		// 1. Remove all clients from the list of ChatNodes
		// 2. Notify all clients that the server is shutting down
		// 3. Close all client sockets and terminate all client threads
		// 4. Terminate the server application
		break;
	}

	// deallocate dynamically allocated memory
	free(arg);

	return NULL;
}
    