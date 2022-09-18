#include <stdlib.h>
#include "./RedBlackTree.h"
#include "./mathHelpers.h"
#include "./stringHelpers.h"

_ASTNode *CreateASTPointNode(double x, double y) {
	_ASTNode *retval = malloc(sizeof(_ASTNode));
	retval->ASTXValue = x;
	retval->ASTYValue = y;
	return retval;
}

_ASTNode *CreateASTNumberNode(int num) {
	_ASTNode *retval = malloc(sizeof(_ASTNode));
	retval->ASTNumberValue = num;
	return retval;
}

_ASTNode *CreateASTStringNode(char *str) {
	_ASTNode *retval = malloc(sizeof(_ASTNode));
	retval->ASTStringValue = str;
	return retval;
}

int isNaN(char *val) {
	if (strcmp(val, "") == 0) {
		return 0;
	}
	if (strcmp(val, ".") == 0) {
		return 1;
	}
	for (int i = 0, len = strlen(val); i < len; i++) {
		if (!((val[i] >= '0' && val[i] <= '9') || val[i] == '.' || val[i] == '+' || val[i] == '-' || val[i] == 'e')) {
			return 1;
		}
	}
	return 0;
}

RedBlackTree* rbt_create() {
	RedBlackTree* tree = (RedBlackTree *) malloc(sizeof(RedBlackTree));
	RedBlackNode* nil = (RedBlackNode *) malloc(sizeof(RedBlackNode));
	tree->nil = nil;
	tree->nil->key = strdup("-1");
	tree->nil->left = nil;
	tree->nil->right = nil;
	tree->root = nil;
	tree->size = 0;
	return tree;
}

