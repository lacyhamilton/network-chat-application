// header files
#include "receiver_handler.h"

#include <stdatomic.h>

// for ThreadArgs struct
#include "main.h"

#include "../chat_node.h"
#include "../message.h"
#include "../text_color.h"


/*
  here you OUTLINE the code that is being executed in the receiving thread:
  a function containing the server loop. In addition, you need to have another
  function in place that is being executed when the server connects in the server loop.
  You need to put a big switch statement into this latter function with entry points
  for the different kinds of messages that can be received from the server.
*/

// displays the properly formatted message from a post
static void handle_post(Message *message, char *fill_buff)
{
	// write in post author to buffer
	sprintf(fill_buff, "%s: %s\n", message->chat_node.logical_name,
												message->message_data);
}

// logic to process a join message from another client node
static void handle_join(Message *message, char *fill_buff)
{
	// populate output buffer
	sprintf(fill_buff, "%s joined the chat\n",
                                            message->chat_node.logical_name);
}

// logic to process a join message from another client node
static void handle_leave(Message *message, char *fill_buff)
{
	// fill output parameter with formatted string
	sprintf(fill_buff, "%s left the chat\n", message->chat_node.logical_name);
}

// logic to handle a command to shutdown from server
	// updates is_running output parameter
	// server will always send a SHUTDOWN command to listener when client requests shutdown
static void handle_shutdown(atomic_bool *session_end)
{
	
}

// single-threaded function to read a message from the server and decide on proper action
static void handle_message(int upstream_socket, atomic_bool *session_end)
{
	// buffer to hold received message
	Message message;

	// ######### todo - cleaner parameterization #########
	char out_buff[MESSAGE_LEN + LOGICAL_NAME_LEN + sizeof(": ")] = "";

	// flag for determining if valid message received
	bool display_message = true;

	// check for read status and escape if failed
		// ####################### make sure bfufer is always null terminated #########################
	if (!read_message(upstream_socket, &message)) return;

	// check for proper action
	switch (message.type)
	{
		case JOIN:
			handle_join(&message, out_buff);
			break;
		case POST:
			handle_post(&message, out_buff);
			break;
		case LEAVE:
			handle_leave(&message, out_buff);
			break;
		case SHUTDOWN:
		case SHUTDOWN_ALL:

			// do not display on shutdown ### correct ? ###
			display_message = false;

			handle_shutdown(session_end);
			break;
		default:
			// message not recognized - unset display flag
			display_message = false;
			break;
	}

	// ############################ require messages fill \n or append here ? ########################
		// user input fgets in get_message in src/message.c includes newline or not
	// check for message display
	if (display_message) printf("%s", out_buff);

	// reset buffer
	out_buff[0] = '\0';
}

// primary thread function called from main
void *reciever_handler(void *args)
{
	// properly interpret arguments
	ThreadArgs *local_args = (ThreadArgs *)(args);

	// create listening socket
	int listen_socket;
	struct sockaddr_in address;
	int yes = 1;

	// ignore SIGPIPE on connection closed
	signal(SIGPIPE, SIG_IGN);

	// create socket
	if ((listen_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Client listen socket creation failure");
		exit(EXIT_FAILURE);
	}

	// avoid "Address already in use" error
	if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	{
		perror("setsockopt failure");
		exit(EXIT_FAILURE);
	}

	// bind socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
	// ##################### CHANGE BASED ON PROPERTIES HOLDING IP/PORT ###################
	address.sin_port = htons(atoi(property_get_property(local_args->property_list, "MY_PORT")));

	if (bind(listen_socket, (struct sockaddr *)&address, sizeof(address)) != 0)
    {
        perror("Failed binding socket");
        exit(EXIT_FAILURE);
    }

	if (listen(listen_socket, 1) != 0)
	{
		perror("Client failed to listen on socket");
		exit(EXIT_FAILURE);
	}

	// loop while session maintained
	while (!atomic_load(&local_args->session_end))
	{
		// single server-component thread - no synchronization with sockets
		int upstream_socket = accept(listen_socket, NULL, NULL);

		// check for socket creation failure
		if (upstream_socket == -1)
		{
			fprintf(stderr, "Failed to accept server connection\n");
			// clean state and move to next iteration
			close(upstream_socket);
			continue;
		}

		handle_message(upstream_socket, &local_args->session_end);

		// deallocate socket within iteration's scope
		close(upstream_socket);
	}

	// free allocated resources
	close(listen_socket);

	return NULL;
}
