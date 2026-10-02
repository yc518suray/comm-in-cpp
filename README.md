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

Note: To match the number of transmitted bits to the specific modulation scheme, modify `NUM_BITS` in `Base/transmitter.h`.<br>
For example, for an OTFS system with grid size 64 x 64 without ZP or CP, the minimum number of transmitted bits with 16QAM mapping is 64 x 64 x 4 = 16384 bits.

## To-do list

- Transceiver: channel coding/decoding
- Transceiver: channel estimation (maybe?)
- Transceiver: AFDM
- Channel: fractional delay
- Channel: CFO and STO (maybe?)

## License

This program is under GPL-3.0 license.
