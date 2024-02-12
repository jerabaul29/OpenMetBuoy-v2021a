#ifndef PARAMS_TRACKER
#define PARAMS_TRACKER

#include "Arduino.h"
#include "board_control.h"

//--------------------------------------------------------------------------------
// hardware configuration

constexpr unsigned long baudrate_debug_serial = 1000000;

// for using the I2C port corresponding to the PCB pins
// constexpr int i2c_port_number {1};
// for using the I2C port corresponding to the Qwiic connector
constexpr int i2c_port_number {4};

// TODO: use some form of constexpr if instead
// first declare the variables and document
// then use if constexpr to set the values
// TODO: test on a small snippet first...
#define DEPLOYMENT_MODE 0

//--------------------------------------------------------------------------------
// GNSS params

#if (DEPLOYMENT_MODE == 0)
    #define DEPLOYMENT_INFO "default deployment"

    // parameters for getting the first GNSS fix
    // put 2UL * 60UL for a first fix timeout of 2 minutes
    constexpr unsigned long timeout_first_fix_seconds = 20UL * 60UL;  // a reasonable value
    // put 6UL * 60UL * 60UL for a sleep if no initial fix of 6 hours
    constexpr long sleep_no_initial_fix_seconds = 2L * 60L * 60L;  // for testing

    // parameter for timeout of later (i.e. non first) GNSS fixes
    // put 3UL * 60UL for a later fix timeout of 3 minutes
    constexpr unsigned long timeout_gnss_fix_seconds = 15UL * 60UL;
    // parameters for how often to acquire and if needed send gps data
    // a measurement will always be performed at 00:00:00, and after that each interval_between_gnss_measurements_seconds seconds
    // so put 60UL * 60UL for each hour at 00 minutes, 30UL * 60UL for each hour at 00 and 30 minutes, etc.
    constexpr long interval_between_gnss_measurements_seconds = 30L * 60L;  // a reasonable value

    // parameter for how many GNSS fixes data to remember for later at most, if do not manage to send with Iridium
    // this is useful because we expect that iridium transmission is harder to achieve than GNSS fix
    // to remember up to 256 fixes that could not be transmitted, put 256
    constexpr size_t size_gps_fix_buffer = 256;  // a reasonable value
    // for saving battery, we can decide to transmit bundled GNSS data only once in a while;
    // setting this to 1 will transmit each time a new GNSS fix is obtained,
    // setting to a higher value will wait for the buffer to fill, for example 6 will bundle at least 6 fixes per message.
    constexpr size_t min_nbr_of_fix_per_message = 6;  // a reasonable value
    constexpr size_t max_nbr_GPS_fixes_per_message = 20;

#elif (DEPLOYMENT_MODE == 1)
    #define DEPLOYMENT_INFO "testing"

    constexpr unsigned long timeout_first_fix_seconds = 2UL * 60UL;
    constexpr long sleep_no_initial_fix_seconds = 10L;

    constexpr unsigned long timeout_gnss_fix_seconds = 3UL * 60UL;
    constexpr long interval_between_gnss_measurements_seconds = 5UL;

    constexpr size_t size_gps_fix_buffer = 256;
    constexpr size_t min_nbr_of_fix_per_message = 3;
    constexpr size_t max_nbr_GPS_fixes_per_message = 5;

#elif (DEPLOYMENT_MODE == 2)
    #define DEPLOYMENT_INFO "Malin setup"

    constexpr unsigned long timeout_first_fix_seconds = 2UL * 60UL;
    constexpr long sleep_no_initial_fix_seconds = 60L * 60L;

    constexpr unsigned long timeout_gnss_fix_seconds = 3UL * 60UL;
    constexpr long interval_between_gnss_measurements_seconds = 15L * 60L;

    constexpr size_t size_gps_fix_buffer = 256;
    constexpr size_t min_nbr_of_fix_per_message = 4;
    constexpr size_t max_nbr_GPS_fixes_per_message = 10;
#else
    #define DEPLOYMENT_INFO "No info"
    #error "Unknown deployment type"
#endif

extern long modifiable_interval_between_gnss_measurements_seconds;
extern size_t modifiable_min_nbr_of_fix_per_message;

