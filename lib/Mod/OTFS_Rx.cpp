// cpp file of OTFS Rx module
//
// Author: Raymond Su, raymondsu0110@gmail.com

#include <Eigen/Dense>
#include <cmath>
#include <complex>
#include <iostream>

#include "OTFS.h"
#include "../Utils/functions.h"
#include <unsupported/Eigen/FFT>

using namespace Eigen;


/* ====== non-member function declarations ====== */

/* ============= class definitions ============== */
OTFS_Receiver::OTFS_Receiver(TxRxSettings & set, OTFS_TxRxSettings & otfs_set) : 
					Receiver(set)
{
	// constructor of OTFS_Receiver
	
	otfs_settings = otfs_set;
	Nd = otfs_settings.N_delay;
	Nblock = otfs_settings.M_doppler;
	Npadding = otfs_settings.Npadding;

	if(otfs_settings.frame_format == 0 || otfs_settings.frame_format == 1)
	{
		// ZP or CP
		num_modsym = (Nd - Npadding) * Nblock;
	}
	else
	{
		// RZP or RCP
		num_modsym = Nd * Nblock;
	}
}

OTFS_Receiver::~OTFS_Receiver()
{
	// destructor of OTFS_Receiver
}

void OTFS_Receiver::demodulation(Channel & channel, double Eave)
{
	// transform time-domain samples into modulation symbols
	// step 1: equalization
	// step 2: transform to DD-domain (modulation space)
	// step 3: parallel to serial
	
	double sqrt_Eave = sqrt(Eave);

	if(Nblock == 0)
	{
		std::cerr << "OTFS settings error: size not specified\n";
		exit(1); // exit the program
	}

	// initialize the received data matrix
	if(Y.size() == 0) Y.resize(Nd - Npadding, Nblock);
	Y.setZero();

	Ref<MatrixXcd> Yr = Map<MatrixXcd>(channel.Rx_y.data(), Nd, Nblock);
	Yr /= sqrt_Eave;

	// step 1: equalization
	// we use DT-domain MRC detection here, for integer delay channel
	if(H_est.size() == 0)
	{
		// note that we assume channel is doubly-selective here
		// meaning that the channel matrix is constructed for each frame/iteration

		H_est.resize(Nd * Nblock, Nd * Nblock);
	}

	// construct the channel matrix
	// the second-to-last parameter indicates the target domain
	// 0 -> time domain
	// 1 -> delay-time (DT) domain
	// 2 -> delay-Doppler (DD) domain
	build_perfect_channel_matrix(channel, 1);
	
	// DT-domain MRC detection
	int lmax = *(channel.int_Delay.end() - 1);
	MRC_detection(Yr, 8, lmax); // number of iterations = 8

	// for AWGN channel only
	/*if(otfs_settings.frame_format == 0 || otfs_settings.frame_format == 1)
	{
		Y = Yr.block(0, 0, Nd - Npadding, Nblock);
	}
	else
	{
		Y = Yr;
	}*/

	// step 2: transform to DD-domain
	VectorXcd vec_temp(Y.cols());
	for(int i = 0; i < Y.rows(); i++)
	{
		fft_engine.fwd(vec_temp, Y.row(i));
		Y.row(i) = vec_temp;
	}

	// step 3: parallel to serial
	if(Rx_sym.size() == 0) Rx_sym.resize(Y.size(), complex<double>(0));
	Map<VectorXcd>(Rx_sym.data(), Y.size()) = Y.reshaped();
}

