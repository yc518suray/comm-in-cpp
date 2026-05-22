// header to store the QAM constellations
//
// Author: Raymond Su, raymondsu0110@gmail.com

#ifndef QAMMAP_H
#define QAMMAP_H

// REAL -> row, IMAG -> col

int QAM4_MAP_REAL[2] = {-1, 1};
int QAM4_MAP_IMAG[2] = {1, -1};

int QAM16_MAP_REAL[4] =
{
	-3, -1, 3, 1
};
int QAM16_MAP_IMAG[4] =
{
	3, 1, -3, -1
};

int QAM64_MAP_REAL[8] =
{
	-7, -5, -1, -3, 7, 5, 1, 3
};
int QAM64_MAP_IMAG[8] =
{
	7, 5, 1, 3, -7, -5, -1, -3
};

int QAM256_MAP_REAL[16] =
{
	-15, -13, -9, -11, -1, -3, -7, -5, 15, 13, 9, 11, 1, 3, 7, 5
};
int QAM256_MAP_IMAG[16] =
{
	15, 13, 9, 11, 1, 3, 7, 5, -15, -13, -9, -11, -1, -3, -7, -5
};


#endif
