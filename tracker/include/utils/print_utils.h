#ifndef PRINT_UTILS
#define PRINT_UTILS

#include "Arduino.h"

#include "firmware_configuration.h"

//////////////////////////////////////////////////////////////////////////////////////////
// convenience macros

#define PRINTLN_VAR(v) SERIAL_USB.print(F(#v": ")); SERIAL_USB.println(v);
#define PRINT_VAR(v) SERIAL_USB.print(F(#v": ")); SERIAL_USB.print(v);

//////////////////////////////////////////////////////////////////////////////////////////
// various functions
// we print to SERIAL_USB

// print a uint64_t int to serial, without any padding
// this needs a utils_char_buffer_size that is large enough;
// anything above 20 will be ok; document lower down
void print_uint64(uint64_t to_print);

//////////////////////////////////////////////////////////////////////////////////////////
// the utils char buffer: a buffer for performing formatting etc
// having 1, statically allocated buffer, to make RAM use as constant and effective
// as possible
// be polite, always leave the buffer filled with \0s after you are done!
// (but be defensive, do not rely on this, and null the buffer after and before use!)

// fill the utils char buffer with only null bytes
void clear_utils_char_buffer(void);

// buffer and its properties
static constexpr size_t utils_char_buffer_size {64};
static_assert(utils_char_buffer_size >= 20, "need a buffer large enough to print uint64_t");
extern char utils_char_buffer[utils_char_buffer_size];

#endif