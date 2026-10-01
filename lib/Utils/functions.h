// header file of some useful functions
//
// Author: Raymond Su, raymondsu0110@gmail.com

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <complex>
#include <vector>

using namespace std;


// about QAM calculations
extern double QAM_ave_energy_factor(int qamsize);
extern double QAM_theo_BER(double snr, int qamsize);
extern void QAM_demapping(vector<complex<double>> & v, int qamsize);

// about channel generation
extern void generate_doppler_shifts(mt19937 & gen, double * arr, double fmax, int P);

// about basic signal processing
extern double vec_norm(vector<complex<double>> x);

// about program I/O
extern void save_ber_data();


#endif
