#include "main.h"
#include "receiver_handler.h"
#include "sender_handler.h"

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
        // sender does not determine program end ????
    // pthread_join(sender, NULL);

    // sender thread terminates on end of main
    return EXIT_SUCCESS;
}
