// header file of the transmitter module
//
// Author: Raymond Su, raymondsu0110@gmail.com

#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#define NUM_BITS 10000

#include<string>
#include<bitset>
#include<random>

using namespace std;


/* ============ struct definitions =============*/
struct TxRxSettings
{
	/* 1. general settings */
	bool tx_mode = true;			// true -> Tx, false -> Rx
	int N_antenna = 1;				// number of Tx/Rx antennas

	/* 2. modulation settings */
	int mod_type = 0;				// 0 -> BPSK
									// 1 -> QPSK
									// 2 -> 8PSK
									// 3 -> 16QAM
									// 4 -> 64QAM
									// 5 -> 256QAM
	bool differential = false;		// true -> differential encoding

	/* 3. other settings */
	string name;					// name of this Tx/Rx node
};

/* ============ class declarations ============ */
class Transmitter
{
private:
	mt19937 gen;						// random generator
	TxRxSettings settings;				// basic settings
	bitset<NUM_BITS> Tx_bits;			// raw data bits
	vector<vector<double>> Tx_qamsym;	// mapped constellation points
	vector<vector<double>> Tx_qamsym_p;	// with specified power

public:
	Transmitter(mt19937 & rand_gen, TxRxSettings set);
	~Transmitter();
	void generate_bits();
	void channel_coding();
	void modulation(double Eave);
	void generate_waveform();

	// for testing
	bitset<NUM_BITS> get_bits();
	vector<vector<double>> get_symbols();
};


#endif
