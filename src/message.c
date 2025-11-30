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

// ###################################### utility function prototypes #######################################

static MessageType get_message_type(char *command);

static bool is_white_space(char character);

static void parse_word(char **buffer, char *first_word);

static ssize_t read_complete(int upstream_socket, void *buffer, ssize_t size);

static ssize_t send_complete(int upstream_socket, void *buffer, ssize_t size);

static char to_lower(char c);

static void to_host_byte_order(Message *message);

static void to_network_byte_order(Message *message);

// ###################################### library function definitions #######################################

// read user input until newline and store in a Message struct
void get_message(Message *output_message)
{
    // buffer to hold message
    char buffer[MESSAGE_LEN] = "";
    // hold first word - represent message type
    char command[MESSAGE_LEN];

    // point to buffer for parameter
    char *buf_ptr = buffer;

    // hold the type of message
    MessageType type;

    // read user intput until newline
    fgets(buffer, MESSAGE_LEN, stdin);

    // check for valid message
    parse_word(&buf_ptr, command);

    type = get_message_type(command);

    output_message->type = type;
    strcpy(output_message->message_data, buf_ptr);
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
    to_host_byte_order(buffer);

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

// ############################### utility function definitions #################################

// ################################## TODO - ADD NO_MESSAGE MESSAGE TYPE ??? #######################
// return type of message if command matches, -1 otherwise
static MessageType get_message_type(char *command)
{
    // check for no input
    if (!command) return -1;

    // check for no differences
    else if (!strcmp(command, "join")) return JOIN;
    
    else if (!strcmp(command, "post")) return POST;

    else if (!strcmp(command, "leave")) return LEAVE;

    // ######################### TODO - LOGIC WITH SHUTDOWN VS SHUT DOWN ????? ##########################
        // command recognition
    else if (!strcmp(command, "shutdown")) return SHUTDOWN;

    else if (!strcmp(command, "shutdown_all")) return SHUTDOWN_ALL;

    // fail command
    return -1;
}

// utility function to check if a character is a whitespace character
static bool is_white_space(char character)
{
    return character == ' ' || character == '\t' || character == '\n';
}

// take in a character buffer as input and write the first word into the output
static void parse_word(char **buffer, char *first_word)
{
    // count size of first word
    int word_size = 0;
    // clear output buffer
    first_word[0] = '\0';

    // pass leading whitespace
    while (is_white_space(**buffer)) (*buffer)++;

    // ######################## needs logic to keep reading if command is shutdown ?? whitespace not correct logic ??? ####################
    // loop while not at end of string
    while (word_size < MESSAGE_LEN && **buffer != '\0')
    {
        // check for whitespace
        if (is_white_space(**buffer)) break;
        
        // copy over character
        first_word[word_size] = to_lower(**buffer);

        // increment pointer
        word_size++;
        (*buffer)++;

        // update state
        first_word[word_size] = '\0';
    }

    // pass trailing whitespace from first word
    while (is_white_space(**buffer)) (*buffer)++;
}

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

// utility function called by read_message to convert a struct to local byte order
static void to_host_byte_order(Message *message)
{
    // network integer conversion
    message->type = ntohl(message->type);
    // network unsigned short integer conversion
    message->chat_node.port = ntohs(message->chat_node.port);
}

// return a lower-case shifted character if c is upper-case, return same character otherwise
static char to_lower(char c)
{
    // check if value is upper case letter
    if (c >= 'A' && c <= 'Z') return c + 'a' - 'A';

    // default behavior
    return c;
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