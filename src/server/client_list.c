#include "client_list.h"

void add_node(NodeList *list, ChatNode *new_node)
{
	pthread_mutex_lock(&list->mutex);

	new_node->next = list->head;
	list->head = new_node;

	pthread_mutex_unlock(&list->mutex);
}

// iterate through node list, compare elements, and check if node already exists
bool is_in_list(NodeList *list, ChatNode *node)
{
	return false;
}

void remove_node(NodeList *list, ChatNode *target)
{
	ChatNode *current = NULL;
	ChatNode *previous = NULL;

	// critical section entry when list access made
	pthread_mutex_lock(&list->mutex);

	current = list->head;

	while (current != NULL)
	{
		// ########## POINTERS WONT NECESSARILY BE THE SAME ??????? ##############
		// if (current == target)
		if (same_node(current, target))
		{
			if (previous == NULL)
			{
				list->head = current->next;
			}
			else
			{
				previous->next = current->next;
			}

			free(current);
			break;
		}

		previous = current;
		current = current->next;
	}
	// critical section exit
	pthread_mutex_unlock(&list->mutex);

}