#include "gnss_manager.h"

SFE_UBLOX_GNSS gnss;

GNSS_Manager gnss_manager;

bool GNSS_Manager::get_a_fix(unsigned long timeout_seconds, bool set_RTC_time, bool perform_full_start, bool perform_full_stop){
  wdt.restart();
  good_fit = false;

  Serial.println(F("attempt gnss fix"));

  if (perform_full_start){
    // power things up and connect to the GNSS; if fail several time, restart the board
    bool gnss_startup {false};
    for (int i=0; i<5; i++){
      Wire1.begin();
      Serial.println(F("Wire1 started"));
      turn_gnss_on();
      delay(1000); // Give it time to power up
      wdt.restart();

      Serial.println(F("gnss powered up"));
      Serial.flush();

      if (!gnss.begin(Wire1)){
        Serial.println(F("problem starting GNSS"));
        
        // power things down
        turn_gnss_off();
        Wire1.end();
        delay(500);
        continue;
      }
      else{
        Serial.println(F("success starting GNSS"));
        gnss_startup = true;
        break;
      }
      wdt.restart();
    }

    wdt.restart();

    if (!gnss_startup){
      Serial.println(F("failed to start GNSS; reboot"));
      while (1){;}
    }

    // now we know that we can talk to the gnss
    gnss.setI2COutput(COM_TYPE_UBX); // Limit I2C output to UBX (disable the NMEA noise)
    delay(100);

    // If we are going to change the dynamic platform model, let's do it here.
    // Possible values are:
    // PORTABLE,STATIONARY,PEDESTRIAN,AUTOMOTIVE,SEA,AIRBORNE1g,AIRBORNE2g,AIRBORNE4g,WRIST,BIKE
    if (!gnss.setDynamicModel(DYN_MODEL_STATIONARY)){
      Serial.println(F("GNSS could not set dynamic model"));
    }

    wdt.restart();
  }

  // now ready take a measurement
  byte gnss_fix_status {0};
  wdt.restart();

  Serial.println(F("attempt GNSS fix, remaining time to fix timeout:"));
  delay(5);
  for (int i=0; i<timeout_seconds;i++){
    Serial.print(F("-"));
  }
  Serial.println();
  delay(10);

  for (unsigned long start_millis=millis(); (gnss_fix_status != 3) && (millis() - start_millis < timeout_seconds * 1000UL); ){
    wdt.restart();
    Serial.print(F("-"));
    delay(500);
    gnss_fix_status = gnss.getFixType();
  }
  Serial.println();

  wdt.restart();

  if (gnss_fix_status == 3){
    // read the data and store it here
    good_fit = true;
    milliseconds = gnss.getMillisecond();
    second = gnss.getSecond();
    minute = gnss.getMinute();
    hour = gnss.getHour();
    day = gnss.getDay();
    month = gnss.getMonth();
    year = gnss.getYear();
    latitude = gnss.getLatitude();
    longitude = gnss.getLongitude();
    altitude = gnss.getAltitudeMSL();
    speed = gnss.getGroundSpeed();
    satellites = gnss.getSIV();
    course = gnss.getHeading();
    pdop = gnss.getPDOP();

    Serial.print(F("we got a gnss fix:"));
    Serial.print(year); Serial.print(F("-")); Serial.print(month); Serial.print(F("-")); Serial.print(day);
      Serial.print(F(" ")); Serial.print(hour); Serial.print(F(":")); Serial.print(minute); Serial.print(F(":"));
      Serial.print(second); Serial.print(F(" ")); Serial.print(latitude); Serial.print(F(",")); Serial.print(longitude);
      Serial.println();

    wdt.restart();

    // compute the corresponding posix timestamp
    common_working_posix_timestamp = posix_timestamp_from_YMDHMS(
      year, month, day, hour, minute, second
    );

    Serial.print(F("we computed a posix timestamp: ")); Serial.println((unsigned long)common_working_posix_timestamp);
    
    posix_timestamp = common_working_posix_timestamp;

    if (set_RTC_time){
      board_time_manager.set_posix_timestamp(common_working_posix_timestamp);
    }
  }
  else{
    Serial.println(F("GNSS timed out without fix"));
  }

  wdt.restart();

  // power things down
  if (perform_full_stop){
    turn_gnss_off();
    Wire1.end();
  }

  wdt.restart();

  return good_fit;
}

