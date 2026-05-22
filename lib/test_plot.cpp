// This is a simple program to test the plot_ber functionality.
// This program simulates BPSK in AWGN channel.
//
// Author: Raymond Su, raymondsu0110@gmail.com

#include<string>
#include<iostream>
#include<random>
#include<cmath>
#include<vector>
#include<bitset>

#include"plot_BER.h"
#include"transmitter.h"

using namespace std;

int main()
{
    // BER plotting settings
    BERPlot plot;

    // some variables
    int Niter = 10000;            // number of iterations

    int N_snr = 16;
    vector<double> SNR_dB(N_snr, 0.0);
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

    // random settings
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<double> normal_dist(0, 1); // for AWGN

    // transmitter settings
    TxRxSettings tx_settings;
	tx_settings.name = "test_transmitter";
	Transmitter tx(gen, tx_settings);

	tx.generate_bits();
	bitset<NUM_BITS> bits = tx.get_bits();

    // the great loop (Niter iterations)
    vector<double> awgn(NUM_BITS);
    vector<int> y(NUM_BITS, 0);
    for(int i = 0; i < Niter; i++)
    {
        for(int k = 0; k < NUM_BITS; k++)
        {
            awgn[k] = normal_dist(gen);
        }

        for(size_t k = 0; k < SNR_dB.size(); k++)
        {
			tx.modulation(SNR_lin[k]);
			vector<vector<double>> x = tx.get_symbols();

            double xr = 0;
            int bit_error_count = 0;
            for(int r = 0; r < NUM_BITS; r++)
            {
                // awgn channel
                xr = x[r][0] + awgn[r];
                // Rx (hard decision)
                y[r] = (xr > 0);
                bit_error_count += (y[r] == bits[r])? 0: 1;
            }
            num_error_bits[k] += bit_error_count;
            // error rate calculation
            BER[k] = num_error_bits[k] / ((double)(i + 1) * NUM_BITS);
        }

        if(i == 0)
        {
            // add BER curves to plot (first time)
            double * snr = new double [SNR_dB.size()];
            for(int k = 0; k < SNR_dB.size(); k++) snr[k] = SNR_dB[k];
            
            // configure appearance of the first curve
            CurveSettings settings;
            settings.name = "sim";
            settings.LineWidth = 2.5;
            settings.LineColor = "black";
            settings.MarkerType = 6;
            settings.MarkerSize = 1.5;
            
            plot.addData(BER, snr, SNR_dB.size(), 1, settings);

			// configure apperance of the second curve
            settings.name = "theo";
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

    return 0;
}
