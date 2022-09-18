#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <emscripten/emscripten.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "bit_set.h"
#include "LinkedList.h"
#include "RedBlackTree.h"
#include "types.h"
#include "mathHelpers.h"
#include "dct.h"
//#include "apollo.h"

//extern unsigned char bmpApollo[141056];
unsigned char bmpHuffSmall[512] = {0,0,0,255,2,2,2,255,1,1,1,255,11,11,11,255,3,3,3,255,7,7,7,255,1,1,1,255,2,2,2,255,253,253,253,255,247,247,247,255,249,249,249,255,253,253,253,255,248,248,248,255,254,254,254,255,255,255,255,255,251,251,251,255,0,0,0,255,13,13,13,255,14,14,14,255,240,240,240,255,255,255,255,255,10,10,10,255,1,1,1,255,5,5,5,255,253,253,253,255,255,255,255,255,255,255,255,255,245,245,245,255,21,21,21,255,0,0,0,255,0,0,0,255,255,255,255,255,14,14,14,255,18,18,18,255,0,0,0,255,255,255,255,255,0,0,0,255,245,245,245,255,7,7,7,255,2,2,2,255,250,250,250,255,243,243,243,255,237,237,237,255,19,19,19,255,0,0,0,255,254,254,254,255,23,23,23,255,248,248,248,255,7,7,7,255,0,0,0,255,8,8,8,255,255,255,255,255,228,228,228,255,4,4,4,255,14,14,14,255,2,2,2,255,255,255,255,255,246,246,246,255,255,255,255,255,3,3,3,255,20,20,20,255,0,0,0,255,249,249,249,255,254,254,254,255,8,8,8,255,0,0,0,255,3,3,3,255,0,0,0,255,253,253,253,255,243,243,243,255,3,3,3,255,9,9,9,255,251,251,251,255,247,247,247,255,231,231,231,255,241,241,241,255,242,242,242,255,8,8,8,255,255,255,255,255,247,247,247,255,0,0,0,255,18,18,18,255,1,1,1,255,1,1,1,255,11,11,11,255,230,230,230,255,0,0,0,255,9,9,9,255,254,254,254,255,255,255,255,255,253,253,253,255,255,255,255,255,255,255,255,255,1,1,1,255,254,254,254,255,249,249,249,255,14,14,14,255,0,0,0,255,246,246,246,255,250,250,250,255,255,255,255,255,255,255,255,255,0,0,0,255,0,0,0,255,255,255,255,255,255,255,255,255,251,251,251,255,1,1,1,255,0,0,0,255,9,9,9,255,247,247,247,255,253,253,253,255,2,2,2,255,3,3,3,255,1,1,1,255,5,5,5,255,0,0,0,255,0,0,0,255,14,14,14,255,0,0,0,255,252,252,252,255,242,242,242,255,255,255,255,255,255,255,255,255,255,255,255,255,252,252,252,255,253,253,253,255,251,251,251,255};

int zigzag[8][8] = {
	{0,1,5,6,14,15,27,28},
	{2,4,7,13,16,26,29,42},
	{3,8,12,17,25,30,41,43},
	{9,11,18,24,31,40,44,53},
	{10,19,23,32,39,45,52,54},
	{20,22,33,38,46,51,55,60},
	{21,34,37,47,50,56,59,61},
	{35,36,48,49,57,58,62,63}
};
int zigzag_flat[64] = {
	0,1,5,6,14,15,27,28,
	2,4,7,13,16,26,29,42,
	3,8,12,17,25,30,41,43,
	9,11,18,24,31,40,44,53,
	10,19,23,32,39,45,52,54,
	20,22,33,38,46,51,55,60,
	21,34,37,47,50,56,59,61,
	35,36,48,49,57,58,62,63
};

