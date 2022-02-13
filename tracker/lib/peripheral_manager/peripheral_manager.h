#ifndef PERIPHERAL_MANAGER_H
#define PERIPHERAL_MANAGER_H

#include "Embedded_Template_Library.h"
#include "etl/vector.h"
#include "etl/array.h"
#include "etl/algorithm.h"

#include "print_utils.h"

// what we want to solve with this class is how to manage peripherals (I2C, SPI) that may be used
// to communicate with several devices; for example, if device1 is using I2C1, and then another
// device2 needs to use it too, one does not want to re-start the bus and confuse device1; similarly,
// when device1 is not needed any longer, but device2 is still active, one does not want to end the
// peripheral. but we do not want to have the peripheral being on all the time either - that would
// waste power, and the peripherals need to be off when sleeping anyways.

// a device ID is 3 chars (need not be null-terminated), for example "imu", "tmp", "ird", ...
using DeviceID = etl::array<char, 3>;
// a peripheral ID is 3 chars (need not be null-terminated), for example "I2C1, SPI2, ..."
using PeriphID = etl::array<char, 3>;

// allow at most max_devices_per_periph devices per peripheral
// could be increased if needed
static constexpr size_t max_devices_per_periph {4};

template<typename T>
class PeripheralManager {
    public:
        // when creating a PeripheralManager, NEED to say which peripheral it manages
        PeripheralManager(T*, PeriphID);
        PeripheralManager() = delete; // we do not want to build a PeripheralManager witout its raw peripheral assigned

        // the begin and end that will usually be used to begin and end:
        // begin only begins if not begun yet
        // end only ends if no more active decives
        bool begin_if_need(DeviceID);
        bool end_if_possible(DeviceID);

        // if we really want to force end or restart the peripheral, we can do it;
        // that may mess up with the devices attached, though!
        void force_end(void);

        // if we need to do anything more advanced, here is the raw pointer to
        // the underlying interface
        T *raw_periph;

        // we can also print a bit of information, or query it
        void print_info(void) const;
        bool is_active(void) const;
        int remaining_slots(void) const;

    private:
        etl::vector<DeviceID, max_devices_per_periph> active_devices {};
        PeriphID periph_name;

};

#endif