// logics tests
static_assert(max_nbr_GPS_fixes_per_message <= 25);  // 3 + 13 * 25 = 328, i.e. 25 fixes is the max that fits in 340 bytes
static_assert(min_nbr_of_fix_per_message <= max_nbr_GPS_fixes_per_message);  // at most 8 fixes in one message

//--------------------------------------------------------------------------------
// iridium parameters

// parameter for max time for charging the supercaps
// this may need to be increased for very weak power solutions
// for a max charging time of 2 minutes, put 2UL * 60UL
constexpr unsigned long timeout_cap_charging_seconds = 3UL * 60UL;

// for how long we retry transmitting a given message
constexpr int timeout_attempt_transmit_seconds {300};

// uncommend the define to print additional iridium information
#define ISBD_DIAGNOSTICS

//--------------------------------------------------------------------------------
// IMU params
constexpr bool imu_use_magnetometer {false};
constexpr bool blink_when_use_IMU {true};
constexpr int number_update_between_blink {30};

//--------------------------------------------------------------------------------
// wave data sampling and wave data processing parameters
// this is a bit of hard coding for now

// whether to use wave measurements mode at all or not
constexpr bool use_wave_measurements {true};

// we use FFT of length 2048; smaller is bad for frequency resolution
constexpr size_t fft_length {2048};

// the IMU is sampled at a rate of 10 Hz
// 20 minutes would be 20*60*10=12000 samples ie 5.85 non overlapping segments
// in practise, let's use 6 non overlapping segments then, i.e. 12288 samples (a bit over 20 minutes)
constexpr size_t total_number_of_samples {2048 * 6};

// use a 75% overlap in the Welch computation
// this name is actually very confusing and badly chosen (TODO: refactor name), as fft_overlap
// is actually corresponding to the non-overlap length between two consecutive segments...
constexpr size_t fft_overlap {512};

// how many segments does that give us in total?
// we want to cover 1228.8 seconds
// the first segment brings in (fft_length / sampling_freq), where sampling_freq is 10Hz (see the imu_manager), i.e. 204.8 seconds
// each subsequent segment brings in (fft_length / sampling_freq / 4) seconds, where 4 comes from 75% overlap, i.e. 51.2 seconds
// so we need a number of welch segments: (1 + (1228.8 - 204.8) / 51.2) = 21.0
constexpr size_t number_welch_segments {21};

// frequency resolution is identical for Welch and FFT
// = (sampling_freq_hz / fft_length)
constexpr float welch_frequency_resolution {10.0f / static_cast<float>(fft_length)};

constexpr size_t welch_bin_min {9};
constexpr size_t welch_bin_max {64};
constexpr size_t nbr_wave_packet_freqs = welch_bin_max - welch_bin_min;

constexpr size_t size_wave_packet_buffer {128};

// how often to take wave spectrum measurements
// this is a value in seconds, that:
// - should correspond to a GPS measurement
// - will indicate when the wave spectrum measurements are started
// for example:
// - if GPS measurements are performed each 30 minutes, then can take wave measurements each 30 minutes, or each hour, or each 2 hours, etc
// - to start each 2 hours, the value would be: 2 * 60 * 60
constexpr long interval_between_wave_spectra_measurements {2 * 60 * 60};
// tolerance in seconds for jitter; typically 5 minutes should be more than enough
constexpr long tolerance_seconds_start_wave_measurements {10 * 60};

// to test the welch processing code, we have the possibility ot use dummy IMU data:
constexpr bool use_dummy_imu_data {false};  // if true, use dummy IMU data
// the properties of the dummy signal that we generate; its form is "dummy_accel = amp * cos(frq * t) + DC"
// where amp = amp_eta * omega**2 with amp_eta the water elevation amplitude
constexpr float dummy_imu_elevation_amplitude = 1.0f;  // amp_eta, in meters
constexpr float dummy_imu_accel_frequency = 0.2266f;  // frq, in hz; this is "in the middle" of two frequency bins, convenient to check spectral energy leakage
// constexpr float dummy_imu_accel_frequency = 0.2246f;  // frq, in hz; this is "spot on" to one specific frequency bin
constexpr float pi = 3.14159265359f;
constexpr float sqrt2_f = 1.4142135623730951f;
constexpr float dummy_imu_accel_omega = 2.0f * pi * dummy_imu_accel_frequency;  // omega

