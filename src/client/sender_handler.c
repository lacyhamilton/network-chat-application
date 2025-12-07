// header files
#include "sender_handler.h"
#include "../chat_node.h"
#include "../message.h"
#include "../network.h"

// thread argument access
#include "main.h"

#include <stdatomic.h>

// function definitions

// ####################################### utility function prototypes ########################################

static void handle_join(Message *message, ChatNode *server, bool *joined);

static void handle_leave(Message *message, ChatNode *server, bool *joined);

static void handle_post(Message *message, ChatNode *server, bool joined);

static void handle_shutdown(Message *message, ChatNode *server, atomic_bool *session_end);

// ####################################### library function definitions #######################################

// single sender thread running - no internal synchronization
void *sender_handler(void* args)
{
	// interpret thread arguments
	ThreadArgs *local_args = (ThreadArgs *)args;
	Properties *properties = local_args->property_list;

	Message message;

	// dynamically allocate node to represent self
	ChatNode *node_self = create_node(
									property_get_property(properties, "LOGICAL_NAME"),
									property_get_property(properties, "MY_IP"),
									atoi(property_get_property(properties, "MY_PORT")));

	// represent server for connections
	ChatNode *server = create_node(
									"",
									property_get_property(properties, "SERVER_IP"),
									atoi(property_get_property(properties, "SERVER_PORT")));
	// state variable to manage sender-side state logic
	bool joined = false;

	// loop while program is running
	while (!atomic_load(&local_args->session_end))
	{

		// ###################### todo - client hangs on fgets ? freeing resources ? ################
		get_message(&message);

		// burrow self
		message.chat_node = *node_self;

		// determine proper action - none taken if invalid type
		switch (message.type)
		{
			case JOIN:
				handle_join(&message, server, &joined);
				break;
			case LEAVE:
				handle_leave(&message, server, &joined);
				break;
			case POST:
				handle_post(&message, server, joined);
				break;
			case SHUTDOWN:
			case SHUTDOWN_ALL:
				handle_shutdown(&message, server, &local_args->session_end);
				break;
			// command not recognized, do not send to server
			default:
				break;
		}

		// clear the buffer input field
		message.message_data[0] = '\0';
	}

	// deallocate memory
    free(node_self);
	free(server);

    return NULL;
}

// ####################################### utility function definitions #######################################

// updates thread-local joined state variable if not already joined and sends message to server
static void handle_join(Message *message, ChatNode *server, bool *joined)
{
	// check for already joined - do not allow to run again
	if (*joined) return;

	// attempt connection
	int socket = open_connection(server);
	// check for successful connection
	if (socket >= 0)
	{
		// notify join
		send_message(socket, message);
		close(socket);

		// update state
		*joined = true;
	}
}

// updates thread-local joined state variable if not already left and sends message to server
static void handle_leave(Message *message, ChatNode *server, bool *joined)
{
	// check if not in session
	if (!(*joined)) return;

	// attempt connection
	int socket = open_connection(server);
	// check for successful connection
	if (socket >= 0)
	{
		// notify join
		send_message(socket, message);
		close(socket);
		// update state
		*joined = false;
	}
}

// checks for valid state and updates output parameter
static void handle_post(Message *message, ChatNode *server, bool joined)
{
	// check if invalid state or nothing to send - exit call
	if (!joined || message->message_data[0] == '\0') return;

	int socket = open_connection(server);

	// successful message pass
	if (socket >= 0)
	{
		send_message(socket, message);
		close(socket);
	}
}

// updates synchronized atomic state variable and sends a message to the server
static void handle_shutdown(Message *message, ChatNode *server, atomic_bool *session_end)
{
	int socket = open_connection(server);

	if (socket >= 0)
	{
		send_message(socket, message);
		close(socket);
	}

	// set local end regardless of server communication
	atomic_store(session_end, true);
}