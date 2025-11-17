// header files
#include "message.h"

// function definitions

/*
  similarly, you need to have a structure in place that represents messages. 
  Messages have a type and other information, i.e. chat_node information and 
  text that represents a NOTE. Here, you will also have code in place that 
  handles messages, like sending and receiving them. Again, this code needs 
  to be OUTLINED.
*/

void deserialize_message(Message *message)
{

}

int interpret_message(char *str_buffer, Message *output_message)
{
	return -1;
}

ssize_t read_message(int upstream_socket, Message *buffer)
{
	return 0;
}

ssize_t send_message(int socket, Message *message)
{
	return 0;
}

void serialize_message(Message *message)
{
    
}