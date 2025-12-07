// header files
#include "receiver_handler.h"

// for ThreadArgs struct
#include "main.h"

#include "../chat_node.h"
#include "../message.h"
#include "../text_color.h"

#include <stdatomic.h>


// ###################################### utility function prototypes ###########################################

static void handle_post(Message *message, char *fill_buff);

static void handle_join(Message *message, ChatNode *node_self, char *fill_buff);

static void handle_leave(Message *message, ChatNode *node_self, char *fill_buff);

static void handle_shutdown(Message *message, ChatNode *node_self, atomic_bool *session_end, char *fill_buff);

static void handle_shutdown_all(atomic_bool *session_end, char *fill_buff);

static void handle_message(int upstream_socket, ChatNode *node_self, atomic_bool *session_end);

// ######################################## library function definitions ########################################

// primary thread function called from main
void *reciever_handler(void *args)
{
	// properly interpret arguments
	ThreadArgs *local_args = (ThreadArgs *)(args);

	// recognize source
	ChatNode *node_self = create_node(
						property_get_property(local_args->property_list, "LOGICAL_NAME"),
						property_get_property(local_args->property_list, "MY_IP"),
						atoi(property_get_property(local_args->property_list, "MY_PORT")));

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

		handle_message(upstream_socket, node_self, &local_args->session_end);

		// deallocate socket within iteration's scope
		close(upstream_socket);
	}

	// free allocated resources
	close(listen_socket);

	free(node_self);

	return NULL;
}


// ######################################### utility funciton definitions ###########################################

// ###### sometimes the RESET_COLOR filter is not being applied
// ######	I am worried that it might be that one of these formatted strings becomes null terminated

// displays the properly formatted message from a post
static void handle_post(Message *message, char *fill_buff)
{
	// write in post author to buffer
	sprintf(fill_buff, NOTE_COLOR "%s:" RESET_COLOR " %s\n",
        					message->chat_node.logical_name,
        					message->message_data);

}

// logic to process a join message from another client node
static void handle_join(Message *message, ChatNode *node_self, char *fill_buff)
{
	// check for sender as same client
	if (same_node(&message->chat_node, node_self))
	{
		// server message back to source
		sprintf(fill_buff, TEXT_RED "Welcome" RESET_COLOR "\n");
	}

	// default
	else
	{
		// populate output buffer
		sprintf(fill_buff, JOINED_COLOR "%s joined the chat" RESET_COLOR "\n",
                                            message->chat_node.logical_name);
	}
}

// logic to process a join message from another client node
static void handle_leave(Message *message, ChatNode *node_self, char *fill_buff)
{
	// check for self sending message
	if (same_node(&message->chat_node, node_self))
	{
		// interpret server message back to same source
		sprintf(fill_buff, TEXT_RED "Goodbye" RESET_COLOR "\n");
	}

	// default - message received from a separate node
	else
	{
		// fill output parameter with formatted string
		sprintf(fill_buff, LEFT_COLOR "%s left the chat" RESET_COLOR "\n",
											message->chat_node.logical_name);
	}
}

static void handle_shutdown(Message *message, ChatNode *node_self, atomic_bool *session_end, char *fill_buff)
{
	// check if command came from same client source
	if (same_node(&message->chat_node, node_self))
	{
		// set output parameter
		sprintf(fill_buff, TEXT_RED "Shutdown" RESET_COLOR "\n");

		// set state
		atomic_store(session_end, true);
	}

	// make same as leave message after debugging ?
	else
	{
		sprintf(fill_buff, TEXT_RED "%s has terminated" RESET_COLOR "\n",
												message->chat_node.logical_name);
	}
}

// logic to handle a command for forceful shutdown from server
	// updates is_running output parameter
	// server will always send a SHUTDOWN command to listener when client requests shutdown
static void handle_shutdown_all(atomic_bool *session_end, char *fill_buff)
{
	// set output parameter
	sprintf(fill_buff, TEXT_RED "Shutdown" RESET_COLOR "\n");

	// set state
	atomic_store(session_end, true);
}

// ############################# server vs client logic is reversed for shutdown vs shutdown_all ##########################

// single-threaded function to read a message from the server and decide on proper action
static void handle_message(int upstream_socket, ChatNode *node_self, atomic_bool *session_end)
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
			handle_join(&message, node_self, out_buff);
			break;
		case POST:
			handle_post(&message, out_buff);
			break;
		case LEAVE:
			handle_leave(&message, node_self, out_buff);
			break;
		case SHUTDOWN:
			handle_shutdown(&message, node_self, session_end, out_buff);
			break;
		case SHUTDOWN_ALL:
			handle_shutdown_all(session_end, out_buff);
			break;
		default:
			// message not recognized - unset display flag
			display_message = false;
			break;
	}

	// check for message display
	if (display_message) printf("%s", out_buff);

	// reset buffer
	out_buff[0] = '\0';
}