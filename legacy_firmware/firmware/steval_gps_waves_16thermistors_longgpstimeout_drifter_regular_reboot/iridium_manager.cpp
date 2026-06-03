#include "iridium_manager.h"

unsigned long millis_last_callback {0};
unsigned long millis_iridium_callback_reset {0};
constexpr unsigned long callback_printout_freq_ms {2000UL};

void IridiumManager::clear_buffers(void){
    iridium_rx_buffer.clear();
    iridium_tx_buffer.clear();
    rx_message_available = false;
}

bool IridiumManager::has_received_new_message(void){
    return rx_message_available;
}

bool IridiumManager::send_receive_message(unsigned long timeout_cap_charging_seconds){
    last_message_went_through = false;

    Serial.println(F("iridium attempt send receive message"));
    print_vector_uc(iridium_tx_buffer);

    wdt.restart();
    turn_gnss_off();

    //--------------------------------------------------------------------------------
    // charge the supercaps
    Serial.println(F("charge supercaps"));
    pinMode(superCapChgEN, OUTPUT);
    digitalWrite(superCapChgEN, HIGH); // Enable the super capacitor charger
    delay(2000);
    wdt.restart();

    bool charge_ready {false};

    for (int i=0; i<timeout_cap_charging_seconds; i++){
        Serial.print(F("-"));
    }
    Serial.println();
    delay(10);
    
    for (unsigned long tstart=millis(); (!charge_ready) && (millis()-tstart < 1000UL * timeout_cap_charging_seconds);){
      charge_ready = (bool)digitalRead(superCapPGOOD);
      delay(1000);
      wdt.restart();
      Serial.print("-");
    }
    Serial.println();

    if (!charge_ready){
        Serial.println(F("failed to charge supercaps, abort transmission"));
        turn_iridium_off();
        send_receive_last_went_through = false;
        return false;
    }

    Serial.println(F("supercaps charged"));

    delay(1000);  // top up charge
    wdt.restart();

    //--------------------------------------------------------------------------------
    // turn on the iridium module
    turn_iridium_on();
    wdt.restart();
    delay(1000);
    wdt.restart();

    iridium_serial.begin(19200);
    delay(1000);
    wdt.restart();
    iridium_sbd.setPowerProfile(IridiumSBD::USB_POWER_PROFILE);
    wdt.restart();
    millis_last_callback = millis();

    if (iridium_sbd.begin() != ISBD_SUCCESS){  // note: could get an error message here
        Serial.println(F("failed to start the iridium modem!"));
        turn_iridium_off();
        send_receive_last_went_through = false;
        return false;
    }
    else{
        Serial.println(F("iridium modem started"));
    }
    wdt.restart();

    iridium_sbd.useMSSTMWorkaround(true);

    //--------------------------------------------------------------------------------
    // perform the send_receive

    Serial.println(F("attempt iridium transmission"));

    for (int i=0; i<150; i++){
        Serial.print(F("-"));
    }
    Serial.println();
    delay(10);

    // I am afraid doing something wrong giving the vectors data to the send receive; instead, use raw buffers and copy back and forth
    // this parameter is quite twisted: it is given in to specify how large the RX buffer is, then modified so that it contains how many RX bytes are available...
    rx_buffer_amount = iridium_rx_buffer_size;
    iridium_rx_buffer.clear();

    tx_buffer_amount = iridium_tx_buffer.size();
    for (size_t i=0; i<iridium_tx_buffer.size(); i++){
        iridium_tx_raw_buffer[i] = iridium_tx_buffer[i];
    }

    millis_iridium_callback_reset = millis();
    iridium_sbd.adjustSendReceiveTimeout(timeout_attempt_transmit_seconds);
    int transmission_status = iridium_sbd.sendReceiveSBDBinary(
      iridium_tx_raw_buffer, tx_buffer_amount,
      iridium_rx_raw_buffer, rx_buffer_amount
    );

    Serial.println();
    Serial.print(F("transmission status [0 is success]: ")); Serial.println(transmission_status);

    Serial.print(F("rx_buffer_amount [271 is nothing received]: ")); Serial.println(rx_buffer_amount);

    if (rx_buffer_amount != iridium_rx_buffer_size){
        Serial.print(F("received iridium command: "));

        for (size_t i=0; i<rx_buffer_amount; i++){
            Serial.print(iridium_rx_raw_buffer[i], HEX); Serial.print(F(" "));
            iridium_rx_buffer.push_back(iridium_rx_raw_buffer[i]);
        }

        Serial.println();
        rx_message_available = true;
    }
    else{
        Serial.println(F("no command received"));
    }

    wdt.restart();

    // say how many messages remaining
    Serial.print("Messages remaining to be retrieved: ");
    Serial.println(iridium_sbd.getWaitingMessageCount());

    // if we ask for a reboot, we have to make SURE that it takes place.
    // for this, test for reboot need already here, as seems that sometimes receives the message, but it is no
    // registered as valid by the modem (some ACK lost on the way back?)
    reboot_if_requested_through_iridium();
    read_apply_iridium_instructions();

    // NOTE: over time, this has become quite ugly, with lots of repetitions etc; would need a good refactoring!
    // TODO: refactor switch off the iridium modem and call only one...
    if (transmission_status != ISBD_SUCCESS){
        Serial.println(F("iridium transmission failed"));
        delay(500);
        wdt.restart();

        iridium_sbd.clearBuffers(ISBD_CLEAR_MO); // Clear MO buffer; could have some error checking
        wdt.restart();
        iridium_sbd.sleep();
        wdt.restart();

        turn_iridium_off();
        wdt.restart();
        iridium_serial.end();
        wdt.restart();

        send_receive_last_went_through = false;
        return false;
    }
    else{
        Serial.println(F("iridium transmission successful"));
        last_message_went_through = true;
    }

    iridium_sbd.clearBuffers(ISBD_CLEAR_MO); // Clear MO buffer; could have some error checking
    wdt.restart();
    iridium_sbd.sleep();
    wdt.restart();

    //--------------------------------------------------------------------------------
    // turn all off
    turn_iridium_off();
    wdt.restart();
    iridium_serial.end();
    wdt.restart();

    send_receive_last_went_through = true;
    return true;
}

