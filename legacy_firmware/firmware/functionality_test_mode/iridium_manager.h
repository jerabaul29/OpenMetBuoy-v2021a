#ifndef IRIDIUM_MANAGER
#define IRIDIUM_MANAGER

#include "Arduino.h"

#include "etl.h"
#include "etl/vector.h"

#include "watchdog_manager.h"
#include "params.h"
#include "board_control.h"
#include <IridiumSBD.h>
#include "helpers.h"
#include "gnss_manager.h"
#include "wave_analyzer.h"
#include "thermistors_manager.h"

class IridiumManager{
  public:
    void clear_buffers(void);  // clear the rx and tx buffers and reset the flag rx_message_available
    bool has_received_new_message(void);  // have we a received message available in rx buffer?
    bool send_receive_message(unsigned long timeout_cap_charging_seconds=timeout_cap_charging_seconds);  // attempt a send_receive; return a success flag

    void attempt_transmit_gps_fixes(size_t min_nbr_messages);
    void attempt_transmit_wave_spectra(void);
    void attempt_transmit_thermistors_packets(size_t min_nbr_packets);

    bool last_communication_was_successful(void) const;  // if the last iridium transmission did go through
    bool last_attempt_tried_sending(void) const;  // if the last attempt did try to send something (i.e., there was something in buffer)

    static constexpr size_t iridium_tx_buffer_size = 340;
    static constexpr size_t iridium_rx_buffer_size = 271;  // this is a hack: set to 271, so that if nothing received, 271 remains the same, but if something received, the value will be strictly <271 since max size of msg is 270...

    etl::vector<unsigned char, iridium_tx_buffer_size> iridium_tx_buffer;
    etl::vector<unsigned char, iridium_rx_buffer_size> iridium_rx_buffer;

    bool rx_message_available {false};
    bool last_message_went_through {false};

    void reboot_if_requested_through_iridium(void);

  private:
    Uart iridium_serial{1, 25, 24};
    IridiumSBD iridium_sbd{iridium_serial, iridiumSleep, iridiumRI};

    unsigned char iridium_tx_raw_buffer[iridium_tx_buffer_size];
    uint8_t iridium_rx_raw_buffer[iridium_rx_buffer_size];
    size_t rx_buffer_amount {0};
    size_t tx_buffer_amount {0};

    bool send_receive_last_went_through {true};
    bool attempt_tried_sending {true};
};

extern IridiumManager iridium_manager;

#endif
