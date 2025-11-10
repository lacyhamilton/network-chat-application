// preprocessor directives
#ifndef MESSAGE_H
#define MESSAGE_H

#include "chat_node.h"
#define LOGICAL_NAME_LEN 32
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
    // sender's chat node
    ChatNode chat_node;
    // message contents, does not always contain data
    char message_data[MESSAGE_LEN];

} Message;

// function prototypes

#endif // MESSAGE_H