constexpr float dummy_imu_accel_constant_component = 9.81f;  // DC
constexpr float dummy_imu_accel_amplitude = dummy_imu_elevation_amplitude * dummy_imu_accel_omega * dummy_imu_accel_omega;  // amp accel

constexpr bool use_hanning_window {true};
constexpr bool use_hamming_window {false};

static_assert(use_hanning_window, "use of hanning window is recommended; only override with a good reason!");
static_assert(!(use_hamming_window && use_hanning_window), "can only use one windowing at a time!");
static_assert((use_hamming_window || use_hanning_window), "we STRONGLY recommend to use windowing; otherwise, spectral leakage combined with acceleration spectrum scaling leads to SWH values that are off by up to 20 percents.");
static_assert(interval_between_wave_spectra_measurements % interval_between_gnss_measurements_seconds == 0, "should perform wave measurements at the same time as some GNSS fix acquisition");

extern long modifiable_interval_between_wave_spectra_measurements;

//--------------------------------------------------------------------------------
// thermistor related parameters

// defining this macro variable will macro out all the thermistor related code
// to be able to use thermistors, comment the next macro definition out
// #define DISABLE_ALL_THERMISTOR

constexpr bool use_thermistor_string {true};

#ifdef DISABLE_ALL_THERMISTOR
    // actually, the code will compile with the DISABLE_ALL_THERMISTOR defined and the use_thermistor_string to true, but this is likely indicative of user mis configuration
    static_assert(!use_thermistor_string, "you can only use_thermistor_string [set to true] if the DISABLE_ALL_THERMISTOR is not defined; otherwise, try to use thermistor string, while the thermistor code is not included!");
#endif

// when to perform thermistors measurements; use the same convention as the other methods
constexpr long interval_between_thermistors_measurements_seconds = interval_between_gnss_measurements_seconds;  // thermistors measurements should be quite power efficient; do each time the GNSS position is taken
// tolerance in seconds for jitter on when to perform the thermistors measurement; typically 5 minutes should be more than enough
constexpr long tolerance_seconds_start_thermistors_measurements {5 * 60};

// how many thermistors to use on the thermistor string, at most
constexpr int number_of_thermistors {4};

// duration over which sample thermistor data
constexpr int duration_thermistor_acquisition_ms {60000};
// duration over which sample IMU data to get the attitude information
// constexpr int duration_thermistor_imu_acquisition_ms {60000};
// constexpr int number_of_thermistor_imu_measurements = duration_thermistor_imu_acquisition_ms / 100;

// max number of thermistor packets that we transmit in a single thermistors message
constexpr size_t max_nbr_thermistor_packets {14};

// min number of thermistors readings sent at once by defaul
constexpr size_t min_default_nbr_thermistor_packets {4};

// how many thermistor packets we keep in memory
constexpr size_t size_thermistors_packets_buffer {256};

// roll is -180 to 180; to map it to -128 to 127 maximum
// constexpr float roll_float_to_int8_factor {0.7f};  // 180 * 0.7 = 126
// pitch is -90 to 90; to map it to -128 to 127 maximum
// constexpr float pitch_float_to_int8_factor {1.4f};  // 90 * 1.4 = 126

static_assert(interval_between_thermistors_measurements_seconds % interval_between_gnss_measurements_seconds == 0, "should perform thermistors measurements at the same time as some GNSS fix acquisition");

extern long modifiable_interval_between_thermistors_measurements_seconds;

//--------------------------------------------------------------------------------
// various minor params
// NOTE: would like a constexpr if better, but not supported yet it seems; 1 to blink, 0 to not blink
#define PERFORM_SLEEP_BLINK 1
constexpr unsigned long interval_between_sleep_LED_blink_seconds {120};
constexpr unsigned long duration_sleep_LED_blink_milliseconds {350};

//--------------------------------------------------------------------------------
void print_params(void);

#endif
