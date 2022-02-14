#include "sleep_manager.h"

// NOTE: the recommended source of sleep code is from the Artemis OpenLog
// https://github.com/sparkfun/OpenLog_Artemis/blob/69682123ea2ae3ceb3983a7d8260eaef6259a0e2/Firmware/OpenLog_Artemis/lowerPower.ino

//--------------------------------------------------------------------------------
// default, weak functions for the user actions

void user_sleep_pre_actions(void)  __attribute__((weak));
void user_sleep_post_actions(void)  __attribute__((weak));

void user_sleep_pre_actions(void) {};
void user_sleep_post_actions(void) {};

//--------------------------------------------------------------------------------
// a few low level functions

// use the hal to put in low power mode
void hal_prepare_to_sleep(void){
  digitalWrite(busVoltageMonEN, LOW);
  digitalWrite(LED, LOW);

  // Turn off ADC
  power_adc_disable();
    
  // Disabling the debugger GPIOs saves about 1.2 uA total:
  am_hal_gpio_pinconfig(20 /* SWDCLK */, g_AM_HAL_GPIO_DISABLE);
  am_hal_gpio_pinconfig(21 /* SWDIO */, g_AM_HAL_GPIO_DISABLE);

  // These two GPIOs are critical: the TX/RX connections between the Artemis module and the CH340S
  // are prone to backfeeding each other. To stop this from happening, we must reconfigure those pins as GPIOs
  // and then disable them completely:
  am_hal_gpio_pinconfig(48 /* TXO-0 */, g_AM_HAL_GPIO_DISABLE);
  am_hal_gpio_pinconfig(49 /* RXI-0 */, g_AM_HAL_GPIO_DISABLE);

  // Power down Flash, SRAM, cache
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_ALL); // Power down all flash and cache
  am_hal_pwrctrl_memory_deepsleep_retain(AM_HAL_PWRCTRL_MEM_SRAM_384K); // Retain all SRAM

  // Keep the 32kHz clock running for RTC
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_XTAL_32KHZ);
}

// use the hal to wakeup
void hal_wake_up(void){
  //Power up SRAM, turn on entire Flash
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_MAX);

  //Go back to using the main clock
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_HFRC_3MHZ);

  // Restore the TX/RX connections between the Artemis module and the CH340S on the Blackboard
  am_hal_gpio_pinconfig(48 /* TXO-0 */, g_AM_BSP_GPIO_COM_UART_TX);
  am_hal_gpio_pinconfig(49 /* RXI-0 */, g_AM_BSP_GPIO_COM_UART_RX);

  // Reenable the debugger GPIOs
  am_hal_gpio_pinconfig(20 /* SWDCLK */, g_AM_BSP_GPIO_SWDCK);
  am_hal_gpio_pinconfig(21 /* SWDIO */, g_AM_BSP_GPIO_SWDIO);

  //Turn on ADC
  ap3_adc_setup();
}

//--------------------------------------------------------------------------------
// a few high level ways to control sleep

void sleep_for_seconds(unsigned long const number_of_seconds){
  if (number_of_seconds > max_sleep_seconds){
    PRINT_VAR(number_of_seconds);
    SERIAL_USB->println(F(" is suspicious; cut!"));
    sleep_for_seconds(default_error_sleep_seconds);
    return;
  }

  SERIAL_USB->print(F("sleep for ")); SERIAL_USB->print(number_of_seconds); SERIAL_USB->println(F(" seconds"));

  hal_prepare_to_sleep();
  user_sleep_pre_actions();

  unsigned long int millis_lost {0};
  unsigned long int millis_start_blink {0};

  for (unsigned long i=0; i<number_of_seconds; i++){
    am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    wdt.restart();

    // making sure to blink
    if constexpr (blink_during_sleep){
      millis_start_blink = millis();
      if (i % seconds_between_sleep_blink == 0){
        pinMode(LED, OUTPUT);
        digitalWrite(LED, HIGH);
        delay(millis_duration_sleep_blink);
        digitalWrite(LED, LOW);
        pinMode(LED, INPUT);
      }
      millis_lost += millis() - millis_start_blink;
      if (millis_lost > 1000UL){
        i++;
        millis_lost -= 1000UL;
      }
    }
  }

  hal_wake_up();
  user_sleep_post_actions();

  SERIAL_USB->print(F("wakeup"));
}

unsigned long seconds_to_sleep_until_posix(kiss_time_t const posix_timestamp){
  unsigned long number_seconds_to_sleep {0};

  if (board_time_manager.posix_timestamp_is_valid()){
    number_seconds_to_sleep = posix_timestamp - board_time_manager.get_posix_timestamp();
    if (number_seconds_to_sleep > max_sleep_seconds){
      SERIAL_USB->println(F("W suspicious posix sleep duration; cut!"));
      number_seconds_to_sleep = default_error_sleep_seconds;
    }
  }
  else{
    SERIAL_USB->println(F("W invalid posix; sleep default duration"));
    number_seconds_to_sleep = default_error_sleep_seconds;
  }

  return number_seconds_to_sleep;
}

void sleep_until_posix(kiss_time_t const posix_timestamp){
  SERIAL_USB->print(F("Sleep until posix ")); SERIAL_USB->println(posix_timestamp);

  unsigned long number_seconds_to_sleep = seconds_to_sleep_until_posix(posix_timestamp);
  sleep_for_seconds(number_seconds_to_sleep);
}
