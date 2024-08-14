#ifndef SLEEP_MANAGER
#define SLEEP_MANAGER

#include "Arduino.h"
#include "board_control.h"
#include "time_manager.h"
#include "watchdog_manager.h"
#include "params.h"

//--------------------------------------------------------------------------------
// a few high level ways to control sleep
void sleep_for_seconds(long number_of_seconds, int nbr_blinks=1);
void sleep_until_posix(time_t posix_timestamp);

//--------------------------------------------------------------------------------
// a few low level functions
void prepare_to_sleep(void);
void wake_up(void);

#endif
