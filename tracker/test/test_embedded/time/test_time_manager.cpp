#include <Arduino.h>
#include <unity.h>
#include "time/time_manager.h"

void test_start_invalid(void) {
    TEST_ASSERT_EQUAL(2, 1+1);
}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();

    test_dummy();

    UNITY_END();
}

void loop() {
}
