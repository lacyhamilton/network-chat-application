// header files
#include "sender_handler.h"
#include "main.h"

// function definitions

/*
  here you OUTLINE the code of the function that is being executed in the sending thread.
  You have to have the main loop in place that processes the different pieces of information 
  a user may input. This, again, involves a code structure with entry points to process the 
  different commands or a note the user put in.
*/

void *senderHandler(void* args)
{

  SenderArgs *senderArgs = (SenderArgs*)args;

  char *serverIP = senderArgs->serverIP;
  char *clientIP = senderArgs->clientIP;
  char *clientPort = senderArgs->clientPort;
  char *serverPort = senderArgs->serverPort;

  while(true){

  }
  

  return NULL;
}
