#include <Arduino.h>

#include "firmware_configuration.h"

#include "time_manager.h"

void setup() {
  SERIAL_USB->begin(BAUD_RATE_USB);

  print_firmware_config();
}

void loop() {
  // put your main code here, to run repeatedly:
}