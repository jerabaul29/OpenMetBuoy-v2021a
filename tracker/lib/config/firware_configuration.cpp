#include "firmware_configuration.h"

//////////////////////////////////////////////////////////////////////////////////////////
// serial related

Uart * SERIAL_USB {&Serial};

//////////////////////////////////////////////////////////////////////////////////////////
// functions

void print_firmware_config(void){
    // generate date, time, version
    SERIAL_USB->print(F("Compiled: "));
    SERIAL_USB->print(F(__DATE__));
    SERIAL_USB->print(F(", "));
    SERIAL_USB->print(F(__TIME__));
    SERIAL_USB->print(F(", compiler version "));
    SERIAL_USB->println(F(__VERSION__));
    SERIAL_USB->print(F("Commit ID: "));
    SERIAL_USB->println(F(commit_id));
}