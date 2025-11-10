// header files
#include "receiver_handler.h"

// function definitions

/*
  here you OUTLINE the code that is being executed in the receiving thread:
  a function containing the server loop. In addition, you need to have another
  function in place that is being executed when the server connects in the server loop.
  You need to put a big switch statement into this latter function with entry points
  for the different kinds of messages that can be received from the server.
*/
void *reciever_handler()
{
  char* properties_file = "properties.txt";
  Properties* properties;
  char* key = "CHAT_PORT";
  char* chat_port;

  properties = property_read_properties(properties_file);
  chat_port = property_get_property(properties, key);
  key = "SERVER_PORT";
  char *server_port = property_get_property(properties, key);
  key = "SERVER_IP";
  char *server_ip= property_get_property(properties, key);
  key = "CHAT_IP";
  char *chat_ip= property_get_property(properties, key);

  return NULL;
}

