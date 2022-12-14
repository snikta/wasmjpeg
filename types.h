#pragma once

struct MCU {
	double Y[64];
	double Cb[64];
	double Cr[64];
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