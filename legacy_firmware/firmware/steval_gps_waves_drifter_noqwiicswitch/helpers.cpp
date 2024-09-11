#include "helpers.h"

/* Buffer for storing the formatted data */
char serial_print_buff[serial_print_max_buffer];

void serialPrintf(const char *fmt, ...) {
  /* pointer to the variable arguments list */
  va_list pargs;
  /* Initialise pargs to point to the first optional argument */
  va_start(pargs, fmt);
  /* create the formatted data and store in buff */
  vsnprintf(serial_print_buff, serial_print_max_buffer, fmt, pargs);
  va_end(pargs);
  Serial.print(serial_print_buff);
}

void print_vector_uc(const etl::ivector<unsigned char>& vector){
    Serial.println(F("--------------------"));
    Serial.print(F("vector of unsigned char, size "));
    Serial.print(vector.size());
    Serial.print(F(" max size "));
    Serial.println(vector.max_size());

    Serial.print(F("content: "));

    for (const unsigned char crrt_char : vector){
        Serial.print(F("0x"));
        Serial.print(crrt_char, HEX);
        Serial.print(F(" | "));
    }
    Serial.println();

    Serial.print(F("Rock7 format content: "));
    for (const unsigned char crrt_char : vector){
      snprintf(serial_print_buff, serial_print_max_buffer, "%02X", crrt_char);
      Serial.print(serial_print_buff);
    }
    Serial.println();

    Serial.println(F("--------------------"));
}

void serial_print_int_width_4(int const input){
  snprintf(serial_print_buff, serial_print_max_buffer, "%04i", input);
  Serial.print(serial_print_buff);
}

void serial_print_float_width_16_prec_8(float const input){ 
  dtostrf(input, 16, 8, serial_print_buff);
  Serial.print(serial_print_buff);
}

void serial_print_float_width_24_prec_8(float const input){ 
  dtostrf(input, 24, 8, serial_print_buff);
  Serial.print(serial_print_buff);
}

void serial_print_uint16_width_5(uint16_t const input){
  snprintf(serial_print_buff, serial_print_max_buffer, "%05u", input);
  Serial.print(serial_print_buff);
}

