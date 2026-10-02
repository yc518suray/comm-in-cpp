// cpp file of convolutions
//
// Author: Raymond Su, raymondsu0110@gmail.com

#include "convolution.h"

using namespace std;


void lin_conv(ComplexVec & output, ComplexVec & input, ComplexVec & tdl)
{
	// direct implementation of linear convolution in LTI systems
	// output 	-> the output sequence
	// input	-> the input sequence, including the trailing part
	// 			   of the previous frame
	// tdl		-> tapped-delay line coefficients

	if(output.size() == 0) output.resize(input.size(), complex<double>(0));

	size_t N = input.size();
	size_t L = tdl.size();
	for(size_t n = L - 1; n < N; n++)
	{
		output[n - L + 1] = 0;
		for(size_t m = 0; m < L; m++)
		{
			output[n - L + 1] += tdl[m] * input[n - m];
		}
	}
}

void overlap_save()
{
	// implementation of overlap-save block convolution
}

void overlap_add()
{
	// implementation of overlap-add block convolution
}

void lin_tv_conv(ComplexVec & output, ComplexVec & input,
				 ComplexVec & tdl, vector<double> & dp, vector<int> & dl, int N1, int N2)
{
	// implementation of convolution in a linear time-variant system
	// output 	-> the output sequence
	// input	-> the input sequence, including the trailing part
	// 			   of the previous frame
	// tdl		-> coefficients of each path
	// dp		-> the fractional Doppler shifts
	// dl		-> the integer delay shifts
	// N1		-> magic number 1
	// N2		-> magic number 2
	
	if(output.size() == 0) output.resize(input.size(), complex<double>(0));

	int N = input.size();
	int P = tdl.size();
	int L = *(dl.end() - 1);
	for(int n = L; n < N; n++)
	{
		output[n - L] = 0;
		for(int m = 0; m < P; m++)
		{
			// case 1: count in N1
			//double expo = 2 * M_PI * dp[m] * (n + N1 - dl[m]) / N2;
			// case 2: NOT count in N1
			double expo = 2 * M_PI * dp[m] * (n - dl[m]) / N2;
			complex<double> expo_comp = complex<double>(0, 1) * expo;

			output[n - L] += tdl[m] * exp(expo_comp) * input[n - dl[m]];
		}
	}
}
