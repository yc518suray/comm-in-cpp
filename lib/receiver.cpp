// cpp file for the receiver module
//
// Author: Raymond Su, raymondsu0110@gmail.com

#include<string>
#include<cmath>
#include<vector>

#include"channel.h"
#include"receiver.h"

using namespace std;


/* ====== non-member function declarations ====== */

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

void Receiver::demodulation(Channel & channel, double Eave)
{
	// demodulation and decision
	
	int modtp = settings.mod_type;
	if(QamSize[modtp] > 0)
	{
		// QAM demodulation
	}
	else
	{
		if(modtp == 0)
		{
			// BPSK demodulation
			
			for(int i = 0; i < NUM_BITS; i++)
			{
				Rx_bits[i] = (channel.Rx_sym[i][0] > 0);
			}
		}
		else if(modtp == 2)
		{
			// 8PSK demodulation
		}
		else;
	}
}

void Receiver::error_count(Transmitter & tx)
{
	// summarize the error rate performance
	
	num_error_bits = 0;
	for(int i = 0; i < NUM_BITS; i++)
	{
		num_error_bits += (Rx_bits[i] == tx.Tx_bits[i])? 0: 1;
	}
}

int Receiver::get_num_error_bits()
{
	return num_error_bits;
}

/* ====== non-member function definitions ======= */

