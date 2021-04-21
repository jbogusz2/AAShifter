#ifndef AAShifterEx_h
#define AAShifterEx_h

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

#include <Arduino.h>
#include <ArrayByte.h>

#define MAX_OUTBANK_COUNT		3

/*
Set/Reset a bit in one of the elements of the buffer array
	The parenthesis are needed

	(*shifter[1])[7] = true;

  Same as above, but through an AAByte variable

	ArrayByte& x = *shifter[0];
	x[7] = true;

Set an element of the buffer array to a value

	*shifter[0] = value;

*/


enum SHIFT_PROTOCOL
{
	SHIFT_REGISTER,
	SHIFT_SPI
};

enum BITLEN_MODE
{
	OVERALL,
	EACH
};

template <class T> class AAShifterEx {
  public:

	  // initialization
	  void init(int dataPin, int clockPin, int latchPin, bool onesComplementBits, uint8_t BitOrder, SHIFT_PROTOCOL shift_Protocol, uint8_t BitLength);
	  void init(int dataPin, int clockPin, int latchPin, bool onesComplementBits, uint8_t BitOrder, SHIFT_PROTOCOL shift_Protocol);

	  void setBitLength(uint8_t BitLength);		// manually sets the number of bits to shift per buffer element.  
	  void setLength(uint8_t buffer_length);	// initializes the buffer and length
	  void setBitLengthMode(BITLEN_MODE mode);	// sets the way bits are counted for output
												// OVERALL = over the output of all array elements, output will stop at bitLength count
												// EACH = over the output of all array element bits, output will stop at bitLength count for each array element shifted
												//			each array element will be shifted, but only for bitLength number of bits.
												//				ex. bitLength = 4, 3 elements.  Each element is shifted out for 4 bits
												//					you ay need this if some bits are status and others output.

	  // clear/set operations
	  void clear();							// clears the entire buffer to all 0's.		does not output
	  void set();							// sets the entire buffer to all 1's.		does not output
	  void set(ArrayByte<T> value);			// sets each buffer element to the value.   does not output
  
	  // operators

	  ArrayByte<T> * operator[] (const int index);	// access the buffer by index

	  void output(T* data, uint8_t size);	// overwrites buffer and outputs.  sets length = size
	  void output(T data);					// same as above, but overwrites buffer[0].  sets length = 1
  
	  // outputs buffer
	  void commit();						// shifts out the current sttate of the buffer
  
	  // utilities
	  byte autoSenseBitLength(int sensePin);

	  // properties
	  byte Bits();							// returns the number of bits to shift per buffer elenent		
	  byte Length();						// number of buffer elements

  private:

		int dataPin;
		int clockPin;
		int latchPin;
  
		ArrayByte<T>* buffer;					// array of T values

		SHIFT_PROTOCOL shiftProtocol = SHIFT_PROTOCOL::SHIFT_REGISTER;
		BITLEN_MODE bitLenMode = BITLEN_MODE::OVERALL;

		uint8_t length = 1;
		uint8_t bitOrder;						// MSB or LSB FIRST?
		uint8_t bitLength;
		int bitCounter = 0
		bool onesComplement = false;

		void initBuffer();

		// clear/set operations
		void bulkOp(ArrayByte<T> value);		// used to commit clear and set operations.

		// output operations
		bool shift(T val);
};

template class AAShifterEx<uint8_t>;
template class AAShifterEx<unsigned char>;
template class AAShifterEx<unsigned short>;
template class AAShifterEx<unsigned int>;
template class AAShifterEx<unsigned long>;
#endif
