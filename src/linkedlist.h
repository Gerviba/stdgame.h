/**
 * @file linkedlist.h
 * @author Gerviba (Szabo Gergely)
 * @brief Generic type linked list (header)
 *
 * @par Definition:
 * 		linkedlist.c
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

/**
 * New list 'constructor' macro for generic type
 * @param x Type of the list
 */
#define newList(x) (newLinkedListPointer(sizeof(x)))

/**
 * Foreach for lists
 * @param it Iterator
 * @param first First item of the iteration
 */
#define foreach(it, first) for (it = first; it != NULL; it = it->next)

/**
 * List item
 */
typedef struct ListItem {
	void *data;
	struct ListItem *next;
} ListItem, *Iterator;

/**
 * Linked list header
 */
typedef struct {
	size_t dataSize;
	ListItem *first;
	ListItem *last;
} LinkedList;


LinkedList newLinkedList(size_t size);
LinkedList* newLinkedListPointer(size_t size);
void listPush(LinkedList* list, void* data);
void* listGetValue(ListItem* it);
void listFree(LinkedList* list);

#endif /* LINKEDLIST_H_ */
