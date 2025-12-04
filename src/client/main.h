#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

// gcc -Wall src/client/main.c src/client/sender_handler.c src/client/receiver_handler.c src/message.c src/chat_node.c src/properties.c -o o.exe

// session variable check
#include <stdatomic.h>

#include "../message.h"
#include "../properties.h"

#define PROPERTIES_FILE_PATH "src/client/properties.txt"

typedef struct
{
    Properties *property_list;  // hold all arguments read from file
    atomic_bool session_end;    // allow communication from listener to sender
} ThreadArgs;


// ##################### STATE EXISTS IN PROCEDURE ONLY - NO SHARED DATA ###################
// // associate chat node ?
// typedef struct
// {
//     NodeState state;
//     pthread_mutex_t state_lock;
//     Properties *property_list;
// } ThreadArgs;
