#include "main.h"
#include "receiver_handler.h"
#include "sender_handler.h"

int main() 
{
    //  start listener and chat threads
    pthread_t listener;
    pthread_t sender;

    // allocate space for shared arguments to pass to both threads
    ThreadArgs shared_args;
    // default ot waiting for entry
    shared_args.state = SUSPENDED;
    // read properties data
    shared_args.property_list = property_read_properties(PROPERTIES_FILE_PATH);
    // initialize lock
    pthread_mutex_init(&shared_args.state_lock, NULL);

    // spawn threads
    pthread_create(&listener, NULL, reciever_handler, (void*)&shared_args);
    pthread_create(&sender, NULL, sender_handler, (void*)&shared_args);

    // wait for threads to finish execution
    // pthread_join(listener, NULL);
    pthread_detach(listener);
    pthread_join(sender, NULL);

    return EXIT_SUCCESS;
}
