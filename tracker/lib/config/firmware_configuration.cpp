#include "firmware_configuration.h"

//////////////////////////////////////////////////////////////////////////////////////////
// serial related

Uart * SERIAL_USB {&Serial};

//////////////////////////////////////////////////////////////////////////////////////////
// functions

void print_firmware_config(void){
    SERIAL_USB->println(F("OpenMetBuoy-v2021 | https://github.com/jerabaul29/OpenMetBuoy-v2021a"));
    SERIAL_USB->print(F("Compiled: "));
    SERIAL_USB->print(F(__DATE__));
    SERIAL_USB->print(F(", "));
    SERIAL_USB->print(F(__TIME__));
    SERIAL_USB->print(F(", compiler version "));
    SERIAL_USB->println(F(__VERSION__));
    SERIAL_USB->print(F("git branch: "));
    SERIAL_USB->print(F(git_branch));
    SERIAL_USB->print(F(" | commit ID: "));
    SERIAL_USB->println(F(commit_id));
    delay(10);

    // show information about who compiled
    SERIAL_USB->print(F("compiling hostname: ")); SERIAL_USB->print(STRINGIFY_CONTENT(COMPILING_HOST_NAME));
    SERIAL_USB->print(F(" | username: ")); SERIAL_USB->println(STRINGIFY_CONTENT(COMPILING_USER_NAME));
    delay(10);

    // show the chip ID
    SERIAL_USB->print(F("Chip ID: ")); print_uint64(read_chip_id()); SERIAL_USB->println();
    delay(10);
}

uint64_t read_chip_id(void){
    am_hal_mcuctrl_device_t sDevice;
    am_hal_mcuctrl_info_get(AM_HAL_MCUCTRL_INFO_DEVICEID, &sDevice);
    // what is the most / less significant ChipID ui32? this is up to us (I think)
    uint64_t chip_id = (static_cast<uint64_t>(sDevice.ui32ChipID0) << 32) + static_cast<uint64_t>(sDevice.ui32ChipID1);

    return chip_id;
}
