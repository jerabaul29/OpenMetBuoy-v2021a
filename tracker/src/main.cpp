#include <Arduino.h>

#include "firmware_configuration.h"

#include "utils/print_utils.h"

#include "time/time_manager.h"

void setup() {
  SERIAL_USB.begin(BAUD_RATE_USB);

  board_time_manager.print_status();
  board_time_manager.set_posix_timestamp(12345678901);
  board_time_manager.print_status();
  delay(5050);
  board_time_manager.print_status();
}

void loop() {
  // put your main code here, to run repeatedly:
}