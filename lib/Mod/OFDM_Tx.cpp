// cpp file of OFDM Tx module
//
// Author: Raymond Su, raymondsu0110@gmail.com

#include <Eigen/Dense>
#include <complex>
#include <iostream>

#include "OFDM.h"
#include <unsupported/Eigen/FFT>

using namespace Eigen;


/* ============= class definitions ============== */
OFDM_Transmitter::OFDM_Transmitter(mt19937 & rand_gen, TxRxSettings set, OFDM_TxRxSettings ofdm_set) : 
					   Transmitter(rand_gen, set)
{
	// constructor of OFDM_Transmitter

	ofdm_settings = ofdm_set;
	Nfft = ofdm_settings.Nsubc;
	Nblock = ofdm_settings.Nblock;
	Ncp = static_cast<int>(Nfft * ofdm_settings.CP_ratio);

	num_modsym = Nblock * Nfft;
}

OFDM_Transmitter::~OFDM_Transmitter()
{
	// destructor of OFDM_Transmitter
}

void OFDM_Transmitter::modulation()
{
	// transform modulation symbols to time-domain samples
	// step 1: form data matrix
	// step 2: perform IDFT
	// step 3: add cyclic prefix (CP)
	// step 4: parallel to serial
	
	if(Nblock == 0)
	{
		std::cerr << "OFDM settings error: size not specified\n";
		exit(1); // exit the program cause' something's wrong
	}

	// initialize the time-domain matrix
	if(X.size() == 0) X.resize(Nfft + Ncp, Nblock);
	X.setZero();

	// step 1: construct the data matrix
	MatrixXcd X_temp(Nfft, Nblock);
	X_temp = Map<MatrixXcd>(Tx_sym.data(), X_temp.rows(), X_temp.cols());

	// step 2: perform IDFT
	VectorXcd vec_temp(X_temp.rows());
	for(int i = 0; i < X_temp.cols(); i++)
	{
		fft_engine.inv(vec_temp, X_temp.col(i));
		X_temp.col(i) = vec_temp;
	}

	// step 3: append CP to the time-domain matrix
	MatrixXcd tails = X_temp.block(Nfft - Ncp - 2, 0, Ncp, Nblock);
	X.topRows(tails.rows()) = tails;
	X.bottomRows(X_temp.rows()) = X_temp;

	// step 4: parallel to serial
	if(Tx_x.size() == 0) Tx_x.resize(X.size(), complex<double>(0));
	Map<VectorXcd>(Tx_x.data(), X.size()) = X.reshaped();
}

/* ====== non-member function definitions ======= */
