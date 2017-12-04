/**
 * @file linkedlist.c
 * @author Gerviba (Szabo Gergely)
 * @brief Generic type linked list
 *
 * @par Header:
 * 		linkedlist.h
 *
 * @note For iterations use the foreach() macro.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "stdgame.h"

/**
 * New list value
 *
 * @param size The size of the list type
 * @note It uses malloc, so the listFree() function is required to free its values.
 * @deprecated Use dynamic allocated newLinkedListPointer() instead.
 * @see newLinkedListPointer()
 */
LinkedList newLinkedList(size_t size) {
	LinkedList list = {size, NULL, NULL};
	return list;
}

/**
 * New list pointer
 *
 * @param size The size of the list type
 * @note It uses malloc, so the listFree() function is required to free its values.
 * @return Dynamicly allocated linked list
 * @see listFree()
 */
LinkedList* newLinkedListPointer(size_t size) {
	LinkedList *list = new(LinkedList);
	list->dataSize = size;
	list->first = NULL;
	list->last = NULL;
	return list;
}

/**
 * Add a new list into the end of the list
 *
 * @note The content will be copied
 *
 * @param list LinkedList head object
 * @param data Pointer to the data
 */
void listPush(LinkedList *list, void *data) {
	ListItem *new = new(ListItem);
	new->data = calloc(1, list->dataSize);
	new->next = NULL;

	int i;
	for (i = 0; i < list->dataSize; ++i)
		*(char *)(new->data + i) = *(char *)(data + i);

	if (list->first == NULL)
		list->first = new;

	if (list->last != NULL)
		list->last->next = new;

	list->last = new;
}

/**
 * Returns a pointer to the value of the ListItem
 *
 * @param list Actual item
 * @return Value pointer
 */
void* listGetValue(ListItem *it) {
	return it->data;
}

/**
 * Free the dynamicly allocated memory
 *
 * @note The list could not be used after calling this method.
 * @param list LinkedList head object
 */
void listFree(LinkedList *list) {
	if (list == NULL)
		return;
	ListItem* head = list->first;
	ListItem* tmp;

	while (head != NULL) {
		tmp = head->next;
		if (head->data != NULL)
			free(head->data);
		free(head);
		head = tmp;
	}

	list->first = NULL;
	list->last = NULL;
}
