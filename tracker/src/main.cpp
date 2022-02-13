#include <Arduino.h>

#include "firmware_configuration.h"
#include "user_configuration.h"

#include "time_manager.h"

#include "sleep_manager.h"

#include "watchdog_manager.h"

void setup() {
  // WDT has 1 Hz frequency and raises interrupt after 32 ticks and resets after 32 ticks,
  // i.e. 32 seconds reset time.
  wdt.configure(WDT_1HZ, 32, 32);
  wdt.start();
  
  SERIAL_USB->begin(BAUD_RATE_USB);

  print_firmware_config();
  wdt.restart();
  print_all_user_configs();
  wdt.restart();
}

void loop() {
  // put your main code here, to run repeatedly:
}