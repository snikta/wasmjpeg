#pragma once
#include "types.h"

typedef struct LinkedList {
	struct LinkedListNode *firstNode;
	struct LinkedListNode *lastNode;
} LinkedList;

typedef struct LinkedListNode {
    struct LinkedListNode *nextNode;
	MCU mcu;
	struct LinkedList *lst;
	struct RedBlackTree *rbt;
	char *str;
	huffmanNode *huffNode;
} LinkedListNode;

void InsertNodeAtBeginningOfLinkedList(
	struct LinkedList *lst,
	struct LinkedListNode *node
);
void AppendNodeToLinkedList(
	struct LinkedList *lst,
	struct LinkedListNode *node
);
LinkedList *CopyHuffmanLinkedList(struct LinkedList *lst);
void ClearLinkedList(struct LinkedList *lst);

LinkedListNode *GetLinkedListNodeByIndex(struct LinkedList *lst, int index);
int GetLinkedListSize(struct LinkedList *lst);
LinkedList *CreateLinkedList();
void RemoveLastLinkedListNode(LinkedList *lst);
LinkedListNode *CreateLinkedListNodeFromMCU(MCU mcu);
LinkedListNode *CreateLinkedListNodeFromHuffmanNode(huffmanNode *huffNode);
LinkedListNode *CreateLinkedListNodeFromLinkedList(struct LinkedList *lst);
LinkedListNode *CreateLinkedListNodeFromNewLinkedList();
LinkedListNode *CreateLinkedListNodeFromRedBlackTree(struct RedBlackTree *rbt);
LinkedListNode *CreateLinkedListNodeFromNewRedBlackTree();
LinkedListNode *CreateLinkedListNodeFromString(char *str);