#include "Arduino.h"
#include "unity.h"

#include "sleep_manager.h"
#include "time_manager.h"
#include "print_utils.h"

// NOTE: millis does not elapse when sleeping since no interrupt
// but posix_timestamp does elapse, because interrupts are caught during
// the 1 second wakeups

void test_sleep_for_seconds(void) {

    // valid sleep
    kiss_time_t rtc_start = board_time_manager.get_posix_timestamp();
    sleep_for_seconds(2);    
    kiss_time_t rtc_end = board_time_manager.get_posix_timestamp();

    unsigned long elapsed = rtc_end - rtc_start;

    TEST_ASSERT_TRUE(
        (elapsed == 2)
    );

    // we would like to test an invalid value, like 3232538, but do not want to
    // sleep for default 6 hours...
}

void test_sleep_until_posix(void) {

    // valid sleep
    kiss_time_t time_start {455896};
    // set the posix time
    board_time_manager.set_posix_timestamp(time_start);
    // sleep
    sleep_until_posix(time_start + 2);

    TEST_ASSERT_TRUE(
        board_time_manager.get_posix_timestamp() == (time_start + 2)
    )

    // we would like to test an invalid sleep, like from 1999 to 1998,
    // but do not want to go to sleep for default many hours...

    // we would like to test a sleep when the posix timestamp is not set,
    // but do not want to go to sleep for default many hours...
}

void test_seconds_to_sleep_until_posix(void){
    unsigned long to_sleep {0};
    kiss_time_t start_posix {1234567};
    kiss_time_t end_posix;


    // board time manager not set
    to_sleep = seconds_to_sleep_until_posix(45682);
    TEST_ASSERT_TRUE(
        to_sleep == default_error_sleep_seconds
    )

    // set a valid posix into time manager
    board_time_manager.set_posix_timestamp(start_posix);
    
    // good duration
    end_posix = start_posix + 10;
    to_sleep = seconds_to_sleep_until_posix(end_posix);
    TEST_ASSERT_TRUE(
        to_sleep == 10
    )

    // wrong duration 1
    end_posix = start_posix - 1;
    to_sleep = seconds_to_sleep_until_posix(end_posix);
    TEST_ASSERT_TRUE(
        to_sleep == default_error_sleep_seconds
    )

    // wrong duration 2
    end_posix = start_posix + 3600UL * 6UL + 10;
    to_sleep = seconds_to_sleep_until_posix(end_posix);
    TEST_ASSERT_TRUE(
        to_sleep == default_error_sleep_seconds
    )
}

void setup() {
    UNITY_BEGIN();

    RUN_TEST(test_sleep_for_seconds);
    RUN_TEST(test_sleep_until_posix);
    RUN_TEST(test_seconds_to_sleep_until_posix);

    UNITY_END();
}

void loop() {
}
