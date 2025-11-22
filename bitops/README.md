
# Table of contents

- [bitops.udr - Informix Extension](#bitopsudr---informix-extension)
  - [Warning](#warning)
  - [BitOps Functions](#bitops-functions)
    - [`bop_left(val,shift)`](#bop_leftvalshift)
    - [`bop_right(val,shift)`](#bop_rightvalshift)
    - [`bop_and(val1,val2)`](#bop_andval1val2)
    - [`bop_or(val1,val2)`](#bop_orval1val2)
    - [`bop_not(val1)`](#bop_notval1)
    - [`bop_xor(val2,val2)`](#bop_xorval2val2)
  - [Print Functions](#print-functions)
    - [`bitprint()`](#bitprint)
    - [`hexprint()`](#hexprint)
    - [`hexprint4()`](#hexprint4)
    - [`bitprint4()`](#bitprint4)
    - [Print Example](#print-example)

# bitops.udr - Informix Extension

## WARNING

This isn't in production anywhere, and may neve be ( although the hexprint might, because informix's `hex()` annoys me...

## BitOps Functions

A series of routines for doing bitwise operations in Informix.

The Builtin informix routines, eg `ifx_bit_leftshift` when faced with an overflow situation, will promote types if possible ( eg smallint to integer, integer to bigint ), and if that's not possible, throw an overflow exception.

Most algorthms that rely on bitshifts assume the more common behaviour that bits "shifted off the edge" just disappear.

So that's what these versions do too, making these a closer representation of the native C operations, and at least for some operations, they seem faster too. But you will get 0 values rather than overflow if you shift "all the way" for your given type.

Because informix only supports signed integer types, all results have the high bit masked off before returning ( that is to say, they return 15, 31, or 63 bit values. That happens last though, so you can right shift the sign bit into your remaining value  ( I'm not sure if this is a good idea or not tbh )

__This package may or may not exist purely because I thought a set of functions called 'bop_left, bop_righ' etc sounded funny__

### `bop_left(val,shift)`
### `bop_right(val,shift)`
### `bop_and(val1,val2)`
### `bop_or(val1,val2)`
### `bop_not(val1)`
### `bop_xor(val2,val2)`

## Print Functions

### `bitprint()`

Prints an integer as 4 bitgrouped "binary strings" (that is `1010_1010_0010...` )
The minimum number of bit-groups are used to print the value

### `hexprint()` 

Prints a bare hex number without any leading `0x0000...` 

### `hexprint4()`

Prints a bare hex number in 4 digit (16 bit) groups 

### `bitprint4()`

As per bitprint, bit 16bit groups ( 4x4bits )

### Print Example

```
sqlq -d tjb demo.sql

  value  |           bitprint            |                bitprint4                | hexprint | hexprint4
---------+-------------------------------+-----------------------------------------+----------+-----------
   35752 | 1000_1011_1010_1000           | 1000_1011_1010_1000                     | 8ba8     | 8ba8
  357523 | 0101_0111_0100_1001_0011      | 0000_0000_0000_0101_0111_0100_1001_0011 | 57493    | 00057493
 3575253 | 0011_0110_1000_1101_1101_0101 | 0000_0000_0011_0110_1000_1101_1101_0101 | 368dd5   | 00368dd5
```
