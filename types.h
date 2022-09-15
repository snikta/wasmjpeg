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