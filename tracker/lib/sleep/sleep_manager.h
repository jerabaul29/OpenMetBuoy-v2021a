#ifndef SLEEP_MANAGER_H
#define SLEEP_MANAGER_H

#include "Arduino.h"
#include "kiss_posix_time_utils.hpp"
#include "firmware_configuration.h"
#include "time_manager.h"
#include "user_configuration.h"
#include "watchdog_manager.h"

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

// for now: start all I2Cs, Serials, SPIs etc when waking up, and stop all of these when
// done and ready to sleep... This way, no questions and no problems. Likely too many peripherals
// will be on, but this has quite limited consumption anyways... Another solution would be to
// turn on only what is needed when it is needed, but this is complicated to get right when sharing
// buses for several components for example... A possibility may be to derive from serial / i2c
// and count active peripherals, and use this to decide when to actually switch on / off.

//////////////////////////////////////////////////////////////////////////////////////////
// a few high level ways to control sleep

void sleep_for_seconds(unsigned long const number_of_seconds);

void sleep_until_posix(kiss_time_t const posix_timestamp);

//////////////////////////////////////////////////////////////////////////////////////////
// ideally the following would be "hidden" by not sharing in the header file, but for now
// keep it here, as we want to test it in the unit tests

unsigned long seconds_to_sleep_until_posix(kiss_time_t const posix_timestamp);

#endif