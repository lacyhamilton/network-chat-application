#include "client_list.h"

void add_node(NodeList *list, ChatNode *node)
{
	pthread_mutex_lock(&list->mutex);

	node->next = list->head;
	list->head = node;

	pthread_mutex_unlock(&list->mutex);
}

void remove_node(NodeList *list, ChatNode *node)
{
	pthread_mutex_lock(&list->mutex);

	ChatNode *current = list->head;
	ChatNode *previous = NULL;

	while (current != NULL)
	{
		if (current == node)
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

	pthread_mutex_unlock(&list->mutex);
}