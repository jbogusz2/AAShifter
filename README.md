AAShifterEx
======
Shift Register Manager with easy bit manipulation and an array of 1-32 bits of single operation output for Arduino
--------------------------------------

The AAShifterEx library for Arduino is a flexible way to manage shift registers using the standard shift register or SPI-like protocol.  It allows access to the array elements using array syntax.  Each element is an ArrayByte object that acts like and is accessed like an array of bits.  AAShifterEx is a template class able to be instantiatied as any unsigned integer data type: char, int, short, or long.

Technically, since the underlying storage is an array, a single operation can be (Elements * ElementBitSize) bits.  Think of AAShifterEX as and array of an array of bits that know how to output themselves.

### Inspiration

This was inspired by the need to operate on daisy-chained shift register as a single unit.  Shift registers are usually 8 bits wide, I needed 24 bits.  It was tedious to track bytes and bits within them.  I wanted an array of bits and ease of use.

### Basic Usage

Most of the time you have enough bits in an unsigned long (32) to fulfill your needs.  There won't be a need for more than one element in AAShifterEx.  You don't need to use all those bits, though. You can set the number of bits to shift for all elements.  In my need of 24 bits, it's larger than an int (16) and smaller than a long (32).  You set the number of bits to shift, for me it was 24, and you won't overrun your registers.  If you need more bits, add another element to the buffer by calling setLength().

