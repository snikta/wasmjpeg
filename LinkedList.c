#include <stdlib.h>
#include <string.h>
#include "./LinkedList.h"
#include "./types.h"

void InsertNodeAtBeginningOfLinkedList(
	struct LinkedList *lst,
	struct LinkedListNode *node
) {
	node->nextNode = lst->firstNode;
	lst->firstNode = node;
}

LinkedListNode *CreateLinkedListNodeFromHuffmanNode(huffmanNode *huffNode) {
	LinkedListNode *Node = (LinkedListNode *) malloc(sizeof(LinkedListNode));
	Node->huffNode = huffNode;
	return Node;
}

LinkedListNode *CreateLinkedListNodeFromMCU(MCU mcu) {
	LinkedListNode *Node = (LinkedListNode *) malloc(sizeof(LinkedListNode));
	Node->mcu = mcu;
	return Node;
}

LinkedListNode *CreateLinkedListNodeFromLinkedList(LinkedList *lst) {
	LinkedListNode *Node = (LinkedListNode *) malloc(sizeof(LinkedListNode));
	Node->lst = lst;
	return Node;
}

LinkedListNode *CreateLinkedListNodeFromNewLinkedList() {
	LinkedListNode *Node = (LinkedListNode *) malloc(sizeof(LinkedListNode));
	Node->lst = CreateLinkedList();
	return Node;
}

LinkedListNode *CreateLinkedListNodeFromString(char *str) {
	LinkedListNode *Node = (LinkedListNode *) malloc(sizeof(LinkedListNode));
	Node->str = strdup(str);
	return Node;
}

void AppendNodeToLinkedList(
	struct LinkedList *lst,
	struct LinkedListNode *node
) {
	if (lst->firstNode == 0) {
		lst->firstNode = node;
		lst->lastNode = node;
	} else {
		lst->lastNode->nextNode = node;
		lst->lastNode = node;
	}
	node->nextNode = 0;
}

void ClearLinkedList(struct LinkedList *lst) {
	LinkedListNode *curNode;
	while ((curNode = lst->firstNode) != 0) {
		lst->firstNode = lst->firstNode->nextNode;
		free(curNode);
		curNode = 0;
	}
	lst->firstNode = 0;
	lst->lastNode = 0;
}

LinkedListNode *GetLinkedListNodeByIndex(struct LinkedList *lst, int index) {
	LinkedListNode *curNode = lst->firstNode;
	int i = 0;
	while (curNode != 0) {
		if (i == index) {
			return curNode;
		}
		curNode = curNode->nextNode;
		i++;
	}
	return 0;
}

int GetLinkedListSize(struct LinkedList *lst) {
	LinkedListNode *curNode = lst->firstNode;
	int i = 0;
	while (curNode != 0) {
		curNode = curNode->nextNode;
		i++;
	}
	return i;
}

LinkedList *CreateLinkedList() {
	struct LinkedList *List = (LinkedList *) malloc(sizeof(LinkedList));
	List->firstNode = 0;
	List->lastNode = 0;
	return List;
}

void RemoveLastLinkedListNode(LinkedList *lst) {
	LinkedListNode *Node = lst->firstNode;
	if (Node == lst->lastNode) {
		lst->firstNode = 0;
		lst->lastNode = 0;
		free(Node);
	} else {
		while (Node != 0 && Node->nextNode != lst->lastNode) {
			Node = Node->nextNode;
		}
		free(lst->lastNode);
		lst->lastNode = Node;
		Node->nextNode = 0;
	}
}