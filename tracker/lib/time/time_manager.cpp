#include "time_manager.h"

//--------------------------------------------------------------------------------
// our project-wide objects

volatile kiss_time_t posix_timestamp {0};

TimeManager board_time_manager {};

kiss_time_t common_working_kiss_time;
kiss_calendar_time common_working_kiss_calendar;

//--------------------------------------------------------------------------------
// the TimeManager class

// Actually there may be a catch here...
// kiss_time_t is uint64_t, so this may not be atomic to read / write, but it is
// also volatile and interrupt modified... So to be on the safe side, may need to
// make SURE that the read and write happen correctly! Otherwise we may have a
// read or write that is not a single operation and gets "intermixed" with an IRC
// update. To make sure, always do a read / write a check. We know that it will be
// updated by the ISR only once per minute, so we will not fail often nor several
// times in a row.

TimeManager::TimeManager(): posix_is_set{false}{
  this->setup_RTC();
};

void TimeManager::set_posix_timestamp(kiss_time_t const crrt_posix_timestamp){
  // write and read until we get a match, see comment about uint64_t and atomicity
  while (true){
    posix_timestamp = crrt_posix_timestamp;
    delayMicroseconds(4);
    if(posix_timestamp == crrt_posix_timestamp){
      break;
    }
  }
  posix_is_set = true;
}

kiss_time_t TimeManager::get_posix_timestamp(void) const {
  // read until we get 2 consecutive identical values, see comment about uint64_t
  // and atomicity

  kiss_time_t value_read_1 = posix_timestamp;
  delayMicroseconds(4);

  while (true){
    if (posix_timestamp == value_read_1){
      break;
    }
    else {
      value_read_1 = posix_timestamp;
    }
    delayMicroseconds(4);
  }

  return value_read_1;
}

bool TimeManager::posix_timestamp_is_valid(void) const {
  return posix_is_set;
}

void TimeManager::print_status(void) const {
  SERIAL_USB.println(F("- TimeManager -"));
  PRINTLN_VAR(posix_is_set)
  Serial.print(F("posix_timestamp: ")); print_uint64(get_posix_timestamp()); Serial.println();
  // Serial.print(F("posix_timestamp: ")); SERIAL_USB.print(static_cast<long int>(get_posix_timestamp())); Serial.println();
  print_iso(get_posix_timestamp(), utils_char_buffer, utils_char_buffer_size);
  Serial.print(F("gregorian: ")); Serial.print(utils_char_buffer); Serial.println();
  clear_utils_char_buffer();
  SERIAL_USB.println(F("---------------"));
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

//--------------------------------------------------------------------------------
// RTC alarm Interrupt Service Routine

extern "C" void am_rtc_isr(void)
{
  // Clear the RTC alarm interrupt.
  am_hal_rtc_int_clear(AM_HAL_RTC_INT_ALM);

  // Increment seconds_count
  posix_timestamp += 1;
}
