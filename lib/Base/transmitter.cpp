// cpp file of the transmitter module
//
// Author: Raymond Su, raymondsu0110@gmail.com

#include <cmath>

#include "../Mod/qammap.h"
#include "transmitter.h"

using namespace std;


/* ====== non-member function declarations ====== */
vector<int> bit2qamnum(const bitset<NUM_BITS> & b, int qam_size);
void qammod(vector<int> qn, ComplexVec & sym, int qam_size);

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

	// clear Tx_sym from the last frame/iteration
	Tx_sym.resize(0);

	// reset sqrt_Eave_prev
	sqrt_Eave_prev = 1.0;
}

void Transmitter::channel_coding()
{
	// channel coding
	
	return;
}

void Transmitter::mapping(double Eave)
{
	// map bits to constellation points
	// Eave -> average energy of modulation symbols
	// note: SNR is specified in the modulation domain (e.g. DFT domain for OFDM)

	int qamtype = settings.qam_type;
	double sqrt_Eave = sqrt(Eave);

	if(QamSize[qamtype] > 0)
	{
		// QAM constellation

		if(Tx_sym.size() == 0)
		{
			vector<int> qam_num = bit2qamnum(Tx_bits, QamSize[qamtype]);
			num_qamsym = qam_num.size();
			if(num_modsym == 0) num_modsym = num_qamsym;

			Tx_sym.resize(num_qamsym, complex<double>(0));
			qammod(qam_num, Tx_sym, QamSize[qamtype]);
		}

		double factor = sqrt_Eave / sqrt_Eave_prev;
		for(int i = 0; i < num_qamsym; i++)
		{
			Tx_sym[i] = factor * Tx_sym[i];
		}

		sqrt_Eave_prev = sqrt_Eave;
	}
	else
	{
		if(qamtype == 0)
		{
			// BPSK -> symbol is real
			
			num_modsym = NUM_BITS;

			if(Tx_sym.size() == 0)
			{
				Tx_sym.resize(NUM_BITS, complex<double>(0));
				for(int i = 0; i < NUM_BITS; i++)
				{
					Tx_sym[i].real(2.0 * Tx_bits[i] - 1.0);
				}
			}

			double factor = sqrt_Eave / sqrt_Eave_prev;
			for(int i = 0; i < NUM_BITS; i++)
			{
				Tx_sym[i] = factor * Tx_sym[i];
			}

			sqrt_Eave_prev = sqrt_Eave;
		}
		else if(qamtype == 2)
		{
			// 8PSK -> not implemented yet
			
			return;
		}
		else;
	}
}

/* ====== non-member function definitions ======= */
vector<int> bit2qamnum(const bitset<NUM_BITS> & b, int qam_size)
{
	// convert sets of bits to integer representations in QAM
	
	int N = static_cast<int>(log2(static_cast<double>(qam_size)));
	vector<int> qn(NUM_BITS / N, -1);
	bitset<16> x;
	
	for(int i = 0; i < NUM_BITS / N; i++)
	{
		for(int j = 0; j < N; j++) x[j] = b[i * N + j];
		qn[i] = static_cast<int>(x.to_ulong());
	}

	return qn;
}

void qammod(vector<int> qn, ComplexVec & sym, int qam_size)
{
	// convert integers to QAM constellation points
	
	int * map_real;
	int * map_imag;
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

	int L = qn.size();
	int M = static_cast<int>(sqrt(static_cast<double>(qam_size)));
	for(int i = 0; i < L; i++)
	{
		sym[i].real(map_real[qn[i] / M]); // row -> real part
		sym[i].imag(map_imag[qn[i] % M]); // col -> imag part
	}
}

void Transmitter::modulation()
{
	Tx_x = Tx_sym; // note that this is deep copy
}

/* ================ for debugging ================= */
bitset<NUM_BITS> Transmitter::get_bits()
{
	return Tx_bits;
}

ComplexVec Transmitter::get_symbols()
{
	return Tx_sym;
}
