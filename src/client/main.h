#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "../message.h"
#include "../properties.h"

#define PROPERTIES_FILE_PATH "properties.txt"

// associate chat node ?
typedef struct
{
    NodeState state;
    pthread_mutex_t state_lock;
    Properties *property_list;
} ThreadArgs;
