#pragma once
#include "types.h"

typedef struct WASMJPEGLinkedList {
	struct WASMJPEGLinkedListNode *firstNode;
	struct WASMJPEGLinkedListNode *lastNode;
} WASMJPEGLinkedList;

typedef struct WASMJPEGLinkedListNode {
    struct WASMJPEGLinkedListNode *nextNode;
	MCU mcu;
	struct WASMJPEGLinkedList *lst;
	struct WASMJPEGRedBlackTree *rbt;
	char *str;
	huffmanNode *huffNode;
} WASMJPEGLinkedListNode;

void InsertNodeAtBeginningOfWASMJPEGLinkedList(
	struct WASMJPEGLinkedList *lst,
	struct WASMJPEGLinkedListNode *node
);
void AppendNodeToWASMJPEGLinkedList(
	struct WASMJPEGLinkedList *lst,
	struct WASMJPEGLinkedListNode *node
);
WASMJPEGLinkedList *CopyHuffmanWASMJPEGLinkedList(struct WASMJPEGLinkedList *lst);
void ClearWASMJPEGLinkedList(struct WASMJPEGLinkedList *lst);

WASMJPEGLinkedListNode *GetWASMJPEGLinkedListNodeByIndex(struct WASMJPEGLinkedList *lst, int index);
int GetWASMJPEGLinkedListSize(struct WASMJPEGLinkedList *lst);
WASMJPEGLinkedList *CreateWASMJPEGLinkedList();
void RemoveLastWASMJPEGLinkedListNode(WASMJPEGLinkedList *lst);
WASMJPEGLinkedListNode *CreateWASMJPEGLinkedListNodeFromMCU(MCU mcu);
WASMJPEGLinkedListNode *CreateWASMJPEGLinkedListNodeFromHuffmanNode(huffmanNode *huffNode);
WASMJPEGLinkedListNode *CreateWASMJPEGLinkedListNodeFromWASMJPEGLinkedList(struct WASMJPEGLinkedList *lst);
WASMJPEGLinkedListNode *CreateWASMJPEGLinkedListNodeFromNewWASMJPEGLinkedList();
WASMJPEGLinkedListNode *CreateWASMJPEGLinkedListNodeFromWASMJPEGRedBlackTree(struct WASMJPEGRedBlackTree *rbt);
WASMJPEGLinkedListNode *CreateWASMJPEGLinkedListNodeFromNewWASMJPEGRedBlackTree();
WASMJPEGLinkedListNode *CreateWASMJPEGLinkedListNodeFromString(char *str);