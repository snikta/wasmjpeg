#pragma once

struct MCU {
	int Y[64];
	int Cb[64];
	int Cr[64];
} typedef MCU;

struct YCbCr {
	double Y;
	double Cb;
	double Cr;
} typedef YCbCr;

struct RGB {
	double R;
	double G;
	double B;
} typedef RGB;

struct huffmanNode {
	struct huffmanNode *left;
	struct huffmanNode *right;
	char *b;
	char *val;
} typedef huffmanNode;