int qTables[2][64] = {
	/*{16, 11, 10, 16, 24, 40, 51, 61,
	12, 12, 14, 19, 26, 58, 60, 55,
	14, 13, 16, 24, 40, 57, 69, 56,
	14, 17, 22, 29, 51, 87, 80, 62,
	18, 22, 37, 56, 68, 109, 103, 77,
	24, 36, 55, 64, 81, 104, 113, 92,
	49, 64, 78, 87, 103, 121, 120, 101,
	72, 92, 95, 98, 112, 100, 103, 99},
	{16, 11, 10, 16, 24, 40, 51, 61,
	12, 12, 14, 19, 26, 58, 60, 55,
	14, 13, 16, 24, 40, 57, 69, 56,
	14, 17, 22, 29, 51, 87, 80, 62,
	18, 22, 37, 56, 68, 109, 103, 77,
	24, 36, 55, 64, 81, 104, 113, 92,
	49, 64, 78, 87, 103, 121, 120, 101,
	72, 92, 95, 98, 112, 100, 103, 99}*/
	{16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16},
	{16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16,
	16,16,16,16,16,16,16,16}/*
	{8,16,19,22,26,27,29,34,
	16,16,22,24,27,29,34,37,
	19,22,26,27,29,34,34,38,
	22,22,26,27,29,34,37,40,
	22,26,27,29,32,35,40,48,
	26,27,29,32,35,40,48,58,
	26,27,29,34,38,46,56,69,
	27,29,35,38,46,56,69,83},
	{8,16,19,22,26,27,29,34,
	16,16,22,24,27,29,34,37,
	19,22,26,27,29,34,34,38,
	22,22,26,27,29,34,37,40,
	22,26,27,29,32,35,40,48,
	26,27,29,32,35,40,48,58,
	26,27,29,34,38,46,56,69,
	27,29,35,38,46,56,69,83}*//*
	{16, 11, 10, 16, 24, 40, 51, 61,
	12, 12, 14, 19, 26, 58, 60, 55,
	14, 13, 16, 24, 40, 57, 69, 56,
	14, 17, 22, 29, 51, 87, 80, 62,
	18, 22, 37, 56, 68, 109, 103, 77,
	24, 36, 55, 64, 81, 104, 113, 92,
	49, 64, 78, 87, 103, 121, 120, 101,
	72, 92, 95, 98, 112, 100, 103, 99}, // recommended (fixed) lum table
	{17, 18, 24, 47, 99, 99, 99, 99,
	18, 21, 26, 66, 99, 99, 99, 99,
	24, 26, 56, 99, 99, 99, 99, 99,
	47, 66, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99}*/
};

LinkedList *r_htLumDC = 0;
LinkedList *r_htChrDC = 0;
LinkedList *r_htLumAC = 0;
LinkedList *r_htChrAC = 0;
LinkedList *htByCode = 0;

unsigned char *buffer;
const int MACROBLOCK_SIZE = 8;
int frameWidth = 48;
int frameHeight = 24;

char *bytes = "";
int curBitIdx = 0;
int curByteIdx = 0;
int absBitIndex = 0;
int absBitCount = 0;
int bitIndex = 0;
int byteIndex = 0;
int curBitSet = 0;

int signedDC[12] = {
	0,
	-1,
	-3,
	-7,
	-15,
	-31,
	-63,
	-127,
	-255,
	-511,
	-1023,
	-2047
};

char *append_char_to_string(char *lhs, char rhs) {
	int len_lhs = strlen(lhs);
	char *retval = (char *) malloc(sizeof(char) * (len_lhs + 2));
	strcpy(retval, lhs);
	retval[len_lhs] = rhs;
	retval[len_lhs+1] = '\0';
	free(lhs);
	return retval;
}

char *append_string(char *lhs, char *rhs) {
	char *retval = (char *) malloc(sizeof(char) * (strlen(lhs) + strlen(rhs) + 2));
	strcpy(retval, lhs);
	strcat(retval, rhs);
    free(lhs);
	return retval;
}

char getBit() {
	if (bitIndex == 8)
	{
		bitIndex = 0;
		byteIndex++;
	}
	if (absBitIndex >= absBitCount)
	{
		return 0;
	}
	char bit = (bytes[byteIndex] & (1 << (7 - bitIndex))) != 0 ? '1' : '0';
	bitIndex++;
	absBitIndex++;
	return bit;
}

void addBit(int bit)
{
	if (bit)
	{
		curBitSet |= (1 << (7 - curBitIdx));
	}
	curBitIdx++;
	if (curBitIdx == 8)
	{
		char cur = curBitSet;
		bytes = append_char_to_string(bytes, cur);
		curBitIdx = 0;
		curByteIdx++;
		curBitSet = 0;
	}
	absBitCount++;
}

char *dec2bin(int dec, int flip)
{
	int size = 1;
	int origDec = dec;
	while ((int)pow((double)2, (double)size) <= dec)
	{
		size++;
	}
	int remainder, i = 0;
	bit_set bsOut;
	bsOut = bit_set_init(bsOut, size);
	do
	{
		remainder = dec % 2;
		dec /= 2;
		if (remainder)
		{
			bsOut = bit_set_set(bsOut, i);
		}
		i++;
	} while (dec && dec != 1);
	if (dec)
	{
		bsOut = bit_set_set(bsOut, size - 1);
	}

	if (flip)
	{
		bsOut = bit_set_invert(bsOut);
	}

	return bit_set_to_string(bsOut);
}

