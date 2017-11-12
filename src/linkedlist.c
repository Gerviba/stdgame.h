#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "includes.h"

LinkedList newLinkedList(size_t size) {
	LinkedList list = {0, size, NULL, NULL};
	return list;
}

LinkedList* newLinkedListPointer(size_t size) {
	LinkedList *list = new(LinkedList);
	list->size = 0;
	list->dataSize = size;
	list->first = NULL;
	list->last = NULL;
	return list;
}

void listPush(LinkedList *list, void *data) {
	ListElement *new = new(ListElement);
	new->data = malloc(list->dataSize);
	new->next = NULL;

	int i;
	for (i = 0; i < list->dataSize; ++i)
		*(char *)(new->data + i) = *(char *)(data + i);

	if (list->first == NULL)
		list->first = new;

	if (list->last != NULL)
		list->last->next = new;

	list->last = new;
	++list->size;
}

void* listGetValue(ListElement *it) {
	return it->data;
}

void listFree(LinkedList *list) {
	ListElement* head = list->first;
	ListElement* tmp;

	while (head != NULL) {
		tmp = head;
		free(head->data);
		head = head->next;
		free(tmp);
	}

	list->size = 0;
	list->first = NULL;
	list->last = NULL;
}
