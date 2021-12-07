#include "time/time_manager.h"

//--------------------------------------------------------------------------------
// RTC alarm Interrupt Service Routine

extern "C" void am_rtc_isr(void)
{
  // Clear the RTC alarm interrupt.
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);

  // Increment seconds_count
  posix_timestamp += 1;
}

//--------------------------------------------------------------------------------
// our project-wide objects

volatile kiss_time_t posix_timestamp {0};

TimeManager board_time_manager {};

kiss_time_t common_working_kiss_time;
kiss_calendar_time common_working_kiss_calendar;

//--------------------------------------------------------------------------------
// the TimeManager class

TimeManager::TimeManager(): posix_is_set{false}{
  this->setup_RTC();
};

void TimeManager::set_posix_timestamp(kiss_time_t const crrt_posix_timestamp){
  posix_timestamp = crrt_posix_timestamp;
  posix_is_set = true;
}

kiss_time_t TimeManager::get_posix_timestamp(void) const {
  return posix_timestamp;
}

bool TimeManager::posix_timestamp_is_valid(void) const {
  return posix_is_set;
}

void TimeManager::print_status(void) const {
  SERIAL_USB.println(F("- TimeManager -"));
  PRINTLN_VAR(posix_is_set)
  Serial.print(F("posix_timestamp: ")); print_uint64(get_posix_timestamp()); Serial.println();
  // Serial.print(F("posix_timestamp: ")); SERIAL_USB.print(static_cast<long int>(get_posix_timestamp())); Serial.println();
  print_iso(posix_timestamp, utils_char_buffer, utils_char_buffer_size);
  Serial.print(F("gregorian: ")); Serial.print(utils_char_buffer); Serial.println();
  clear_utils_char_buffer();
  SERIAL_USB.println(F("---------------"));
}

/*
void TimeManager::print_status(void) const {
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
*/

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
