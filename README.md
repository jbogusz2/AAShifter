AAShifterEx
======
Shift Register Manager with easy bit manipulation and an array of 1-32 bits of single operation output for Arduino
--------------------------------------

The AAShifterEx library for Arduino is a flexible way to manage shift registers using the standard shift register or SPI-like protocol.  It allows access to the array elements using array syntax.  Each element is an ArrayByte object that acts like and is accessed like an array of bits.  AAShifterEx is a template class able to be instantiatied as any unsigned integer data type: char, int, short, or long.
Technically, since the underlying storage is an array, there is the possiblility of bit counts of (Elements * ElementBitSize) as a single operation.

### Inspiration

This was inspired by the need to operate on daisy-chained shift register as a single unit.  Shift registers are usually 8 bits wide, I needed 24 bits.  It was tedious to track bytes and bits within them.  I wanted an array of bits and ease of use.

### Basic Usage

