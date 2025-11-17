// preprocessor directives
#ifndef CHAT_NODE_H
#define CHAT_NODE_H

#include <stdlib.h>
#include <stdbool.h>

// for constants
//
// ################ STATES NOT NEEDED ??? IMPLEMENTED THROUGH PROCEDURE ###########
// typedef enum 
// {
//     EXIT,
//     JOINED,
//     SUSPENDED,
// } NodeState;

#define IP_STR_LEN 15

typedef struct ChatNode
{
    char logical_name[32];
    char ip[IP_STR_LEN];    // ip address can be retrieved from config where the user specifies it manually
    unsigned short int port;// port number from config
    struct ChatNode *next;
} ChatNode;

// function prototypes

// dynamically allocates a ChatNode struct and returns a pointer to it
ChatNode *create_node(char *ip, unsigned short int port);

bool same_node(ChatNode *node1, ChatNode *node2);

#endif // CHAT_NODE_H
