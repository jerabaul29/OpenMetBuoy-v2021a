#include <Arduino.h>
#include <unity.h>

#include "time_manager.h"

void test_time_manager(void) {

    TEST_ASSERT_FALSE( board_time_manager.posix_timestamp_is_valid() );

    board_time_manager.set_posix_timestamp(123456789);
    TEST_ASSERT_TRUE( board_time_manager.posix_timestamp_is_valid() );
    // it is possible that one second has elapsed in the meantime...
    TEST_ASSERT_TRUE(
        (board_time_manager.get_posix_timestamp() == 123456789) ||
        (board_time_manager.get_posix_timestamp() == 123456790)
    );

    delay(2050);  // make sure that we increment at least 2 times since setting, so that the test values are disjoint

    TEST_ASSERT_TRUE( board_time_manager.posix_timestamp_is_valid() );
    // it is possible that one second has elapsed in the meantime...
    TEST_ASSERT_TRUE(
        (board_time_manager.get_posix_timestamp() == 123456791) ||
        (board_time_manager.get_posix_timestamp() == 123456792)
    );

    delay(4050);  // make sure that we increment at least 4 times since setting, so that the test values are disjoint

    TEST_ASSERT_TRUE( board_time_manager.posix_timestamp_is_valid() );
    // it is possible that one second has elapsed in the meantime...
    TEST_ASSERT_TRUE(
        (board_time_manager.get_posix_timestamp() == 123456795) ||
        (board_time_manager.get_posix_timestamp() == 123456797)
    );

    // TODO: would like to test the print_status, but not clear how to test
    // that the computer receives the right stuff yet...
    // for now, just run it to check that "no dragons appear", but testing would be best
    SERIAL_USB->println();
    board_time_manager.print_status();
    SERIAL_USB->println();

}

void test_time_manager_re_set(void) {

    board_time_manager.set_posix_timestamp(123456789);
    TEST_ASSERT_TRUE( board_time_manager.posix_timestamp_is_valid() );
    // it is possible that one second has elapsed in the meantime...
    TEST_ASSERT_TRUE(
        (board_time_manager.get_posix_timestamp() == 123456789) ||
        (board_time_manager.get_posix_timestamp() == 123456790)
    );


    board_time_manager.set_posix_timestamp(12345678);
    TEST_ASSERT_TRUE( board_time_manager.posix_timestamp_is_valid() );
    // it is possible that one second has elapsed in the meantime...
    TEST_ASSERT_TRUE(
        (board_time_manager.get_posix_timestamp() == 12345678) ||
        (board_time_manager.get_posix_timestamp() == 12345679)
    );

}

void test_time_manager_in_the_future(void) {

    board_time_manager.set_posix_timestamp(3532609615);
    TEST_ASSERT_TRUE( board_time_manager.posix_timestamp_is_valid() );
    // it is possible that one second has elapsed in the meantime...
    TEST_ASSERT_TRUE(
        (board_time_manager.get_posix_timestamp() == 3532609615) ||
        (board_time_manager.get_posix_timestamp() == 3532609616)
    );

    delay(2050);

    TEST_ASSERT_TRUE( board_time_manager.posix_timestamp_is_valid() );
    // it is possible that one second has elapsed in the meantime...
    TEST_ASSERT_TRUE(
        (board_time_manager.get_posix_timestamp() == 3532609617) ||
        (board_time_manager.get_posix_timestamp() == 3532609618)
    );

    board_time_manager.print_status();

}

void setup() {
    UNITY_BEGIN();

    RUN_TEST(test_time_manager);
    RUN_TEST(test_time_manager_re_set);
    RUN_TEST(test_time_manager_in_the_future);

    UNITY_END();
}

void loop() {
}
