#include "network.h"

// socket binding and connection
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

// socket close
#include <unistd.h>

// create a socket for sending to specific node
    // return socket FD on successful connection
    // return a negative value if failure
int open_connection(ChatNode *connect_dest)
{
    int send_socket;
    struct sockaddr_in send_address;;
    
    send_socket = socket(AF_INET, SOCK_STREAM, 0);

    // check for failed socket creation and send error socket
    if (send_socket < 0) return send_socket;

	// reset memory occupied
	memset(&send_address, 0, sizeof(send_address));

	// create addr struct - base address by node that exists outside of synchronized list
	send_address.sin_family = AF_INET;
	send_address.sin_addr.s_addr = inet_addr(connect_dest->ip);
	send_address.sin_port = htons(connect_dest->port);

	// connect to client's listener - check for failure
	if (connect(send_socket, (struct sockaddr *)&send_address, sizeof(send_address)) == -1)
	{
		perror("Failed connecting to client");
		close(send_socket);
		// send error
		return -1;
	}

    return send_socket;
}