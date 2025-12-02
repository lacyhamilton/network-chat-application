#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <signal.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
// #include <time.h>
#include <netdb.h>

#include "message.h"
#include "chat_node.h"
#include "properties.h"
#include "server/client_list.h"
#include "server/client_handler.h"

#define PROPERTIES_FILE_PATH "server_properties.txt"
#define NUM_CONNECTIONS 20

// gcc -Wall src/server_test.c src/chat_node.c src/message.c src/properties.c src/server/client_list.c src/server/client_handler.c -o o.exe -lpthread

int main()
{

   // ******************************************************************** 
   // these files contain detailed code that is fully functional           
   // it won't need any more work. Specifically, main.c reads properties   
   // and then implements the server loop                                  
   // ******************************************************************** 

    // socket information for listening
    int server_socket;
    struct sockaddr_in server_address;
    int yes = 1;

    Properties *property_list = property_read_properties(PROPERTIES_FILE_PATH);
    // ############### MAKE SURE MATCHES WITH PROPERTIES FILE FIELDS ################
    char *server_port = property_get_property(property_list, "SERVER_PORT");

    // ignore SIGPIE on client disconnect
    signal(SIGPIPE, SIG_IGN);

    // create unname listen socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failure");
        exit(EXIT_FAILURE);
    }

    // avoid "Address already in use" error
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
    {
        perror("setsockopt failure");
        exit(EXIT_FAILURE);
    }

    // bind socket
    server_address.sin_family = AF_INET;                // accept IP addresses
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // accept clients on any interface
    server_address.sin_port = htons(atoi(server_port)); // specify port to listen on

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) != 0)
    {
        perror("Failed binding socket");
        exit(EXIT_FAILURE);
    }

    // listen on created socket
    if (listen(server_socket, NUM_CONNECTIONS) != 0)
    {
        perror("Failed to listen on socket");
        exit(EXIT_FAILURE);
    }

    // initialize chat client linked list
    NodeList* chat_clients = malloc(sizeof(NodeList));
    chat_clients->head = NULL;
    pthread_mutex_init(&chat_clients->mutex, NULL);

    // server loop
    while (true)
    {
        // Identify clients by IP/port.
        // This info will be manually added to the properties file in key-value pairs, where we can retrieve it for use here.

        // accept a client connection
        int client_socket = accept(server_socket, NULL, NULL);
        // check if accept fails
        if (client_socket == -1)
        {
            perror("Failed to accept client connection");
            continue; //  do not exit the loop here
        }

        // create struct to hold the thread args so that it can be passed to pthread_create()
        ClientThreadArgs *thread_args = malloc(sizeof(ClientThreadArgs));
        thread_args->client_socket = client_socket;
        thread_args->client_list = chat_clients;

        pthread_t thread_id;

        // THREAD CREATION
        // create thread to handle client inside of talk_to_client (defined in client_handler.c)

        // ###############################################################################
        // if (pthread_create(&thread_id, NULL, talk_to_client, thread_args) != 0) {
        if (pthread_create(&thread_id, NULL, talk_to_client, (void *)thread_args) != 0) {
            perror("Failed to create thread for client");
            close(client_socket);
            free(thread_args);
            continue;
        }
        // THREAD DESTRUCTION
        pthread_detach(thread_id);
    }

    return 0;
}