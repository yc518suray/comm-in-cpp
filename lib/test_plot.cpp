// This is a simple program to test the libraries.
// This program simulates BPSK in AWGN channel.
//
// Author: Raymond Su, raymondsu0110@gmail.com

#include<string>
#include<iostream>
#include<random>
#include<cmath>
#include<vector>
#include<bitset>

#include"transmitter.h"
#include"channel.h"
#include"receiver.h"
#include"plot_BER.h"

using namespace std;


int main()
{
    BERPlot plot;
    int Niter = 10000;

	/* ===== BER settings ===== */
    int N_SNR = 16;
    vector<double> SNR_dB(N_SNR, 0.0);
    vector<double> SNR_lin(SNR_dB.size());
    vector<double> num_error_bits(SNR_dB.size());
    double * BER = new double [SNR_dB.size()];
	double * theo_BER = new double [SNR_dB.size()];

    for(size_t i = 0; i < SNR_dB.size(); i++)
    {
        SNR_dB[i] = double(i);
        SNR_lin[i] = pow(10.0, SNR_dB[i] / 10.0);
    }
	for(size_t i = 0; i < SNR_dB.size(); i++)
	{
		theo_BER[i] = 0.5 * erfc(sqrt(0.5 * SNR_lin[i]));
	}


	/* === random settings ==== */
    random_device rd;
    mt19937 gen(rd());

	/* ========== Tx ========== */
    TxRxSettings tx_settings;
	tx_settings.tx_mode = true;
	tx_settings.mod_type = 0;
	tx_settings.name = "test_transmitter";
	
	// create Tx
	Transmitter tx(gen, tx_settings);
	tx.generate_bits();

	/* ======== Channel ======= */
	ChannelSettings chnl_settings;
	chnl_settings.name = "test_channel";

	// create channel
	Channel channel(gen, chnl_settings);

	/* ========== Rx ========== */
	TxRxSettings rx_settings;
	rx_settings.tx_mode = false;
	rx_settings.mod_type = 0;
	rx_settings.name = "test_receiver";

	// create Rx
	Receiver rx(rx_settings);


    /* ====== simulation ====== */
    for(int i = 0; i < Niter; i++)
    {
        for(size_t k = 0; k < SNR_dB.size(); k++)
        {
			// modulation
			tx.modulation(0.5 * SNR_lin[k]);

			// awgn channel
			channel.convolution(tx);
			channel.awgn();

			// demodulation
			rx.demodulation(channel, 0.5 * SNR_lin[k]);
			rx.error_count(tx);

            // error rate calculation
			num_error_bits[k] += rx.get_num_error_bits();
            BER[k] = num_error_bits[k] / ((double)(i + 1) * NUM_BITS);
        }


		// presentation
        if(i == 0)
        {
            // add BER curves to plot (first time)
            double * snr = new double [SNR_dB.size()];
            for(int k = 0; k < SNR_dB.size(); k++) snr[k] = SNR_dB[k];
            
            // configure appearance of the first curve
            CurveSettings settings;
            settings.name = "BPSK-sim";
            settings.LineWidth = 2.5;
            settings.LineColor = "black";
            settings.MarkerType = 6;
            settings.MarkerSize = 1.5;
            
            plot.addData(BER, snr, SNR_dB.size(), 1, settings);

			// configure apperance of the second curve
            settings.name = "BPSK-theo";
            settings.LineWidth = 2.5;
            settings.LineColor = "red";
            settings.MarkerType = 7;
            settings.MarkerSize = 1.5;

			plot.addData(theo_BER, snr, SNR_dB.size(), 2, settings);
        }
        else if((i % 5) == 0)
        {
            // update BER curves
            plot.updateData(BER, SNR_dB.size(), 1);
        	plot.plot("BER of BPSK in AWGN channel");
        }
		else;

        if((i % 1000) == 0)
        {
            cout << "iteration: " << i << " / " << Niter << endl;
        }
    }

	cout << "simulation complete" << endl;
    return 0;
}
