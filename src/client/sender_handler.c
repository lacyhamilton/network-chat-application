// header files
#include "sender_handler.h"
#include "main.h"
#include "../chat_node.h"
#include "../message.h"

// function definitions

/*
  here you OUTLINE the code of the function that is being executed in the sending thread.
  You have to have the main loop in place that processes the different pieces of information 
  a user may input. This, again, involves a code structure with entry points to process the 
  different commands or a note the user put in.
*/

// updates joined state variable if not already joined and sends message to server
static void handle_join(int client_socket, bool *joined)
{

}

// updates joined state variable if not already left and sends message to server
static void handle_leave(int client_socket, bool *joined)
{

}

// send a message to the server
static void handle_post(int client_socket, Message message)
{

}

// updates joined state variable and sends a message to the server
static void handle_shutdown(int client_socket, bool *joined)
{

}

// single sender thread running - no internal synchronization
void *sender_handler(void* args)
{
	// interpret properties list from thread arguments
	Properties *properties = (Properties *)args;
	// dynamically allocate node to represent self
	ChatNode *node_self = create_node(
									property_get_property(properties, "LOGICAL_NAME"),
									property_get_property(properties, "MY_IP"),
									atoi(property_get_property(properties, "MY_PORT")));
	// state variable to manage sender-side state logic
	bool joined = true;

	Message message;

	// buffer for reading user input
	// ############################ TODO: CHANGE SIZE TO ACCOUNT FOR COMMAND ??? ##########################
	char user_input[MESSAGE_LEN];

	// descriptor for connections made to the server
	int client_socket;
	struct sockaddr_in client_address;

	// reset memory occupied
	memset(&client_address, 0, sizeof(client_address));

	// create addr struct
	client_address.sin_family = AF_INET;
	client_address.sin_addr.s_addr = inet_addr(property_get_property(properties, "SERVER_IP"));
	client_address.sin_port = htons(atoi(property_get_property(properties, "SERVER_PORT")));

	// ignore SIGPIPE on connection closed
	signal(SIGPIPE, SIG_IGN);

	// loop while program is running
	while (joined)
	{
		// get user input
			// ################## READS INPUT PROPERLY ??? ##################
		scanf("%s", user_input);
		// ################## TODO - LOGIC TO SKIP LOOP ITERATION IF BAD MESSAGE INPUT ####################
		// interpret as proper message struct
		interpret_message(user_input, &message);

		// #################### SOCKET LOGIC REUSE OKAY ???? SCOPE DECLARE HERE ???? ###################
		client_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (client_socket == -1)
		{
			perror("Client socket creation error");
			continue;
		}

		// establish connection with server
		if (connect(client_socket, (struct sockaddr *)&client_address, sizeof(client_address)) == -1)
		{
			perror("Connection error");
			close(client_socket);
			continue;
		}

		// determine proper action
		switch (message.type)
		{
			case JOIN:
				handle_join(client_socket, &joined);
				break;
			case LEAVE:
				handle_leave(client_socket, &joined);
				break;
			case POST:
				handle_post(client_socket, message);
				break;
			case SHUTDOWN:
			case SHUTDOWN_ALL:
				handle_shutdown(client_socket, &joined);
				break;
		}

		// close connection
	    close(client_socket);
	}

	// deallocate memory
    free(node_self);

    return NULL;
}