huffmanNode *huffNil = 0;
huffmanNode *ht = 0;
huffmanNode* searchHuffmanTree(char *needle, huffmanNode* huffRoot)
{
	huffmanNode* node = huffRoot;
	int i, len;
	for (i = 0, len = strlen(needle); i < len; i++)
	{
		if (needle[i] == '1' && node->right != huffNil) {
			node = node->right;
		}
		else if (node->left != huffNil) {
			node = node->left;
		}
		if (node == huffNil) {
			break;
		}
	}
	if (i == len && node && strcmp(node->b, needle) == 0) {
		return node;
	}
	else {
		return huffNil;
	}
}

int bin2dec(char *bin)
{
	int i;
	int n = strlen(bin);
	int col = 1;
	int dec = 0;
	for (i = n - 1; i >= 0; i--)
	{
		dec += col * (bin[i] == '1' ? 1 : 0);
		col *= 2;
	}
	return dec;
}

int hexToDec(char hex[2]) {
	int retval = 0;
	if (hex[0] >= 'A' && hex[0] <= 'F') {
		retval = (hex[0] - 'A' + 10) * 16;
	} else {
		retval = (hex[0] - '0') * 16;
	}
	if (hex[1] >= 'A' && hex[1] <= 'F') {
		retval += (hex[1] - 'A' + 10);
	} else {
		retval += hex[1] - '0';
	}
	return retval;
}

int huffmanEncode(int Yval, int i, int Y_zrl) {
	if (Yval == 0) {
		Y_zrl++;
	}
	else {
		int hcode = 0, bsZRL = 0, bsCat = 0;
		if (Y_zrl) {
			int zrl_left = Y_zrl;
			while (zrl_left >= 15) {
				zrl_left -= 15;
			}

			bsZRL = zrl_left;

			if (bsZRL & (1 << 0)) hcode |= 1 << 4;
			if (bsZRL & (1 << 1)) hcode |= 1 << 5;
			if (bsZRL & (1 << 2)) hcode |= 1 << 6;
			if (bsZRL & (1 << 3)) hcode |= 1 << 7;

			int cat = 0, max = 0;
			while (max < Yval) {
				cat++;
				max = (int)pow((double)2, (double)cat) - 1;
			}

			bsCat = cat;
			if (bsCat & (1 << 0)) hcode |= 1 << 0;
			if (bsCat & (1 << 1)) hcode |= 1 << 1;
			if (bsCat & (1 << 2)) hcode |= 1 << 2;
			if (bsCat & (1 << 3)) hcode |= 1 << 3;

			Y_zrl = 0;
		}
		char *additionalBits = 0;

		if (Yval == 0)
		{
			additionalBits = strdup("");
		}
		else if (Yval > 0)
		{
			additionalBits = dec2bin(Yval, 0);
		}
		else
		{
			additionalBits = dec2bin(-Yval, 1);
		}

		int bsLenTaken = strlen(additionalBits);

		if (bsLenTaken & (1 << 0)) hcode |= (1 << 0);
		else hcode &= ~(1 << 0);

		if (bsLenTaken & (1 << 1)) hcode |= (1 << 1);
		else hcode &= ~(1 << 1);

		if (bsLenTaken & (1 << 2)) hcode |= (1 << 2);
		else hcode &= ~(1 << 2);

		if (bsLenTaken & (1 << 3)) hcode |= (1 << 3);
		else hcode &= ~(1 << 3);

		char *bitstring = strdup(rbt_search(GetLinkedListNodeByIndex(htByCode, 2)->rbt, itoa__(hcode, 10), 0)->value->ASTStringValue);
		bitstring = append_string(bitstring, strdup(additionalBits));
		for (int i = 0, len = strlen(bitstring); i < len; i++)
		{
			addBit(bitstring[i] == '1');
		}
	}
	return Y_zrl;
}

RedBlackTree *rbtCosines = 0;

char *getDCTKey(int y, int x, int Y, int X) {
	char *key = strdup("");
	key = append_string(key, itoa__(y, 10));
	key = append_char_to_string(key, '_');
	key = append_string(key, itoa__(x, 10));
	key = append_char_to_string(key, '_');
	key = append_string(key, itoa__(Y, 10));
	key = append_char_to_string(key, '_');
	key = append_string(key, itoa__(X, 10));
	return key;
}

RGB *YCbCr_to_RGB(double Y, double Cb, double Cr) {
	double R;
	double G;
	double B;

	//Y += 128.0;
	//Cb += 128.0;
	//Cr += 128.0;
	R = Y + 1.402 * ((double)Cr - 128.0);
	G = Y - 0.34414 * ((double)Cb - 128.0) - 0.71414 * ((double)Cr - 128.0);
	B = Y + 1.772 * ((double)Cb - 128.0);
	R = (int)clamp(round((int)R), 0.0, 255.0);
	G = (int)clamp(round((int)G), 0.0, 255.0);
	B = (int)clamp(round((int)B), 0.0, 255.0);

	RGB *retval = malloc(sizeof(RGB));
	retval->R = R;
	retval->G = G;
	retval->B = B;

	return retval;
}

