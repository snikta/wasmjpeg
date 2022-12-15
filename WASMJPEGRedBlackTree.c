#include <stdlib.h>
#include "./WASMJPEGRedBlackTree.h"
#include "./mathHelpers.h"
#include "./stringHelpers.h"

/*WASMJPEGASTNode *CreateASTPointNode(double x, double y) {
	WASMJPEGASTNode *retval = malloc(sizeof(WASMJPEGASTNode));
	retval->ASTXValue = x;
	retval->ASTYValue = y;
	return retval;
}

WASMJPEGASTNode *CreateASTNumberNode(int num) {
	WASMJPEGASTNode *retval = malloc(sizeof(WASMJPEGASTNode));
	retval->ASTNumberValue = num;
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
}*/

WASMJPEGASTNode *WASMJPEGCreateASTStringNode(char *str) {
	WASMJPEGASTNode *retval = malloc(sizeof(WASMJPEGASTNode));
	retval->ASTStringValue = str;
	return retval;
}

WASMJPEGRedBlackTree* wasmjpeg_rbt_create() {
	WASMJPEGRedBlackTree* tree = (WASMJPEGRedBlackTree *) malloc(sizeof(WASMJPEGRedBlackTree));
	WASMJPEGRedBlackNode* nil = (WASMJPEGRedBlackNode *) malloc(sizeof(WASMJPEGRedBlackNode));
	tree->nil = nil;
	tree->nil->key = strdup("-1");
	tree->nil->left = nil;
	tree->nil->right = nil;
	tree->root = nil;
	tree->size = 0;
	return tree;
}

