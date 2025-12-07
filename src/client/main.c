#include "main.h"
#include "receiver_handler.h"
#include "sender_handler.h"

#include <stdatomic.h>

int main(int argc, char *argv[]) 
{
    // handle argc argv for properties path
    const char *properties_path = 
        (argc > 1) ? argv[1] : PROPERTIES_FILE_PATH;

    // load properties
    Properties *property_list = property_read_properties(properties_path);

    //  start listener and chat threads
    pthread_t listener;
    pthread_t sender;

    // ready message to client console
    printf("You are ready to go!\n");

    // read properties data
    ThreadArgs args =
    {
        .property_list = property_list,
        .session_end = false
    };

    // spawn threads
    pthread_create(&listener, NULL, reciever_handler, (void*)&args);
    pthread_create(&sender, NULL, sender_handler, (void*)&args);

    pthread_join(sender, NULL);

    // wait for threads to finish execution
        // program end on listener receiving 
    pthread_join(listener, NULL);

    // wait for sender thread to process shutdown and release resources
    pthread_exit(NULL);

    // sender thread terminates on end of main
    return EXIT_SUCCESS;
}
