#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "../message.h"
#include "receiver_handler.h"
#include "sender_handler.h"
#include "../properties.h"

typedef struct
{
  char *serverIP;
  char *serverPort;
  char *clientIP;
  char *clientPort;
  char *logicalName;
  ChatState *state;
} SenderArgs;
