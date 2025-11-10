// preprocessor directives
#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "message.h"
#include "chat_node.h"
#include "client_list.h"

typedef struct {
	int client_socket;
	NodeList* client_list;
} ClientThreadArgs;

// function prototypes

void talk_to_client(void *arg);

#endif // CLIENT_HANDLER_H