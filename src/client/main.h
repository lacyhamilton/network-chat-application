#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

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