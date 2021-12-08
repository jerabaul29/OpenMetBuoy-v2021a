#ifndef TIME_MANAGER
#define TIME_MANAGER

#include "Arduino.h"

#include "kiss_posix_time_utils.hpp"
#include "kiss_posix_time_extras.hpp"

#include "firmware_configuration.h"
#include "print_utils.h"

// a class for managing time
// this is some wrappers around kiss_posix_time and the RTC HAL that allow to use the RTC
// to keep track of posix time and convert back and forth between posix time, calendar
// time.
class TimeManager{
  public:
    TimeManager();

    // set the posix timestamp to the given value in the TimeManager
    // this will allow the TimeManager to keep track of time, relatively
    // to this initial set value
    void set_posix_timestamp(kiss_time_t const crrt_posix_timestamp);

    // get the current posix timestamp from the time manager
    // note that this is only valid if posix_timestamp_is_valid!!
    kiss_time_t get_posix_timestamp(void) const;

    // whether the current posix timestamp provided by TimeManager is valid
    // it is valid if it has been set at least once
    // if not the timestamp is valid, it will actually count time since the
    // start of the board, relative to the unix epoch
    bool posix_timestamp_is_valid(void) const;

    // perform some serial print of the information in the TimeManager
    // will display posix and calendar time, as well as status
    void print_status(void) const;

  private:
    // set the low level RTC properties through the HAL to allow it to count
    // seconds in an interrupt-driven way
    void setup_RTC(void);

    // have we ever set a posix timestamp?
    bool posix_is_set;
};

// isr for the rtc, used to do interrupt based seconds counting
extern "C" void arm_rtc_isr(void);

// we need an isr modifiable posix_timestamp to count seconds
extern volatile kiss_time_t posix_timestamp;

// we use one single TimeManager instance for the board
extern TimeManager board_time_manager;

// we pre allocate all what we need to do timestamp conversions
extern kiss_time_t common_working_kiss_time;
extern kiss_calendar_time common_working_kiss_calendar;

#endif
