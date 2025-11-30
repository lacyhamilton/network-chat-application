// preprocessor directives
#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <unistd.h>

#include "../message.h"
#include "../chat_node.h"
#include "client_list.h"

// ############### TODO SHOULD ALSO BE RECEIVING CHATNODE LIST AS A PARAMETER ###################
typedef struct {
	int client_socket;		// from client connected by accept
	NodeList* client_list;	// head of clients known by the server
} ClientThreadArgs;

// function prototypes

void *talk_to_client(void *arg);

#endif // CLIENT_HANDLER_H