WASMJPEGRedBlackNode* wasmjpeg_rbt_search(WASMJPEGRedBlackTree *tree, char *key, int closest)
{
	int dKey = atoi(key);
	WASMJPEGRedBlackNode* x = tree->root;
	WASMJPEGRedBlackNode* lastGood = x;
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
WASMJPEGRedBlackNode* wasmjpeg_rbt_closest(WASMJPEGRedBlackTree *tree, char *key)
{
	return wasmjpeg_rbt_search(tree, key, 1);
};
void wasmjpeg_rbt_leftRotate(WASMJPEGRedBlackTree *tree, WASMJPEGRedBlackNode *x)
{
	WASMJPEGRedBlackNode* y = x->right;
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
void wasmjpeg_rbt_rightRotate(WASMJPEGRedBlackTree *tree, WASMJPEGRedBlackNode *x)
{
	WASMJPEGRedBlackNode* y = x->left;
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
WASMJPEGRedBlackNode* wasmjpeg_rbt_binaryInsert(WASMJPEGRedBlackTree *tree, char *newKey, struct WASMJPEGASTNode *value)
{
	WASMJPEGRedBlackNode* z = (WASMJPEGRedBlackNode *) malloc(sizeof(WASMJPEGRedBlackNode)); // free later?
	z->color = wasm_jpeg_black;
	z->key = strdup(newKey);
	z->left = tree->nil;
	z->right = tree->nil;
	z->parent = tree->nil;
	WASMJPEGRedBlackNode* y = tree->nil;
	WASMJPEGRedBlackNode* x = tree->root;
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
WASMJPEGRedBlackNode* wasmjpeg_rbt_insert(WASMJPEGRedBlackTree *tree, char *newKey, struct WASMJPEGASTNode *value)
{
	WASMJPEGRedBlackNode *match = wasmjpeg_rbt_search(tree, newKey, 0);
	if (match != tree->nil)
	{
		match->value = value;
		return match;
	}
	WASMJPEGRedBlackNode* x = wasmjpeg_rbt_binaryInsert(tree, newKey, value);
	WASMJPEGRedBlackNode* newNode = x;
	WASMJPEGRedBlackNode* y;
	x->color = wasm_jpeg_red;
	while (x != tree->root && x->parent->color == wasm_jpeg_red)
	{
		if (x->parent == x->parent->parent->left)
		{
			y = x->parent->parent->right;
			if (y->color == wasm_jpeg_red)
			{
				x->parent->color = wasm_jpeg_black;
				y->color = wasm_jpeg_black;
				x->parent->parent->color = wasm_jpeg_red;
				x = x->parent->parent;
			}
			else
			{
				if (x == x->parent->right)
				{
					x = x->parent;
					wasmjpeg_rbt_leftRotate(tree, x);
				}
				x->parent->color = wasm_jpeg_black;
				x->parent->parent->color = wasm_jpeg_red;
				wasmjpeg_rbt_rightRotate(tree, x->parent->parent);
			}
		}
		else
		{
			y = x->parent->parent->left;
			if (y->color == wasm_jpeg_red)
			{
				x->parent->color = wasm_jpeg_black;
				y->color = wasm_jpeg_black;
				x->parent->parent->color = wasm_jpeg_red;
				x = x->parent->parent;
			}
			else
			{
				if (x == x->parent->left)
				{
					x = x->parent;
					wasmjpeg_rbt_rightRotate(tree, x);
				}
				x->parent->color = wasm_jpeg_black;
				x->parent->parent->color = wasm_jpeg_red;
				wasmjpeg_rbt_leftRotate(tree, x->parent->parent);
			}
		}
	}
	tree->root->color = wasm_jpeg_black;
	return newNode;
};
WASMJPEGRedBlackNode* wasmjpeg_rbt_minimum(WASMJPEGRedBlackTree *tree, WASMJPEGRedBlackNode* x)
{
	while (x->left != tree->nil)
	{
		x = x->left;
	}
	return x;
};
WASMJPEGRedBlackNode* wasmjpeg_rbt_maximum(WASMJPEGRedBlackTree *tree, WASMJPEGRedBlackNode* x)
{
	while (x->right != tree->nil)
	{
		x = x->right;
	}
	return x;
};
WASMJPEGRedBlackNode* wasmjpeg_rbt_predecessor(WASMJPEGRedBlackTree *tree, WASMJPEGRedBlackNode* x)
{
	WASMJPEGRedBlackNode* y;
	if (x->left != tree->nil)
	{
		return wasmjpeg_rbt_maximum(tree, x->left);
	}
	y = x->parent;
	while (y != tree->nil && x == y->left)
	{
		x = y;
		y = y->parent;
	}
	return y;
};
WASMJPEGRedBlackNode* wasmjpeg_rbt_successor(WASMJPEGRedBlackTree *tree, WASMJPEGRedBlackNode* x)
{
	WASMJPEGRedBlackNode* y;
	if (x->right != tree->nil)
	{
		return wasmjpeg_rbt_minimum(tree, x->right);
	}
	y = x->parent;
	while (y != tree->nil && x == y->right)
	{
		x = y;
		y = y->parent;
	}
	return y;
};
void wasmjpeg_rbt_deleteNodeFixup(WASMJPEGRedBlackTree *tree, WASMJPEGRedBlackNode* x)
{
	WASMJPEGRedBlackNode* w;
	while (x != tree->root && x->color == wasm_jpeg_black)
	{
		if (x == x->parent->left)
		{
			w = x->parent->right;
			if (w->color == wasm_jpeg_red)
			{
				w->color = wasm_jpeg_black;
				x->parent->color = wasm_jpeg_red;
				wasmjpeg_rbt_leftRotate(tree, x->parent);
				w = x->parent->right;
			}
			if (w->left->color == wasm_jpeg_black && w->right->color == wasm_jpeg_black)
			{
				w->color = wasm_jpeg_red;
				x = x->parent;
			}
			else
			{
				if (w->right->color == wasm_jpeg_black)
				{
					w->left->color = wasm_jpeg_black;
					w->color = wasm_jpeg_red;
					wasmjpeg_rbt_rightRotate(tree, w);
					w = x->parent->right;
				}
				w->color = x->parent->color;
				x->parent->color = wasm_jpeg_black;
				w->right->color = wasm_jpeg_black;
				wasmjpeg_rbt_leftRotate(tree, x->parent);
				x = tree->root;
			}
		}
		else
		{
			w = x->parent->left;
			if (w->color == wasm_jpeg_red)
			{
				w->color = wasm_jpeg_black;
				x->parent->color = wasm_jpeg_red;
				wasmjpeg_rbt_rightRotate(tree, x->parent);
				w = x->parent->left;
			}
			if (w->right->color == wasm_jpeg_black && w->left->color == wasm_jpeg_black)
			{
				w->color = wasm_jpeg_red;
				x = x->parent;
			}
			else
			{
				if (w->left->color == wasm_jpeg_black)
				{
					w->right->color = wasm_jpeg_black;
					w->color = wasm_jpeg_red;
					wasmjpeg_rbt_leftRotate(tree, w);
					w = x->parent->left;
				}
				w->color = x->parent->color;
				x->parent->color = wasm_jpeg_black;
				w->left->color = wasm_jpeg_black;
				wasmjpeg_rbt_rightRotate(tree, x->parent);
				x = tree->root;
			}
		}
	}
	x->color = wasm_jpeg_black;
};
WASMJPEGRedBlackNode* wasmjpeg_rbt_deleteNode(WASMJPEGRedBlackTree* tree, WASMJPEGRedBlackNode* z)
{
	WASMJPEGRedBlackNode* x;
	WASMJPEGRedBlackNode* y;
	if (z == tree->nil) {
		return tree->nil;
	}
	if (z->left == tree->nil || z->right == tree->nil)
	{
		y = z;
	}
	else
	{
		y = wasmjpeg_rbt_successor(tree, z);
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
	if (y->color == wasm_jpeg_black)
	{
		wasmjpeg_rbt_deleteNodeFixup(tree, x);
	}
	free(y);
	tree->size--;
	return tree->nil;
};

WASMJPEGRedBlackNode* wasmjpeg_rbt_push(WASMJPEGRedBlackTree* tree, struct WASMJPEGASTNode *value) {
	WASMJPEGRedBlackNode *wasmjpeg_rbt_max = wasmjpeg_rbt_maximum(tree, tree->root);
	WASMJPEGRedBlackNode *new_node = wasmjpeg_rbt_insert(tree, strdup(itoa__(atoi(strdup(wasmjpeg_rbt_max->key)) + 1, 10)), value);
	return new_node;
}

WASMJPEGRedBlackNode* wasmjpeg_rbt_search_for_value(WASMJPEGRedBlackTree *tree, struct WASMJPEGASTNode *needle) {
	WASMJPEGRedBlackNode *node = wasmjpeg_rbt_minimum(tree, tree->root);
	while (node && node != tree->nil) {
		if (node->value == needle) {
			return node;
		}
		node = wasmjpeg_rbt_successor(tree, node);
	}
	return 0;
}