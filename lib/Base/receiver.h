// header file of the receiver module
//
// Author: Raymond Su, raymondsu0110@gmail.com

#ifndef RECEIVER_H
#define RECEIVER_H

#include <bitset>

#include "transmitter.h"
#include "channel.h"


/* ============ struct definitions ============ */

/* ============ class declarations ============ */
class Receiver
{
private:
	TxRxSettings settings;
	std::bitset<NUM_BITS> Rx_bits;
	ComplexVec Rx_sym;

	int num_modsym = 0;
	int num_rx_bits = 0;
	int num_error_bits = 0;

public:
	Receiver(TxRxSettings set);
	~Receiver();
	void demodulation(Channel & channel);
	void demapping(double Eave);
	void error_count(Transmitter & tx);
	int get_num_rx_bits();

	// for debugging
	int get_num_error_bits();
	ComplexVec get_symbols();

	// friend class
	friend class OFDM_Receiver;
	friend class OTFS_Receiver;
	friend class AFDM_Receiver;
};


#endif
