#pragma once
/*typedef struct MergedCellProperties {
    int topRowIndex;
    int leftColIndex;
    int bottomRowIndex;
    int rightColIndex;
    int rowIndex;
    int colIndex;
} MergedCellProperties;
typedef unsigned long int TScreenColor;*/
typedef struct WASMJPEGASTNode {
	int ASTNumberValue;
	char *ASTStringValue;
} WASMJPEGASTNode;
enum WASMJPEGRedBlackColor { wasm_jpeg_red, wasm_jpeg_black };
typedef struct WASMJPEGRedBlackNode
{
	int color;
	struct WASMJPEGRedBlackNode* left;
	struct WASMJPEGRedBlackNode* right;
	struct WASMJPEGRedBlackNode* parent;
	char *key;
	struct WASMJPEGASTNode *value;
	int deleted;
} WASMJPEGRedBlackNode;
typedef struct WASMJPEGRedBlackTree
{
	struct WASMJPEGRedBlackNode* nil;
	struct WASMJPEGRedBlackNode* root;
	int size;
} WASMJPEGRedBlackTree;
WASMJPEGRedBlackTree* wasmjpeg_rbt_create();
WASMJPEGRedBlackNode* wasmjpeg_rbt_closest(WASMJPEGRedBlackTree *tree, char *key);
WASMJPEGRedBlackNode* wasmjpeg_rbt_search(WASMJPEGRedBlackTree *tree, char *key, int closest);
void wasmjpeg_rbt_leftRotate(WASMJPEGRedBlackTree *tree, WASMJPEGRedBlackNode* x);
void wasmjpeg_rbt_rightRotate(WASMJPEGRedBlackTree *tree, WASMJPEGRedBlackNode* x);
WASMJPEGRedBlackNode* wasmjpeg_rbt_binaryInsert(WASMJPEGRedBlackTree *tree, char *newKey, struct WASMJPEGASTNode *value);
WASMJPEGRedBlackNode* wasmjpeg_rbt_insert(WASMJPEGRedBlackTree *tree, char *newKey, struct WASMJPEGASTNode *value);
WASMJPEGRedBlackNode* wasmjpeg_rbt_minimum(WASMJPEGRedBlackTree *tree, WASMJPEGRedBlackNode* x);
WASMJPEGRedBlackNode* wasmjpeg_rbt_maximum(WASMJPEGRedBlackTree *tree, WASMJPEGRedBlackNode* x);
WASMJPEGRedBlackNode* wasmjpeg_rbt_predecessor(WASMJPEGRedBlackTree *tree, WASMJPEGRedBlackNode* x);
WASMJPEGRedBlackNode* wasmjpeg_rbt_successor(WASMJPEGRedBlackTree *tree, WASMJPEGRedBlackNode* x);
void wasmjpeg_rbt_deleteNodeFixup(WASMJPEGRedBlackTree *tree, WASMJPEGRedBlackNode* x);
WASMJPEGRedBlackNode* wasmjpeg_rbt_deleteNode(WASMJPEGRedBlackTree *tree, WASMJPEGRedBlackNode* z);
WASMJPEGRedBlackNode* wasmjpeg_rbt_push(WASMJPEGRedBlackTree *tree, struct WASMJPEGASTNode *value);
WASMJPEGRedBlackNode* wasmjpeg_rbt_search_for_value(WASMJPEGRedBlackTree *tree, struct WASMJPEGASTNode *value);

struct WASMJPEGASTNode *WASMJPEGCreateASTStringNode(char *str);
int isNaN(char *val);
/*WASMJPEGASTNode *CreateASTPointNode(double x, double y);
WASMJPEGASTNode *CreateASTNumberNode(int num);*/