YCbCr *RGB_to_YCbCr(double R, double G, double B) {
	double Y = 0.299 * R + 0.587 * G + 0.114 * B;
	double Cb = (B - Y) / (2 - 2 * 0.114) + 128;
	double Cr = (R - Y) / (2 - 2 * 0.299) + 128;

	//Y -= 128.0;
	//Cb -= 128.0;
	//Cr -= 128.0;

	YCbCr *result = malloc(sizeof(YCbCr));
	result->Y = Y;
	result->Cb = Cb;
	result->Cr = Cr;

	return result;
}

void encodeImage(unsigned char *curFrame) {
	LinkedList *oldMCUs = CreateLinkedList();
	
	for (int y = 0, height = frameHeight; y < height; y += MACROBLOCK_SIZE) {
		for (int x = 0, width = frameWidth; x < width; x += MACROBLOCK_SIZE) {
			MCU myMCU;
			for (int v = 0; v < MACROBLOCK_SIZE; v++) {
				for (int u = 0; u < MACROBLOCK_SIZE; u++) {
					int r = (y + v) * width * 4 + (x + u) * 4;
					int idx = v * MACROBLOCK_SIZE + u;
					YCbCr *result = RGB_to_YCbCr(
						(double)(unsigned int)curFrame[r],
						(double)(unsigned int)curFrame[r + 1],
						(double)(unsigned int)curFrame[r + 2]
					);

					myMCU.Y[idx] = (curFrame[r]+curFrame[r + 1]+curFrame[r + 2])/3;
					myMCU.Cb[idx] = (curFrame[r]+curFrame[r + 1]+curFrame[r + 2])/3;
					myMCU.Cr[idx] = (curFrame[r]+curFrame[r + 1]+curFrame[r + 2])/3;

					free(result);
				}
			}
			/*int matrix[64] = {52, 55, 61, 66, 70, 61, 64, 73,
	63, 59, 55, 90, 109, 85, 69, 72,
	62, 59, 68, 113, 144, 104, 66, 73,
	63, 58, 71, 122, 154, 106, 70, 69,
	67, 61, 68, 104, 126, 88, 68, 70,
	79, 65, 60, 70, 77, 68, 58, 75,
	85, 71, 64, 59, 55, 61, 65, 83,
	87, 79, 69, 68, 65, 76, 78, 94};*/
			for (int k = 0; k < 64; k++) {
				myMCU.Y[k] = myMCU.Y[k]-128;
				myMCU.Cb[k] = myMCU.Cb[k]-128;
				myMCU.Cr[k] = myMCU.Cr[k]-128;
			}
			AppendNodeToLinkedList(oldMCUs, CreateLinkedListNodeFromMCU(myMCU));
		}
	}
	
	int mcuIdx = 0;
	for (int y = 0, height = frameHeight; y < height; y += MACROBLOCK_SIZE) {
		for (int x = 0, width = frameWidth; x < width; x += MACROBLOCK_SIZE) {
			MCU oldMCU = GetLinkedListNodeByIndex(oldMCUs, mcuIdx)->mcu;
			MCU newMCU;
			MCU ptrMCU = dct(0, oldMCU);
			for (int i = 0; i < 64; i++) {
				oldMCU.Y[i] = ptrMCU.Y[i];
				oldMCU.Cb[i] = ptrMCU.Cb[i];
				oldMCU.Cr[i] = ptrMCU.Cr[i];
			}

			int Y_zrl = 0;
			for (int i = 0; i < 64; i++) {
				int zz = zigzag_flat[i];
				newMCU.Y[zz] = oldMCU.Y[i];
			}
			for (int i = 0; i < 64; i++) {
				int Yval = (int)newMCU.Y[i];//(int)round((double)newMCU.Y[i] / (double)qTables[0][i]);
				Yval/=16;
				Y_zrl = huffmanEncode(Yval, i, Y_zrl);
			}
			addBit(1);
			addBit(0);
			addBit(1);
			addBit(0);

			Y_zrl = 0;
			for (int i = 0; i < 64; i++) {
				int zz = zigzag_flat[i];
				newMCU.Cb[zz] = oldMCU.Cb[i];
			}
			for (int i = 0; i < 64; i++) {
				int Cbval = (int)newMCU.Y[i];//(int)round((double)newMCU.Cb[i] / (double)qTables[1][i]);
				Cbval/=16;
				Y_zrl = huffmanEncode(Cbval, i, Y_zrl);
			}
			addBit(1);
			addBit(0);
			addBit(1);
			addBit(0);

			Y_zrl = 0;
			for (int i = 0; i < 64; i++) {
				int zz = zigzag_flat[i];
				newMCU.Cr[zz] = oldMCU.Cr[i];
			}
			for (int i = 0; i < 64; i++) {
				int Crval = (int)newMCU.Y[i];//(int)round((double)newMCU.Cr[i] / (double)qTables[1][i]);
				Crval/=16;
				Y_zrl = huffmanEncode(Crval, i, Y_zrl);
			}
			addBit(1);
			addBit(0);
			addBit(1);
			addBit(0);

			mcuIdx++;
		}
	}
}

