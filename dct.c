#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "FastDct8.h"
#include "types.h"
#include "stringHelpers.h"

extern int zigzag_flat[64];

void dct(int idct, MCU *myMCU) {
	double inputY[8][8];
	double inputCb[8][8];
	double inputCr[8][8];
	for (int j = 0; j < 8; j++) {
		for (int i = 0; i < 8; i++) {
			int idx = i * 8 + j;
			if (idct) {
				idx = zigzag_flat[idx];
			}
			inputY[j][i] = (double)myMCU->Y[idx];
			inputCb[j][i] = (double)myMCU->Cb[idx];
			inputCr[j][i] = (double)myMCU->Cr[idx];
		}
		if (idct) {
			FastDct8_inverseTransform(inputY[j]);
			FastDct8_inverseTransform(inputCb[j]);
			FastDct8_inverseTransform(inputCr[j]);
		}
		else {
			FastDct8_transform(inputY[j]);
			FastDct8_transform(inputCb[j]);
			FastDct8_transform(inputCr[j]);
		}
	}

	double outputY[8][8];
	double outputCb[8][8];
	double outputCr[8][8];
	for (int j = 0; j < 8; j++) {
		for (int i = 0; i < 8; i++) {
			outputY[j][i] = inputY[i][j];
			outputCb[j][i] = inputCb[i][j];
			outputCr[j][i] = inputCr[i][j];
		}
		if (idct) {
			FastDct8_inverseTransform(outputY[j]);
			FastDct8_inverseTransform(outputCb[j]);
			FastDct8_inverseTransform(outputCr[j]);
		}
		else {
			FastDct8_transform(outputY[j]);
			FastDct8_transform(outputCb[j]);
			FastDct8_transform(outputCr[j]);
		}
	}

	for (int j = 0; j < 8; j++) {
		for (int i = 0; i < 8; i++) {
			int idx = j * 8 + i;
			myMCU->Y[idx] = outputY[j][i];
			myMCU->Cb[idx] = outputCb[j][i];
			myMCU->Cr[idx] = outputCr[j][i];
		}
	}
}