#ifndef CLIENT_LIST_H
#define CLIENT_LIST_H

#include <pthread.h>
#include "../chat_node.h"  // for struct ChatNode


typedef struct NodeList
{
    ChatNode *head;       // pointer to the first connected client
    pthread_mutex_t mutex;   // mutex for thread safety
} NodeList;


void add_node(NodeList *list, int client_socket);

bool is_in_list(NodeList *list, ChatNode *target);

void remove_node(NodeList *list, int client_socket);


#endif // CLIENT_LIST_H
