// header file of the receiver module
//
// Author: Raymond Su, raymondsu0110@gmail.com

#include<string>
#include<cmath>
#include<vector>

#include"transmitter.h"
#include"channel.h"

using namespace std;


/* ============ struct definitions ============ */

/* ============ class declarations ============ */
class Receiver
{
private:
	TxRxSettings settings;
	bitset<NUM_BITS> Rx_bits;
	int num_error_bits = 0;

public:
	Receiver(TxRxSettings set);
	~Receiver();
	void demodulation(Channel & channel, double Eave);
	void error_count(Transmitter & tx);

	int get_num_error_bits();
};
