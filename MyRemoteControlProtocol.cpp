/*
 * MyRemoteControlProtocol.cpp
 *
 *  Created on: 27.02.2016
 *      Author: butch
 */

#include "MyRemoteControlProtocol.h"
#include "arduino.h"


#define IS_DEBUG_MYRCP 0


MyRemoteControlProtocol::MyRemoteControlProtocol()
{
	// TODO Auto-generated constructor stub
	_rcChannel  =0;

	for (int i=0; i<COUNT_ANALOG_PORT; i++)
	{
		_analogPortValues[i] = 0;
	}
	for (unsigned int i=0; i<COUNT_DIGITAL_PORT ; i++)
	{
		_digitalPortValues[i] = 0;
	}
	_protocolValue = 0;

}

MyRemoteControlProtocol::~MyRemoteControlProtocol() {
	// TODO Auto-generated destructor stub
}


/**
 * This function sets the rc channel
 * 0-3 is allowed.
 *
 */
void MyRemoteControlProtocol::setRCChannel(int rcChannel)
{
	//check if value is between 0 and 3
	if (rcChannel <=3 && rcChannel >=0)
	{
		this->_rcChannel = rcChannel;
	}
	else
	{
		#if IS_DEBUG_MYRCP
			Serial.print("ERROR: MyRemoteControlProtocol::setRCChannel: Channel needs to be beween 0 and 3. You set: ");
			Serial.print(rcChannel);
		#endif
	}
}

/**
 * this function sets the 6 bit value for an analog output.
 * - analogPort: A=0;B=1;C=3;D=3
 * - analogValue: 0 - 63 as int
 */
void MyRemoteControlProtocol::setAnalog(int analogPort, int analogValue)
{
	// check if analog value is in correct 6 bit range
	if (analogValue >=0 && analogValue <= 63)
	{
		// check if analogPort is in correct Range
		if (analogPort >=0 && analogPort < COUNT_ANALOG_PORT)
		{
			_analogPortValues[analogPort] =  analogValue;
		}
		else
		{
			#if IS_DEBUG_MYRCP
				Serial.print("ERROR: MyRemoteControlProtocol::setAnalog: analogPort needs to be between 0 and 3. You set: ");
				Serial.println(analogPort);
			#endif
		}
	}
	else
	{
		#if IS_DEBUG_MYRCP
			Serial.print("ERROR: MyRemoteControlProtocol::setAnalog: analogValue needs to be between 0 and 63. You set: ");
			Serial.println(analogValue);
		#endif

	}

}



/**
 * this function sets the 1 bit value for an digital output.
 * - digitalPort: E=0, F=1, ..., J=7
 * - digitalValue: 0 or 1 as int
 */
void MyRemoteControlProtocol::setDigital(int digitalPort, int digitalValue)
{
	// check if digital value is in correct 6 bit range
	if (digitalValue ==0  || digitalValue == 1)
	{
		// check if analogPort is in correct Range
		if (digitalPort >=0 && digitalPort < COUNT_DIGITAL_PORT)
		{
			_digitalPortValues[digitalPort] =  digitalValue;
		}
		else
		{
			#if IS_DEBUG_MYRCP

				Serial.print("ERROR: MyRemoteControlProtocol::setDigital: digitalPort needs to be between 0 and 5. You set: ");
				Serial.println(digitalPort);
			#endif

		}
	}
	else
	{
		#if IS_DEBUG_MYRCP
			Serial.print("ERROR: MyRemoteControlProtocol::setDigital: digitalValue needs to be 0 or 1. You set: ");
			Serial.println(digitalValue);
		#endif
	}

}



/**
 * This function combines all values to one protocol value and returns the 32 bit number as unsinged long.
 */
