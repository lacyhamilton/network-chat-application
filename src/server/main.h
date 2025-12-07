// preprocessor directives
#ifndef MAIN_H
#define MAIN_H

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

#define PROPERTIES_FILE_PATH "src/server/properties.txt"
#define NUM_CONNECTIONS 20

#endif // MAIN_H