huffmanNode* generateHuffmanTree(LinkedList *h, int sum, int htIdx) {
	huffmanNode* node = (huffmanNode *) malloc(sizeof(huffmanNode));
	node->b = strdup("");
	node->val = strdup("");
	node->left = 0;
	node->right = 0;
	huffmanNode* root = node;
	LinkedList *row = CreateLinkedList();
	
	int count = 0;
	AppendNodeToLinkedList(row, CreateLinkedListNodeFromHuffmanNode(node));

	LinkedList *_node = 0;
	RedBlackTree *myByCode = GetLinkedListNodeByIndex(htByCode, htIdx)->rbt;

	int hIndex = 0;
	int hLen = GetLinkedListSize(h);
	do {
		if (count >= sum) {
			return root;
		}
		_node = GetLinkedListNodeByIndex(h, hIndex)->lst;
		int _nodeIndex = 0;
		int _nodeLen = GetLinkedListSize(_node);
		LinkedList *oldRow = CopyHuffmanLinkedList(row);
		if (_nodeLen)
		{
			do {
				char *__node = GetLinkedListNodeByIndex(_node, _nodeIndex)->str;
				huffmanNode* node_ = oldRow->firstNode->huffNode;
				oldRow->firstNode = oldRow->firstNode->nextNode;
				node_->val = strdup(__node);
				rbt_insert(myByCode, itoa__(hexToDec(__node), 10), CreateASTStringNode(strdup(node_->b)));
				count++;
				if (count >= sum) {
					return root;
				}
			} while (++_nodeIndex < _nodeLen);
		}
		ClearLinkedList(row);
		if (GetLinkedListSize(oldRow)) {
			huffmanNode* r = 0;
			int rIndex = 0;
			int rLen = GetLinkedListSize(oldRow);
			do {
				r = GetLinkedListNodeByIndex(oldRow, rIndex)->huffNode;
				r->left = (huffmanNode *) malloc(sizeof(huffmanNode));
				r->right = (huffmanNode *) malloc(sizeof(huffmanNode));
				r->left->val = strdup("");
				r->right->val = strdup("");
				r->left->left = 0;
				r->left->right = 0;
				r->right->left = 0;
				r->right->right = 0;
				r->left->b = append_string(strdup(r->b), strdup("0"));
				r->right->b = append_string(strdup(r->b), strdup("1"));
				AppendNodeToLinkedList(row, CreateLinkedListNodeFromHuffmanNode(r->left));
				AppendNodeToLinkedList(row, CreateLinkedListNodeFromHuffmanNode(r->right));
			} while (++rIndex < rLen);
		}
	} while (++hIndex < hLen);

	return root;
}

void finishMCU(int *comp, huffmanNode* huffRoot, int qTblIdx) {
	char *bin = strdup("");
	char bit;
	int AC_counter = 0;
	for (; AC_counter < 64; AC_counter++) {
		comp[AC_counter] = 0;
	}
	AC_counter = 0;
	while ((bit = getBit())) {
		bin = append_char_to_string(bin, bit);
		huffmanNode* hResult = searchHuffmanTree(bin, huffRoot);
		if (hResult && strcmp(hResult->val, "") != 0) {
			if (strcmp(hResult->val, "00") == 0) {
				return;
			}
			int val = hexToDec(hResult->val);
			int nr_of_prev_0 = val >> 4;
			int n = val & 0xF;
			int i = 0;
			char *addBits = strdup("");

			while (i < n && (bit = getBit())) {
				addBits = append_char_to_string(addBits, bit);
				i++;
			}

			int nbDec = bin2dec(addBits);
			int result = (nbDec < (int)pow((double)2, (double)(n - 1)) ? signedDC[n] : 0) + nbDec;
			
			bin = strdup("");
			AC_counter += nr_of_prev_0;

			if (AC_counter < 64) {
				comp[AC_counter] = result * qTables[qTblIdx][AC_counter];
			}
			AC_counter++;
		}
	}
}

