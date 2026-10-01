// header file of channel model data
//
// Author: Raymond Su, raymondsu0110@gmail.com


/* ===== EVA model ====== */
/* Note: delays are in ns */
/* and powers are in dB   */
/* (relative to 1 watt)   */
const int EVA_channel_size = 9;
const double EVA_channel_delays [EVA_channel_size] =
{
	0.0, 30.0, 150.0, 310.0, 370.0,
	710.0, 1090.0, 1730.0, 2510.0
};
const double EVA_channel_PDP [EVA_channel_size] =
{
	0.0, -1.5, -1.4, -3.6, -0.6,
	-9.1, -7.0, -12.0, -16.9
};

/* ===== test model ===== */