unsigned long MyRemoteControlProtocol::getProtocolValue()
{
	// TODO:
	// Combine everything


	// process the channel
	bitWrite(_protocolValue,BITSTART_CHANNEL, bitRead(_rcChannel,0));
	bitWrite(_protocolValue,BITSTART_CHANNEL +1 , bitRead(_rcChannel,1));


	// process DigitalValues
	for (int digitalPortId=0; digitalPortId<COUNT_DIGITAL_PORT; digitalPortId++)
	{
		// for all digital ports do
		// calculate start position in bitstream

		int targetBit = digitalPortId * BITLENGTH_DIGITAL + BITSTART_J;
		for (int bitNumber=0; bitNumber<BITLENGTH_DIGITAL; bitNumber++)
		{
			bitWrite(_protocolValue,targetBit, bitRead(_digitalPortValues[digitalPortId],bitNumber));
		}
	}


	// process AnalogValues
	for (int analogPortId=0; analogPortId<COUNT_ANALOG_PORT; analogPortId++)
	{
		// for all digital ports do
		// calculate start position in bitstream

		int targetBit = 0; // the bit which will be changed in _protocolValue
		for (int bitNumber=0; bitNumber<BITLENGTH_ANALOG; bitNumber++)
		{
			targetBit= analogPortId * BITLENGTH_ANALOG + BITSTART_D + bitNumber;
			bitWrite(_protocolValue,targetBit, bitRead(_analogPortValues[analogPortId],bitNumber));
		}
	}
	// return the protocolValue / telegram
	return _protocolValue;
}



/**
 * this functions sets the protocol value.
 * This is especially needed before subvalues can be decoded.
 */
void MyRemoteControlProtocol::setProtocolValue(unsigned long bits)
{
	this->_protocolValue = bits;
	decodeProtocolValue();
}


/**
 * returns the rc channel
 */
int MyRemoteControlProtocol::getRCChannel()
{
	return _rcChannel;
}


/**
 * returns value of given analogPort.
 * returns -1 if given analogPort is out of range. / index out of bounds.
 */
int MyRemoteControlProtocol::getAnalog(int analogPort)
{
	// check if analogPort is in expected range
	if (analogPort >= 0 && analogPort < COUNT_ANALOG_PORT)
	{
		return _analogPortValues[analogPort];
	}
	else
	{
		#if IS_DEBUG_MYRCP
			Serial.print("ERROR: MyRemoteControlProtocol::getAnalog: analogPort is out of expected range. You set ");
			Serial.println(analogPort);
		#endif
		return -1;
	}
}


/**
 * returns value of given digitalPort.
 * returns -1 if given digitalPort is out of range. / index out of bounds.
 */
int MyRemoteControlProtocol::getDigital(int digitalPort)
{
	// check if analogPort is in expected range
	if (digitalPort >= 0 && digitalPort < COUNT_DIGITAL_PORT)
	{
		return _digitalPortValues[digitalPort];
	}
	else
	{
		#if IS_DEBUG_MYRCP
			Serial.print("ERROR: MyRemoteControlProtocol::getDigital: analogPort is out of expected range. You set ");
			Serial.println(digitalPort);
		#endif
		return -1;
	}
}


/**
 * takes the protocolValue and decodes it.
 * It decodes _rcChannel, _analogValues and _digitalValues.
 * After decoding they are available by getter functions.
 */
void MyRemoteControlProtocol::decodeProtocolValue()
{
	// decode the channel
	_rcChannel = 0;
	for (int bit=0; bit<BITLENGTH_CHANNEL;bit++)
	{
		bitWrite(_rcChannel, 0 + bit, bitRead(_protocolValue,BITSTART_CHANNEL + bit));
	}

	// decode all analogPorts
	for (int analogId=0; analogId< COUNT_ANALOG_PORT;analogId++)
	{
		// reset the former written analogValue
		_analogPortValues[analogId] = 0;

		// copy bitwise
		for (int bit=0; bit<BITLENGTH_ANALOG;bit++)
		{
			bitWrite(_analogPortValues[analogId], 0 + bit, bitRead(_protocolValue,analogId * BITLENGTH_ANALOG + BITSTART_D + bit));
		}
	}

	// decode all digitalPorts
	for (int digitalId=0; digitalId<COUNT_DIGITAL_PORT;digitalId++)
	{
		// reset the former written analogValue
		_digitalPortValues[digitalId] = 0;

		// copy bitwise
		for (int bit=0; bit<BITLENGTH_DIGITAL;bit++)
		{
			bitWrite(_digitalPortValues[digitalId], 0 + bit, bitRead(_protocolValue,digitalId * BITLENGTH_DIGITAL + BITSTART_J + bit));
		}
	}

}
