#include <stdio.h>
#include <stdlib.h>
#include "FastDct8.h"
#include "types.h"

extern int zigzag_flat[64];

MCU dct(int idct, MCU myMCU) {
	double inputY[8][8];
	double inputCb[8][8];
	double inputCr[8][8];
	for (int j = 0; j < 8; j++) {
		for (int i = 0; i < 8; i++) {
			int idx = i * 8 + j;
			if (idct) {
				//idx = zigzag_flat[idx];
			}
			inputY[j][i] = (double)myMCU.Y[idx];
			inputCb[j][i] = (double)myMCU.Cb[idx];
			inputCr[j][i] = (double)myMCU.Cr[idx];
		}
		double *vectorY = malloc(sizeof(double) * 8);
		double *vectorCb = malloc(sizeof(double) * 8);
		double *vectorCr = malloc(sizeof(double) * 8);
		for (int x = 0; x < 8; x++) {
			vectorY[x] = inputY[j][x];
			vectorCb[x] = inputCb[j][x];
			vectorCr[x] = inputCr[j][x];
		}
		if (idct) {
			FastDct8_inverseTransform(vectorY);
			FastDct8_inverseTransform(vectorCb);
			FastDct8_inverseTransform(vectorCr);
		}
		else {
			FastDct8_transform(vectorY);
			FastDct8_transform(vectorCb);
			FastDct8_transform(vectorCr);
		}
		for (int x = 0; x < 8; x++) {
			inputY[j][x] = vectorY[x];
			inputCb[j][x] = vectorCb[x];
			inputCr[j][x] = vectorCr[x];
		}
		free(vectorY);
		free(vectorCb);
		free(vectorCr);
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
		double *vectorY = malloc(sizeof(double) * 8);
		double *vectorCb = malloc(sizeof(double) * 8);
		double *vectorCr = malloc(sizeof(double) * 8);
		for (int x = 0; x < 8; x++) {
			vectorY[x] = outputY[j][x];
			vectorCb[x] = outputCb[j][x];
			vectorCr[x] = outputCr[j][x];
		}
		if (idct) {
			FastDct8_inverseTransform(vectorY);
			FastDct8_inverseTransform(vectorCb);
			FastDct8_inverseTransform(vectorCr);
		}
		else {
			FastDct8_transform(vectorY);
			FastDct8_transform(vectorCb);
			FastDct8_transform(vectorCr);
		}
		for (int x = 0; x < 8; x++) {
			outputY[j][x] = vectorY[x];
			outputCb[j][x] = vectorCb[x];
			outputCr[j][x] = vectorCr[x];
		}
		free(vectorY);
		free(vectorCb);
		free(vectorCr);
	}

	for (int j = 0; j < 8; j++) {
		for (int i = 0; i < 8; i++) {
			int idx = j * 8 + i;
			myMCU.Y[idx] = (int)outputY[j][i];
			myMCU.Cb[idx] = (int)outputCb[j][i];
			myMCU.Cr[idx] = (int)outputCr[j][i];
			printf("(%d, %d, %d)\n", myMCU.Y[idx], myMCU.Cb[idx], myMCU.Cr[idx]);
		}
	}
	
	return myMCU;
}