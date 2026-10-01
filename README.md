# comm-in-cpp

This program simulates various wireless communication schemes, including QAM, OFDM, and OTFS, in C++.

## Dependency

The following libraries should be included:

- Eigen
- FFTW

## Compile

Compile the source code using `make`.

The path of the Eigen library should be specified in the Makefile.

## Usage

The program consists of three base modules: Transmitter, Channel, and Receiver. OFDM and OTFS modules are derived from Transmitter and Receiver modules. Other modulation schemes can be added in a similar way.

## To-do list

- Transceiver: channel coding/decoding
- Transceiver: channel estimation (maybe?)
- Transceiver: AFDM
- Channel: fractional delay
- Channel: CFO and STO (maybe?)

## License

This program is under GPL-3.0 license.
