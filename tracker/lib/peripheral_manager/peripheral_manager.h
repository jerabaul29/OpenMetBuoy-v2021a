#ifndef PERIPHERAL_MANAGER_H
#define PERIPHERAL_MANAGER_H

#include "Embedded_Template_Library.h"
#include "etl/vector.h"
#include "etl/array.h"
#include "etl/algorithm.h"

#include "print_utils.h"

#include "interrupt_utils.h"

// what we want to solve with this class is how to manage peripherals (I2C, SPI) that may be used
// to communicate with several devices; for example, if device1 is using I2C1, and then another
// device2 needs to use it too, one does not want to re-start the bus and confuse device1; similarly,
// when device1 is not needed any longer, but device2 is still active, one does not want to end the
// peripheral. but we do not want to have the peripheral being on all the time either - that would
// waste power, and the peripherals need to be off when sleeping anyways. In addition, we have to make
// sure that we do not let concurrent use of device1 and device2 lead to race conditions, i.e., make
// sure there is no way that communications with 2 devices interwind and mess up with each other.

// this class is thought for use in a context that is single-threaded, but with interrupts,
// for example a bare metal (no RTOS) MCU.
// there are a number of considerations to take when using an I2C port with several devices
// to avoid race conditions; for example, make sure one back and forth communication with
// a device is over before some other communication with the device or another device takes place.
// in a bare metal single threaded + interrupt environment, where the I2C port can be used from ISR,
// a sufficient condition to make things safe is to disable interrupts,
// ie hidden multi threading, when using the I2C port.

// if this is to be used in a multi threaded environment in the future, making this race conditions
// safe will require some extra work (adding a mutex and / or replacing ISR disablement by a mutex).

// also note that this is not super efficient: all other work will be disabled while the I2C
// port is under use, including interrupts! Only use with short I2C communication bursts, as this
// will block ISRs and the CPU. using a fully async / state machine I2C library could help alleviate
// this, but it would be possibly a lot of work.

// a device ID is 3 chars (need not be null-terminated), for example "imu", "tmp", "ird", ...
using DeviceID = etl::array<char, 3>;
// a peripheral ID is 3 chars (need not be null-terminated), for example "I2C1, SPI2, ..."
using PeriphID = etl::array<char, 3>;

// allow at most max_devices_per_periph devices per peripheral
// could be increased if needed
static constexpr size_t max_devices_per_periph {4};

// this class takes care of:
// - checking if a peripheral is active or not, i.e. used by any device, for power management; this
// is basically to help decide when to call begin and end
// - checking if the peripheral is currently borrowed or not; this is to make sure communicating with
// several devices does not lead to race conditions and interwinded communications
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

        // we can also print a bit of information, or query it
        void print_info(void) const;
        bool is_active(void) const;
        bool is_borrowed(void) const;
        int remaining_slots(void) const;

    private:
        etl::vector<DeviceID, max_devices_per_periph> active_devices {};
        PeriphID periph_name;

        // if we need to do anything more advanced, here is the raw pointer to
        // the underlying interface
        T *raw_periph;

        bool borrowed;

        friend class BorrowedPeripheral<T>;
};

// a RAII way to borrow the peripheral in a safe way:
// - check that the peripheral is available for borrowing, i.e. that it is not borrowed yet
// - disable interrupts when borrowing the peripheral, so that safe to do all the operations
// needed; when destroying the peripheral, re-enable the interrupts if needed only (ie let the
// interrupts in the same state they were found).
template<typename T>
class PeripheralBorrower {
    explicit PeripheralBorrower(PeripheralManager<T> peripheral_in);

    // try to borrow the peripheral to use with a specific device
    // if successful, give a pointer to the raw peripheral in the inout
    // parameter and return 0;
    // if not successfull, return nullptr to T*, and
    // - if not successful because already borrowed, return error code 1
    // - if not successful because no such device, return error code 2
    int borrow_peripheral_device(DeviceID, T*);

    ~PeripheralBorrower();

    private:
        uint32_t constructor_interrupt_status;
};

#endif