void OTFS_Receiver::build_perfect_channel_matrix(Channel & channel, int domain)
{
	// construct the channel matrix (perfect CSI)
	// channel 	-> the channel object
	// domain 	-> indicate the target domain
	// 			   0 -> time domain
	// 			   1 -> DT domain
	// 			   2 -> DD domain
	//
	// Note: the constructed matrix works only for ZP-OTFS and RZP-OTFS, for now

	if(domain == 0 || domain == 1)
	{
		int P = channel.settings.Npath;		// number of paths
		int NN = Nd * Nblock;				// number of rows of H_est

		// iterate over rows of H
		for(int n = 0; n < NN; n++)
		{
			// iterate over all paths
			for(int m = 0; m < P; m++)
			{
				if(channel.settings.isDelayFractional)
				{
					// fractional delay
				}
				else
				{
					// integer delay
					
					double expo = 2 * M_PI * channel.Doppler[m] * (n - channel.int_Delay[m]) / NN;
					complex<double> expo_comp = complex<double>(0, 1) * expo;
					int l = channel.int_Delay[m];
						
					// for time domain channel matrix
					int indx1 = n;
					int indx2 = (l > n)? (n - l + NN): (n - l);
					if(domain == 1)
					{
						// for DT domain channel matrix
						indx1 = indx1 / Nd + (indx1 % Nd) * Nblock;
						indx2 = indx2 / Nd + (indx2 % Nd) * Nblock;
					}
					H_est(indx1, indx2) += channel.TDL_model[m] * exp(expo_comp);
				}
			}
		}
	}
	else if(domain == 2)
	{
		// DD domain channel matrix
	}
	else;
}

void OTFS_Receiver::MRC_detection(Ref<MatrixXcd> R, int Niter, int lmax)
{
	// implement MRC detection in DT domain
	// R		-> the received signal matrix (including ZP/CP)
	// Niter	-> number of iterations
	// lmax		-> max integer delay
	//
	// Note 1: this method is designed for integer delay channels
	// Note 2: the implementation is for ZP-OTFS (for now)

	// initializations
	VectorXcd x_m(Nblock);
	VectorXcd b_m(Nblock);
	VectorXcd vtemp2(Nblock);
	
	vector<complex<double>> vec_temp1(Nblock, complex<double>(0));
	Map<VectorXcd> vtemp1(vec_temp1.data(), vec_temp1.size());
	
	// compute matrices D_m
	int NN = Nblock * Nblock;
	int indx1, indx2;

	vector<complex<double>> D_m_vector(NN * (Nd - Npadding), complex<double>(0));
	vector<Map<MatrixXcd>> D_m_matrices;
	D_m_matrices.reserve(Nd - Npadding);
	
	for(int m = 0; m < Nd - Npadding; m++)
	{
		D_m_matrices.emplace_back(D_m_vector.data() + m * NN, Nblock, Nblock);
		for(int l = 0; l <= lmax; l++)
		{
			indx1 = (m + l) * Nblock;
			indx2 = m * Nblock;
			
			D_m_matrices[m] += H_est.block(indx1, indx2, Nblock, Nblock) *
							   H_est.block(indx1, indx2, Nblock, Nblock);
		}
	}

	// iterations
	for(int i = 0; i < Niter; i++)
	{
		// over all delay vectors (except ZP or CP)
		for(int m = 0; m < Nd - Npadding; m++)
		{
			x_m.setZero();
			// collect each branch and combine
			for(int l = 0; l <= lmax; l++)
			{
				b_m.setZero();
				for(int ll = 0; ll <= lmax; ll++)
				{
					if(ll == l) continue;
					if(m < ll - l || m >= Nd - Npadding + ll - l) break;
					
					indx1 = (m + l) * Nblock;
					indx2 = (m + l - ll) * Nblock;
					b_m += H_est.block(indx1, indx2, Nblock, Nblock) *
						   Y.row(m + l - ll).transpose();
				}

				b_m = R.row(m + l).transpose() - b_m;
				indx1 = (m + l) * Nblock;
				indx2 = m * Nblock;
				
				x_m += H_est.block(indx1, indx2, Nblock, Nblock).adjoint() * b_m;
			}

			// ZF equalization
			x_m = D_m_matrices[m].partialPivLu().solve(x_m);

			// transform to DD domain delay vector
			fft_engine.fwd(vtemp1, x_m);
			
			// hard decision
			QAM_demapping(vec_temp1, settings.qam_type);

			// transform back to DT domain delay vector
			fft_engine.inv(vtemp2, vtemp1);
			Y.row(m) = vtemp2;
		}
	}
}

/* ====== non-member function definitions ======= */
