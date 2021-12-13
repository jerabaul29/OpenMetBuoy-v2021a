#include "sleep_manager.h"

// NOTE: the recommended source of sleep code is from the Artemis OpenLog
// https://github.com/sparkfun/OpenLog_Artemis/blob/69682123ea2ae3ceb3983a7d8260eaef6259a0e2/Firmware/OpenLog_Artemis/lowerPower.ino

//--------------------------------------------------------------------------------
// a few low level functions

// use the HAL to put in low power mode

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
// some user-specific functions: remember to turn stuff off

//--------------------------------------------------------------------------------
// a few high level ways to control sleep

void sleep_for_seconds(unsigned long number_of_seconds){

}

void sleep_until_posix(kiss_time_t posix_timestamp){

}