void IridiumManager::attempt_transmit_gps_fixes(size_t min_nbr_messages){
    bool gnss_iridium_went_through {false};

    if (gnss_manager.gps_fixes_buffer.size() >= min_nbr_messages){
      attempt_tried_sending = true;
      // prepare iridium message
      clear_buffers();
      size_t nbr_of_fixes_in_message = gnss_manager.write_message_to_buffer(iridium_tx_buffer);
      wdt.restart();

      Serial.print(F("attempting to transmit "));
      Serial.print(nbr_of_fixes_in_message);
      Serial.println(F(" fixes in iridium message"));

      // send and remove the now transmitted positions from the GNSS buffer
      gnss_iridium_went_through = send_receive_message();
      if (gnss_iridium_went_through){
        gnss_manager.clear_number_sent_fixes(nbr_of_fixes_in_message);
      }
      wdt.restart();

      Serial.print(F("number of fixes left in buffer: "));
      Serial.println(gnss_manager.gps_fixes_buffer.size());
    }

    else{
      attempt_tried_sending = false;
      Serial.print(F(" only "));
      Serial.print(gnss_manager.gps_fixes_buffer.size());
      Serial.print(F(" fixes, transmit a message when at least "));
      Serial.println(min_nbr_messages);
    }
}

#ifndef DISABLE_ALL_THERMISTOR
    void IridiumManager::attempt_transmit_thermistors_packets(size_t min_nbr_packets){
        bool thermistors_data_went_through {false};

        if (board_thermistors_manager.thermistors_packets_buffer.size() >= min_nbr_packets){
          attempt_tried_sending = true;
          // prepare iridium message
          clear_buffers();
          size_t nbr_of_packets_in_message = board_thermistors_manager.write_message_to_buffer(iridium_tx_buffer);
          wdt.restart();

          Serial.print(F("attempting to transmit "));
          Serial.print(nbr_of_packets_in_message);
          Serial.println(F(" packets in iridium message"));

          // send and remove the now transmitted positions from the GNSS buffer
          thermistors_data_went_through = send_receive_message();
          if (thermistors_data_went_through){
            board_thermistors_manager.clear_number_sent_packets(nbr_of_packets_in_message);
          }
          wdt.restart();

          Serial.print(F("number of fixes left in buffer: "));
          Serial.println(board_thermistors_manager.thermistors_packets_buffer.size());
        }

        else{
          attempt_tried_sending = false;
          Serial.print(F(" only "));
          Serial.print(board_thermistors_manager.thermistors_packets_buffer.size());
          Serial.print(F(" packets, transmit a message when at least "));
          Serial.println(min_nbr_packets);
        }
    }
