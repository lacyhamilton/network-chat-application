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

void *sender_handler(void* args)
{
  SenderArgs *senderArgs = (SenderArgs*)args;
  char* properties_file = "properties.txt";
  Properties* properties;
  char* key = "CHAT_PORT";
  char *chatPort= property_get_property(properties, key);
  key = "CHAT_IP";
  char *chatIP= property_get_property(properties, key);

  char *serverIP = senderArgs->ip;

  printf("%s\n", serverIP);
  

  return NULL;
}
