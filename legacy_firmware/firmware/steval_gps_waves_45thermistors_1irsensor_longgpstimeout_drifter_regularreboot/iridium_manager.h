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
    #ifndef DISABLE_ALL_THERMISTOR
      void attempt_transmit_thermistors_packets(size_t min_nbr_packets);
    #endif

    bool last_communication_was_successful(void) const;  // if the last iridium transmission did go through
    bool last_attempt_tried_sending(void) const;  // if the last attempt did try to send something (i.e., there was something in buffer)

    static constexpr size_t iridium_tx_buffer_size = 340;
    static constexpr size_t iridium_rx_buffer_size = 271;  // this is a hack: set to 271, so that if nothing received, 271 remains the same, but if something received, the value will be strictly <271 since max size of msg is 270...

    etl::vector<unsigned char, iridium_tx_buffer_size> iridium_tx_buffer;
    etl::vector<unsigned char, iridium_rx_buffer_size> iridium_rx_buffer;

    bool rx_message_available {false};
    bool last_message_went_through {false};

    void reboot_if_requested_through_iridium(void);  // read (i.e. parse) and apply the reboot instruction
    // to reboot the instrument, send the message over iridium:
    // BOOT [ i.e. rx_buffer[0]=='B', ... rx_buffer[3]=='T' ]

    void read_apply_iridium_instructions(void);  // read (i.e. parse) and apply the iridium instructions
    // there can be several such instructions send together at the same time; for this, just write several such instructions, one after the other
    // list of supported instructions:
    // $GFQXX; [Gps FreQuency setup: where XX is a number that indicates multiple of 15 minutes]
    // $WFQXX; [Wave FreQuency setup: where XX is a number that indicates multiple of 30 minutes]
    // $TFQXX; [Temperature_sensor FreQuency setup: where XX is a number that indicates multiple of 30 minutes]
    // $GMLXX; [Gps Min message Length; the minimum number of packets in a GNSS message; this can be used to make sure there is an iridium transmission attempt now and then]
    // for example:
    // $GFQ01; will set to measure GPS position each 1*15 minutes; we do not allow more seldom than 
    // $GFQ12; will set to measure GPS position each 12*15 minutes, i.e. each 180 minutes, ie each 3 hours
    // $GFQ02;$WFQ04; will set to measure GPS position each 2*15 minutes ie each 30 minutes, and set wave measurements to measure each 4*30 minutes ie each 2 hours
    // etc, etc
    // NOTE that there are some soft limiations to the parameters that can be set; namely, the measurement of waves and temperature sensors
    // should happen at the same time as GPS measurements otherwise, these may be ignored; i.e., the set of times when performing GPS measurements must be a superset of the set of times
    // when performing waves and temperatures measurements. For example, GNSS each 30 minutes and waves each 1 hour is ok, but gnss each 4 hours and waves
    // each hour is not ok (or rather, all will work fine, but waves will be measured each 4 hours, not each hour). This is the responsibility of the user to enforce these conditions.
    // we put some logics test to avoid shooting yourself in the foot and / or "bricking" the instrument and making it useless :)
    // however, still be CAREFUL with these options: you can still shoot yourself in the foot even with the safety guards (and the safety guards
    // can always have some unexpected flaws that would let you shoot yourself in the foot badly). In particular, the instrument will check for new transmissions
    // i.e. try to update the parameters only when having some data to transmit... I.e., if the instrument has parameters that mean that
    // it tries to transmit only each 6 hours, you will not be able to turn higher up measurement frequency until the next message is sent and received, which in this example would be 6 hours.
    // to avoid to shoot yourself in the foot too bad, we have some logics guards in place so that the instrument always try to transmit
    // at least a GNSS message at least each 6 hours; i.e., IF there are no flaws in the logics guards, it should not be possible to put the instrument
    // in such a "bad" state that it does not try to send / receive an iridium message at least once each 6 hours.
    // But still, be careful, and double check both the formatting and the logics of your messages before sending them!!

  private:
    Uart iridium_serial{1, 25, 24};
    IridiumSBD iridium_sbd{iridium_serial, iridiumSleep, iridiumRI};

    unsigned char iridium_tx_raw_buffer[iridium_tx_buffer_size];
    uint8_t iridium_rx_raw_buffer[iridium_rx_buffer_size];
    size_t rx_buffer_amount {0};
    size_t tx_buffer_amount {0};

    bool send_receive_last_went_through {true};
    bool attempt_tried_sending {true};

    int read_value_from_command(size_t rx_ind_start);
};

extern IridiumManager iridium_manager;

#endif
