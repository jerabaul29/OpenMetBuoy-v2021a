#include "time_manager.h"

//--------------------------------------------------------------------------------
// our project-wide objects

volatile kiss_time_t posix_timestamp {0};

TimeManager board_time_manager {};

//--------------------------------------------------------------------------------
// the TimeManager class

// Actually there may be a catch here...
// kiss_time_t is uint32_t or uint64_t, so this may not be atomic to read / write, but it is
// also volatile and interrupt modified... So to be on the safe side, may need to
// make SURE that the read and write happen correctly! Otherwise we may have a
// read or write that is not a single operation and gets "intermixed" with an IRC
// update.
// To make sure, one method is to always do a read / write check. We know that it will be
// updated by the ISR only once per minute, so we will not fail often nor several
// times in a row. Another method is to simply wrap in a context with no interrupts.
// You can choose which method you want to use by choosing which #if branch to take.
// choose which method to use, NO_INTERRUPT, or READ_WRITE_CHECK

static constexpr int TIME_MANAGER_NO_INTERRUPT     {1};
static constexpr int TIME_MANAGER_READ_WRITE_CHECK {0};

static_assert(
  TIME_MANAGER_NO_INTERRUPT + TIME_MANAGER_READ_WRITE_CHECK == 1,
  "need to choose one and only one of the two race condition preventing methods"
);

TimeManager::TimeManager(): posix_is_set{false}{
  this->setup_RTC();
};

void TimeManager::set_posix_timestamp(kiss_time_t const crrt_posix_timestamp){
  if constexpr (TIME_MANAGER_READ_WRITE_CHECK){
    // write and read until we get a match, see comment about uint64_t and atomicity
    while (true){
      posix_timestamp = crrt_posix_timestamp;
      delayMicroseconds(4);
      if(posix_timestamp == crrt_posix_timestamp){
        break;
      }
    }
  }

  if constexpr (TIME_MANAGER_NO_INTERRUPT){
    // no interrupt to make sure we can read / write the volatile value without race condition
    noInterrupts();
    posix_timestamp = crrt_posix_timestamp;
    interrupts();
  }

  posix_is_set = true;
}

kiss_time_t TimeManager::get_posix_timestamp(void) const {
  kiss_time_t value_read;

  if constexpr (TIME_MANAGER_READ_WRITE_CHECK){
    // read until we get 2 consecutive identical values, see comment about uint64_t
    // and atomicity
    delayMicroseconds(4);
    value_read = posix_timestamp;

    while (true){
      if (posix_timestamp == value_read){
        break;
      }
      else {
        value_read = posix_timestamp;
      }
      delayMicroseconds(4);
    }
  }

  if constexpr (TIME_MANAGER_NO_INTERRUPT){
    // no interrupt to make sure we can read / write the volatile value without race condition
    noInterrupts();
    value_read = posix_timestamp;
    interrupts();
  }

  return value_read;
}

bool TimeManager::posix_timestamp_is_valid(void) const {
  return posix_is_set;
}

void TimeManager::print_status(void) const {
  SERIAL_USB->println(F("- TimeManager -"));
  PRINTLN_VAR(posix_is_set)
  SERIAL_USB->print(F("posix_timestamp: ")); SERIAL_USB->print(get_posix_timestamp()); SERIAL_USB->println();
  constexpr size_t utils_char_buffer_size {24};
  char utils_char_buffer[utils_char_buffer_size] {'\0'};
  print_iso(get_posix_timestamp(), utils_char_buffer, utils_char_buffer_size);
  SERIAL_USB->print(F("gregorian: ")); SERIAL_USB->print(utils_char_buffer); SERIAL_USB->println();
  SERIAL_USB->println(F("---------------"));
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

  posix_timestamp += 1;
}
