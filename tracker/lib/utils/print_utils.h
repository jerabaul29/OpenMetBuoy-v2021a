#ifndef PRINT_UTILS
#define PRINT_UTILS

#include "Arduino.h"

#include "firmware_configuration.h"

//////////////////////////////////////////////////////////////////////////////////////////
// convenience macros

#define PRINTLN_VAR(v) SERIAL_USB.print(F(#v": ")); SERIAL_USB.println(v);
#define PRINT_VAR(v) SERIAL_USB.print(F(#v": ")); SERIAL_USB.print(v);

//////////////////////////////////////////////////////////////////////////////////////////
// various print functions
// we print to SERIAL_USB

// print a uint64_t int to serial, without any padding
void print_uint64(uint64_t to_print);

#endif