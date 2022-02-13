#include <Arduino.h>
#include <unity.h>

#include "peripheral_manager.h"

void test_scenarios_1(void){
    ////////////////////////////////////////////////////////////
    // create a manager from I2C port

    // non active at start

    // all slots available at start

    ////////////////////////////////////////////////////////////
    // register one device

    // active now

    // all slots - 1 available now

    ////////////////////////////////////////////////////////////
    // register one more device

    // active still

    // all slots - 2 active now

    ////////////////////////////////////////////////////////////
    // de register device 2

    // active still

    // all slots - 1 active now

    ////////////////////////////////////////////////////////////
    // try de register device 2 again... this fails

    // active still

    // all slots - 1 active now

    ////////////////////////////////////////////////////////////
    // try register device 1 again... this fails

    // active still

    // all slots - 1 active now

    ////////////////////////////////////////////////////////////
    // de register device 1

    // no longer active

    // all slots active now

    ////////////////////////////////////////////////////////////
    // force_end
}

void test_scenarios_2(void){
    ////////////////////////////////////////////////////////////
    // register one device

    ////////////////////////////////////////////////////////////
    // register one more device

    ////////////////////////////////////////////////////////////
    // force_end

    // not active now

    // all slots available now

    ////////////////////////////////////////////////////////////
    // register one device

    ////////////////////////////////////////////////////////////
    // register one more device

    ////////////////////////////////////////////////////////////
    // force_end
}

void setup() {
    UNITY_BEGIN();

    RUN_TEST(test_scenarios_1);
    RUN_TEST(test_scenarios_2);

    UNITY_END();
}

void loop() {
}
