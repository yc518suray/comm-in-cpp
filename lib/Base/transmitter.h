// header file of the transmitter module
//
// Author: Raymond Su, raymondsu0110@gmail.com

#ifndef TRANSMITTER_H
#define TRANSMITTER_H

// number of bits per iteration/block/frame
#define NUM_BITS 16400

#include <string>
#include <bitset>
#include <complex>
#include <vector>
#include <random>


/* =============== typedef section ============== */
typedef std::vector<std::complex<double>> ComplexVec;

/* ============= struct definitions ============= */
struct TxRxSettings
{
	/* 1. general settings */
	bool tx_mode = true;			// true -> Tx, false -> Rx
	int N_antenna = 1;				// number of Tx/Rx antennas, for MIMO scenario

	/* 2. channel coding settings */
	int code_type = 0;				// 0 -> LDPC
									// 1 -> convolutional code

	/* 3. modulation settings */
	int qam_type = 0;				// 0 -> BPSK
									// 1 -> QPSK
									// 2 -> 8PSK
									// 3 -> 16QAM
									// 4 -> 64QAM
									// 5 -> 256QAM
	int mod_type = 0;				// 0 -> raw QAM symbols
									// 1 -> OFDM
									// 2 -> OTFS
									// 3 -> AFDM
	bool differential = false;		// true -> differential encoding

	/* 4. other settings */
	std::string name;				// name of this Tx/Rx node
};

/* ============= class declarations ============= */
class Transmitter
{
private:
	std::mt19937 gen;					// random generator
	TxRxSettings settings;				// basic Tx settings
	
	std::bitset<NUM_BITS> Tx_bits;		// raw data bits
	ComplexVec Tx_sym;					// mapped constellation points
	ComplexVec Tx_x;					// transmitted time-domain samlpes

	int num_qamsym = 0;					// number of generated QAM symbols
	int num_modsym = 0;					// number of modulation symbols
										// example: num_modsym equals N x M for OTFS
	double sqrt_Eave_prev = 1.0;

public:
	Transmitter(std::mt19937 & rand_gen, TxRxSettings set);
	~Transmitter();
	void generate_bits();
	void channel_coding();
	void mapping(double Eave);
	void modulation(); // trivial

	// for debugging
	std::bitset<NUM_BITS> get_bits();
	ComplexVec get_symbols();

	// friend class
	friend class Channel;
	friend class Receiver;

	// friend derived class
	friend class OFDM_Transmitter;
	friend class OTFS_Transmitter;
	friend class AFDM_Transmitter;
};


#endif
