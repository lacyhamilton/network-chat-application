#include "../message.h"
#include "receiver_handler.h"
#include "sender_handler.h"
#include "main.h"

int main() 
{

  bool join = false;
  char command[MESSAGE_LEN];
  char ip[24];
  printf("Type JOIN [server IP] to connect\n");

  while(!join)
  {
    scanf("%s %s", command, ip);
    if(strncmp(command, "JOIN", 4) == 0)
    {
      join = true;
    }
  }

  SenderArgs senderArgs;
  senderArgs.ip = ip;
  //  start listener and chat threads
  pthread_t listener;
  pthread_t sender;
  puts("MAIN 28)");

  pthread_create(&listener, NULL, reciever_handler, NULL);
  pthread_create(&sender, NULL, sender_handler, (void*)&senderArgs);

  pthread_join(listener, NULL);
  pthread_join(sender, NULL);

  return EXIT_SUCCESS;
}
