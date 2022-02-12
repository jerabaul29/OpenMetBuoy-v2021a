#ifndef USER_CONFIGURATION_H
#define USER_CONFIGURATION_H

//////////////////////////////////////////////////////////////////////////////////////////
// sleep setup

// should we blink during sleep?
constexpr bool blink_during_sleep {true};
// how many seconds between 2 blinks?
constexpr unsigned long seconds_between_sleep_blink {120UL};
// how long should a blink flash last?
constexpr unsigned long millis_duration_sleep_blink {350UL};

// sanity checks
static_assert(seconds_between_sleep_blink > 10);  // do not blink too often
static_assert(millis_duration_sleep_blink > 150);  // blink long enough to be visible
static_assert(millis_duration_sleep_blink < 960);  // blink short enough to be kind to battery

void print_sleep_configs(void);

//////////////////////////////////////////////////////////////////////////////////////////
// print all configs

void print_all_user_configs(void);


#endif