// cpp file of OFDM Rx module
//
// Author: Raymond Su, raymondsu0110@gmail.com

#include <complex>
#include <iostream>

#include "OFDM.h"

using namespace std;
using namespace Eigen;


/* ====== non-member function declarations ====== */
void build_perfect_channel_matrix(MatrixXcd & H, ComplexVec h, FFT<double> & fft, int N);

/* ============= class definitions ============== */
OFDM_Receiver::OFDM_Receiver(TxRxSettings set, OFDM_TxRxSettings ofdm_set) : 
					Receiver(set)
{
	// constructor of OFDM_Receiver
	
	ofdm_settings = ofdm_set;
	Nfft = ofdm_settings.Nsubc;
	Nblock = ofdm_settings.Nblock;
	Ncp = static_cast<int>(Nfft * ofdm_settings.CP_ratio);

	num_modsym = Nblock * Nfft;
}

OFDM_Receiver::~OFDM_Receiver()
{
	// destructor of OFDM_Receiver
}

void OFDM_Receiver::demodulation(Channel & channel)
{
	// transform time-domain samples into modulation symbols
	// step 1: serial to parallel
	// step 2: remove cyclic prefix (CP)
	// step 3: perform DFT
	// step 4: equalization
	// step 5: parallel to serial
	
	if(Nblock == 0)
	{
		std::cerr << "OFDM settings error: size not specified\n";
		exit(1);
	}

	// initialize the received data matrix
	if(Y.size() == 0) Y.resize(Nfft, Nblock);
	Y.setZero();

	// step 1: serial to parallel
	MatrixXcd Y_temp(Nfft + Ncp, Nblock);
	Y_temp = Map<MatrixXcd>(channel.Rx_y.data(), Y_temp.rows(), Y_temp.cols());
	
	// step 2: remove CP from the time-domain matrix
	Y = Y_temp.block(Ncp, 0, Nfft, Nblock);

	// step 3: perform DFT
	VectorXcd vec_temp(Y.rows());
	for(int i = 0; i < Y.cols(); i++)
	{
		fft_engine.fwd(vec_temp, Y.col(i));
		Y.col(i) = vec_temp;
	}
	
	// step 4: equalization
	if(channel.settings.type != 0)
	{
		// skip equalization for AWGN channels

		if(H_est.size() == 0)
		{
			// note that we assume channel is static here
			// once the matrix is created, it is used thereafter
			
			H_est.resize(Nfft, Nfft);
			build_perfect_channel_matrix(H_est, channel.TDL_model, fft_engine, Nfft);
		}
		Y = H_est.partialPivLu().solve(Y); // ZF equalization
	}

	// step 5: parallel to serial
	if(Rx_sym.size() == 0) Rx_sym.resize(Y.size(), complex<double>(0));
	Map<VectorXcd>(Rx_sym.data(), Y.size()) = Y.reshaped();
}

/* ====== non-member function definitions ======= */
void build_perfect_channel_matrix(MatrixXcd & H, ComplexVec h, FFT<double> & fft, int N)
{
	// construct the channel matrix (perfect CSI)
	// H 	-> DFT-domain channel matrix
	// h 	-> time-domain channel response
	// fft 	-> the fft engine of the receiver object
	// N 	-> FFT size of the OFDM system

	// zero-padding the channel response
	if(h.size() < N) h.resize(N, complex<double>(0));
	
	// carry out DFT
	VectorXcd output(N);
	Map<VectorXcd> input(h.data(), h.size());
	fft.fwd(output, input);

	// construct the channel matrix
	H = output.asDiagonal().toDenseMatrix();
}
