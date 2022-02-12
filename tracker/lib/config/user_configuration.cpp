#include "print_utils.h"
#include "user_configuration.h"

void print_sleep_configs(void){
    SERIAL_USB->println(F("-- sleep config start --"));
    PRINTLN_VAR(blink_during_sleep);
    PRINTLN_VAR(seconds_between_sleep_blink);
    PRINTLN_VAR(millis_duration_sleep_blink);
    SERIAL_USB->println(F("-- sleep config end   --"));
    delay(10);
}

void print_all_user_configs(void){
    SERIAL_USB->println(F("***** all user configs start *****"));
    print_sleep_configs();
    SERIAL_USB->println(F("***** all user configs end   *****"));
    delay(10);
}