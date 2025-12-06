#ifndef CLIENT_LIST_H
#define CLIENT_LIST_H

#include <pthread.h>
#include "../chat_node.h"  // for struct ChatNode


typedef struct NodeList
{
    ChatNode *head;       // pointer to the first connected client
    pthread_mutex_t mutex;   // mutex for thread safety
} NodeList;


void add_node(NodeList *list, ChatNode *new_node);

bool is_in_list(NodeList *list, ChatNode *target);

void safe_remove_node(NodeList *list, ChatNode *target);

void unsafe_remove_node(ChatNode **head, ChatNode *target);


#endif // CLIENT_LIST_H
