// preprocessor directives
#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <unistd.h>
#include <stdatomic.h>

#include "../message.h"
#include "../chat_node.h"
#include "client_list.h"

typedef struct {
	int client_socket;			// from client connected by accept
	ChatNode *node_self;		// remember self - allow self-shutdown
	atomic_bool *enter_shutdown;// state variable to modify server_socket variable
	atomic_bool *session_end;	// thread-safe state variable
	NodeList* client_list;		// head of clients known by the server
} ClientThreadArgs;

// function prototypes

void *talk_to_client(void *arg);

#endif // CLIENT_HANDLER_H