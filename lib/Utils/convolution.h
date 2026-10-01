// header file of convolutions
//
// Author: Raymond Su, raymondsu0110@gmail.com

#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include <cmath>
#include <vector>
#include <complex>

#include "../Base/transmitter.h"


extern void lin_conv(ComplexVec & output, ComplexVec & input, ComplexVec & tdl);
extern void overlap_save();
extern void overlap_add();
extern void lin_tv_conv(ComplexVec & output, ComplexVec & input,
						ComplexVec & tdl, vector<double> & dp, vector<int> & dl, int N1, int N2);


#endif
