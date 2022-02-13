#include <Arduino.h>

#include "firmware_configuration.h"

#include "time_manager.h"

#include "peripheral_manager.h"

void setup() {
  // we put a bit of prelude, in order to always:
  // use watchdog
  // start the USB at baudrate 1000000 for external communication  # TODO: make sure to start and stop with sleep
  // print the firmware config et co, and user settings

  SERIAL_USB->begin(BAUD_RATE_USB);

  print_firmware_config();

}

void loop() {
  // put your main code here, to run repeatedly:
}