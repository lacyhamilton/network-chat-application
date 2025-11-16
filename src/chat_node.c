// header files
#include "chat_node.h"

// function definitions

/*
  these define the structure that represents the identifying information of a chat node,
  i.e. IP, port and logical name. You will also want to foresee functionality that acts 
  upon chat nodes. One important example is to have code in place that implements linked 
  lists for chat nodes. This code needs to be OUTLINED.
*/

// ################ TODO - DOES THIS MAKE SENSE FOR CLIENT-SENDER LOGIC ???? ##################
ChatNode *create_node(char *ip, unsigned short int port)
{
    return NULL;
}

// checks if all identifying fields in two nodes are identical
bool same_node(ChatNode *node1, ChatNode *node2)
{
    return false;
}