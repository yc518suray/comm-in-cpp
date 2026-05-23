// header file of the channel module
//
// Author: Raymond Su, raymondsu0110@gmail.com

#ifndef CHANNEL_H
#define CHANNEL_H

#include<string>
#include<random>
#include<cmath>

#include"transmitter.h"

using namespace std;


/* ============= struct definitions ============ */
struct ChannelSettings
{
	int length = 1;						// channel length
	string name;						// name of the channel
};

/* ============= class definitions ============= */
class Channel
{
private:
	mt19937 gen;						// random generator
	ChannelSettings settings;			// basic channel settings
	vector<vector<double>> TDL_model;	// tapped-delay line model
	vector<vector<double>> Rx_sym;		// received symbols at the Rx

public:
	Channel(mt19937 & rand_gen, ChannelSettings set);
	~Channel();
	void convolution(Transmitter & tx);
	void awgn();

	// for testing
	vector<vector<double>> get_received();

	// friend class
	friend class Receiver;
};


#endif
