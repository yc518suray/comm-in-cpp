// This is a simple program to test the libraries.
//
// Note: SNR is defined as ratio of average
// modulation symbol energy to complex AWGN PSD
//
// Author: Raymond Su, raymondsu0110@gmail.com

#include <string>
#include <iostream>
#include <iomanip>
#include <random>
#include <cmath>
#include <vector>
#include <bitset>

#include "Base/transmitter.h"
#include "Base/channel.h"
#include "Base/receiver.h"
#include "Mod/qammap.h"
#include "Utils/channel_model.h"
#include "Utils/functions.h"
#include "Utils/plot_BER.h"

#include "Mod/OTFS.h"

#define LIGHTSPEED 299792458

using namespace std;


int main()
{
	/* === general settings === */
	BERPlot plot;
    int Niter = 10000;
	int Qam = 3;		// 0 -> BPSK
						// 1 -> QPSK
						// 2 -> 8PSK
						// 3 -> 16QAM
						// 4 -> 64QAM
						// 5 -> 256QAM

	/* === system settings ==== */
	double f_carrier = 4.0e9;		// carrier frequency = 4 GHz
	double speed = 500.0;			// relative speed of Rx to Tx, in km/hr
	double delta_f = 15000.0;		// subcarrier spacing, in Hz
	double T_block = 1 / delta_f;	// block duration for OFDM and OTFS, in sec
	

	/* ==== OFDM settings ===== */
	//int Nfft = 64;
	//int Nblock = 64;
	//double CP_ratio = 0.125;

	/* ==== OTFS settings ===== */
	int Ndelay = 64; // delay axis
	int Mblock = 64; // Doppler axis
	int Npadding = 4;

	/* === channel settings === */
	double max_Doppler = (speed / 3.6) * (f_carrier / LIGHTSPEED);
	double Doppler_vec [EVA_channel_size] = {0.0};

	/* ===== BER settings ===== */
    int N_SNR = 16;
	double SNR_lower_bound = 5;
	double SNR_upper_bound = 20;
    vector<double> SNR_dB(N_SNR, 0.0);
    vector<double> SNR_lin(N_SNR);
    vector<double> num_error_bits(N_SNR);
    double * BER = new double [N_SNR];
	double * theo_BER = new double [N_SNR];

    for(int i = 0; i < N_SNR; i++)
    {
        SNR_dB[i] = SNR_lower_bound + i;
        SNR_lin[i] = pow(10.0, 0.1 * SNR_dB[i]);
    }

	//double energy_factor = 0.5; // for BPSK
	double energy_factor = QAM_ave_energy_factor(QamSize[Qam]); // for QAM
	//energy_factor = Nfft * energy_factor;		// for OFDM
	energy_factor = Mblock * energy_factor;		// for OTFS
	for(size_t i = 0; i < N_SNR; i++)
	{
		theo_BER[i] = QAM_theo_BER(SNR_lin[i], QamSize[Qam]); // for QAM
		//theo_BER[i] = 0.5 * erfc(sqrt(0.5 * SNR_lin[i])); // for BPSK
	}

	/* === random settings ==== */
    random_device rd;
    mt19937 gen(rd());


	/* ========== Tx ========== */
    TxRxSettings tx_settings;
	tx_settings.tx_mode = true;
	tx_settings.qam_type = Qam;
	tx_settings.name = "test_transmitter";
	
	//OFDM_TxRxSettings ofdm_settings;
	//ofdm_settings.Nsubc = Nfft;
	//ofdm_settings.Nblock = Nblock;
	//ofdm_settings.CP_ratio = CP_ratio;

	OTFS_TxRxSettings otfs_settings;
	otfs_settings.N_delay = Ndelay;
	otfs_settings.M_doppler = Mblock;
	otfs_settings.Npadding = Npadding;
	otfs_settings.frame_format = 0; // ZP-OTFS

	// create Tx
	//OFDM_Transmitter tx(gen, tx_settings, ofdm_settings);
	OTFS_Transmitter tx(gen, tx_settings, otfs_settings);

	/* ======== Channel ======= */
	ChannelSettings chnl_settings;
	chnl_settings.type = 2;
	chnl_settings.Npath = EVA_channel_size;
	//chnl_settings.delay_resolution = T_block * 1e9 / Nfft;		// for OFDM
	//chnl_settings.Doppler_resolution = delta_f;					// for OFDM
	chnl_settings.delay_resolution = T_block * 1e9 / Ndelay;	// for OTFS
	chnl_settings.Doppler_resolution = delta_f / Mblock;		// for OTFS
	chnl_settings.name = "test_channel";

	// create channel
	Channel channel(gen, chnl_settings);
	
	/* ========== Rx ========== */
	TxRxSettings rx_settings;
	rx_settings.tx_mode = false;
	rx_settings.qam_type = Qam;
	rx_settings.name = "test_receiver";

	// create Rx
	//OFDM_Receiver rx(rx_settings, ofdm_settings);
	OTFS_Receiver rx(rx_settings, otfs_settings);


    /* ====== simulation ====== */
	int num_rx_bits = 0;
    for(int i = 0; i < Niter; i++)
    {
		cout << "The " << i << "th iteration" << endl;

		// update transmitted bits
		tx.generate_bits();

		// generate Doppler shifts for each path
		generate_doppler_shifts(gen, Doppler_vec, max_Doppler, EVA_channel_size);

		// update transmission channel
        channel.generation(EVA_channel_delays, EVA_channel_PDP, Doppler_vec);

		for(size_t k = 0; k < N_SNR; k++)
        {
			// modulation
			tx.mapping(energy_factor * SNR_lin[k]);
			tx.modulation();

			// channel
			channel.convolution(tx, i, Ndelay * Mblock);			// for OTFS
			//channel.convolution(tx, i, Nfft * Nblock);			// for OFDM
			channel.awgn();

			// demodulation
			rx.demodulation(channel, energy_factor * SNR_lin[k]);	// for OTFS
			//rx.demodulation(channel);								// for OFDM
			rx.demapping(energy_factor * SNR_lin[k]);
			rx.error_count(tx);
	
			// error rate calculation
			if(num_rx_bits == 0) num_rx_bits = rx.get_num_rx_bits();
			num_error_bits[k] += rx.get_num_error_bits();
            BER[k] = num_error_bits[k] / ((i + 1.0) * num_rx_bits);
        }


		// plot the BER curves
        if(i == 0)
        {
            // add BER curves to plot (first time)
            double * snr = new double [N_SNR];
            for(int k = 0; k < N_SNR; k++) snr[k] = SNR_dB[k];
            
            // configure appearance of the first curve
            CurveSettings settings;
            settings.name = "16QAM-sim-OTFS";
            settings.LineWidth = 2.5;
            settings.LineColor = "black";
            settings.MarkerType = 6;
            settings.MarkerSize = 1.5;
            
            plot.addData(BER, snr, N_SNR, 1, settings);

			// configure apperance of the second curve
            settings.name = "16QAM-theo-AWGN";
            settings.LineWidth = 2.5;
            settings.LineColor = "red";
            settings.MarkerType = 7;
            settings.MarkerSize = 1.5;

			plot.addData(theo_BER, snr, N_SNR, 2, settings);
        }
        else if((i % 10) == 0)
        {
            // update BER curves
            plot.updateData(BER, N_SNR, 1);
        	plot.plot("BER of OTFS in doubly-selective channel");
        }
		else;

		/* --- for debugging --- */
		if((i % 100) == 0)
		{
			cout << "num_rx_bits = " << num_rx_bits << endl;
		
			cout << scientific << setprecision(3) << BER[0];
			for(int k = 1; k < N_SNR; k++)
			{
				cout << ", " << scientific << setprecision(3) << BER[k];
			}
			cout << endl << endl;
		}
		/* --- for debugging --- */

        if((i % 1000) == 0)
        {
            cout << "iteration: " << i << " / " << Niter << endl;
        }
    }

	cout << "simulation complete" << endl;
    return 0;
}
