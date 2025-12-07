// preprocessor directives
#ifndef CHAT_NODE_H
#define CHAT_NODE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// constants
#define IP_STR_LEN 15
#define LOGICAL_NAME_LEN 32

// ChatNode struct definition
typedef struct ChatNode
{
    char logical_name[LOGICAL_NAME_LEN];
    char ip[IP_STR_LEN];    // ip address can be retrieved from config where the user specifies it manually
    unsigned short int port;// port number from config
    struct ChatNode *next;
} ChatNode;

// function prototypes

ChatNode *create_node(char *logical_name, char *ip, unsigned short int port);

bool same_node(ChatNode *node1, ChatNode *node2);

#endif // CHAT_NODE_H
