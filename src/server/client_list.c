#include "client_list.h"

void add_node(NodeList *list, ChatNode *node)
{
	pthread_mutex_lock(&list->mutex);

	node->next = list->head;
	list->head = node;

	pthread_mutex_unlock(&list->mutex);
}

// iterate through node list, compare elements, and check if node already exists
bool is_in_list(NodeList *list, ChatNode *node)
{
	return false;
}

void remove_node(NodeList *list, ChatNode *target)
{
	ChatNode *current = list->head;
	ChatNode *previous = NULL;

	while (current != NULL)
	{
		// ########## POINTERS WONT NECESSARILY BE THE SAME ??????? ##############
		// if (current == target)
		if (same_node(current, target))
		{
			// critical section entry at list modification only
			pthread_mutex_lock(&list->mutex);

			if (previous == NULL)
			{
				list->head = current->next;
			}
			else
			{
				previous->next = current->next;
			}

			// modification complete
			pthread_mutex_unlock(&list->mutex);

			free(current);
			break;
		}

		previous = current;
		current = current->next;
	}

}