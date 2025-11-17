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



void talk_to_client(void* arg)
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


}
    