// cpp file of OTFS Tx module
//
// Author: Raymond Su, raymondsu0110@gmail.com

#include <complex>
#include <iostream>

#include "OTFS.h"

using namespace std;
using namespace Eigen;


/* ============= class definitions ============== */
OTFS_Transmitter::OTFS_Transmitter(mt19937 & rand_gen, TxRxSettings set, OTFS_TxRxSettings otfs_set) : 
					   Transmitter(rand_gen, set)
{
	// constructor of OTFS_Transmitter
	
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

OTFS_Transmitter::~OTFS_Transmitter()
{
	// destructor of OTFS_Transmitter
}

void OTFS_Transmitter::modulation()
{
	// transform modulation symbols to time-domain samples
	// step 1: form data matrix (including ZP/CP)
	// step 2: perform IDFT along Doppler axis
	// step 3: parallel to serial
	
	if(Nblock == 0)
	{
		std::cerr << "OTFS settings error: size not specified\n";
		exit(1); // exit the program
	}

	// initialize the time-domain matrix
	if(X.size() == 0) X.resize(Nd, Nblock);
	X.setZero();

	// step 1: construct the data matrix
	if(otfs_settings.frame_format == 0)
	{
		// ZP-OTFS
		
		MatrixXcd X_temp(Nd - Npadding, Nblock);
		MatrixXcd ZP(Npadding, Nblock);
		X_temp = Map<MatrixXcd>(Tx_sym.data(), X_temp.rows(), X_temp.cols());
		ZP.setZero();

		// fill in data and ZP
		X.topRows(X_temp.rows()) = X_temp;
		X.bottomRows(ZP.rows()) = ZP;
	}
	else if(otfs_settings.frame_format == 1)
	{
		// CP-OTFS
	}
	else if(otfs_settings.frame_format == 2)
	{
		// RZP-OTFS
	}
	else
	{
		// RCP-OTFS
	};

	// step 2: perform IDFT along Doppler axis
	VectorXcd vec_temp(X.cols());
	for(int i = 0; i < X.rows(); i++)
	{
		fft_engine.inv(vec_temp, X.row(i));
		X.row(i) = vec_temp;
	}

	// step 3: parallel to serial
	if(Tx_x.size() == 0) Tx_x.resize(X.size(), complex<double>(0));
	Map<VectorXcd>(Tx_x.data(), X.size()) = X.reshaped();
}

/* ====== non-member function definitions ======= */
