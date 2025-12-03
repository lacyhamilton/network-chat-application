// header files
#include "sender_handler.h"
#include "../chat_node.h"
#include "../message.h"

// thread argument access
#include "main.h"

#include <stdatomic.h>

// function definitions

/*
  here you OUTLINE the code of the function that is being executed in the sending thread.
  You have to have the main loop in place that processes the different pieces of information 
  a user may input. This, again, involves a code structure with entry points to process the 
  different commands or a note the user put in.
*/

// updates thread-local joined state variable if not already joined and sends message to server
static bool handle_join(bool *joined)
{
	// check for already joined - do not allow to run again
	if (*joined) return false;

	*joined = true;

	return true;
}

// updates thread-local joined state variable if not already left and sends message to server
static bool handle_leave(bool *joined)
{
	// check if not in session
	if (*joined) return true;

	*joined = false;

	return false;
}

// checks for valid state and updates output parameter
static bool handle_post(bool joined, Message *message)
{
	// check for logical connection with server and data to send
	return joined && message->message_data[0] != '\0';
}

// updates synchronized atomic state variable and sends a message to the server
static bool handle_shutdown(bool joined, atomic_bool *session_end)
{
	if (joined && !atomic_load(session_end))
	{
		// update session variable
		atomic_store(session_end, true);
		// correct conditions met
		return true;
	}
	return false;
}

// single sender thread running - no internal synchronization
void *sender_handler(void* args)
{
	// interpret properties list from thread arguments
	// Properties *properties = (Properties *)args;
	ThreadArgs *local_args = (ThreadArgs *)args;
	Properties *properties = local_args->property_list;

	Message message;

	// ############################################### TODO - DYNAMIC MEMORY ALLOCATION CLEARING ON SHUTDOWN RECEIVED ? ##################
	// dynamically allocate node to represent self
	ChatNode *node_self = create_node(
									property_get_property(properties, "LOGICAL_NAME"),
									property_get_property(properties, "MY_IP"),
									atoi(property_get_property(properties, "MY_PORT")));
	// state variable to manage sender-side state logic
	bool joined = false;

	// buffer for reading user input
	// ############################ TODO: CHANGE SIZE TO ACCOUNT FOR COMMAND ??? ##########################
	// char user_input[MESSAGE_LEN];

	// descriptor for connections made to the server
	int client_socket;
	struct sockaddr_in client_address;

	// burrow self
	message.chat_node = *node_self;

	// reset memory occupied
	memset(&client_address, 0, sizeof(client_address));

	// create addr struct
	client_address.sin_family = AF_INET;
	client_address.sin_addr.s_addr = inet_addr(property_get_property(properties, "SERVER_IP"));
	client_address.sin_port = htons(atoi(property_get_property(properties, "SERVER_PORT")));

	// ignore SIGPIPE on connection closed
	signal(SIGPIPE, SIG_IGN);

	// loop while program is running
	while (!atomic_load(&local_args->session_end))
	{
		// state variable for message type and logic
		bool is_valid;

		// ###################### todo - client hangs on fgets ? freeing resources ? ################
		get_message(&message);

		// determine proper action - none taken if invalid type
		switch (message.type)
		{
			case JOIN:
				

				is_valid = handle_join(&joined);
				break;
			case LEAVE:
				is_valid = handle_leave(&joined);
				break;
			case POST:
				is_valid = handle_post(joined, &message);
				break;
			case SHUTDOWN:
			case SHUTDOWN_ALL:
				is_valid = handle_shutdown(joined, &local_args->session_end);
				break;
			// command not recognized, do not send to server
			default:
				// update loop-local state variable
				is_valid = false;
				break;
		}

		// pass correct message to server
		if (is_valid)
		{

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

			send_message(client_socket, &message);

			// close connection
			close(client_socket);
		}

		// clear the buffer input field
		message.message_data[0] = '\0';
	}

	// deallocate memory
    free(node_self);

    return NULL;
}
