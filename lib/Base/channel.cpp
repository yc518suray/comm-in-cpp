// cpp file of the channel module
//
// Author: Raymond Su, raymondsu0110@gmail.com

#include <algorithm>

#include "channel.h"
#include "../Utils/functions.h"
#include "../Utils/convolution.h"

using namespace std;


/* ======== member function definitions ======== */
Channel::Channel(mt19937 & rand_gen, ChannelSettings set)
{
	// constructor
	
	gen = rand_gen;
	settings = set;
}

Channel::~Channel()
{
	// destructor
}

void Channel::generation(const double * delays, const double * PDP, const double * dopplers)
{
	// generate TDL_model
	// delays 	-> channel delays, not normalized
	// PDP		-> power delay profile
	// dopplers -> channel Dopplers, not normalized
	// 
	// Note that TDL_model has different definitions
	// When settings.type == 1, TDL_model is the tap-delay line model
	// When settings.type == 2, TDL_model stores coefficients of each path

	int Np = settings.Npath;
	normal_distribution<double> randn(0.0, sqrt(0.5));
	
	if(settings.type == 0)
	{
		// AWGN channel

		return;
	}
	else
	{
		// 1. frequency-selective channel
		// 2. doubly-selective channel

		// normalized Doppler shifts
		if(settings.type == 2)
		{
			if(Doppler.size() == 0) Doppler.resize(Np, 0.0);
			for(int i = 0; i < Np; i++)
			{
				Doppler[i] = dopplers[i] / settings.Doppler_resolution;
			}
		}

		// check if TDL_model is already generated
		if(TDL_model.size() != 0) return;
		
		if(settings.isDelayFractional)
		{
			// fractional delay
			// using sinc reconstruction
		}
		else
		{
			// integer delay

			// normalized delay shifts
			if(int_Delay.size() == 0) int_Delay.resize(Np, 0.0);
			for(int i = 0; i < Np; i++)
			{
				int_Delay[i] = static_cast<int>(delays[i] / settings.delay_resolution + 0.5);
			}

			// determine size of TDL_model
			if(settings.type == 1)
			{
				TDL_model.resize(int_Delay[Np - 1] + 1, complex<double>(0));
			}
			else if(settings.type == 2)
			{
				TDL_model.resize(Np, complex<double>(0));
			}
			else;

			// generate path coefficient
			for(int i = 0; i < Np; i++)
			{
				double A = sqrt(pow(10.0, 0.1 * PDP[i]));
				if(settings.type == 1)
				{
					double re = TDL_model[int_Delay[i]].real();
					double im = TDL_model[int_Delay[i]].imag();
					TDL_model[int_Delay[i]].real(re + A * randn(gen));
					TDL_model[int_Delay[i]].imag(im + A * randn(gen));
				}
				else if(settings.type == 2)
				{
					TDL_model[i].real(A * randn(gen));
					TDL_model[i].imag(A * randn(gen));
				}
				else;
			}

			// normalization of all taps' power
			double norm = vec_norm(TDL_model);
			for(int i = 0; i < TDL_model.size(); i++)
			{
				TDL_model[i] = TDL_model[i] / norm;
			}
		}
	}
}

void Channel::convolution(Transmitter & tx, int Nit, int NN)
{
	// convolution with Tx samples
	// tx	-> the transmitter object
	// Nit	-> current frame/iteration index
	// NN	-> Doppler-related parameter (dependent on transmitter type)
	// 		   For OFDM_Transmitter, NN = Nfft
	// 		   For OTFS_Transmitter, NN = Nd * Nblock

	if(settings.type == 0)
	{
		// AWGN channel
		
		Rx_y = tx.Tx_x; // note that this is deep copy
		return;
	}
	
	// determine the size of the trailing part
	if(Tx_x_prev.size() == 0)
	{
		int L = 0;
		if(settings.type == 1)
		{
			L = TDL_model.size() - 1;
		}
		else if(settings.type == 2)
		{
			L = int_Delay[settings.Npath - 1];
		}
		else;

		Tx_x_prev.resize(L, complex<double>(0));
	}
	
	// initialize the input sequence and the trailing part
	if(Tx_x_total.size() == 0)
	{
		Tx_x_total.resize(tx.Tx_x.size() + Tx_x_prev.size(), complex<double>(0));
	}
	copy(Tx_x_prev.begin(), Tx_x_prev.end(), Tx_x_total.begin());
	copy(tx.Tx_x.begin(), tx.Tx_x.end(), Tx_x_total.begin() + Tx_x_prev.size());

	// perform convolution
	if(settings.type == 1)
	{
		lin_conv(Rx_y, Tx_x_total, TDL_model);
		//overlap_save(Rx_y, Tx_x_total, TDL_model);
		//overlap_add(Rx_y, Tx_x_total, TDL_model);
	}
	else if(settings.type == 2)
	{
		int N = Nit * tx.Tx_x.size();
		lin_tv_conv(Rx_y, Tx_x_total, TDL_model, Doppler, int_Delay, N, NN);
	}
	else;
	
	Tx_x_prev.assign(tx.Tx_x.end() - Tx_x_prev.size(), tx.Tx_x.end());
}

void Channel::awgn()
{
	// add AWGN at the receiver
	
	normal_distribution<double> randn(0.0, sqrt(0.5));
	
	int L = Rx_y.size();
	for(int i = 0; i < L; i++)
	{
		Rx_y[i].real(Rx_y[i].real() + randn(gen));
		Rx_y[i].imag(Rx_y[i].imag() + randn(gen));
	}
}

/* =============== for debugging =============== */
ComplexVec Channel::get_received()
{
	return Rx_y;
}

ComplexVec Channel::get_TDL_model()
{
	return TDL_model;
}
