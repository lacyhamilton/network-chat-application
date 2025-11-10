// header files
#include "receiver_handler.h"

// for ThreadArgs struct
#include "main.h"

#include "../chat_node.h"
#include "../message.h"

/*
  here you OUTLINE the code that is being executed in the receiving thread:
  a function containing the server loop. In addition, you need to have another
  function in place that is being executed when the server connects in the server loop.
  You need to put a big switch statement into this latter function with entry points
  for the different kinds of messages that can be received from the server.
*/

// displays the properly formatted message from a post
static void handle_post(Message *message)
{

}

// updates a node's state if not already joined
static void handle_join(ThreadArgs *args)
{

}

// updates a node's state if not already left
static void handle_leave(ThreadArgs *args)
{

}

// updates nodes state
static void handle_shutdown(ThreadArgs *args)
{
	
}

// single-threaded function to read a message from the server and decide on proper action
static void handle_message(int upstream_socket, ThreadArgs *thread_args)
{
	// buffer to hold received message
	Message message;
	// read from server
	ssize_t bytes_read = read_message(upstream_socket, &message);

	// check for read error
	if (bytes_read <= 0)
	{
		perror("Read error");
		return;
	}

	// check for proper action
	switch (message.type)
	{
		case JOIN:
			handle_join(thread_args);
			break;
		case POST:
			handle_post(&message);
			break;
		case LEAVE:
			handle_leave(thread_args);
			break;
		case SHUTDOWN:
		case SHUTDOWN_ALL:
			handle_shutdown(thread_args);
			break;
	}
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
    address.sin_port = htons(atoi(property_get_property(local_args->property_list, "NODE_PORT")));

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

	// loop while the program runs
	while (local_args->state != EXIT)
	{
		// check if listen
		if (local_args->state == JOINED)
		{
			// attempt for connection
			int upstream_socket = accept(listen_socket, NULL, NULL);

			// check if accept fails
			if (upstream_socket == -1)
			{
				perror("Failed to accept server connection");
				continue; //  do not exit the loop here
			}

			// get input from server
			handle_message(upstream_socket,  local_args);
			// close connection
			close(upstream_socket);
		}

		return NULL;
	}

	// char* properties_file = "properties.txt";
	// Properties* properties;
	// char* key = "CHAT_PORT";
	// char* chat_port;

	// properties = property_read_properties(properties_file);
	// chat_port = property_get_property(properties, key);
	// key = "SERVER_PORT";
	// char *server_port = property_get_property(properties, key);
	// key = "SERVER_IP";
	// char *server_ip= property_get_property(properties, key);
	// key = "CHAT_IP";
	// char *chat_ip= property_get_property(properties, key);

	return NULL;
}