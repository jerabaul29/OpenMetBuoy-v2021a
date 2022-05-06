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

bool IridiumManager::last_communication_was_successful(void) const {
    return send_receive_last_went_through;
}

bool IridiumManager::last_attempt_tried_sending(void) const{
   return attempt_tried_sending; 
}

IridiumManager iridium_manager;
