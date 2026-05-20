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

using namespace std;

int main()
{
    // BER plotting settings
    BERPlot plot;

    // some variables
    const int num_bits = 10000;   // number of bits per iteration
    int Niter = 10000;            // number of iterations
    int num_total_bits = 0;       // number of total simulated bits

    int N_snr = 16;
    vector<double> SNR_dB(N_snr, 0.0);
    vector<double> SNR_lin(SNR_dB.size());
    vector<double> num_error_bits(SNR_dB.size());
    double * BER = new double [SNR_dB.size()];

    for(size_t i = 0; i < SNR_dB.size(); i++)
    {
        SNR_dB[i] = double(i);
        SNR_lin[i] = pow(10.0, SNR_dB[i] / 10.0);
    }

    // random settings
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, 1);
    normal_distribution<double> normal_dist(0, 1); // for AWGN

    // random bits
    bitset<num_bits> bits;
    
    // the great loop (Niter iterations)
    vector<double> awgn(num_bits);
    vector<double> x(num_bits, 0.0);
    vector<int> y(num_bits, 0);
    for(int i = 0; i < Niter; i++)
    {
        for(int k = 0; k < num_bits; k++)
        {
            bits.set(k, dist(gen));
            awgn[k] = normal_dist(gen);
        }

        for(size_t k = 0; k < SNR_dB.size(); k++)
        {
            double xr = 0;
            int bit_error_count = 0;
            for(int r = 0; r < num_bits; r++)
            {
                // Tx (bits to symbols)
                x[r] = sqrt(SNR_lin[k]) * (2 * bits[r] - 1);
                // awgn channel
                xr = x[r] + awgn[r];
                // Rx (hard decision)
                y[r] = (xr > 0);
                bit_error_count += (y[r] == bits[r])? 0: 1;
            }
            num_error_bits[k] += bit_error_count;
            // error rate calculation
            BER[k] = num_error_bits[k] / ((double)(i + 1) * num_bits);
        }

        if(i == 0)
        {
            // add BER curves to plot (first time)
            double * snr = new double [SNR_dB.size()];
            for(int k = 0; k < SNR_dB.size(); k++) snr[k] = SNR_dB[k];
            
            // configure appearance of the curve (first time)
            CurveSettings settings;
            settings.name = "test";
            settings.LineWidth = 2.5;
            //settings.LineColor = "black";
            settings.MarkerType = 7;
            settings.MarkerSize = 1.5;
            
            plot.addData(BER, snr, SNR_dB.size(), 1, settings);
        }
        else
        {
            // update BER curves
            plot.updateData(BER, SNR_dB.size(), 1);
        }
        plot.plot("test title");

        if((i % 1000) == 0)
        {
            cout << "iteration: " << i << " / " << Niter << endl;
            //cout << "BER vector = [";
            //for(size_t k = 0; k < SNR_dB.size(); k++)
            //{
            //    cout << BER[k];
            //    if(k < SNR_dB.size() - 1) cout << ", ";
            //    else cout << "]" << endl;
            //}
        }
    }

    return 0;
}
