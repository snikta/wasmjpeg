#pragma once
typedef struct MergedCellProperties {
    int topRowIndex;
    int leftColIndex;
    int bottomRowIndex;
    int rightColIndex;
    int rowIndex;
    int colIndex;
} MergedCellProperties;
typedef unsigned long int TScreenColor;
int isNaN(char *val);
enum RedBlackColor { red, black };
typedef struct RedBlackNode
{
	int color;
	struct RedBlackNode* left;
	struct RedBlackNode* right;
	struct RedBlackNode* parent;
	char *key;
	struct _ASTNode *value;
	int deleted;
} RedBlackNode;
typedef struct RedBlackTree
{
	struct RedBlackNode* nil;
	struct RedBlackNode* root;
	int size;
} RedBlackTree;
RedBlackTree* rbt_create();
RedBlackNode* rbt_closest(RedBlackTree *tree, char *key);
RedBlackNode* rbt_search(RedBlackTree *tree, char *key, int closest);
void rbt_leftRotate(RedBlackTree *tree, RedBlackNode* x);
void rbt_rightRotate(RedBlackTree *tree, RedBlackNode* x);
RedBlackNode* rbt_binaryInsert(RedBlackTree *tree, char *newKey, struct _ASTNode *value);
RedBlackNode* rbt_insert(RedBlackTree *tree, char *newKey, struct _ASTNode *value);
RedBlackNode* rbt_minimum(RedBlackTree *tree, RedBlackNode* x);
RedBlackNode* rbt_maximum(RedBlackTree *tree, RedBlackNode* x);
RedBlackNode* rbt_predecessor(RedBlackTree *tree, RedBlackNode* x);
RedBlackNode* rbt_successor(RedBlackTree *tree, RedBlackNode* x);
void rbt_deleteNodeFixup(RedBlackTree *tree, RedBlackNode* x);
RedBlackNode* rbt_deleteNode(RedBlackTree *tree, RedBlackNode* z);
RedBlackNode* rbt_push(RedBlackTree *tree, struct _ASTNode *value);
RedBlackNode* rbt_search_for_value(RedBlackTree *tree, struct _ASTNode *value);

typedef struct _ASTNode {
	int ASTNumberValue;
	char *ASTStringValue;
	char *cachedResult;
	TScreenColor backgroundColor;
	RedBlackTree *dependedOnBy;
	RedBlackTree *dependsOn;
	int highlight;
	int bold;
	int italic;
	int underline;
	int strikethrough;
	MergedCellProperties *merged;
} _ASTNode;

_ASTNode *CreateASTNumberNode(int num);
_ASTNode *CreateASTStringNode(char *str);