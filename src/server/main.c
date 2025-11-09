// header files
#include "main.h"
#include "../properties.h"

// main program
int main()
{

    /* ******************************************************************** */
    /* these files contain detailed code that is fully functional           */
    /* it won't need any more work. Specifically, main.c reads properties   */
    /* and then implements the server loop                                  */
    /* ******************************************************************** */

    // socket information for listening
    int server_socket;
    struct sockaddr_in server_address;
    int yes = 1;

    Properties *property_list = property_read_properties(PROPERTIES_FILE_PATH);
    // ############### MAKE SURE MATCHES WITH PROPERTIES FILE FIELDS ################
    char *server_ip = property_get_property(property_list, "IP");
    char *server_port = property_get_property(property_list, "Port");

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

    if (bind(server_socket, (struct sockaddr     *)&server_address, sizeof(server_address)) != 0)
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

    // server loop
    while (true)
    {
        // ############### IDENTIFY CLIENTS BY LOGICAL NAME OR IP/PORT ??? ###################

        // accept a client connection
        int client_socket = accept(server_socket, NULL, NULL);
    }

    return 0;
}
