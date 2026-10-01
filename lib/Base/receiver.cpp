// cpp file of the receiver module
//
// Author: Raymond Su, raymondsu0110@gmail.com

#include <cmath>
#include <vector>
#include <limits>

#include "../Mod/qammap.h"
#include "receiver.h"

using namespace std;


/* ====== non-member function declarations ====== */
void qamnum2bit(bitset<NUM_BITS> & b, const vector<int> & qn, int qam_size);
void qamdemod(vector<int> & qn, const ComplexVec & sym, int qam_size);

/* ======== member function definitions ========= */
Receiver::Receiver(TxRxSettings set)
{
	// constructor
	
	settings = set;
}

Receiver::~Receiver()
{
	// destructor
}

void Receiver::demodulation(Channel & channel)
{
	// base demodulation function
	
	if(channel.settings.type == 0)
	{
		// ideal channel (AWGN)

		Rx_sym = channel.Rx_y;
	}
	else if(channel.settings.type == 1)
	{
		// frequency-selective channel
		// implement equalizers e.g. LMS filter
	}
	else
	{
		// doubly-selective channel
		// for single carrier transmission, this is not implemented
	}
}

void Receiver::demapping(double Eave)
{
	// demap constellation points to bits
	// Eave -> average energy of modulation symbols
	
	int qamtype = settings.qam_type;
	int qamsize = QamSize[qamtype];
	double sqrt_Eave = sqrt(Eave);
	if(num_modsym == 0) num_modsym = Rx_sym.size();

	if(QamSize[qamtype] > 0)
	{
		// QAM demapping

		int N = static_cast<int>(log2(static_cast<double>(qamsize)));
		num_rx_bits = num_modsym * N;

		for(int i = 0; i < num_modsym; i++)
		{
			Rx_sym[i] = Rx_sym[i] / sqrt_Eave;
		}

		vector<int> qam_num(num_modsym, 0);
		qamdemod(qam_num, Rx_sym, QamSize[qamtype]);
		qamnum2bit(Rx_bits, qam_num, QamSize[qamtype]);
	}
	else
	{
		if(qamtype == 0)
		{
			// BPSK demapping
			
			num_rx_bits = num_modsym;
			for(int i = 0; i < NUM_BITS; i++)
			{
				Rx_bits[i] = (Rx_sym[i].real() > 0);
			}
		}
		else if(qamtype == 2)
		{
			// 8PSK demapping
			
			num_rx_bits = num_modsym * 3;
		}
		else;
	}
}

void Receiver::error_count(Transmitter & tx)
{
	// summarize the bit error rate
	
	num_error_bits = 0;
	bitset<NUM_BITS> temp_bits;

	temp_bits = Rx_bits ^ tx.Tx_bits;
	for(int i = num_rx_bits; i < NUM_BITS; i++) temp_bits.reset(i);
	num_error_bits = temp_bits.count();
}

int Receiver::get_num_error_bits()
{
	return num_error_bits;
}

int Receiver::get_num_rx_bits()
{
	return num_rx_bits;
}

ComplexVec Receiver::get_symbols()
{
	return Rx_sym;
}

/* ====== non-member function definitions ======= */
void qamnum2bit(bitset<NUM_BITS> & b, const vector<int> & qn, int qam_size)
{
	// convert interger representations in QAM to bits
	
	int N = static_cast<int>(log2(static_cast<double>(qam_size)));
	int M = qn.size();
	bitset<16> x;

	for(int i = 0; i < M; i++)
	{
		x = qn[i];
		for(int j = 0; j < N; j++) b.set(i * N + j, x[j]);
	}
}

void qamdemod(vector<int> & qn, const ComplexVec & sym, int qam_size)
{
	// convert QAM constellation points to integers
		
	int * map_real = nullptr;
	int * map_imag = nullptr;
	switch(qam_size)
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

	int L = sym.size();
	int M = static_cast<int>(sqrt(static_cast<double>(qam_size)));
	for(int i = 0; i < L; i++)
	{
		double diff_real = numeric_limits<double>::max();
		double diff_imag = numeric_limits<double>::max();
		int row_index = -1;
		int col_index = -1;
		for(int j = 0; j < M; j++)
		{
			if(abs(sym[i].real() - map_real[j]) < diff_real)
			{
				diff_real = abs(sym[i].real() - map_real[j]);
				row_index = j;
			}
			if(abs(sym[i].imag() - map_imag[j]) < diff_imag)
			{
				diff_imag = abs(sym[i].imag() - map_imag[j]);
				col_index = j;
			}
		}
		qn[i] = row_index * M + col_index;
	}
}
