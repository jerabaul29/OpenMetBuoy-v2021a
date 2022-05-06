#include "sleep_manager.h"

// NOTE: the recommended source of sleep code is from the Artemis OpenLog
// https://github.com/sparkfun/OpenLog_Artemis/blob/69682123ea2ae3ceb3983a7d8260eaef6259a0e2/Firmware/OpenLog_Artemis/lowerPower.ino

//--------------------------------------------------------------------------------

void sleep_for_seconds(long number_of_seconds, int nbr_blinks){
  Serial.print(F("sleep for "));
  Serial.print(number_of_seconds);
  Serial.println(F(" seconds"));

  prepare_to_sleep();
  unsigned long int millis_lost {0};
  unsigned long int last_millis {0};

  for (long i=0; i<number_of_seconds; i++){
    am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
    wdt.restart();

    #if (PERFORM_SLEEP_BLINK == 1)
      last_millis = millis();
      if (i % interval_between_sleep_LED_blink_seconds == 0){
        for (int j=0; j<nbr_blinks; j++){
          pinMode(LED, OUTPUT);
          digitalWrite(LED, HIGH);
          delay(duration_sleep_LED_blink_milliseconds);
          digitalWrite(LED, LOW);
          delay(duration_sleep_LED_blink_milliseconds);
        }
      }
      millis_lost += millis() - last_millis;
      if (millis_lost > 1000UL){
        i++;
        millis_lost -= 1000UL;
      }
    #endif
  }

  wake_up();
}

void sleep_until_posix(time_t posix_timestamp){
  Serial.print(F("sleep until posix "));
  Serial.println((long)posix_timestamp);

  prepare_to_sleep();

  if (board_time_manager.posix_timestamp_is_valid()){
    while (board_time_manager.get_posix_timestamp() < posix_timestamp){
      am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
      wdt.restart();
    }
  }
  else{
    Serial.println(F("no valid posix timestamp, cannot sleep to posix; sleep for default 1 hour"));
    for (long i=0; i<3600; i++){
      am_hal_sysctrl_sleep(AM_HAL_SYSCTRL_SLEEP_DEEP);
      wdt.restart();
    }
  }

  wake_up();
}

//--------------------------------------------------------------------------------
void prepare_to_sleep(void){
  turn_gnss_off();
  turn_iridium_off();

  digitalWrite(busVoltageMonEN, LOW);
  digitalWrite(LED, LOW);

  turn_qwiic_switch_off();
  turn_thermistors_off();
  delay(50);

  Serial.println(F("power down ArtemisWire"));
  ArtemisWire.end();
  wdt.restart();
  delay(100);

  Serial.println(F("power down board"));
  Serial.println(F("------ SLEEP ------"));
  delay(100);
  Serial.flush();
  Serial.end();
  delay(50);

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

  //Power down Flash, SRAM, cache
  am_hal_pwrctrl_memory_deepsleep_powerdown(AM_HAL_PWRCTRL_MEM_ALL); // Power down all flash and cache
  am_hal_pwrctrl_memory_deepsleep_retain(AM_HAL_PWRCTRL_MEM_SRAM_384K); // Retain all SRAM

  //Keep the 32kHz clock running for RTC
  am_hal_stimer_config(AM_HAL_STIMER_CFG_CLEAR | AM_HAL_STIMER_CFG_FREEZE);
  am_hal_stimer_config(AM_HAL_STIMER_XTAL_32KHZ);
}

void wake_up(void){
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
  ap3_set_pin_to_analog(busVoltagePin);

  Serial.begin(baudrate_debug_serial);
  delay(100);
  Serial.println();
  Serial.println(F("------ WAKE UP ------"));
  Serial.println(F("powered up board"));

  ArtemisWire.begin();
  delay(100);
  ArtemisWire.setClock(1000000);
  delay(100);
  wdt.restart();
  Serial.println(F("started ArtemisWire"));

  Serial.println(F("start qwiic switch"));
  if (qwiic_switch.begin(ArtemisWire) == false){
      Serial.println(F("Qwiic Power Switch not detected at default I2C address. Please check wiring. Freezing."));
      while (true){;}
  }
  turn_qwiic_switch_off();

  setup_pins();
  turn_gnss_off();
  turn_iridium_off();
}
