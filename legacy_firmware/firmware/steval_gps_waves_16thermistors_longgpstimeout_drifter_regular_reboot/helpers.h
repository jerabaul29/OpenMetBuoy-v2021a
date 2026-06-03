#ifndef HELPERS
#define HELPERS

#include "Arduino.h"
#include "stdarg.h"

#include "etl.h"
#include "etl/vector.h"

#include "ard_supers/avr/dtostrf.h"

//--------------------------------------------------------------------------------
// common printing buffer

constexpr size_t serial_print_max_buffer = 256;
extern char serial_print_buff[serial_print_max_buffer];

//--------------------------------------------------------------------------------

// variadic serial print using printf formatting
void serialPrintf(const char *fmt, ...);

// print etl containers
void print_vector_uc(const etl::ivector<unsigned char>& vector);

// a couple of fixed width prints
void serial_print_int_width_4(int const input);
void serial_print_uint16_width_5(uint16_t const input);
void serial_print_float_width_16_prec_8(float const input);
void serial_print_float_width_24_prec_8(float const input);

#endif