#endif

void IridiumManager::attempt_transmit_wave_spectra(){
    bool wave_spectrum_went_through {false};

    if (board_wave_analyzer.wave_packet_buffer.size() > 0){
        attempt_tried_sending = true;
        Serial.println(F("there are available wave packets to transmit"));

        // prepare iridium message
        clear_buffers();

        board_wave_analyzer.write_message_to_buffer(iridium_tx_buffer);

        // try to transmit
        wave_spectrum_went_through = send_receive_message();

        if (wave_spectrum_went_through){
            Serial.println(F("wave spectrum went through, pop_back"));
            board_wave_analyzer.wave_packet_buffer.pop_back();
        }
        else{
            Serial.println(F("transmission failed, try again later"));
        }
        wdt.restart();
    }

    else{
        attempt_tried_sending = false;
        Serial.println(F("no wave package to transmit"));
    }
}

bool ISBDCallback(){
    wdt.restart();
    if (millis() - millis_last_callback > callback_printout_freq_ms){
        Serial.print(F("-"));
        millis_last_callback = millis();
    }
    return true;
}

#ifdef ISBD_DIAGNOSTICS
    void ISBDConsoleCallback(IridiumSBD *device, char c){
      Serial.write(c);
    }

    void ISBDDiagsCallback(IridiumSBD *device, char c){
      Serial.write(c);
    }
#endif

void IridiumManager::reboot_if_requested_through_iridium(void){
    Serial.println(F("check if iridium requested reboot"));

    if (rx_message_available){
        Serial.println(F("message available"));

        if (iridium_rx_buffer.size() >= 4){

            if (iridium_rx_buffer[0] == 'B' && iridium_rx_buffer[1] == 'O' && iridium_rx_buffer[2] == 'O' && iridium_rx_buffer[3] == 'T'){
                Serial.println(F("BOOT message, reboot!"));
                while (true) {;}
            }

        }
        else{
            Serial.println(F("too short to be a reboot"));
        }
    }
}

int IridiumManager::read_value_from_command(size_t rx_ind_start){
    char crrt_buff[3];
    crrt_buff[0] = '0';
    crrt_buff[1] = '1';
    crrt_buff[2] = '\0';
    crrt_buff[0] = iridium_rx_buffer[rx_ind_start+0];
    crrt_buff[1] = iridium_rx_buffer[rx_ind_start+1];
    int res = atoi(crrt_buff);
    return res;
}

