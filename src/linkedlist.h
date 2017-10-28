#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

typedef struct ListElement {
	void *data;
	struct ListElement *next;
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
void* getListValue(ListElement *it);
void listClear(LinkedList *list);

#endif /* LINKEDLIST_H_ */
