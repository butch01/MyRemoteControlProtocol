/*
 * MyRemoteControlProtocol.h
 *
 *  Created on: 27.02.2016
 *      Author: butch
 */

#ifndef MyRemoteControlProtocol_H_
#define MyRemoteControlProtocol_H_

/**
 * my RC Protocol:
 * ccaa|aaaa|bbbb|bbcc|cccc|dddd|ddef|ghij
 *
 *
 *
 * cc channel 0 - 3
 * aaaaaa 6 bit analog axis a (64 steps) _analogPort[0]
 * bbbbbb 6 bit analog axis b (64 steps) _analogPort[1]
 * cccccc 6 bit analog axis c (64 steps) _analogPort[2]
 * dddddd 6 bit analog axis d (64 steps) _analogPort[3]
 * e-j    1 bit digital on / off (2 bits can be combined for digital forward and reverse _digitalPort[0]-[7]
 *
 *
 *
 */

// define length for protocol entries
#define BITLENGTH_CHANNEL 2
#define BITLENGTH_ANALOG  6
#define BITLENGTH_DIGITAL 1

// define bit starting positions for protocol entries
#define BITSTART_CHANNEL 30
#define BITSTART_A 24
#define BITSTART_B 18
#define BITSTART_C 12
#define BITSTART_D 6
#define BITSTART_E 5
#define BITSTART_F 4
#define BITSTART_G 3
#define BITSTART_H 2
#define BITSTART_I 1
#define BITSTART_J 0


// define number of elements (its just static yet)
#define COUNT_ANALOG_PORT 4
#define COUNT_DIGITAL_PORT 6




class MyRemoteControlProtocol {
private:
	int _rcChannel;

	unsigned long _protocolValue;

	int _analogPortValues[COUNT_ANALOG_PORT];
	int _digitalPortValues[COUNT_DIGITAL_PORT];





public:
	MyRemoteControlProtocol();
	virtual ~MyRemoteControlProtocol();

	// encoding stuff
	void setRCChannel(int rcChannel);
	void setAnalog(int analogPort, int analogValue);
	void setDigital(int digitalPort, int digitalValue);

	unsigned long getProtocolValue(); // gets the 32bit bitmask ready for sending

	// decoding stuff

	// set complete bitmask which will be used for decoding
	// ToDo: how to use pointers to save memory?
	void setProtocolValue(unsigned long bits);

	int getRCChannel();
	int getAnalog(int analogPort);
	int getDigital(int digitalPort);

	void decodeProtocolValue();


};

#endif /* MyRemoteControlProtocol_H_ */
