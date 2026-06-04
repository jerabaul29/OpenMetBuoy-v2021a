#include "time_manager.h"

TimeManager board_time_manager {};
volatile time_t posix_timestamp {0};

//--------------------------------------------------------------------------------
TimeManager::TimeManager(): posix_is_set{false}{
  this->setup_RTC();
};

void TimeManager::set_posix_timestamp(long crrt_posix_timestamp){
  posix_timestamp = crrt_posix_timestamp;
  posix_is_set = true;
}

long TimeManager::get_posix_timestamp(void) const {
  return posix_timestamp;
}

bool TimeManager::posix_timestamp_is_valid(void) const {
  return posix_is_set;
}

void TimeManager::print_posix_status(void) const {
  Serial.print(F("posix is set: "));
  if (posix_is_set){
    Serial.print(F("true"));
  }
  else{
    Serial.print(F("false"));
  }

  Serial.print(F(" | value = "));
  Serial.println((long)posix_timestamp);
  Serial.print(F("i.e.: "));
  common_working_struct_YMDHMS = YMDHMS_from_posix_timestamp(posix_timestamp);
  serialPrintf(
    "%04d-%02d-%02d %02d:%02d:%02d",
    common_working_struct_YMDHMS.year,
    common_working_struct_YMDHMS.month,
    common_working_struct_YMDHMS.day,
    common_working_struct_YMDHMS.hour,
    common_working_struct_YMDHMS.minute,
    common_working_struct_YMDHMS.second
  );
  Serial.println();
}

//--------------------------------------------------------------------------------
// low level control: RTC setup and 1 second interrupt

// Set up the RTC to generate interrupts every second
void TimeManager::setup_RTC(void)
{
  // Enable the XT for the RTC.
  am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_XTAL_START, 0);
  
  // Select XT for RTC clock source
  am_hal_rtc_osc_select(AM_HAL_RTC_OSC_XT);
  
  // Enable the RTC.
  am_hal_rtc_osc_enable();
  
  // Set the alarm interval to 1 second
  am_hal_rtc_alarm_interval_set(AM_HAL_RTC_ALM_RPT_SEC);
  
  // Clear the RTC alarm interrupt.
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);
  
  // Enable the RTC alarm interrupt.
  am_hal_rtc_int_enable(AM_HAL_RTC_INT_ALM);
  
  // Enable RTC interrupts to the NVIC.
  NVIC_EnableIRQ(RTC_IRQn);

  // Enable interrupts to the core.
  am_hal_interrupt_master_enable();
}

// RTC alarm Interrupt Service Routine
extern "C" void am_rtc_isr(void)
{
  // Clear the RTC alarm interrupt.
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);

  // Increment seconds_count
  posix_timestamp += 1;
}

//--------------------------------------------------------------------------------
tmElements_t common_working_time_struct;
time_t common_working_posix_timestamp;
struct_YMDHMS common_working_struct_YMDHMS;

time_t& posix_timestamp_from_YMDHMS(int year, int month, int day, int hour, int minute, int second){
  common_working_time_struct.Year = year - 1970; // years since 1970, so deduct 1970
  common_working_time_struct.Month = month;  // months start from 0, so deduct 1
  common_working_time_struct.Day = day;
  common_working_time_struct.Hour = hour;
  common_working_time_struct.Minute = minute;
  common_working_time_struct.Second = second;

  common_working_posix_timestamp =  makeTime(common_working_time_struct);
  return common_working_posix_timestamp;
}

struct_YMDHMS& YMDHMS_from_posix_timestamp(time_t posix_timestamp){
  breakTime(posix_timestamp, common_working_time_struct);
  common_working_struct_YMDHMS.year = 1970 + static_cast<int>(common_working_time_struct.Year);
  common_working_struct_YMDHMS.month = static_cast<int>(common_working_time_struct.Month);
  common_working_struct_YMDHMS.day = static_cast<int>(common_working_time_struct.Day);
  common_working_struct_YMDHMS.hour = static_cast<int>(common_working_time_struct.Hour);
  common_working_struct_YMDHMS.minute = static_cast<int>(common_working_time_struct.Minute);
  common_working_struct_YMDHMS.second = static_cast<int>(common_working_time_struct.Second);
  return common_working_struct_YMDHMS;
}


