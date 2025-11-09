// preprocessor directives
#ifndef CHAT_NODE_H
#define CHAT_NODE_H

// for constants
#include "message.h"

typedef struct ChatNode
{
    char logical_name[LOGICAL_NAME_LEN];
    char *ip; // ip address can be retrieved from config where the user specifies it manually
    int port; // port number from config
    struct ChatNode *next;
} ChatNode;

// function prototypes

#endif // CHAT_NODE_H
