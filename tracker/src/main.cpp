#include <Arduino.h>

#include "firmware_configuration.h"
#include "user_configuration.h"

#include "time_manager.h"

#include "sleep_manager.h"

void setup() {
  SERIAL_USB->begin(BAUD_RATE_USB);

  print_firmware_config();
  print_all_user_configs();
}

void loop() {
  // put your main code here, to run repeatedly:
}