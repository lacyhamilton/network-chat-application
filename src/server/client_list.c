#include "client_list.h"

#define DBG
#include "../dbg.h"

void add_node(NodeList *list, ChatNode *new_node)
{
	if (!list || !new_node) return;

	pthread_mutex_lock(&list->mutex);

	// new_node->next = list->head;
	// list->head = new_node;

	if (!list->head)
	{
		list->head = new_node;
		pthread_mutex_unlock(&list->mutex);
		return;
	}
	
	// variable for list traversal
	ChatNode *curr = list->head;
	while (curr)
	{
		if (!curr->next)
		{
			curr->next = new_node;
			curr = NULL;
		}
		else curr = curr->next;
	}

	pthread_mutex_unlock(&list->mutex);

	debug("added node named %s at %s %hu\n",
											new_node->logical_name,
											new_node->ip,
											new_node->port);
}

// iterate through node list, compare elements, and check if node already exists
bool is_in_list(NodeList *list, ChatNode *node)
{
	return false;
}

void safe_remove_node(NodeList *list, ChatNode *target)
{
	// critical section entry when list access made
	pthread_mutex_lock(&list->mutex);

	unsafe_remove_node(&list->head, target);

	// critical section exit
	pthread_mutex_unlock(&list->mutex);

}

void unsafe_remove_node(ChatNode **head, ChatNode *target)
{
	ChatNode *current = NULL;
	ChatNode *previous = NULL;

	current = *head;

	while (current != NULL)
	{
		if (same_node(current, target))
		{
			// check if target is head pointer
			if (previous == NULL)
			{
				*head = current->next;
			}
			else
			{
				previous->next = current->next;
			}

			debug("removed node named %s at %s %hu\n",
											current->logical_name,
											current->ip,
											current->port);

			free(current);
			return;
		}

		previous = current;
		current = current->next;
	}

}
