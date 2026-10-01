// header file of the channel module
//
// Author: Raymond Su, raymondsu0110@gmail.com

#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <cmath>
#include <complex>
#include <random>
#include <vector>

#include "transmitter.h"

using namespace std;


/* ============= struct definitions ============ */
struct ChannelSettings
{
	int Npath = 1;						// number of channel paths
	int type = 0;						// 0 -> ideal channel (AWGN)
										// 1 -> frequency-selective
										// 2 -> doubly-selective
	bool isDelayFractional = false;		// indicate if delays are fractional
	double delay_resolution = 0.0;		// delay resolution, in ns
	double Doppler_resolution = 0.0;	// Doppler resolution, in Hz
	string name;						// name of the channel
};

/* ============= class definitions ============= */
class Channel
{
private:
	mt19937 gen;						// random generator
	ChannelSettings settings;			// basic channel settings
	ComplexVec Rx_y;					// received time-domain samples at the receiver
	ComplexVec Tx_x_prev;				// trailing part of Tx_x of the previous frame
	ComplexVec Tx_x_total;				// Tx_x_total = [Tx_x_prev, Tx_x]
	ComplexVec TDL_model;				// tapped-delay line model
	vector<double> Doppler;				// normalized Doppler shifts of the channel
	vector<double> frac_Delay;			// normalized fractional delay shifts of the channel
	vector<int> int_Delay;				// normalized integer delay shifts of the channel

public:
	Channel(mt19937 & rand_gen, ChannelSettings set);
	~Channel();
	void generation(const double * delays, const double * PDP, const double * dopplers);
	void convolution(Transmitter & tx, int Nit, int NN);
	void awgn();

	// for debugging
	ComplexVec get_received();
	ComplexVec get_TDL_model();

	// friend class
	friend class Receiver;
	friend class OFDM_Receiver;
	friend class AFDM_Receiver;
	friend class OTFS_Receiver;
};


#endif