RedBlackNode* rbt_search(RedBlackTree *tree, char *key, int closest)
{
	int dKey = atoi(key);
	RedBlackNode* x = tree->root;
	RedBlackNode* lastGood = x;
	while (x != tree->nil && strcmp(key, x->key) != 0)
	{
		lastGood = x;
		int lt;
		if (isNaN(x->key)) {
			if (strcmp(x->key, "-1") == 0) {
				lt = 0;
			} else {
				lt = strcmp(key, x->key) < 0;
			}
		} else {
			lt = dKey < atoi(x->key);
		}
		if (lt)
		{
			x = x->left;
		}
		else
		{
			x = x->right;
		}
	}
	if (closest == 1 && x == tree->nil)
	{
		return lastGood;
	}
	else
	{
		return strcmp(x->key, key) == 0 ? x : tree->nil;
	}
};
RedBlackNode* rbt_closest(RedBlackTree *tree, char *key)
{
	return rbt_search(tree, key, 1);
};
void rbt_leftRotate(RedBlackTree *tree, RedBlackNode *x)
{
	RedBlackNode* y = x->right;
	x->right = y->left;
	if (y->left != tree->nil)
	{
		y->left->parent = x;
	}
	y->parent = x->parent;
	if (x->parent == tree->nil)
	{
		tree->root = y;
	}
	else
	{
		if (x == x->parent->left)
		{
			x->parent->left = y;
		}
		else
		{
			x->parent->right = y;
		}
	}
	y->left = x;
	x->parent = y;
};
void rbt_rightRotate(RedBlackTree *tree, RedBlackNode *x)
{
	RedBlackNode* y = x->left;
	x->left = y->right;
	if (y->right != tree->nil)
	{
		y->right->parent = x;
	}
	y->parent = x->parent;
	if (x->parent == tree->nil)
	{
		tree->root = y;
	}
	else
	{
		if (x == x->parent->right)
		{
			x->parent->right = y;
		}
		else
		{
			x->parent->left = y;
		}
	}
	y->right = x;
	x->parent = y;
};
RedBlackNode* rbt_binaryInsert(RedBlackTree *tree, char *newKey, struct _ASTNode *value)
{
	RedBlackNode* z = (RedBlackNode *) malloc(sizeof(RedBlackNode)); // free later?
	z->color = black;
	z->key = strdup(newKey);
	z->left = tree->nil;
	z->right = tree->nil;
	z->parent = tree->nil;
	RedBlackNode* y = tree->nil;
	RedBlackNode* x = tree->root;
	int dKey = atoi(z->key);
	while (x != tree->nil)
	{
		y = x;
		int lt;
		if (isNaN(x->key)) {
			if (strcmp(x->key, "-1") == 0) {
				lt = 0;
			} else {
				lt = strcmp(newKey, x->key) < 0;
			}
		} else {
			lt = dKey < atoi(x->key);
		}
		if (lt)
		{
			x = x->left;
		}
		else
		{
			x = x->right;
		}
	}
	z->parent = y;
	if (y == tree->nil)
	{
		tree->root = z;
	}
	else {
		int lt;
		if (isNaN(y->key)) {
			if (strcmp(y->key, "-1") == 0) {
				lt = 1;
			} else {
				lt = strcmp(newKey, y->key) < 0;
			}
		} else {
			lt = dKey < atoi(y->key);
		}
		if (lt) {
			y->left = z;
		}
		else
		{
			y->right = z;
		}
	}
	tree->size++;
	z->value = value;
	return z;
};
RedBlackNode* rbt_insert(RedBlackTree *tree, char *newKey, struct _ASTNode *value)
{
	RedBlackNode *match = rbt_search(tree, newKey, 0);
	if (match != tree->nil)
	{
		match->value = value;
		return match;
	}
	RedBlackNode* x = rbt_binaryInsert(tree, newKey, value);
	RedBlackNode* newNode = x;
	RedBlackNode* y;
	x->color = red;
	while (x != tree->root && x->parent->color == red)
	{
		if (x->parent == x->parent->parent->left)
		{
			y = x->parent->parent->right;
			if (y->color == red)
			{
				x->parent->color = black;
				y->color = black;
				x->parent->parent->color = red;
				x = x->parent->parent;
			}
			else
			{
				if (x == x->parent->right)
				{
					x = x->parent;
					rbt_leftRotate(tree, x);
				}
				x->parent->color = black;
				x->parent->parent->color = red;
				rbt_rightRotate(tree, x->parent->parent);
			}
		}
		else
		{
			y = x->parent->parent->left;
			if (y->color == red)
			{
				x->parent->color = black;
				y->color = black;
				x->parent->parent->color = red;
				x = x->parent->parent;
			}
			else
			{
				if (x == x->parent->left)
				{
					x = x->parent;
					rbt_rightRotate(tree, x);
				}
				x->parent->color = black;
				x->parent->parent->color = red;
				rbt_leftRotate(tree, x->parent->parent);
			}
		}
	}
	tree->root->color = black;
	return newNode;
};
RedBlackNode* rbt_minimum(RedBlackTree *tree, RedBlackNode* x)
{
	while (x->left != tree->nil)
	{
		x = x->left;
	}
	return x;
};
RedBlackNode* rbt_maximum(RedBlackTree *tree, RedBlackNode* x)
{
	while (x->right != tree->nil)
	{
		x = x->right;
	}
	return x;
};
RedBlackNode* rbt_predecessor(RedBlackTree *tree, RedBlackNode* x)
{
	RedBlackNode* y;
	if (x->left != tree->nil)
	{
		return rbt_maximum(tree, x->left);
	}
	y = x->parent;
	while (y != tree->nil && x == y->left)
	{
		x = y;
		y = y->parent;
	}
	return y;
};
RedBlackNode* rbt_successor(RedBlackTree *tree, RedBlackNode* x)
{
	RedBlackNode* y;
	if (x->right != tree->nil)
	{
		return rbt_minimum(tree, x->right);
	}
	y = x->parent;
	while (y != tree->nil && x == y->right)
	{
		x = y;
		y = y->parent;
	}
	return y;
};
void rbt_deleteNodeFixup(RedBlackTree *tree, RedBlackNode* x)
{
	RedBlackNode* w;
	while (x != tree->root && x->color == black)
	{
		if (x == x->parent->left)
		{
			w = x->parent->right;
			if (w->color == red)
			{
				w->color = black;
				x->parent->color = red;
				rbt_leftRotate(tree, x->parent);
				w = x->parent->right;
			}
			if (w->left->color == black && w->right->color == black)
			{
				w->color = red;
				x = x->parent;
			}
			else
			{
				if (w->right->color == black)
				{
					w->left->color = black;
					w->color = red;
					rbt_rightRotate(tree, w);
					w = x->parent->right;
				}
				w->color = x->parent->color;
				x->parent->color = black;
				w->right->color = black;
				rbt_leftRotate(tree, x->parent);
				x = tree->root;
			}
		}
		else
		{
			w = x->parent->left;
			if (w->color == red)
			{
				w->color = black;
				x->parent->color = red;
				rbt_rightRotate(tree, x->parent);
				w = x->parent->left;
			}
			if (w->right->color == black && w->left->color == black)
			{
				w->color = red;
				x = x->parent;
			}
			else
			{
				if (w->left->color == black)
				{
					w->right->color = black;
					w->color = red;
					rbt_leftRotate(tree, w);
					w = x->parent->left;
				}
				w->color = x->parent->color;
				x->parent->color = black;
				w->left->color = black;
				rbt_rightRotate(tree, x->parent);
				x = tree->root;
			}
		}
	}
	x->color = black;
};
RedBlackNode* rbt_deleteNode(RedBlackTree* tree, RedBlackNode* z)
{
	RedBlackNode* x;
	RedBlackNode* y;
	if (z == tree->nil) {
		return tree->nil;
	}
	if (z->left == tree->nil || z->right == tree->nil)
	{
		y = z;
	}
	else
	{
		y = rbt_successor(tree, z);
	}
	if (y->left == tree->nil)
	{
		x = y->right;
	}
	else
	{
		x = y->left;
	}
	x->parent = y->parent;
	if (y->parent == tree->nil)
	{
		tree->root = x;
	}
	else
	{
		if (y == y->parent->left)
		{
			y->parent->left = x;
		}
		else
		{
			y->parent->right = x;
		}
	}
	if (y != z)
	{
		z->key = strdup(y->key);
		// copy any other fields
	}
	if (y->color == black)
	{
		rbt_deleteNodeFixup(tree, x);
	}
	free(y);
	tree->size--;
	return tree->nil;
};

RedBlackNode* rbt_push(RedBlackTree* tree, struct _ASTNode *value) {
	RedBlackNode *rbt_max = rbt_maximum(tree, tree->root);
	RedBlackNode *new_node = rbt_insert(tree, strdup(itoa__(atoi(strdup(rbt_max->key)) + 1, 10)), value);
	return new_node;
}

RedBlackNode* rbt_search_for_value(RedBlackTree *tree, struct _ASTNode *needle) {
	RedBlackNode *node = rbt_minimum(tree, tree->root);
	while (node && node != tree->nil) {
		if (node->value == needle) {
			return node;
		}
		node = rbt_successor(tree, node);
	}
	return 0;
}