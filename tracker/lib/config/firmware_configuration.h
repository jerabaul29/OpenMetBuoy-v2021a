#ifndef FIRMWARE_CONFIGURATION
#define FIRMWARE_CONFIGURATION

#include "Arduino.h"
#include "macro_utils.h"
#include "print_utils.h"

//////////////////////////////////////////////////////////////////////////////////////////
// serial related 

extern Uart * SERIAL_USB;
static constexpr int BAUD_RATE_USB {1000000};

//////////////////////////////////////////////////////////////////////////////////////////
// pins on the PCB

static constexpr int spiCS1          = 4 ; // D4 can be used as an SPI chip select or as a general purpose IO pin
static constexpr int geofencePin     = 10; // Input for the ZOE-M8Q's PIO14 (geofence) pin
static constexpr int busVoltagePin   = 13; // Bus voltage divided by 3 (Analog in)
static constexpr int iridiumSleep    = 17; // Iridium 9603N ON/OFF (sleep) pin: pull high to enable the 9603N
static constexpr int iridiumNA       = 18; // Input for the Iridium 9603N Network Available
static constexpr int LED             = 19; // White LED
static constexpr int iridiumPwrEN    = 22; // ADM4210 ON: pull high to enable power for the Iridium 9603N
static constexpr int gnssEN          = 26; // GNSS Enable: pull low to enable power for the GNSS (via Q2)
static constexpr int superCapChgEN   = 27; // LTC3225 super capacitor charger: pull high to enable the super capacitor charger
static constexpr int superCapPGOOD   = 28; // Input for the LTC3225 super capacitor charger PGOOD signal
static constexpr int busVoltageMonEN = 34; // Bus voltage monitor enable: pull high to enable bus voltage monitoring (via Q4 and Q3)
static constexpr int spiCS2          = 35; // D35 can be used as an SPI chip select or as a general purpose IO pin
static constexpr int iridiumRI       = 41; // Input for the Iridium 9603N Ring Indicator

//////////////////////////////////////////////////////////////////////////////////////////
// misc

static constexpr char commit_id[] {STRINGIFY_CONTENT(REPO_COMMIT_ID)};
static constexpr char git_branch[] {STRINGIFY_CONTENT(REPO_GIT_BRANCH)};

//////////////////////////////////////////////////////////////////////////////////////////
// functions

void print_firmware_config(void);

uint64_t read_chip_id(void);

#endif