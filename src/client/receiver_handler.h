// preprocessor directives
#ifndef RECEIVER_HANDLER_H
#define RECEIVER_HANDLER_H

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
#include <netdb.h>

// function prototypes
void *reciever_handler(void *args);

#endif // RECEIVER_HANDLER_H
