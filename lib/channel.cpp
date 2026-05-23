// cpp file for the channel module
//
// Author: Raymond Su, raymondsu0110@gmail.com

#include"channel.h"

using namespace std;


/* ======== momber function definitions ======== */
Channel::Channel(mt19937 & rand_gen, ChannelSettings set)
{
	// constructor
	
	gen = rand_gen;
	settings = set;
	TDL_model.resize(settings.length, vector<double>(2, 0));
}

Channel::~Channel()
{
	// destructor
}

void Channel::convolution(Transmitter & tx)
{
	// linear convolution with Tx waveforms
	
	if(settings.length == 1)
	{
		// ideal channel (no delay)
		TDL_model[0][0] = 1; // real part
		TDL_model[0][1] = 0; // imag part
		Rx_sym = tx.Tx_qamsym_p;
	}
	else
	{
		// static channel (with delays)
	}
}

void Channel::awgn()
{
	// add AWGN at the receiver
	
	normal_distribution<double> randn(0.0, sqrt(0.5));
	
	int L = Rx_sym.size();
	for(int i = 0; i < L; i++)
	{
		Rx_sym[i][0] += randn(gen);
		Rx_sym[i][1] += randn(gen);
	}
}

/* ================ for testing ================ */
vector<vector<double>> Channel::get_received()
{
	return Rx_sym;
}
