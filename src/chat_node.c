// header files
#include "chat_node.h"

// function definitions

// dynamically allocates and populates a ChatNode struct
ChatNode *create_node(char *logical_name, char *ip, unsigned short int port)
{
    // dynamically allocate space for new node
    ChatNode *new_node = (ChatNode *)malloc(sizeof(ChatNode));

    // check for allocation failure
    if (!new_node)
    {
        fprintf(stderr, "Node allocation failure\n");
        return NULL;
    }

    // populate struct
    strcpy(new_node->logical_name, logical_name);
    strcpy(new_node->ip, ip);
    new_node->port = port;

    return new_node;
}

// checks if all identifying fields in two nodes are identical
bool same_node(ChatNode *node1, ChatNode *node2)
{
    // compare all fields
    return strcmp(node1->logical_name, node2->logical_name) == 0
            && strcmp(node1->ip, node2->ip) == 0
			&& node1->port == node2->port;
}