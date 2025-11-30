// header files
#include "chat_node.h"

// function definitions

/*
  these define the structure that represents the identifying information of a chat node,
  i.e. IP, port and logical name. You will also want to foresee functionality that acts 
  upon chat nodes. One important example is to have code in place that implements linked 
  lists for chat nodes. This code needs to be OUTLINED.
*/

// ######################################## MAKE SURE POINTERS NOT NULL #######################################33

// ################ TODO - DOES THIS MAKE SENSE FOR CLIENT-SENDER LOGIC ???? ##################
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
    return !strcmp(node1->ip, node2->ip)
			&& node1->port == node2->port;
}