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

// #################### TODO - SHOULD THIS NOT BE STATIC ??? ###################
// internally called function to read an entire buffer
static ssize_t read_complete(int upstream_socket, void *buffer, ssize_t size)
{
    ssize_t bytes_read = 0;
    // bytes read per iteration
    ssize_t iter_read = 0;

    // loop while bytes remain to read
    while (bytes_read < size)
    {
        iter_read = recv(upstream_socket,
                            // buffer offset logic
                            (char *)buffer + bytes_read,
                            size - bytes_read,
                            0);
        // check for error
        if (iter_read <= 0)
        {
            fprintf(stderr, "Read error %zd\n", iter_read);
            // send error status to caller
            return iter_read;
        }

        bytes_read += iter_read;

    }

    return bytes_read;
}

// ############################ TODO - SHOULD BE NOT STATIC ??? CALLED IN OTHER FILES ??? ######################
// internally called function to handle full sends
static ssize_t send_complete(int upstream_socket, void *buffer, ssize_t size)
{
    ssize_t bytes_sent = 0;
    // bytes sent per iteration
    ssize_t iter_send = 0;

    // loop while bytes to send
    while (bytes_sent < size)
    {
        iter_send = send(upstream_socket,
                            // buffer offset logic for already sent bytes
                            (char *)buffer + bytes_sent,
                            size - bytes_sent,
                            0);
        // check for send failure
        if (iter_send <= 0)
        {
            fprintf(stderr, "Send error %zd\n", iter_send);
            // send failure
            return iter_send;
        }

        bytes_sent += iter_send;
    }

    return bytes_sent;
}

// ############## MODIFICATIONS MADE IN PLACE - MAKE SURE NO MESSAG USE AFTER SEND ##################
// utility function called by send_message to convert a struct to internet byte order
static void to_network_byte_order(Message *message)
{
    // network integer conversion
    message->type = htonl(message->type);
    // network unsigned short integer conversion
    message->chat_node.port = htons(message->chat_node.port);
}

// utility function called by read_message to convert a struct to local byte order
static void to_host_byte_orde(Message *message)
{
    // network integer conversion
    message->type = ntohl(message->type);
    // network unsigned short integer conversion
    message->chat_node.port = ntohs(message->chat_node.port);
}

int interpret_message(char *str_buffer, Message *output_message)
{
	return -1;
}

// ############## TODO - MAKE SURE GRACEFUL SHUTDOWN NEVER OCCURS DURING FUNCTION SCOPE - WILL BE INTERPRETED AS A FAILURE #################
bool read_message(int upstream_socket, Message *buffer)
{
    // store the return from read_complete
    ssize_t read_status;

    // get message from sender
    read_status = read_complete(upstream_socket, buffer, sizeof(Message));
    // check for meessage not read
    if (read_status <= 0)
    {
        fprintf(stderr, "Failed to read message, status %zd\n", read_status);
        // read failure
        return false;
    }

    // convert byte order from internet presentation
    to_host_byte_orde(buffer);

    // successful send
	return true;
}

bool send_message(int socket, Message *message)
{
    // store bytes sent from send_complete
    ssize_t send_status;

    // convert byte order
    to_network_byte_order(message);

    // send message over internet
    send_status = send_complete(socket, message, sizeof(Message));
    // check for failure
    if (send_status <= 0)
    {
        fprintf(stderr, "Failed to read message, status %zd\n", send_status);
        // send failure
        return false;
    }

    // successful send
	return true;
}