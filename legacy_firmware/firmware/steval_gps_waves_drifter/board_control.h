#ifndef BOARD_CONTROL
#define BOARD_CONTROL

#include "Arduino.h"

#include "params.h"
#include "watchdog_manager.h"
#include "sleep_manager.h"

#include "Wire.h"
#include <SparkFun_Qwiic_Power_Switch_Arduino_Library.h>

//--------------------------------------------------------------------------------
// Artemis Tracker pin definitions
// TODO: move to constexpr
#define spiCS1             4  // D4 can be used as an SPI chip select or as a general purpose IO pin
#define geofencePin        10 // Input for the ZOE-M8Q's PIO14 (geofence) pin
#define busVoltagePin      13 // Bus voltage divided by 3 (Analog in)
#define iridiumSleep       17 // Iridium 9603N ON/OFF (sleep) pin: pull high to enable the 9603N
#define iridiumNA          18 // Input for the Iridium 9603N Network Available
#define LED                19 // White LED
#define iridiumPwrEN       22 // ADM4210 ON: pull high to enable power for the Iridium 9603N
#define gnssEN             26 // GNSS Enable: pull low to enable power for the GNSS (via Q2)
#define superCapChgEN      27 // LTC3225 super capacitor charger: pull high to enable the super capacitor charger
#define superCapPGOOD      28 // Input for the LTC3225 super capacitor charger PGOOD signal
#define busVoltageMonEN    34 // Bus voltage monitor enable: pull high to enable bus voltage monitoring (via Q4 and Q3)
#define spiCS2             35 // D35 can be used as an SPI chip select or as a general purpose IO pin
#define iridiumRI          41 // Input for the Iridium 9603N Ring Indicator
// Make sure you do not have gnssEN and iridiumPwrEN enabled at the same time!
// If you do, bad things might happen to the AS179 RF switch!

// custom pins for the thermistors
#define THERMISTORS_ONE_WIRE_PIN 35  // the data pin; I suggest to put a 100 Ohm between the pin and the sensors; this way, if a cable is cut / shorted, will not burn the pin
#define THERMISTORS_POWER_PIN 4  // the power pin; use a digital pin to be able to switch power on and off
                          // I suggest putting a 100 ohm resistor between the pin out and the sensors; this way, if a cable is cut / shorted, will not burn the pin
                          // 100 ohm sees to work still fine

void setup_pins(void);

void turn_on(void);

//--------------------------------------------------------------------------------
// a few low-level functions to control power to GNSS vs Iridium

void turn_gnss_on(void);

void turn_gnss_off(void);

void turn_iridium_on(void);

void turn_iridium_off(void);

//--------------------------------------------------------------------------------
// some user-convenience functions

void blink_LED_n_times(unsigned int number_of_blinks, float frequency_hz=1.0);

//--------------------------------------------------------------------------------
// functions to save power on the Qwiic switch

void turn_qwiic_switch_on(void);

void turn_qwiic_switch_off(void);

//--------------------------------------------------------------------------------
// functions to turn on / off the thermistors

void turn_thermistors_on(void);

void turn_thermistors_off(void);

//--------------------------------------------------------------------------------
//making the ArtemisWire Qwiic port and Qwiic switch available to all 

extern TwoWire ArtemisWire;
extern QWIIC_POWER qwiic_switch;

#endif
