#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#define newList(x) (newLinkedListPointer(sizeof(x)))

typedef struct StreamElement {
	void *data;
	struct StreamElement *next;
} ListElement;

typedef struct {
	int size;
	size_t dataSize;
	ListElement *first;
	ListElement *last;
} LinkedList;

LinkedList newLinkedList(size_t size);
LinkedList* newLinkedListPointer(size_t size);

void listPush(LinkedList *list, void *data);
void* listGetValue(ListElement *it);
void listFree(LinkedList *list);

#endif /* LINKEDLIST_H_ */
