#include "Arduino.h"

#include "params.h"
#include "time_manager.h"
#include "sleep_manager.h"
#include "gnss_manager.h"
#include "watchdog_manager.h"
#include "iridium_manager.h"
#include "board_control.h"
#include "imu_manager.h"
#include "data_manager.h"
#include "wave_analyzer.h"
#include "thermistors_manager.h"

void setup(){
  // --------------------------------------------------------------------------------
  // startup and configuration...
  delay(1000);

  // first set up the watchdog
  // WDT has 1 Hz frequency and raises interrupt after 32 ticks and resets after 32 ticks,
  // i.e. 32 seconds reset time.
  wdt.configure(WDT_1HZ, 32, 32);
  wdt.start();

  // turn_on is needed to make sure all board pins are in correct state
  // it turns off GNSS and Iridium, sets all pins to control the board, and starts debug serial
  turn_on();
  wdt.restart();
  Serial.flush();

  // to test the functionalities: sleep, thermistors
  Serial.println(F("----- START -----"));
  board_thermistors_manager.start();
  
  Serial.println(F("----- COLLECT -----"));
  board_thermistors_manager.collect_thermistors_conversions();
  
  board_thermistors_manager.stop();
  Serial.println(F("----- DONE -----"));
}

void loop(){
  //--------------------------------------------------------------------------------
  delay(1000);
  wdt.restart();
}
