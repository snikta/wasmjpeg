#include <stdlib.h>
#include <string.h>
#include "./WASMJPEGLinkedList.h"
#include "./WASMJPEGRedBlackTree.h"
#include "./types.h"

void InsertNodeAtBeginningOfWASMJPEGLinkedList(
	struct WASMJPEGLinkedList *lst,
	struct WASMJPEGLinkedListNode *node
) {
	node->nextNode = lst->firstNode;
	lst->firstNode = node;
}

WASMJPEGLinkedListNode *CreateWASMJPEGLinkedListNodeFromHuffmanNode(huffmanNode *huffNode) {
	WASMJPEGLinkedListNode *Node = (WASMJPEGLinkedListNode *) malloc(sizeof(WASMJPEGLinkedListNode));
	Node->huffNode = huffNode;
	return Node;
}

WASMJPEGLinkedListNode *CreateWASMJPEGLinkedListNodeFromMCU(MCU mcu) {
	WASMJPEGLinkedListNode *Node = (WASMJPEGLinkedListNode *) malloc(sizeof(WASMJPEGLinkedListNode));
	Node->mcu = mcu;
	return Node;
}

WASMJPEGLinkedListNode *CreateWASMJPEGLinkedListNodeFromWASMJPEGLinkedList(WASMJPEGLinkedList *lst) {
	WASMJPEGLinkedListNode *Node = (WASMJPEGLinkedListNode *) malloc(sizeof(WASMJPEGLinkedListNode));
	Node->lst = lst;
	return Node;
}

WASMJPEGLinkedListNode *CreateWASMJPEGLinkedListNodeFromNewWASMJPEGLinkedList() {
	WASMJPEGLinkedListNode *Node = (WASMJPEGLinkedListNode *) malloc(sizeof(WASMJPEGLinkedListNode));
	Node->lst = CreateWASMJPEGLinkedList();
	return Node;
}

WASMJPEGLinkedListNode *CreateWASMJPEGLinkedListNodeFromWASMJPEGRedBlackTree(WASMJPEGRedBlackTree *rbt) {
	WASMJPEGLinkedListNode *Node = (WASMJPEGLinkedListNode *) malloc(sizeof(WASMJPEGLinkedListNode));
	Node->rbt = rbt;
	return Node;
}

WASMJPEGLinkedListNode *CreateWASMJPEGLinkedListNodeFromNewWASMJPEGRedBlackTree() {
	WASMJPEGLinkedListNode *Node = (WASMJPEGLinkedListNode *) malloc(sizeof(WASMJPEGLinkedListNode));
	Node->rbt = wasmjpeg_rbt_create();
	return Node;
}

WASMJPEGLinkedListNode *CreateWASMJPEGLinkedListNodeFromString(char *str) {
	WASMJPEGLinkedListNode *Node = (WASMJPEGLinkedListNode *) malloc(sizeof(WASMJPEGLinkedListNode));
	Node->str = strdup(str);
	return Node;
}

void AppendNodeToWASMJPEGLinkedList(
	struct WASMJPEGLinkedList *lst,
	struct WASMJPEGLinkedListNode *node
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

WASMJPEGLinkedList *CopyHuffmanWASMJPEGLinkedList(struct WASMJPEGLinkedList *lst) {
	WASMJPEGLinkedList *copiedList = CreateWASMJPEGLinkedList();
	WASMJPEGLinkedListNode *Node = lst->firstNode;
	while (Node) {
		WASMJPEGLinkedListNode *newNode = (WASMJPEGLinkedListNode *) malloc(sizeof(WASMJPEGLinkedListNode));
		newNode->huffNode = Node->huffNode;
		AppendNodeToWASMJPEGLinkedList(copiedList, newNode);
		Node = Node->nextNode;
	}
	return copiedList;
}

void ClearWASMJPEGLinkedList(struct WASMJPEGLinkedList *lst) {
	WASMJPEGLinkedListNode *curNode;
	while ((curNode = lst->firstNode) != 0) {
		lst->firstNode = lst->firstNode->nextNode;
		free(curNode);
		curNode = 0;
	}
	lst->firstNode = 0;
	lst->lastNode = 0;
}

WASMJPEGLinkedListNode *GetWASMJPEGLinkedListNodeByIndex(struct WASMJPEGLinkedList *lst, int index) {
	WASMJPEGLinkedListNode *curNode = lst->firstNode;
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

int GetWASMJPEGLinkedListSize(struct WASMJPEGLinkedList *lst) {
	WASMJPEGLinkedListNode *curNode = lst->firstNode;
	int i = 0;
	while (curNode != 0) {
		curNode = curNode->nextNode;
		i++;
	}
	return i;
}

WASMJPEGLinkedList *CreateWASMJPEGLinkedList() {
	struct WASMJPEGLinkedList *List = (WASMJPEGLinkedList *) malloc(sizeof(WASMJPEGLinkedList));
	List->firstNode = 0;
	List->lastNode = 0;
	return List;
}

void RemoveLastWASMJPEGLinkedListNode(WASMJPEGLinkedList *lst) {
	WASMJPEGLinkedListNode *Node = lst->firstNode;
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