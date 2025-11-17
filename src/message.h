// preprocessor directives
#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
// for network-host byte-order conversions
#include <arpa/inet.h>

#include "chat_node.h"

#define MESSAGE_LEN 64

typedef enum
{
    JOIN,
    POST,
    LEAVE,
    SHUTDOWN,
    SHUTDOWN_ALL
} MessageType;

typedef struct Message
{
    MessageType type; // enum ^
    // copy of sender's chat node - provides access to listening IP and Port
    ChatNode chat_node;
    // message contents, does not always contain data
    char message_data[MESSAGE_LEN];

} Message;

// function prototypes

// ################ TODO: CHANGE TO BOOL IF MESSAGE TYPE IS NOT IMPORTANT #############
// take in a string and attempt to create a valid Message struct
    // return -1 on failure
int interpret_message(char *str_buffer, Message *output_message);

// function to read a message passed over the internet, returns true if complete read was made
bool read_message(int upstream_socket, Message *buffer);

// function to send a message over the internet, return true if complete send was made
bool send_message(int socket, Message *message);

#endif // MESSAGE_H
