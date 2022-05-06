#include "params.h"

void print_params(void){
    Serial.println(F("----- TEST_MODE -----"));
    Serial.print(F("DEPLOYMENT_MODE: ")); Serial.println(DEPLOYMENT_MODE);
    Serial.print(F("DEPLOYMENT_INFO: ")); Serial.println(DEPLOYMENT_INFO);
    delay(50);

    Serial.println(F("----- IMU_DATA_GATHERING_MODE -----"));
    Serial.print(F("use_dummy_imu_data: ")); Serial.println(use_dummy_imu_data);
    Serial.println(F("IF use_dummy_imu_data is TRUE, then use synthetic dummy signal (harmonic signal); if FALSE, then use true signal"));
    Serial.print(F("dummy_imu_elevation_amplitude: ")); Serial.println(dummy_imu_elevation_amplitude, 4);
    Serial.print(F("std dummy_imu_elevation_amplitude: ")); Serial.println(dummy_imu_elevation_amplitude / sqrt2_f, 4);
    Serial.print(F("SWH dummy_imu_elevation_amplitude: ")); Serial.println(4.0f * dummy_imu_elevation_amplitude / sqrt2_f, 4);
    Serial.print(F("dummy_imu_accel_constant_component: ")); Serial.println(dummy_imu_accel_constant_component, 4);
    Serial.print(F("dummy_imu_accel_amplitude: ")); Serial.println(dummy_imu_accel_amplitude, 4);
    Serial.print(F("std dummy_imu_accel_amplitude: ")); Serial.println(dummy_imu_accel_amplitude / sqrt2_f, 4);
    Serial.print(F("dummy_imu_accel_frequency: ")); Serial.println(dummy_imu_accel_frequency, 4);
    delay(50);

    Serial.println(F("----- PARAMS GNSS ----"));
    Serial.print(F("timeout_first_fix_seconds: ")); Serial.println(timeout_first_fix_seconds);
    Serial.print(F("sleep_no_initial_fix_seconds: ")); Serial.println(sleep_no_initial_fix_seconds);
    Serial.print(F("timeout_gnss_fix_seconds: ")); Serial.println(timeout_gnss_fix_seconds);
    Serial.print(F("interval_between_gnss_measurements_seconds: ")); Serial.println(interval_between_gnss_measurements_seconds);
    Serial.print(F("size_gps_fix_buffer: ")); Serial.println(size_gps_fix_buffer);
    Serial.print(F("min_nbr_of_fix_per_message: ")); Serial.println(min_nbr_of_fix_per_message);
    Serial.print(F("max_nbr_GPS_fixes_per_message: ")); Serial.println(max_nbr_GPS_fixes_per_message);
    delay(50);

    Serial.println(F("----- PARAMS IRIDIUM -----"));
    Serial.print(F("timeout_cap_charging_seconds: ")); Serial.println(timeout_cap_charging_seconds);
    #ifdef ISBD_DIAGNOSTICS
        Serial.println(F("provide ISBD diagnostics"));
    #else
        Serial.println(F("no ISBD diagnostics"));
    #endif
    delay(50);

    Serial.println(F("----- IMU PARAMS -----"));
    Serial.print(F("using magnometer: ")); Serial.println(imu_use_magnetometer);
    Serial.print(F("using HARD CODED Kalman output freq [Hz]: ")); Serial.println(10);
    Serial.print(F("blink_when_use_IMU: ")); Serial.println(blink_when_use_IMU);
    Serial.print(F("number_update_between_blink: ")); Serial.println(number_update_between_blink);
    delay(50);

    Serial.println(F("----- SPECTRA PARAMS -----"));
    Serial.print(F("use_wave_measurements [1: true; 0: false]: ")); Serial.println(use_wave_measurements);
    Serial.print(F("using fft transform length: ")); Serial.println(fft_length);
    Serial.print(F("using fft overlap nbr of points: ")); Serial.println(fft_overlap);
    Serial.print(F("using total number of samples: ")); Serial.println(total_number_of_samples);
    Serial.print(F("using number Welch segments: ")); Serial.println(number_welch_segments);
    Serial.print(F("welch and fft frequency resolution [Hz per bin]: ")); Serial.println(welch_frequency_resolution, 4);
    Serial.print(F("first transmitted bin has ind: ")); Serial.println(welch_bin_min);
    Serial.print(F("last transmitted bin has ind: ")); Serial.println(welch_bin_max);
    Serial.print(F("first transmitted bin has freq: ")); Serial.println(welch_bin_min * welch_frequency_resolution);
    Serial.print(F("last transmitted bin has freq: ")); Serial.println(welch_bin_max * welch_frequency_resolution);
    Serial.print(F("interval_between_wave_spectra_measurements: ")); Serial.println(interval_between_wave_spectra_measurements);
    Serial.print(F("tolerance_seconds_start_wave_measurements: ")); Serial.println(tolerance_seconds_start_wave_measurements);
    Serial.print(F("use windowing (only one windowing can be used at a time) | hanning ")); Serial.print(use_hanning_window); Serial.print(F(" | hamming ")); Serial.println(use_hamming_window);
    delay(50);

    Serial.println(F("----- TEMPERATURE PARAMS -----"));
    Serial.print(F("use_thermistor_string [1: true; 0: false]: ")); Serial.println(use_thermistor_string);
    Serial.print(F("interval_between_thermistors_measurements_seconds: ")); Serial.println(interval_between_thermistors_measurements_seconds);
    Serial.print(F("tolerance_seconds_start_thermistors_measurements: ")); Serial.println(tolerance_seconds_start_thermistors_measurements);
    Serial.print(F("max number of temp sensors: ")); Serial.println(number_of_thermistors);
    Serial.print(F("duration temp sensors acquisition [ms]: ")); Serial.println(duration_thermistor_acquisition_ms);
    Serial.print(F("duration imu acquisition for temp string attitutde [ms]: ")); Serial.println(duration_thermistor_imu_acquisition_ms);
    Serial.print(F("max_nbr_thermistor_packet: ")); Serial.println(max_nbr_thermistor_packets);
    Serial.print(F("min_default_nbr_thermistor_packet: ")); Serial.println(min_default_nbr_thermistor_packets);
    Serial.print(F("size_thermistors_packet_buffer: ")); Serial.println(size_thermistors_packets_buffer);
    Serial.print(F("roll_float_to_int8_factor: ")); Serial.println(roll_float_to_int8_factor);
    Serial.print(F("pitch_float_to_int8_factor: ")); Serial.println(pitch_float_to_int8_factor);
    delay(50);

    Serial.println(F("----- MISC PARAMS -----"));
    #if (PERFORM_SLEEP_BLINK == 1)
        Serial.println(F("perform sleep blink"));
        Serial.print(F("interval_between_sleep_LED_blink_seconds: ")); Serial.println(interval_between_sleep_LED_blink_seconds);
        Serial.print(F("duration_sleep_LED_blink_milliseconds: ")); Serial.println(duration_sleep_LED_blink_milliseconds);
    #else
        Serial.println(F("no sleep blink"));
    #endif
    delay(50);

    Serial.println(F("----- END PARAMS -----"));
}

