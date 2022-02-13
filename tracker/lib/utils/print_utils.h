#ifndef PRINT_UTILS
#define PRINT_UTILS

#include "Arduino.h"

#include "firmware_configuration.h"

//////////////////////////////////////////////////////////////////////////////////////////
// convenience macros

#define PRINTLN_VAR(v) SERIAL_USB->print(F(#v": ")); SERIAL_USB->println(v);
#define PRINT_VAR(v) SERIAL_USB->print(F(#v": ")); SERIAL_USB->print(v);

//////////////////////////////////////////////////////////////////////////////////////////
// various print functions
// we print to SERIAL_USB

// print a uint64_t int to serial, without any padding
void print_uint64(uint64_t to_print);

// print a number of different unsigned ints to serial, as 0x... format hex, with 0 padding
// for all these functions, signature is i) pointer to array of uints of given length,
// number of uints in the array.
void print_hex_u8s(uint8_t *data, size_t length);
void print_hex_u16s(uint16_t *data, size_t length);
void print_hex_u32s(uint32_t *data, size_t length);
void print_hex_u64s(uint64_t *data, size_t length);
// and these are printing a single such uint
void print_hex_u8(uint8_t data);
void print_hex_u16(uint16_t data);
void print_hex_u32(uint32_t data);
void print_hex_u64(uint64_t data);


#endif