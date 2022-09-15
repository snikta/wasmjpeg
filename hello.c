#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <emscripten/emscripten.h>
#include <math.h>
#include <string.h>
#include "bit_set.h"
#include "FastDct8.h"
#include "dct.h"
#include "LinkedList.h"
#include "types.h"

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

unsigned char *buffer;
const int MACROBLOCK_SIZE = 8;
int frameWidth;
int frameHeight;

char *bytes = "";
int curBitIdx = 0;
int curByteIdx = 0;
int absBitIndex = 0;
int bitIndex = 0;
int byteIndex = 0;
int curBitSet = 0;

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
		bytes += cur;
		curBitIdx = 0;
		curByteIdx++;
		curBitSet = 0;
	}
}

char *dec2bin(int dec, int flip)
{
	int size = 1;
	int origDec = dec;
	while (pow(2, size) <= dec)
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
				max = pow(2, cat) - 1;
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

		//char *bitstring = strdup(htByCode[2][hcode]);
		char *bitstring = append_string(bitstring, additionalBits);
		for (int i = 0, len = strlen(bitstring); i < len; i++)
		{
			addBit(bitstring[i] == '1');
		}
	}
	return Y_zrl;
}

YCbCr RGB_to_YCbCr(double R, double G, double B) {
	double Y = 0.299 * R + 0.587 * G + 0.114 * B;
	double Cb = (B - Y) / (2 - 2 * 0.114) + 128;
	double Cr = (R - Y) / (2 - 2 * 0.299) + 128;

	Y -= 128.0;
	Cb -= 128.0;
	Cr -= 128.0;

	YCbCr result;
	result.Y = Y;
	result.Cb = Cb;
	result.Cr = Cr;

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
					YCbCr result = RGB_to_YCbCr(
						(unsigned int)curFrame[r + 2],
						(unsigned int)curFrame[r + 1],
						(unsigned int)curFrame[r]
					);

					myMCU.Y[idx] = result.Y;
					myMCU.Cb[idx] = result.Cb;
					myMCU.Cr[idx] = result.Cr;
				}
			}
			AppendNodeToLinkedList(oldMCUs, CreateLinkedListNodeFromMCU(myMCU));
		}
	}
	
	int mcuIdx = 0;
	for (int y = 0, height = frameHeight; y < height; y += MACROBLOCK_SIZE) {
		for (int x = 0, width = frameWidth; x < width; x += MACROBLOCK_SIZE) {
			MCU oldMCU = GetLinkedListNodeByIndex(oldMCUs, mcuIdx)->mcu;
			MCU newMCU;
			oldMCU = dct(0, oldMCU);

			int Y_zrl = 0;
			for (int i = 0; i < 64; i++) {
				int zz = zigzag_flat[i];
				newMCU.Y[zz] = oldMCU.Y[i];
			}
			for (int i = 0; i < 64; i++) {
				int Yval = round(newMCU.Y[i] / qTables[0][i]);
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
				int Cbval = round(newMCU.Cb[i] / qTables[1][i]);
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
				int Crval = round(newMCU.Cr[i] / qTables[1][i]);
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

void init() {
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
}

unsigned char *getBuffer(
	unsigned char *ptr,
	const int WIDTH,
	const int HEIGHT
) {
	const int PITCH = 4;
	frameWidth = WIDTH;
	frameHeight = HEIGHT;

	buffer = ptr;
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			int idx = y * WIDTH * PITCH + x * PITCH;
			buffer[idx] = 255;
			buffer[idx + 1] = 255;
			buffer[idx + 2] = 0;
			buffer[idx + 3] = 255;
		}
	}

	return buffer;
}