unsigned char *decodedFrame = 0;
void decodeImage(huffmanNode *ht) {
	int XX = 0;
	int YY = 0;
	int width = frameWidth;
	int height = frameHeight;
	int mcuIdx = 0;
	int byteLen = strlen(bytes);
	decodedFrame = (unsigned char *) malloc(sizeof(unsigned char) * width * height * 4);
	while (byteIndex < byteLen)
	{
		MCU newMCU;
		int *newY = malloc(sizeof(int)*64);
		int *newCb = malloc(sizeof(int)*64);
		int *newCr = malloc(sizeof(int)*64);
		for (int i = 0; i < 64; i++) {
			newY[i] = 0;
			newCb[i] = 0;
			newCr[i] = 0;
		}
		finishMCU(newY, ht, 0);
		finishMCU(newCb, ht, 1);
		finishMCU(newCr, ht, 1);
		for (int i = 0; i < 64; i++) {
			newMCU.Y[i] = newY[i];
			newMCU.Cb[i] = newCb[i];
			newMCU.Cr[i] = newCr[i];
		}
		free(newY);
		free(newCb);
		free(newCr);
		
		MCU ptrMCU = dct(1, newMCU);
		for (int i = 0; i < 64; i++) {
			newMCU.Y[i] = ptrMCU.Y[i]+128;
			newMCU.Cb[i] = ptrMCU.Cb[i]+128;
			newMCU.Cr[i] = ptrMCU.Cr[i]+128;
		}
		for (int j = 0; j < 8; j++) {
			char *str = strdup("");
			for (int i = 0; i < 8; i++) {
				int num = newMCU.Y[j * 8 + i];
				char *str2 = strdup("");
				if (num < 0) {
					str2 = append_char_to_string(str2, '-');
				}
				str2 = append_string(str2, strdup(itoa__(abs(num), 10)));
				str2 = append_char_to_string(str2, ' ');
				str = append_string(str, str2);
				free(str2);
			}
			printf("{%s}\n", str);
			free(str);
		}

		for (int y = 0; y < MACROBLOCK_SIZE; y++) {
			for (int x = 0; x < MACROBLOCK_SIZE; x++) {
				int idx = y * MACROBLOCK_SIZE + x;
				RGB *RGB_val = (RGB *) malloc(sizeof(RGB));/* = YCbCr_to_RGB(
					(double)newMCU.Y[idx],
					(double)newMCU.Cb[idx],
					(double)newMCU.Cr[idx]
				);*/
				RGB_val->R = (double)newMCU.Y[idx];
				RGB_val->G = (double)newMCU.Y[idx];
				RGB_val->B = (double)newMCU.Y[idx];
				int r = (YY + y) * width * 4 + (XX + x) * 4;
				decodedFrame[r] = (unsigned char)(int)RGB_val->R;
				decodedFrame[r + 1] = (unsigned char)(int)RGB_val->G;
				decodedFrame[r + 2] = (unsigned char)(int)RGB_val->B;
				decodedFrame[r + 3] = 255;
				//printf("%f..%f..%f\n", RGB_val->R, RGB_val->G, RGB_val->B);
				free(RGB_val);
			}
		}
		XX += MACROBLOCK_SIZE;
		if (XX >= width) {
			XX = 0;
			YY += MACROBLOCK_SIZE;
		}
		if (YY >= height) {
			break;
		}
		mcuIdx++;
	}
}

