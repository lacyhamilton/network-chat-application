#include "message.h"
#include "chat_node.h"
#include "properties.h"
#include "client/receiver_handler.h"
#include "client/sender_handler.h"

// gcc -Wall src/client_test.c src/client/receiver_handler.c src/client/sender_handler.c src/chat_node.c src/message.c src/properties.c -o p.exe -lpthread

#define PROPERTIES_FILE_PATH "client_properties.txt"

int main() 
{
    //  start listener and chat threads
    pthread_t listener;
    pthread_t sender;

    // read properties data
    Properties *property_list = property_read_properties(PROPERTIES_FILE_PATH);

    // spawn threads
    pthread_create(&listener, NULL, reciever_handler, (void*)property_list);
    pthread_create(&sender, NULL, sender_handler, (void*)property_list);

    pthread_join(sender, NULL);

    // wait for threads to finish execution
        // program end on listener receiving 
    pthread_join(listener, NULL);

    // sender thread terminates on end of main
    return EXIT_SUCCESS;
}
