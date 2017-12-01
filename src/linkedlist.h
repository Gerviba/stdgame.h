/**
 * @file linkedlist.h
 * @author Gerviba (Szabo Gergely)
 * @brief Generic type linked list
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
	int size;
	size_t dataSize;
	ListItem *first;
	ListItem *last;
} LinkedList;

/**
 * New list value
 *
 * @param size The size of the list type
 * @note It uses malloc, so the listFree() function is required to free its values.
 * @deprecated Use dynamic allocated newLinkedListPointer() instead.
 * @see newLinkedListPointer()
 */
LinkedList newLinkedList(size_t size);

/**
 * New list pointer
 *
 * @param size The size of the list type
 * @note It uses malloc, so the listFree() function is required to free its values.
 * @return Dynamicly allocated linked list
 * @see listFree()
 */
LinkedList* newLinkedListPointer(size_t size);

/**
 * Add a new list into the end of the list
 */
void listPush(LinkedList* list, void* data);

/**
 * Returns a pointer to the value of the ListItem
 *
 * @return Value pointer
 */
void* listGetValue(ListItem* it);

/**
 * Free the dynamicly allocated memory
 *
 * @note The list could not be used after calling this method.
 */
void listFree(LinkedList* list);


#endif /* LINKEDLIST_H_ */