bool GNSS_Manager::get_and_push_fix(unsigned long timeout_seconds){
  wdt.restart();
  
  Serial.println(F("start with GNSS buffer:"));
  print_GNSS_fixes_buffer();
  
  if(get_a_fix(timeout_seconds, true, true, false)){

    // clear the accumulators
    crrt_accumulator_latitude.clear();
    crrt_accumulator_longitude.clear();
    crrt_accumulator_posix_timestamp.clear();

    // push the current fix
    // crrt_accumulator_latitude.push_back(latitude);
    // crrt_accumulator_longitude.push_back(longitude);
    // crrt_accumulator_posix_timestamp.push_back(posix_timestamp);

    // to be on the safe side, get a few extra fixes, and do some filtering on it to keep only "good" fixes;
    // maybe this helps avoid the "bad fix" problems (?)
    wdt.restart();
    delay(5000); // give a bit of time for the GPS reading to "warm up"?
    wdt.restart();

    // sample of few extra fixes for the n-sigma filtering; we sample at most enough to fill
    // the accumulators, and we use at most 60 seconds
    int crrt_fix_nbr {0};
    unsigned long millis_start = millis();
    while ( (crrt_fix_nbr < size_accumulators-1) && (millis() - millis_start < 60000UL)){
      if(get_a_fix(5UL, true, false, false)){
      
        if ((latitude == 0) && (longitude == 0)){
          Serial.println(F("ignore lat lon 0 fix"));
          continue;
        }
        
        crrt_fix_nbr += 1;
        crrt_accumulator_latitude.push_back(latitude);
        crrt_accumulator_longitude.push_back(longitude);
        crrt_accumulator_posix_timestamp.push_back(posix_timestamp);
        wdt.restart();
      }
    }

    // we turn off by hand, since we did not perform full start stop in the loop
    turn_gnss_off();
    Wire1.end();

    if (crrt_fix_nbr > 0){
        Serial.println(F("compute averaged fix"));
        
        // then, get the values for the filtered lat, lon, timestamp
        long crrt_latitude = accurate_sigma_filter<long>(crrt_accumulator_latitude, 2.0);
        long crrt_longitude = accurate_sigma_filter<long>(crrt_accumulator_longitude, 2.0); 
        long crrt_posix_timestamp = accurate_sigma_filter<long>(crrt_accumulator_posix_timestamp, 2.0); 

        fix_information crrt_fix {crrt_posix_timestamp, crrt_latitude, crrt_longitude};

        // if already full, need to make some space
        if (gps_fixes_buffer.full()){
          gps_fixes_buffer.pop_front();
        }

        // push at the end the last fix
        gps_fixes_buffer.push_back(crrt_fix);

        Serial.print(F("pushed fix: "));
        Serial.print(crrt_fix.posix_timestamp);
        Serial.print(F(" | "));
        Serial.print(crrt_fix.latitude);
        Serial.print(F(" | "));
        Serial.print(crrt_fix.longitude);
        Serial.println();

        Serial.println(F("end with GNSS buffer:"));
        print_GNSS_fixes_buffer();
        
        number_of_GPS_fixes += 1;

        return true;
    }
    else {
        Serial.println(F("no valid fix after all, abort fix pushing"));
        Serial.println(F("end with GNSS buffer:"));
        print_GNSS_fixes_buffer();
        return false;
    }
  }

  turn_gnss_off();
  Wire1.end();

  Serial.println(F("no fix, no push to buffer"));

  Serial.println(F("end with GNSS buffer:"));
  print_GNSS_fixes_buffer();
  
  return false;
}

size_t GNSS_Manager::write_message_to_buffer(etl::ivector<unsigned char>& buffer, size_t max_nbr_fixes){
  // message format:
  // G [byte nbr of GPS fixes so far] F [message] F [message] E

  buffer.push_back('G');
  buffer.push_back((unsigned char)(number_of_GPS_fixes%256));

  // how many fixes to write
  size_t nbr_fixes_in_message {0};
  nbr_fixes_in_message = min(max_nbr_fixes, gps_fixes_buffer.size());

  for (int fix_nbr=0; fix_nbr<nbr_fixes_in_message; fix_nbr++){
    // write the messages here
    buffer.push_back('F');

    fix_information current_fix = gps_fixes_buffer[gps_fixes_buffer.size() - 1 - fix_nbr];
    unsigned char * start_of_crrt_fix_information = (unsigned char *) & current_fix;

    // just push the whole struct content
    for (int crrt_byte=0; crrt_byte<sizeof(fix_information); crrt_byte++){
      buffer.push_back(*(start_of_crrt_fix_information+crrt_byte));
    }
  }

  buffer.push_back('E');

  return nbr_fixes_in_message;
}

void GNSS_Manager::clear_number_sent_fixes(size_t number_fixes_to_clear){
  for (size_t ind_to_clean=0; ind_to_clean<number_fixes_to_clear; ind_to_clean++){
    gps_fixes_buffer.pop_back();
  }

  Serial.println(F("after clear end with GNSS buffer:"));
  print_GNSS_fixes_buffer();
  
}

void GNSS_Manager::print_GNSS_fixes_buffer(void){
  Serial.println(F("print GNSS messages from old to new..."));

  for (size_t i=0; i<gps_fixes_buffer.size(); i++){
    wdt.restart();
    
    fix_information current_fix = gps_fixes_buffer[i];
    struct_YMDHMS& crrt_YMDHMS = YMDHMS_from_posix_timestamp(current_fix.posix_timestamp);

    Serial.print(F("posix ")); Serial.print(current_fix.posix_timestamp); Serial.print(F(" = "));
    serialPrintf(
        "%04d-%02d-%02d %02d:%02d:%02d",
        crrt_YMDHMS.year,
        crrt_YMDHMS.month,
        crrt_YMDHMS.day,
        crrt_YMDHMS.hour,
        crrt_YMDHMS.minute,
        crrt_YMDHMS.second
    );
    Serial.print(F(" | lat: ")); Serial.print(current_fix.latitude); Serial.print(F(" | lon: ")); Serial.print(current_fix.longitude);
    Serial.println();

    delay(10);
  }

  Serial.println(F("...done!"));
}

