#include "peripheral_manager.h"

template<typename T>
PeripheralManager<T>::PeripheralManager(T *raw_periph_in, PeriphID periph_id){
    raw_periph = raw_periph_in;
    periph_name = periph_id;
}

template<typename T>
bool PeripheralManager<T>::begin_if_need(DeviceID current_device){
    // initial state of activity
    bool flag_is_active = is_active();

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
            SERIAL_USB->print(current_device[0]);
            SERIAL_USB->print(current_device[1]);
            SERIAL_USB->println(current_device[2]);
            return false;
        }
        else {
            active_devices.push_back(current_device);
        }
    }

    // take care of the state of the peripheral

    // now the device is well registered, need to activate the peripheral if needed
    if (flag_is_active){
        return true;
    }
    else{
        bool result = raw_periph->begin();
        delay(10);
        return result;
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
        SERIAL_USB->print(current_device[0]);
        SERIAL_USB->print(current_device[1]);
        SERIAL_USB->println(current_device[2]);
        return false;
    }

    // take care of the state of the peripheral

    // if the peripheral is now unused, switch if off
    // unused is now equivalent to "no active devices"
    if (!is_active()){
        raw_periph->end();
        delay(10);
    }

    return true;
}

template<typename T>
void PeripheralManager<T>::force_end(void){
    active_devices.clear();
    raw_periph->end();
    delay(10);
}

template<typename T>
void PeripheralManager<T>::print_info(void) const {
    SERIAL_USB->print(F("periph_name: "));
    SERIAL_USB->print(periph_name[0]);
    SERIAL_USB->print(periph_name[1]);
    SERIAL_USB->print(periph_name[2]);
    SERIAL_USB->print(F(" | "));
    bool flag_is_active = is_active();
    PRINT_VAR(flag_is_active);
    SERIAL_USB->print(F(" | "));
    PRINT_VAR(max_devices_per_periph);
    SERIAL_USB->print(F(" | "));
    Serial.print(F("active_devices: "));
    for (auto && elem : active_devices){
        SERIAL_USB->print(elem[0]);
        SERIAL_USB->print(elem[1]);
        SERIAL_USB->print(elem[2]);
        SERIAL_USB->print(F(", "));
    }
    SERIAL_USB->println();
}

template<typename T>
bool PeripheralManager<T>::is_active(void) const {
    return !active_devices.empty();
}

template<typename T>
int PeripheralManager<T>::remaining_slots(void) const {
    return active_devices.available();
}
