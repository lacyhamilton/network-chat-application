#include "main.h"

int main() 
{

  char *logicalName = "";
  bool join = false;
  char command[MESSAGE_LEN];
  char serverIP[24];
  printf("Type JOIN [server IP] to connect\n");

  while(!join)
  {
    scanf("%s %s", command, serverIP);
    if(strncmp(command, "JOIN", 4) == 0)
    {
      join = true;
    }
  }

  char* properties_file = "properties.txt";
  Properties* properties;
  properties = property_read_properties(properties_file);
  char *clientPort= property_get_property(properties, "CLIENT_PORT");
  char *clientIP= property_get_property(properties, "CLIENT_IP");
  char *serverPort = property_get_property(properties, "SERVER_PORT");
  
  ChatState *state = malloc(sizeof(*state));


  SenderArgs *senderArgs = malloc(sizeof(*senderArgs));
  senderArgs->serverIP = serverIP;
  senderArgs->serverPort = serverPort;
  senderArgs->clientPort = clientPort;
  senderArgs->clientIP = clientIP;
  senderArgs->logicalName = logicalName;
  senderArgs->state = state;

  //  start listener and chat threads
  pthread_t listener;
  pthread_t sender;

  pthread_create(&listener, NULL, recieverHandler, (void*)senderArgs);
  pthread_create(&sender, NULL, senderHandler, (void*)senderArgs);

  pthread_join(listener, NULL);
  pthread_join(sender, NULL);

  free(senderArgs);

  return EXIT_SUCCESS;
}
