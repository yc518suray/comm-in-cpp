// cpp file of the transmitter module
//
// Author: Raymond Su, raymondsu0110@gmail.com

#include<string>
#include<bitset>
#include<vector>
#include<random>
#include<cmath>

#include"qammap.h"
#include"transmitter.h"

using namespace std;


/* =============== tables & values ============== */
int QamSize[6] = {-1, 4, -8, 16, 64, 256};

/* ====== non-member function declarations ====== */
vector<int> bit2qamnum(const bitset<NUM_BITS> & b, int qam_size);
void qammod(vector<int> qn, vector<vector<double>> & sym, int qam_size);

/* ========= member function definitions ======== */
Transmitter::Transmitter(mt19937 & rand_gen, TxRxSettings set)
{
	// constructor
	
	gen = rand_gen;
	settings = set;
}

Transmitter::~Transmitter()
{
	// destructor
}

void Transmitter::generate_bits()
{
	// generate raw data bits
	
	uniform_int_distribution<int> binary_rand(0, 1);
	for(int k = 0; k < NUM_BITS; k++) Tx_bits.set(k, binary_rand(gen));

	// clear Tx_qamsym and Tx_qamsym_p
	Tx_qamsym.resize(0);
	Tx_qamsym_p.resize(0);
}

void Transmitter::channel_coding()
{
	// channel coding (e.g. convolutional code)
	
	return;
}

void Transmitter::modulation(double Eave)
{
	// baseband modulation
	// Eave -> symbol average energy

	int modtp = settings.mod_type;

	if(QamSize[modtp] > 0)
	{
		if(Tx_qamsym.size() == 0)
		{
			vector<int> qam_num = bit2qamnum(Tx_bits, QamSize[modtp]);
			Tx_qamsym.resize(qam_num.size(), vector<double>(2, 0));
			Tx_qamsym_p.resize(qam_num.size(), vector<double>(2, 0));
			
			qammod(qam_num, Tx_qamsym, QamSize[modtp]);
		}
		for(int i = 0; i < Tx_qamsym.size(); i++)
		{
			Tx_qamsym_p[i][0] = sqrt(Eave) * Tx_qamsym[i][0];
			Tx_qamsym_p[i][1] = sqrt(Eave) * Tx_qamsym[i][1];
		}
	}
	else
	{
		if(modtp == 0)
		{
			// BPSK -> only the first entry (real part) is used
			if(Tx_qamsym.size() == 0)
			{
				Tx_qamsym.resize(NUM_BITS, vector<double>(2, 0));
				Tx_qamsym_p.resize(NUM_BITS, vector<double>(2, 0));
				for(int i = 0; i < NUM_BITS; i++)
				{
					Tx_qamsym[i][0] = 2.0 * Tx_bits[i] - 1.0;
				}
			}

			// energy with specified SNR (in linear scale)
			for(int i = 0; i < NUM_BITS; i++)
			{
				Tx_qamsym_p[i][0] = sqrt(Eave) * Tx_qamsym[i][0];
			}
		}
		else if(modtp == 2)
		{
			// 8PSK -> use two entries (I and Q part)
			
			return;
		}
		else;
	}
}

void Transmitter::generate_waveform()
{
	// generate time domain waveform
	
	return;
}

/* ====== non-member function definitions ======= */
vector<int> bit2qamnum(const bitset<NUM_BITS> & b, int qam_size)
{
	// convert a set of bits to a single number in QAM
	int N = static_cast<int>(log2(static_cast<double>(qam_size)));
	vector<int> qn(NUM_BITS / N, -1);
	bitset<32> x;
	
	for(int i = 0; i < NUM_BITS / N; i++)
	{
		for(int j = 0; j < N; j++) x[j] = b[i * N + j];
		qn[i] = static_cast<int>(x.to_ulong());
	}

	return qn;
}

void qammod(vector<int> qn, vector<vector<double>> & sym, int qam_size)
{
	// convert a single number to a QAM constellation point (gray coding)
	
	int * map_real;
	int * map_imag;
	switch(qam_size)
	{
		case 4:
			map_real = QAM4_MAP_REAL;
			map_imag = QAM4_MAP_IMAG;
		case 16:
			map_real = QAM16_MAP_REAL;
			map_imag = QAM16_MAP_IMAG;
		case 64:
			map_real = QAM64_MAP_REAL;
			map_imag = QAM64_MAP_IMAG;
		case 256:
			map_real = QAM256_MAP_REAL;
			map_imag = QAM256_MAP_IMAG;
		default:
			;
	}

	int L = qn.size();
	int M = static_cast<int>(sqrt(static_cast<double>(qam_size)));
	for(int i = 0; i < L; i++)
	{
		sym[i][0] = map_real[i / M]; // row -> real part
		sym[i][0] = map_imag[i % M]; // col -> imag part
	}
}

/* ================ for testing ================= */
bitset<NUM_BITS> Transmitter::get_bits()
{
	return Tx_bits;
}

vector<vector<double>> Transmitter::get_symbols()
{
	return Tx_qamsym_p;
}
