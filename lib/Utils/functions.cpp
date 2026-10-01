// cpp file of some useful functions
//
// Author: Raymond Su, raymondsu0110@gmail.com

#include <cmath>
#include <random>
#include <vector>
#include <numeric>

#include "../Mod/qammap.h"
#include "functions.h"

using namespace std;


void QAM_demapping(vector<complex<double>> & v, int qamtype)
{
	// QAM demapper
	// v		-> input complex vector
	// qamtype	-> type number of QAM constellation

	int qamsize = QamSize[qamtype];

	int * map_real;
	int * map_imag;
	switch(qamsize)
	{
		case 4:
			map_real = QAM4_MAP_REAL;
			map_imag = QAM4_MAP_IMAG;
			break;
		case 16:
			map_real = QAM16_MAP_REAL;
			map_imag = QAM16_MAP_IMAG;
			break;
		case 64:
			map_real = QAM64_MAP_REAL;
			map_imag = QAM64_MAP_IMAG;
			break;
		case 256:
			map_real = QAM256_MAP_REAL;
			map_imag = QAM256_MAP_IMAG;
			break;
		default:
			;
	}

	int L = v.size();
	int M = static_cast<int>(sqrt(static_cast<double>(qamsize)));
	for(int i = 0; i < L; i++)
	{
		double diff_real = numeric_limits<double>::max();
		double diff_imag = numeric_limits<double>::max();
		int row_index = -1;
		int col_index = -1;
		for(int j = 0; j < M; j++)
		{
			if(abs(v[i].real() - map_real[j]) < diff_real)
			{
				diff_real = abs(v[i].real() - map_real[j]);
				row_index = j;
			}
			if(abs(v[i].imag() - map_imag[j]) < diff_imag)
			{
				diff_imag = abs(v[i].imag() - map_imag[j]);
				col_index = j;
			}
		}
		v[i].real(map_real[row_index]);
		v[i].imag(map_imag[col_index]);
	}
}

double QAM_ave_energy_factor(int qamsize)
{
	// calculate average symbol energy factor

	return 3.0 / (2 * (qamsize - 1));
}

double QAM_theo_BER(double snr, int qamsize)
{
	// calculate theoretical BER for QAM modulation

	double M = sqrt(static_cast<double>(qamsize));
	double N = log2(static_cast<double>(qamsize));
	double factor = 2 * QAM_ave_energy_factor(qamsize);

	return 2 * (M - 1) * erfc(sqrt(0.5 * factor * snr)) / M / N;
}

double vec_norm(vector<complex<double>> x)
{
	double sum = 0.0;
	for(auto element : x) sum += norm(element);

	return sqrt(sum);
}

void generate_doppler_shifts(mt19937 & gen, double * arr, double fmax, int P)
{
	// generate Doppler shifts of each path, assume Jake's spectrum
	//
	// gen			-> random number generator
	// arr			-> array of Doppler shifts
	// fmax			-> maximum Doppler frequency, in Hz
	// P			-> number of paths

	uniform_real_distribution<double> rand(0.0, 1.0);
	for(int i = 0; i < P; i++)
	{
		arr[i] = fmax * cos(2 * M_PI * rand(gen));
	}
}

void save_ber_data()
{
}
