// preprocessor directives
#ifndef SENDER_HANDLER_H
#define SENDER_HANDLER_H

// header files
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// socket/bind/listen/accept
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

// read/write/close
#include <sys/uio.h>
#include <sys/types.h>
#include <unistd.h>

#include <signal.h>

// function prototypes
void *sender_handler(void *args);

#endif // SENDER_HANDLER_H
