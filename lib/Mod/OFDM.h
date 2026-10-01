// header of OFDM module
//
// Note: CP-OFDM is adopted here
//
// Author: Raymond Su, raymondsu0110@gmail.com

#ifndef OFDM_H
#define OFDM_H

#ifndef EIGEN_FFTW_DEFAULT
#define EIGEN_FFTW_DEFAULT
#endif

#include <Eigen/Dense>
#include <unsupported/Eigen/FFT>
#include <random>

#include "../Base/transmitter.h"
#include "../Base/channel.h"
#include "../Base/receiver.h"

using namespace Eigen;


/* ============= struct definitions ============= */
struct OFDM_TxRxSettings
{
	// settings related to OFDM

	int Nsubc = 256;			// number of subcarriers = Nfft
	int Nblock = 0;				// number of blocks in each iteration
	double CP_ratio = 0.25;		// ratio of CP length to OFDM useful part
};

/* ============= class declarations ============= */
class OFDM_Transmitter : public Transmitter
{
private:
	OFDM_TxRxSettings ofdm_settings;
	int Nfft = 0;			// row size of OFDM data matrix
	int Nblock = 0;			// column size of OFDM data matrix
	int Ncp = 0;			// length of CP
	MatrixXcd X;			// OFDM data matrix, contains Nfft + Ncp rows
							// and Nblock columns
	FFT<double> fft_engine; // FFT engine to perform FFT/IFFT

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	OFDM_Transmitter(mt19937 & rand_gen, TxRxSettings set, OFDM_TxRxSettings ofdm_set);
	~OFDM_Transmitter();
	void modulation();

	// for debugging
	
	// friend class
	friend class Channel;
	friend class OFDM_Receiver;
};

class OFDM_Receiver : public Receiver
{
private:
	OFDM_TxRxSettings ofdm_settings;
	int Nfft = 0;
	int Nblock = 0;
	int Ncp = 0;
	MatrixXcd Y;			// OFDM received data matrix, contains Nfft + Ncp rows
							// and Nblock columns
	MatrixXcd H_est;		// estimated DFT-domain channel matrix at Rx
	FFT<double> fft_engine; // FFT engine to perform FFT/IFFT

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	OFDM_Receiver(TxRxSettings set, OFDM_TxRxSettings ofdm_set);
	~OFDM_Receiver();
	void demodulation(Channel & channel);

	// for debugging
};


#endif