void init() {
	int matrix[64] = {52, 55, 61, 66, 70, 61, 64, 73,
	63, 59, 55, 90, 109, 85, 69, 72,
	62, 59, 68, 113, 144, 104, 66, 73,
	63, 58, 71, 122, 154, 106, 70, 69,
	67, 61, 68, 104, 126, 88, 68, 70,
	79, 65, 60, 70, 77, 68, 58, 75,
	85, 71, 64, 59, 55, 61, 65, 83,
	87, 79, 69, 68, 65, 76, 78, 94};

	MCU m;
	for (int i = 0; i < 64; i++) {
		m.Y[i] = matrix[i]-128;
		m.Cb[i] = matrix[i]-128;
		m.Cr[i] = matrix[i]-128;
	}

	MCU mm = dct(0, m);
	for (int j = 0; j < 8; j++) {
		char *str = strdup("");
		for (int i = 0; i < 8; i++) {
			int num = mm.Y[j * 8 + i];
			char *str2 = strdup("");
			if (num < 0) {
				str2 = append_char_to_string(str2, '-');
			}
			str2 = append_string(str2, strdup(itoa__(abs(num), 10)));
			str2 = append_char_to_string(str2, ' ');
			str = append_string(str, str2);
			free(str2);
		}
		printf("{%s}\n", str);
		free(str);
	}

	r_htLumDC = CreateLinkedList();
	LinkedListNode *Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htLumDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htLumDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("00"));
	AppendNodeToLinkedList(r_htLumDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("01"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("02"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("03"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("04"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("05"));
	AppendNodeToLinkedList(r_htLumDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("06"));
	AppendNodeToLinkedList(r_htLumDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("07"));
	AppendNodeToLinkedList(r_htLumDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("08"));
	AppendNodeToLinkedList(r_htLumDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("09"));
	AppendNodeToLinkedList(r_htLumDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("0A"));
	AppendNodeToLinkedList(r_htLumDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("0B"));
	AppendNodeToLinkedList(r_htLumDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htLumDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htLumDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htLumDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htLumDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htLumDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htLumDC, Node);

	r_htChrDC = CreateLinkedList();
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htChrDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htChrDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("00"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("01"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("02"));
	AppendNodeToLinkedList(r_htChrDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("03"));
	AppendNodeToLinkedList(r_htChrDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("04"));
	AppendNodeToLinkedList(r_htChrDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("05"));
	AppendNodeToLinkedList(r_htChrDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("06"));
	AppendNodeToLinkedList(r_htChrDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("07"));
	AppendNodeToLinkedList(r_htChrDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("08"));
	AppendNodeToLinkedList(r_htChrDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("09"));
	AppendNodeToLinkedList(r_htChrDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("0A"));
	AppendNodeToLinkedList(r_htChrDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("0B"));
	AppendNodeToLinkedList(r_htChrDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htChrDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htChrDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htChrDC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htChrDC, Node);

	r_htLumAC = CreateLinkedList();
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htLumAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htLumAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("01"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("02"));
	AppendNodeToLinkedList(r_htLumAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("03"));
	AppendNodeToLinkedList(r_htLumAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("00"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("04"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("11"));
	AppendNodeToLinkedList(r_htLumAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("05"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("12"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("21"));
	AppendNodeToLinkedList(r_htLumAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("31"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("41"));
	AppendNodeToLinkedList(r_htLumAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("06"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("13"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("51"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("61"));
	AppendNodeToLinkedList(r_htLumAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("07"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("22"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("71"));
	AppendNodeToLinkedList(r_htLumAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("14"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("32"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("81"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("91"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("A1"));
	AppendNodeToLinkedList(r_htLumAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("08"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("23"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("42"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("B1"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("C1"));
	AppendNodeToLinkedList(r_htLumAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("15"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("52"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("D1"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("F0"));
	AppendNodeToLinkedList(r_htLumAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("24"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("33"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("62"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("72"));
	AppendNodeToLinkedList(r_htLumAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htLumAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htLumAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("82"));
	AppendNodeToLinkedList(r_htLumAC, Node);
	
	char lastRow[][2] = {"09", "0A", "16", "17", "18", "19", "1A", "25", "26", "27", "28", "29", "2A", "34", "35", "36",
	"37", "38", "39", "3A", "43", "44", "45", "46", "47", "48", "49", "4A", "53", "54", "55", "56",
	"57", "58", "59", "5A", "63", "64", "65", "66", "67", "68", "69", "6A", "73", "74", "75", "76",
	"77", "78", "79", "7A", "83", "84", "85", "86", "87", "88", "89", "8A", "92", "93", "94", "95",
	"96", "97", "98", "99", "9A", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "B2", "B3",
	"B4", "B5", "B6", "B7", "B8", "B9", "BA", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA",
	"D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "E1", "E2", "E3", "E4", "E5", "E6", "E7",
	"E8", "E9", "EA", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA"};
	Node = CreateLinkedListNodeFromNewLinkedList();
	for (int i = 0, len = sizeof(lastRow) / sizeof(lastRow[0]); i < len; i++) {
		char str[3] = {lastRow[i][0], lastRow[i][1], '\0'};
		AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString(str));
	}
	AppendNodeToLinkedList(r_htLumAC, Node);

	r_htChrAC = CreateLinkedList();
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htChrAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(r_htChrAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("00"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("01"));
	AppendNodeToLinkedList(r_htChrAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("02"));
	AppendNodeToLinkedList(r_htChrAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("03"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("11"));
	AppendNodeToLinkedList(r_htChrAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("04"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("05"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("21"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("31"));
	AppendNodeToLinkedList(r_htChrAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("06"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("12"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("41"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("51"));
	AppendNodeToLinkedList(r_htChrAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("07"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("61"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("71"));
	AppendNodeToLinkedList(r_htChrAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("13"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("22"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("32"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("81"));
	AppendNodeToLinkedList(r_htChrAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("08"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("14"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("42"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("91"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("A1"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("B1"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("C1"));
	AppendNodeToLinkedList(r_htChrAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("09"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("23"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("33"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("52"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("F0"));
	AppendNodeToLinkedList(r_htChrAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("15"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("62"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("72"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("D1"));
	AppendNodeToLinkedList(r_htChrAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("0A"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("16"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("24"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("34"));
	AppendNodeToLinkedList(r_htChrAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("E1"));
	AppendNodeToLinkedList(r_htChrAC, Node);
	Node = CreateLinkedListNodeFromNewLinkedList();
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("25"));
	AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString("F1"));
	AppendNodeToLinkedList(r_htChrAC, Node);

	char lastRow2[][2] = {"17", "18", "19", "1A", "26", "27", "28", "29", "2A", "35", "36", "37", "38", "39", "3A", "43",
	"44", "45", "46", "47", "48", "49", "4A", "53", "54", "55", "56", "57", "58", "59", "5A", "63",
	"64", "65", "66", "67", "68", "69", "6A", "73", "74", "75", "76", "77", "78", "79", "7A", "82",
	"83", "84", "85", "86", "87", "88", "89", "8A", "92", "93", "94", "95", "96", "97", "98", "99",
	"9A", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "B2", "B3", "B4", "B5", "B6", "B7",
	"B8", "B9", "BA", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "D2", "D3", "D4", "D5",
	"D6", "D7", "D8", "D9", "DA", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "F2", "F3",
	"F4", "F5", "F6", "F7", "F8", "F9", "FA"};
	Node = CreateLinkedListNodeFromNewLinkedList();
	for (int i = 0, len = sizeof(lastRow2) / sizeof(lastRow2[0]); i < len; i++) {
		char str[3] = {lastRow2[i][0], lastRow2[i][1], '\0'};
		AppendNodeToLinkedList(Node->lst, CreateLinkedListNodeFromString(str));
	}
	AppendNodeToLinkedList(r_htChrAC, Node);

	LinkedList *huffmanTrees = CreateLinkedList();
	AppendNodeToLinkedList(huffmanTrees, CreateLinkedListNodeFromLinkedList(r_htLumDC));
	AppendNodeToLinkedList(huffmanTrees, CreateLinkedListNodeFromLinkedList(r_htChrDC));
	AppendNodeToLinkedList(huffmanTrees, CreateLinkedListNodeFromLinkedList(r_htLumAC));
	AppendNodeToLinkedList(huffmanTrees, CreateLinkedListNodeFromLinkedList(r_htChrAC));

	Node = huffmanTrees->firstNode;
	while (Node) {
		LinkedListNode *Node2 = Node->lst->firstNode;
		while (Node2) {
			LinkedListNode *Node3 = Node2->lst->firstNode;
			char *str = strdup("");
			while (Node3) {
				str = append_string(str, Node3->str);
				if (Node3->nextNode) {
					str = append_char_to_string(str, ',');
				}
				Node3 = Node3->nextNode;
			}
			printf("{%s}\n", str);
			Node2 = Node2->nextNode;
		}
		printf("...\n");
		Node = Node->nextNode;
	}

	htByCode = CreateLinkedList();
	AppendNodeToLinkedList(htByCode, CreateLinkedListNodeFromNewRedBlackTree());
	AppendNodeToLinkedList(htByCode, CreateLinkedListNodeFromNewRedBlackTree());
	AppendNodeToLinkedList(htByCode, CreateLinkedListNodeFromNewRedBlackTree());
	AppendNodeToLinkedList(htByCode, CreateLinkedListNodeFromNewRedBlackTree());
	
	huffNil = (huffmanNode *) malloc(sizeof(huffmanNode));
	huffNil->left = huffNil;
	huffNil->right = huffNil;
	huffNil->b = strdup("");
	huffNil->val = strdup("");

	ht = generateHuffmanTree(r_htLumAC, 162, 0);
	ht = generateHuffmanTree(r_htLumAC, 162, 1);
	ht = generateHuffmanTree(r_htLumAC, 162, 2);
	ht = generateHuffmanTree(r_htLumAC, 162, 3);

	bytes = strdup("");
	encodeImage(bmpHuffSmall);
	printf("Encoding complete!\n");
	bitIndex = 0;
	byteIndex = 0;
	decodeImage(ht);
	printf("Decoding complete!\n");
}

unsigned char *getBuffer(
	unsigned char *ptr,
	const int WIDTH,
	const int HEIGHT
) {
	const int PITCH = 4;

	buffer = ptr;
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			int idx = y * WIDTH * PITCH + x * PITCH;
			if (x < frameWidth && y < frameHeight) {
				if (decodedFrame != 0) {
					buffer[idx] = decodedFrame[y * frameWidth * 4 + x * 4];
					buffer[idx + 1] = decodedFrame[y * frameWidth * 4 + x * 4 + 1];
					buffer[idx + 2] = decodedFrame[y * frameWidth * 4 + x * 4 + 2];
					buffer[idx + 3] = decodedFrame[y * frameWidth * 4 + x * 4 + 3];
				}
			} else {
				buffer[idx] = 255;
				buffer[idx + 1] = 255;
				buffer[idx + 2] = 255;
				buffer[idx + 3] = 255;
			}
		}
	}

	return buffer;
}