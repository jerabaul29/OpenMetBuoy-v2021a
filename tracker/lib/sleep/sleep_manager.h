#ifndef SLEEP_MANAGER_H
#define SLEEP_MANAGER_H

#include "Arduino.h"
#include "kiss_posix_time_utils.hpp"
#include "firmware_configuration.h"

//////////////////////////////////////////////////////////////////////////////////////////
// keeping track of what is on and when

// For now we use a very simple system: use a set of unsigned short to keep track of which
// peripherals are on or not. If a peripheral_enabled entry is 0, the corresponding
// peripheral is not in use; if it is superior to 0, it is in use by as many "things" as
// the value.

// If programming for a multi threaded or multi cpu program / mcu, this should be atomic;
// for now, interrupts are our only source of concurrency, so it will be enough to have
// simple quantites; still, put an atomic on it, just to be on the safe side in the
// future.

struct peripherals_enabled {
    // serials
    unsigned char serial_usb {0};  // the serial connected to USB
    unsigned char serial_1 {0};  // the serial connected to the iridium modem

    // i2c
    // the i2c connected to the gnss
    // the i2c connected to the qwiic connector

    // spi

};

//////////////////////////////////////////////////////////////////////////////////////////
// a few high level ways to control sleep

void sleep_for_seconds(unsigned long number_of_seconds);

void sleep_until_posix(kiss_time_t posix_timestamp);

#endif