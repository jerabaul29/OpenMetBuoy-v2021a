#ifndef GNSS_MANAGER
#define GNSS_MANAGER

#include "etl.h"
#include "etl/deque.h"
#include "etl/vector.h"

#include <Wire.h> // Needed for I2C
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS

#include "board_control.h"
#include "time_manager.h"
#include "watchdog_manager.h"
#include "params.h"

#include "statistical_processing.h"

extern SFE_UBLOX_GNSS gnss;

struct fix_information{
  long posix_timestamp;
  long latitude;
  long longitude;
};

class GNSS_Manager{
  public:
    bool get_a_fix(unsigned long timeout_seconds=timeout_gnss_fix_seconds, bool set_RTC_time=true, bool perform_full_start=true, bool perform_full_stop=true);

    bool get_and_push_fix(unsigned long timeout_seconds=timeout_gnss_fix_seconds);  // if it worked or not (to decide if iridium)
    
    size_t write_message_to_buffer(etl::ivector<unsigned char>& buffer, size_t max_nbr_fixes=max_nbr_GPS_fixes_per_message);  // G[12B][12B]...\0

    void clear_number_sent_fixes(size_t number_fixes_to_clear);

    void print_GNSS_fixes_buffer(void);

    // have the fit data available for simplicity
    bool good_fit;                  // if the last attempt resulted in a valid fit
    long latitude;                 // Latitude in degrees
    long longitude;                // Longitude in degrees
    long altitude;                  // Altitude above Median Seal Level in m
    long speed;                    // Ground speed in m/s
    byte satellites;                // Number of satellites (SVs) used in the solution
    long course;                    // Course (heading) in degrees
    int pdop;                       // Positional Dilution of Precision in m
    int year;                       // GNSS Year
    uint8_t month;                     // GNSS month
    uint8_t day;                       // GNSS day
    uint8_t hour;                      // GNSS hours
    uint8_t minute;                    // GNSS minutes
    uint8_t second;                    // GNSS seconds
    int milliseconds;               // GNSS milliseconds
    long posix_timestamp;           // the last fit timestamp as a posix timestamp, from the GPS (not the RTC)

    unsigned int number_of_GPS_fixes {0};

    etl::deque<fix_information, size_gps_fix_buffer> gps_fixes_buffer;

    static constexpr size_t size_accumulators {30};
    etl::vector<long, size_accumulators> crrt_accumulator_latitude;
    etl::vector<long, size_accumulators> crrt_accumulator_longitude;
    etl::vector<long, size_accumulators> crrt_accumulator_posix_timestamp;

  private:
};

extern GNSS_Manager gnss_manager;

#endif
