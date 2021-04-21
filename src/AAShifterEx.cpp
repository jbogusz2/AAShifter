/*
The C++ AAShifterEx library for Arduino is a very flexible way to manipulate bits and shift out more than 8 bits with minimal effort.
It can have an array of values with an arbitrary number of bits defined to shift out for all elements
It allows bit manipulation via the array operator.

It excels at handling in a simple operation the manipulating and shifting out to daisy chained registers.

This library requires the ArrayByte library at https://github.com/jbogusz2/ArrayByte
Copyright (c) 2018 Jeff Bogusz, AardvarkLD.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include <AAShifterEx.h>

template <typename T>
void AAShifterEx<T>::init(int data_pin, int clock_pin, int latch_pin, bool onesComplementBits, uint8_t BitOrder, SHIFT_PROTOCOL shift_Protocol, uint8_t BitLength)
{
	dataPin = data_pin;
	clockPin = clock_pin;
	latchPin = latch_pin;

	pinMode(dataPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
	pinMode(latchPin, OUTPUT);

	shiftProtocol = shift_Protocol;

	length = 1;
	bitLength = BitLength;
	bitOrder = BitOrder;
	onesComplement = onesComplementBits;

	initBuffer();
}

template <typename T>
void AAShifterEx<T>::init(int data_pin, int clock_pin, int latch_pin, bool onesComplementBits, uint8_t BitOrder, SHIFT_PROTOCOL shift_Protocol = SHIFT_PROTOCOL::SHIFT_REGISTER)
{
	init(data_pin, clock_pin, latch_pin, onesComplementBits, BitOrder, shift_Protocol, (sizeof(T) * 8));
}

// ************************************************************************
// INITIALIZATION
// ************************************************************************


template <typename T>
void AAShifterEx<T>::setBitLength(uint8_t BitLength)
{ 
	bitLength = BitLength;
}


template <typename T>
void AAShifterEx<T>::initBuffer()
{
	if (buffer != 0)
	{
		delete buffer;
	}

	buffer = new buffer[length];
}



template <typename T>
void AAShifterEx<T>::setLength(uint8_t buffer_length)
{
	length = buffer_length;
	initBuffer();
}

template <typename T>
void AAShifterEx<T>::setBitLengthMode(BITLEN_MODE mode)
{
	bitLenMode = mode;
}


// ************************************************************************
// CLEAR / SET
// ************************************************************************


template <typename T>
void AAShifterEx<T>::clear()
{
	bulkOp(0);
}
template <typename T>
void AAShifterEx<T>::set()
{
	bulkOp(1);
}
template <typename T>
void AAShifterEx<T>::set(ArrayByte<T> value)
{
	bulkOp(value);
}

template <typename T>
void AAShifterEx<T>::bulkOp(ArrayByte<T> value)
{
	for(int i = 0; i < length; i++)
		buffer[i] = value;
}


// ************************************************************************
// OPERATORS
// ************************************************************************


template <typename T>
ArrayByte<T>* AAShifterEx<T>::operator[] (const int index)
{
	return &buffer[index];
}



// ************************************************************************
// OUTPUT OPERATIONS
// ************************************************************************


template <typename T>
void AAShifterEx<T>::output(T* data, uint8_t size)
{
	setLength(size);

	buffer = data;

	output();
}

template <typename T>
void AAShifterEx<T>::output(T data)
{
	setLength(1);

	buffer[0] = data;

	output();
}


template <typename T>
void AAShifterEx<T>::commit()
{
	digitalWrite(latchPin, LOW);	// also CS for SPI
	digitalWrite(clockPin, LOW);
	
	bitCounter = 0;
	int ctr = 0;
	int i = bitOrder == LSBFIRST ? -1 : length;
	bool stopShifting = false;

	// if LSBFIRST, buffer 0-> length, bits 0->n
	// if MSBFIRST, buffer length-1 -> 0, bits n->0

	while (ctr < length)
	{
		if (bitOrder == LSBFIRST)
			i++;
		else
			i--;

		T outputVal = buffer[i];

		outputVal = onesComplement ? ~outputVal : outputVal;

		stopShifting = shift(outputVal);
		ctr++;

		if (stopShifting)
			break;
	}

	//for(int i = (numBanks-1); i >= 0; i--)
	//{
	//	T outputVal = buf[i];
	//	
	//	outputVal = onesComplement ? ~outputVal : outputVal;
	//	
	//	output(outputVal);

	//}
	
	digitalWrite(latchPin, HIGH);

	// latch the data if the target is a shift register
	// SPI targets do not need this
	if(shiftProtocol == SHIFT_PROTOCOL::SHIFT_REGISTER)
		digitalWrite(latchPin, LOW);
}

template <typename T>
bool AAShifterEx<T>::shift(T val)
{
	uint8_t i;
	uint8_t shifted = 0;
	bool outputVal = 0;
	// either shift max bits of size of T bits or bitLength
	uint8_t maxBits = bitMode == BITLEN_MODE::EACH ? bitLength : sizeof(val)*8;
/*
	if (bitMode == BITLEN_MODE::EACH)
		bitCounter = 0;*/

	//for (i = 0; i < bitLength; i++) {
	for (i = 0; i < maxBits; i++) {
		if (bitOrder == LSBFIRST)
			outputVal = !!(val & ((T)1 << i));
		else
			outputVal = !!(val & ( ((T)1 << (bitLength - 1) - i)));

		digitalWrite(dataPin, outputVal);

		digitalWrite(clockPin, HIGH);
		digitalWrite(clockPin, LOW);

		if (++bitCounter >= bitLength)
			return true;
	}
	return false;
}

// ************************************************************************
// PROPERTIES
// ************************************************************************


template <typename T>
byte AAShifterEx<T>::Bits()
{
	return bitLength;
}
template <typename T>
byte AAShifterEx<T>::Length()
{
	return length;
}

// ************************************************************************
// UTILITY OPERATIONS
// ************************************************************************


template <typename T>
byte AAShifterEx<T>::autoSenseBitLength(int sensePin)
{

	// tie sensePin to the last shift register daisy-chain output.

	ArrayByte<T> buf[1];
	byte banks = 0;

	buf[0] = 0;
	
	pinMode(sensePin, INPUT);
	
	// force clear all possible bits
	for(int i = 0; i < MAX_OUTBANK_COUNT*2; i++)
	{
		commit(buf, 1);
	}

	delay(5000);
	buf[0] = 128;
	
	for(int i = 0; i < MAX_OUTBANK_COUNT; i++)
	{
		banks++;
		commit(buf, 1);
		if (banks > 1)
			buf[0] = 0x00;

		delay(5000);

		if (digitalRead(sensePin) == (onesComplement ? LOW : HIGH))
		{
		  break;
		}
		else
		{
		}
	}
	
	return banks;
}
