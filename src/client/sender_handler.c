// header files
#include "sender_handler.h"
#include "main.h"
#include "../chat_node.h"

// function definitions

/*
  here you OUTLINE the code of the function that is being executed in the sending thread.
  You have to have the main loop in place that processes the different pieces of information 
  a user may input. This, again, involves a code structure with entry points to process the 
  different commands or a note the user put in.
*/

void *sender_handler(void* args)
{

  ThreadArgs *local_args = (ThreadArgs *)args;

  NodeState state= local_args->state;

  char user_input[MESSAGE_LEN];

  while(state != EXIT){
    scanf("%s", user_input);
    if(strncmp(user_input, "JOIN", 4) == 0)
    {
      if(state != JOINED)
      {
        printf("Already joined\n");
      }
      else 
      {
        state = JOINED;
      }
    }
    else if(strcmp(user_input, "LEAVE") == 0 && state != JOINED)
    {
      // disconnect user from the server function
      state = SUSPENDED;

      // send leave message to server function
    }
    else if(strcmp(user_input, "SHUTDOWN") == 0)
    {
      // send shutdown message to server function
      state = EXIT;
    }
    else if(strcmp(user_input, "SHUTDOWN ALL") == 0)
    {
      // send shutdown all message to server function
      state = EXIT;
    }
    else
    {
      // send regular message to server function
    }
  }
  

  return NULL;
}