void IridiumManager::read_apply_iridium_instructions(void) {
  Serial.println(F("read apply iridium instructions"));

  size_t crrt_ind {0};

  // do we have enough chars left that worth to try to parse?
  while (iridium_rx_buffer.size() - crrt_ind >= 7){
   Serial.print(F("parse from start ind: ")); Serial.print(crrt_ind); Serial.print(F(" content ASCII ")); Serial.println(iridium_rx_buffer[crrt_ind]);

   // char at 0 and 6 must be ';' and '$', otherwise this is not a message we are trying to parse, try to parse one further in
   if (iridium_rx_buffer[crrt_ind+0] == '$' && iridium_rx_buffer[crrt_ind+6] == ';'){
    Serial.println(F("valid message separators..."));

    int command_value = read_value_from_command(crrt_ind+4);
    Serial.print(F("command_value: ")); Serial.println(command_value);

    // check if one of the accepted message kinds, and in this case, parse and apply

    if (iridium_rx_buffer[crrt_ind+1] == 'G' && iridium_rx_buffer[crrt_ind+2] == 'F' && iridium_rx_buffer[crrt_ind+3] == 'Q'){
       Serial.println(F("this is a GFQ message"));

       // we must take a GPS fix at least each 6 hours
       if (command_value <= 6*4){
        Serial.println(F("command value ok, apply it."));
        modifiable_interval_between_gnss_measurements_seconds = 15 * 60 * command_value;
       }
       else {
         Serial.println(F("command value is above threshold; ignore"));
       }
    }

    if (iridium_rx_buffer[crrt_ind+1] == 'W' && iridium_rx_buffer[crrt_ind+2] == 'F' && iridium_rx_buffer[crrt_ind+3] == 'Q'){
       Serial.println(F("this is a WFQ message"));

       // we can be very loose on how often measure waves, since we can always reach back using the GNSS
       // still put a limit of 12 hours
       if (command_value <= 12*2){
        Serial.println(F("command value ok, apply it."));
        modifiable_interval_between_wave_spectra_measurements = 30 * 60 * command_value;
       }
       else {
         Serial.println(F("command value is above threshold; ignore"));
       }
    }

    if (iridium_rx_buffer[crrt_ind+1] == 'T' && iridium_rx_buffer[crrt_ind+2] == 'F' && iridium_rx_buffer[crrt_ind+3] == 'Q'){
       Serial.println(F("this is a TFQ message"));

       // idem as GPS; not too critical, still put a limit of 12 hours
       if (command_value <= 12*2){
        Serial.println(F("command value ok, apply it."));
        modifiable_interval_between_thermistors_measurements_seconds = 30 * 60 * command_value;
       }
       else {
         Serial.println(F("command value is above threshold; ignore"));
       }
    }

    if (iridium_rx_buffer[crrt_ind+1] == 'G' && iridium_rx_buffer[crrt_ind+2] == 'M' && iridium_rx_buffer[crrt_ind+3] == 'L'){
       Serial.println(F("this is a GML message"));

       if (command_value <= max_nbr_GPS_fixes_per_message){
        Serial.println(F("command value ok, apply it."));
        modifiable_min_nbr_of_fix_per_message = command_value;
       }
       else {
         Serial.println(F("command value is above threshold; ignore"));
       }
    }

    // make ready to read the next packet
    crrt_ind += 7;
   }

   else{
    Serial.println(F("not a valid message"));
    crrt_ind += 1;
    continue;
   }
  }

  // quality check on the logics; make sure that we do not loose access to the instrument for all time in the future!!
  // for this, we make sure that GNSS (which is the "most often active" measurement) is attempting to transmit at least each 6 hours
  // that means 1) taking at least 1 measurement each 6 hours
  if (modifiable_interval_between_gnss_measurements_seconds > 6 * 60 * 60){
    modifiable_interval_between_gnss_measurements_seconds = 6 * 60 * 60;
  }
  if (modifiable_interval_between_gnss_measurements_seconds < 1 * 15 * 60){
    modifiable_interval_between_gnss_measurements_seconds = 15 * 60;
  }
  // 2) that the time between measurements times the min nbr of messages per buffer is at most 6 hours
  // we do not make sure that waves and thermistors are enabled as a multiple of the GNSS; this can be made sure of
  // by the user, and is not as critical (as will not loose contact with the instrument anyways)
  if (modifiable_min_nbr_of_fix_per_message < 1){
        modifiable_min_nbr_of_fix_per_message = 1;
  }
  if (modifiable_interval_between_gnss_measurements_seconds * modifiable_min_nbr_of_fix_per_message > 6 * 60 * 60){
        modifiable_min_nbr_of_fix_per_message = 6 * 60 * 60 / modifiable_interval_between_gnss_measurements_seconds;
  }
  if (modifiable_min_nbr_of_fix_per_message > max_nbr_GPS_fixes_per_message){
        modifiable_min_nbr_of_fix_per_message = max_nbr_GPS_fixes_per_message - 1;
  }

  Serial.println(F("the updated modifiable parameters are now:"));
  Serial.print(F("modifiable_interval_between_gnss_measurements_seconds ")); Serial.println(modifiable_interval_between_gnss_measurements_seconds);
  Serial.print(F("modifiable_min_nbr_of_fix_per_message ")); Serial.println(modifiable_min_nbr_of_fix_per_message);
  Serial.print(F("modifiable_interval_between_wave_spectra_measurements ")); Serial.println(modifiable_interval_between_wave_spectra_measurements);
  Serial.print(F("modifiable_interval_between_thermistors_measurements_seconds ")); Serial.println(modifiable_interval_between_thermistors_measurements_seconds);
}

bool IridiumManager::last_communication_was_successful(void) const {
    return send_receive_last_went_through;
}

bool IridiumManager::last_attempt_tried_sending(void) const{
   return attempt_tried_sending; 
}

IridiumManager iridium_manager;
