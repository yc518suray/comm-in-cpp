// header of OTFS module
//
// Author: Raymond Su, raymondsu0110@gmail.com

#ifndef OTFS_H
#define OTFS_H

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
struct OTFS_TxRxSettings
{
	// settings related to OTFS
	
	int N_delay = 64;			// number of delay bins
	int M_doppler = 64;			// number of Doppler bins
	int frame_format = 0;		// 0 -> ZP-OTFS
								// 1 -> CP-OTFS
								// 2 -> RZP-OTFS
								// 3 -> RCP-OTFS
	int Npadding = 0;			// length of ZP/CP samples
};

/* ============= class declarations ============= */
class OTFS_Transmitter : public Transmitter
{
private:
	OTFS_TxRxSettings otfs_settings;
	int Nd = 0;				// length along the delay axis
	int Nblock = 0;			// length along the Doppler axis
	int Npadding = 0;		// length of ZP/CP samples
	MatrixXcd X;			// OTFS data matrix, contains Nd rows and Nblock columns
	FFT<double> fft_engine;	// FFT engine to perform FFT/IFFT

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	OTFS_Transmitter(mt19937 & rand_gen, TxRxSettings set, OTFS_TxRxSettings otfs_set);
	~OTFS_Transmitter();
	void modulation();

	// for debugging
	
	// friend class
	friend class Channel;
	friend class OTFS_Receiver;
};

class OTFS_Receiver : public Receiver
{
private:
	OTFS_TxRxSettings otfs_settings;
	int Nd = 0;
	int Nblock = 0;
	int Npadding = 0;
	MatrixXcd Y;			// OTFS received data matrix
							// with (Nd - Npadding) rows and Nblock columns
	MatrixXcd H_est;		// estimated channel matrix at Rx
							// the target domain is specified in the implementation files
	FFT<double> fft_engine; // FFT engine to perform FFT/IFFT

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	OTFS_Receiver(TxRxSettings & set, OTFS_TxRxSettings & otfs_set);
	~OTFS_Receiver();
	void demodulation(Channel & channel, double Eave);
	void build_perfect_channel_matrix(Channel & channel, int domain);
	
	// detection methods
	void MRC_detection(Ref<MatrixXcd> R, int Niter, int lmax);

	// for debugging
};


#endif
