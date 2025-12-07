// header files
#include "message.h"

// ###################################### utility function prototypes #######################################

static MessageType get_message_type(char *command);

static bool is_white_space(char character);

static void parse_word(char *buffer, char *first_word);

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

    // hold the type of message
    MessageType type;

    // read user intput until newline
    fgets(buffer, MESSAGE_LEN, stdin);

    // check for valid message
    parse_word(buffer, command);

    type = get_message_type(command);

    output_message->type = type;
    strcpy(output_message->message_data, buffer);
}

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

    // create a copy of the given message to send
    Message send_copy = *message;

    // convert byte order
    to_network_byte_order(&send_copy);

    // send message over internet
    send_status = send_complete(socket, &send_copy, sizeof(Message));
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

// return type of message if command matches, -1 otherwise
static MessageType get_message_type(char *command)
{
    // check for no input
    if (!command) return -1;

    // check for no differences
    else if (!strcmp(command, "join")) return JOIN;

    else if (!strcmp(command, "leave")) return LEAVE;

    else if (!strcmp(command, "shutdown")) return SHUTDOWN;

    else if (!strcmp(command, "shutdown_all")) return SHUTDOWN_ALL;

    // default to a post on an unrecognized command
    return POST;
}

// utility function to check if a character is a whitespace character
static bool is_white_space(char character)
{
    return character == ' ' || character == '\t' || character == '\n';
}

// take in a character buffer as input and write the first word into the output
static void parse_word(char *buffer, char *first_word)
{
    // count size of first word
    int word_size = 0;

    // clear output buffer
    first_word[0] = '\0';

    int buff_index = 0;

    // pass leading whitespace
    while (is_white_space(buffer[buff_index])) buff_index++;

    // loop while not at end of string
    while (word_size < MESSAGE_LEN && buffer[buff_index] != '\0')
    {
        // check for whitespace
        if (is_white_space(buffer[buff_index])) break;
        
        // copy over character
        first_word[word_size] = to_lower(buffer[buff_index]);

        // increment pointer
        word_size++;
        buff_index++;

        // update state
        first_word[word_size] = '\0';
    }

    // pass trailing whitespace from first word
    while (is_white_space(buffer[buff_index])) buff_index++;
}

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

// utility function called by send_message to convert a struct to internet byte order
static void to_network_byte_order(Message *message)
{
    // network integer conversion
    message->type = htonl(message->type);
    // network unsigned short integer conversion
    message->chat_node.port = htons(message->chat_node.port);
}