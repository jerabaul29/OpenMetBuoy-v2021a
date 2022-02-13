#ifndef SLEEP_MANAGER_H
#define SLEEP_MANAGER_H

#include "Arduino.h"
#include "kiss_posix_time_utils.hpp"
#include "firmware_configuration.h"
#include "time_manager.h"
#include "user_configuration.h"
#include "watchdog_manager.h"


// NOTE: could put in an own namespace, but a bit heavier to use

//////////////////////////////////////////////////////////////////////////////////////////
// a few high level ways to control sleep

void sleep_for_seconds(unsigned long const number_of_seconds);

void sleep_until_posix(kiss_time_t const posix_timestamp);

//////////////////////////////////////////////////////////////////////////////////////////
// ideally the following would be "hidden" by not sharing in the header file, but for now
// keep it here, as we want to test it in the unit tests

unsigned long seconds_to_sleep_until_posix(kiss_time_t const posix_timestamp);

//////////////////////////////////////////////////////////////////////////////////////////
// default values as "safety guards"; we do not want to make a mistake and start sleeping
// for thousands of years...
// these are part of the user configuration, see there the section "sleep setup"

//////////////////////////////////////////////////////////////////////////////////////////
// the user may add own routines to run pre (just before) / post (just after) sleep by
// filling the functions (these will override weak default empty functions):
// void user_sleep_pre_actions(void);
// void user_sleep_post_actions(void);

#endif