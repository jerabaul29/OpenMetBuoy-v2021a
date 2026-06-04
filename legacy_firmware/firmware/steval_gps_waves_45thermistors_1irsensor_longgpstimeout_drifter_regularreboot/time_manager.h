#ifndef TIME_MANAGER
#define TIME_MANAGER

#include "Arduino.h"
#include "TimeLib.h"
#include "helpers.h"

class TimeManager{
  public:
    TimeManager();

    void set_posix_timestamp(long crrt_posix_timestamp);
    long get_posix_timestamp(void) const;
    bool posix_timestamp_is_valid(void) const;
    void print_posix_status(void) const;

  private:
    void setup_RTC(void);

    bool posix_is_set;
};

extern "C" void arm_rtc_isr(void);

struct struct_YMDHMS{
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
};

// this is using conventions where month and day start at 1, as is common
time_t& posix_timestamp_from_YMDHMS(int year, int month, int day, int hour, int minute, int second);
struct_YMDHMS& YMDHMS_from_posix_timestamp(time_t posix_timestamp);

extern TimeManager board_time_manager;
extern volatile time_t posix_timestamp; // a timestamp

extern tmElements_t common_working_time_struct;
extern time_t common_working_posix_timestamp;
extern struct_YMDHMS common_working_struct_YMDHMS;

#endif
