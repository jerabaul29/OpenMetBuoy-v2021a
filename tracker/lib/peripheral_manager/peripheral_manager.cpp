#include "peripheral_manager.h"

template<typename T>
PeripheralManager<T>::PeripheralManager(T *raw_periph_in){
    raw_periph = raw_periph_in;
    is_active = false;
}

template<typename T>
bool PeripheralManager<T>::begin_if_need(DeviceID current_device){
    // take care of the registration of the device

    // is the device already registered? if yes, need not add, and the peripheral is
    // already active: we can return
    if (etl::find(active_devices.begin(), active_devices.end(), current_device) != active_devices.end()){
        return true;
    }
    // is the device not already registered? if yes, need add,
    else{
        // but need to check that enough spots left first; if not enough spaces,
        // print an error message and exit
        if (active_devices.full()){
            SERIAL_USB->print(F("W peripheral active devices full, cannot add "));
            SERIAL_USB->print(current_device{0});
            SERIAL_USB->print(current_device{1});
            SERIAL_USB->println(current_device{2});
            return false;
        }
        else {
            active_devices.push_back(current_device);
        }
    }

    // take care of the state of the peripheral

    // now the device is well registered, need to activate the peripheral if needed
    if (is_active){
        return true;
    }
    else{
        is_active = true;
        return raw_periph->begin();
    }
}

template<typename T>
bool PeripheralManager<T>::end_if_possible(DeviceID current_device){
    // take care of the de registration of the device

    // check that the device was actually reported active. if yes, remove it
    auto location = etl::find(active_devices.begin(), active_devices.end(), current_device);
    if (location != active_devices.end()){
        active_devices.erase(location);
    }
    // if not, this command should not do anything, just print error
    else{
        SERIAL_USB->print(F("W peripheral cannot end, device not registered: "));
        SERIAL_USB->print(current_device{0});
        SERIAL_USB->print(current_device{1});
        SERIAL_USB->println(current_device{2});
        return false;
    }

    // take care of the state of the peripheral

    // if the peripheral is now unused, switch if off
    // unused is now equivalent to "no active devices"
    if (active_devices.empty()){
        raw_periph->end();
    }

    return true;
}
