// preprocessor directives
#ifndef MESSAGE_H
#define MESSAGE_H

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
    MessageType type;
    // messages are identified by logical name, matched with port/ip by server
    char logical_name[LOGICAL_NAME_LEN];
    // message contents, does not always contain data
    char message_data[MESSAGE_LEN];
} Message;

// function prototypes

#endif // MESSAGE_H
