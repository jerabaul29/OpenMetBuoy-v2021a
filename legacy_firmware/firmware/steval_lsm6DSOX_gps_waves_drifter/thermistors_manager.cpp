#include "thermistors_manager.h"

//--------------------------------------------------------------------------------
// helpers

void address_to_uint64_t(Address &addr_in, uint64_t &uint64_result) {
  uint64_result = 0;
  for (int crrt_byte_index = 0; crrt_byte_index < 8; crrt_byte_index++) {
    uint64_result += static_cast<uint64_t>(addr_in[crrt_byte_index])
                     << (8 * (7 - crrt_byte_index));
  }
}

void uint64_t_to_address(uint64_t const &uint64_in, Address &addr_out) {
  for (int crrt_byte_index = 0; crrt_byte_index < 8; crrt_byte_index++) {
    addr_out[crrt_byte_index] =
        static_cast<byte>(uint64_in >> (8 * (7 - crrt_byte_index)));
  }
}

void print_uint64(uint64_t const &num) {
  uint32_t low = num % 0xFFFFFFFF;
  uint32_t high = (num >> 32) % 0xFFFFFFFF;

  Serial.print(high);
  Serial.print(low);
}

void print_address(Address const &addr) {
  for (int i = 0; i < 8; i++) {
    Serial.print(addr[i], HEX);
    Serial.write(' ');
  }
}

void uint64_t_to_6bits_int_id(uint64_t const &uint64_in,
                              uint8_t &uint8_6bits_id) {
  Address crrt_address;
  uint64_t_to_address(uint64_in, crrt_address);

  // we know that:
  // the first byte is the "kind", should be 0x28
  // the next 6 bytes are the serial number
  // the LS bytes are first; then end is still 0x00 0x00 (as production has not
  // gone so far) the last byte is the CRC so, use the 6 LSBs of the LS byte of
  // the serial number (these are the ones that "rotate" fastest)

  byte byte_to_use = crrt_address[1];
  byte byte_result = byte_to_use & 0b00111111;
  uint8_6bits_id = byte_result;
}

// function to order thermistors by 6 lower bits
// need to be equivalent to >, NOT to >=, see C++ standard
// https://en.cppreference.com/w/cpp/utility/functional/greater
bool greater_6bits_id(uint64_t const &id_1, uint64_t const &id_2) {
  uint8_t id6_1;
  uint8_t id6_2;
  uint64_t_to_6bits_int_id(id_1, id6_1);
  uint64_t_to_6bits_int_id(id_2, id6_2);
  return id6_1 > id6_2;
}

void print_thermistors_packet(Thermistors_Packet &packet){
  Serial.println(F("--- start print binary thermistor packet ---"));

  Serial.print(F("binary content: "));
  unsigned char * start_packet = (unsigned char *) & packet;
  for (size_t crrt_ind=0; crrt_ind<sizeof(Thermistors_Packet); crrt_ind++){
    Serial.print(* (start_packet+crrt_ind), HEX);
    Serial.print(F(" "));
  }
  Serial.println();

  Serial.print(F("posix_timestamp: ")); Serial.println(packet.posix_timestamp); delay(5);
  // Serial.print(F("reading_number: ")); Serial.println(packet.reading_number); delay(5);

  Serial.print(F("pitch mean: ")); Serial.print(packet.mean_pitch); delay(5);
    Serial.print(F(" | min: ")); Serial.print(packet.min_pitch); delay(5);
    Serial.print(F(" | max: ")); Serial.print(packet.max_pitch); delay(5);
    Serial.println();

  Serial.print(F("roll mean: ")); Serial.print(packet.mean_roll); delay(5);
    Serial.print(F(" | roll min: ")); Serial.print(packet.min_roll); delay(5);
    Serial.print(F(" | roll max: ")); Serial.print(packet.max_roll); delay(5);
    Serial.println();

  for (int crrt_ind=0; crrt_ind<number_of_thermistors; crrt_ind++){
    Serial.print(F("thermistor ")); Serial.print(crrt_ind);
      Serial.print(F(" | ID: ")); Serial.print((packet.data_array[3 * crrt_ind + 0] & 0b11111100) >> 2);
      uint16_t crrt_mean {0};
      crrt_mean |= (packet.data_array[3 * crrt_ind + 0] & 0b00000011) << 10;
      crrt_mean |= (packet.data_array[3 * crrt_ind + 1] & 0b11111111) << 2;
      crrt_mean |= (packet.data_array[3 * crrt_ind + 2] & 0b11000000) >> 6;
      Serial.print(F(" | mean * 16.0: ")); Serial.print(crrt_mean);
      Serial.print(F(" | range * 16.0: ")); Serial.print((packet.data_array[3 * crrt_ind + 2] & 0b00111111));
      Serial.println();
  }

  Serial.println(F("--- done print thermistor packet ---"));
}

//--------------------------------------------------------------------------------
// class implementation

void Thermistors_Manager::start(void) {
    Serial.println(F("start thermistors"));
  // give power to the thermistors
  turn_thermistors_on();

  // get the list of active thermistors
  get_ordered_thermistors_ids();

  // ask for one conversion to start
  request_start_thermistors_conversion();

  // turn the IMU on
  Serial.println(F("gather IMU data for attitude of thermistors"));
  board_imu_manger.start_IMU();

  float acc_N;
  float acc_E;
  float acc_D;
  float yaw;
  float pitch;
  float roll;
  int8_t pitch_int8;
  int8_t roll_int8;

  // in order to avoid overflows etc, discretize the pitch and roll into a uint8_t, and compute the mean using a uint32_t
  int8_t min_roll {INT8_MAX};
  int8_t max_roll {INT8_MIN};
  int8_t min_pitch {INT8_MAX};
  int8_t max_pitch {INT8_MIN};

  int8_t mean_pitch {-128};
  int8_t mean_roll {-128};
  int32_t mean_pitch_accumulator {0};
  int32_t mean_roll_accumulator {0};

    // TODO: do something with these
  for (int i=0; i<number_of_thermistor_imu_measurements;i++){
    board_imu_manger.get_new_reading(acc_N, acc_E, acc_D, yaw, pitch, roll);

    pitch_int8 = static_cast<int8_t>(pitch_float_to_int8_factor * pitch);
    roll_int8 = static_cast<int8_t>(roll_float_to_int8_factor * roll);

    Serial.print(F("yaw ")); Serial.print(yaw); Serial.print(F(" | pitch ")); Serial.print(pitch); Serial.print(F(" discretized ")); Serial.print(pitch_int8); Serial.print(F(" | roll "));  Serial.print(roll); Serial.print(F(" discretized ")); Serial.print(roll_int8);
    Serial.println();

    min_roll = min(min_roll, roll_int8);
    max_roll = max(max_roll, roll_int8);
    min_pitch = min(min_pitch, pitch_int8);
    max_pitch = max(max_pitch, pitch_int8);
    mean_pitch_accumulator += pitch_int8;
    mean_roll_accumulator += roll_int8;

    wdt.restart();
  }

  mean_pitch = static_cast<int8_t>(mean_pitch_accumulator / number_of_thermistor_imu_measurements);
  mean_roll = static_cast<int8_t>(mean_roll_accumulator / number_of_thermistor_imu_measurements);

  Serial.println(F("int8_t stats:"));
  Serial.print(F("mean_pitch: ")); Serial.print(mean_pitch); Serial.print(F(" | min pitch ")); Serial.print(min_pitch); Serial.print(F(" | max pitch ")); Serial.print(max_pitch); Serial.println();
  Serial.print(F("mean_roll: ")); Serial.print(mean_roll); Serial.print(F(" | min roll ")); Serial.print(min_roll); Serial.print(F(" | max roll ")); Serial.print(max_roll); Serial.println();

  thermistors_packet.posix_timestamp =  board_time_manager.get_posix_timestamp();
  // thermistors_packet.reading_number = reading_number;
  thermistors_packet.mean_pitch = mean_pitch;
  thermistors_packet.min_pitch = min_pitch;
  thermistors_packet.max_pitch = max_pitch;
  thermistors_packet.mean_roll = mean_roll;
  thermistors_packet.min_roll = min_roll;
  thermistors_packet.max_roll = max_roll;

  board_imu_manger.stop_IMU();
}

  void Thermistors_Manager::stop(void) {
    turn_thermistors_off();
}

void Thermistors_Manager::get_ordered_thermistors_ids(void){
    Serial.println(F("get list of ordered thermistors IDs"));

    // clear
    Serial.println(F("clear list of thermistors IDs"));
    vector_of_ids.clear();

    // get all IDs
    Address crrt_addr;
    uint64_t crrt_id;

    while (true){

        wdt.restart();
        
      if ( !one_wire_thermistors.search(crrt_addr) ) {
        Serial.println("No more addresses.");
        one_wire_thermistors.reset_search();
        delay(250);
  
        break;
      }
  
      Serial.println(F("found new address..."));
      Serial.print("ROM =");
      print_address(crrt_addr);
      Serial.println();
  
      address_to_uint64_t(crrt_addr, crrt_id);
      Serial.print(F("ID = ")); print_uint64(crrt_id); Serial.println();
      uint8_t crrt_6_bits_id {0};
      uint64_t_to_6bits_int_id(crrt_id, crrt_6_bits_id);
      Serial.print(F("6 bits ID = ")); Serial.print(crrt_6_bits_id); Serial.println();
  
      if (OneWire::crc8(crrt_addr, 7) != crrt_addr[7]) {
        Serial.println("WARNING: CRC is not valid!");
        continue;
      }
      else{
        Serial.println(F("CRC is valid"));
      }
  
      // the first ROM byte indicates which chip
      switch (crrt_addr[0]) {
          case 0x10:
          Serial.println("  WARNING: Chip = DS18S20");  // or old DS1820
          break;
        case 0x28:
          Serial.println("  CORRECT: Chip = DS18B20");
          vector_of_ids.push_back(crrt_id);
          break;
        case 0x22:
          Serial.println("  WARNING: Chip = DS1822");
          break;
        default:
          Serial.println("EROR: Device is not a DS18x20 family device.");
          continue;
      } 
    }
  
    // we sort greater first; i.e., the sensors with greater IDs are sorted first;
    // i.e., if the thermistor string have greater IDs higher up, then the sensors are sorted from higher up to lower down
    // we actually need to order by reduced ID, as these are the ones we are transmitting...
    // etl::sort(vector_of_ids.begin(), vector_of_ids.end(), std::greater<uint64_t>());  // order by normal ID; not what we need
    etl::sort(vector_of_ids.begin(), vector_of_ids.end(), greater_6bits_id);
  
    Serial.println(F("sorted list of IDs"));
  
    for (auto const & crrt_elem : vector_of_ids){
          print_uint64(crrt_elem); Serial.println();
    }
  
    return;
}

void Thermistors_Manager::request_start_thermistors_conversion(void) {
    wdt.restart();

  start_last_conversion_ms = millis();

  Address crrt_address;

  // ask each sensor to start new measurement
  Serial.println(F("ask to start conversion..."));
  for (auto &crrt_id : vector_of_ids) {
    uint64_t_to_address(crrt_id, crrt_address);
    print_address(crrt_address);
    Serial.println();

    one_wire_thermistors.reset();
    one_wire_thermistors.select(crrt_address);
    one_wire_thermistors.write(0x44, 1); // start conversion, with parasite power on at the end
  }

  return;
}

void Thermistors_Manager::collect_thermistors_conversions(void) {
    wdt.restart();
  Address crrt_address;
  byte present = 0;
  byte data[12];
  float celsius;
  byte crc;

  // wait until conversion is ready
  delay(remaining_conversion_time());
  
  vector_of_instantaneous_readings.clear();

  // collect the output of each sensor
  Serial.println(F("collect results..."));
  for (auto &crrt_id : vector_of_ids) {
    uint64_t_to_address(crrt_id, crrt_address);
    print_address(crrt_address);
    Serial.println();
    uint8_t crrt_6bits_id {0};
    uint64_t_to_6bits_int_id(crrt_id, crrt_6bits_id);
    Serial.print(crrt_6bits_id);
    Serial.println();

    present = one_wire_thermistors.reset();
    one_wire_thermistors.select(crrt_address);
    one_wire_thermistors.write(0xBE); // Read Scratchpad

    Serial.print("  Data = ");
    Serial.print(present, HEX);
    Serial.print(" ");
    for (int i = 0; i < 9; i++) { // we need 9 bytes
      data[i] = one_wire_thermistors.read();
      Serial.print(data[i], HEX);
      Serial.print(" ");
    }
    Serial.print(" CRC=");
    crc = OneWire::crc8(data, 8);
    Serial.print(crc, HEX);
    if (crc != data[8]) {
      Serial.println(F(" ERROR: non matching CRC"));
    } else {
      Serial.println(F(" CRC OK"));
    }

    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) {
      raw = raw & ~7;
      Serial.println(F("  WARNING: 9 bit res"));
    } // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) {
      raw = raw & ~3;
      Serial.println(F("  WARNING: 10 bit res"));
    } // 10 bit res, 187.5 ms
    else if (cfg == 0x40) {
      raw = raw & ~1;
      Serial.println(F("  WARNING: 11 bit res"));
    } // 11 bit res, 375 ms
    else if (cfg == 0x60) {
      Serial.println(F("  OK: 12 bits resolution"));
    } else {
      Serial.println(F("  ERROR: unknown resolution config!"));
    }

    celsius = (float)raw / 16.0;
    Serial.print("  Temperature = ");
    Serial.print(celsius);
    Serial.print(" Celsius");
    Serial.println();

    vector_of_instantaneous_readings.push_back(raw);
  }
}

unsigned long Thermistors_Manager::remaining_conversion_time(void){
    if (millis() - start_last_conversion_ms < duration_conversion_thermistor_ms) {
        unsigned long result = duration_conversion_thermistor_ms - (millis() - start_last_conversion_ms);
        Serial.print(F("conversion not over yet; there are ")); Serial.print(result); Serial.println(F(" ms left"));
        return result;
    }
    else{
        return 0UL;
    }
}

void Thermistors_Manager::perform_time_acquisition(void) {
  unsigned long time_start_acquisition = millis();

  sum_of_readings.fill(0);
  max_reading.fill(INT16_MIN);
  min_reading.fill(INT16_MAX);

  int number_of_readings = 0;

  while (millis() - time_start_acquisition <
         duration_thermistor_acquisition_ms) {
    collect_thermistors_conversions();

    // use the data
    Serial.println(F("register the data"));
    size_t crrt_index = 0;
    for (auto & crrt_value : vector_of_instantaneous_readings){
      Serial.print(F("current index: ")); Serial.println(crrt_index);

      sum_of_readings[crrt_index] += crrt_value;
      max_reading[crrt_index] = max(max_reading[crrt_index], crrt_value);
      min_reading[crrt_index] = min(min_reading[crrt_index], crrt_value);

      Serial.print(F("crrt sum: ")); Serial.print(sum_of_readings[crrt_index]);
        Serial.println(F(" | max: ")); Serial.print(max_reading[crrt_index]);
        Serial.println(F(" | min: ")); Serial.print(min_reading[crrt_index]);
        Serial.println();

      crrt_index += 1;
    }
    number_of_readings += 1;

    request_start_thermistors_conversion();
  }

  // done with collecting the stats; put them into the current packet
  Serial.println(F("get the stats and write stats"));
  for (size_t crrt_index=0; crrt_index<number_of_thermistors; crrt_index++){
    int16_t crrt_mean = static_cast<int16_t>(sum_of_readings[crrt_index] / number_of_readings);
    uint16_t crrt_range = static_cast<uint16_t>(max_reading[crrt_index] - min_reading[crrt_index]);

    Serial.print(F("current index: ")); Serial.println(crrt_index);
      
    Serial.print(F("mean: ")); Serial.print(crrt_mean);
      Serial.print(F(" | range: ")); Serial.print(crrt_range);
      Serial.println();

    // NOTE: we first write this into an array, before copying to the final destination, but could write directly
    // to the destination actually
    uint8_t crrt_binary_entry[3];
    crrt_binary_entry[0] = 0;
    crrt_binary_entry[1] = 0;
    crrt_binary_entry[2] = 0;

    // 6 bits LSBs of ID
    uint8_t crrt_6_bits_id {0};
    uint64_t_to_6bits_int_id(vector_of_ids[crrt_index], crrt_6_bits_id);
    uint8_t current_bits_to_write {0};
    current_bits_to_write = crrt_6_bits_id;
    current_bits_to_write = (current_bits_to_write << 2) & 0b11111100;
    crrt_binary_entry[0] |= current_bits_to_write;

    // 12 bits average reading
    // 2 bits first
    current_bits_to_write = 0;
    current_bits_to_write = (crrt_mean >> 10) & 0b00000011;
    crrt_binary_entry[0] |= current_bits_to_write;
    // 8 bits more
    current_bits_to_write = 0;
    current_bits_to_write = (crrt_mean >> 2) & 0b11111111;
    crrt_binary_entry[1] = current_bits_to_write;

    // last 2 bits
    current_bits_to_write = 0;
    current_bits_to_write = (crrt_mean << 6) & 0b11000000;
    crrt_binary_entry[2] = current_bits_to_write;

    // 6 bits range
    current_bits_to_write = 0;
    current_bits_to_write = crrt_range & 0b00111111;
    crrt_binary_entry[2] |= current_bits_to_write; 

    // copy the results into the packet
    thermistors_packet.data_array[3 * crrt_index + 0] = crrt_binary_entry[0];
    thermistors_packet.data_array[3 * crrt_index + 1] = crrt_binary_entry[1];
    thermistors_packet.data_array[3 * crrt_index + 2] = crrt_binary_entry[2];
  }

  print_thermistors_packet(thermistors_packet);
}

void Thermistors_Manager::get_and_push_data(void){
    start();

    perform_time_acquisition();

    stop();

  // put the new thermistors packet in the deque
  // if already full, need to make some space
  if (thermistors_packets_buffer.full()){
      Serial.println(F("warning: thermistors packet deque was full; pop_front"));
      thermistors_packets_buffer.pop_front();
  }

  // push at the end the last fix
  thermistors_packets_buffer.push_back(thermistors_packet);
  Serial.println(F("done pushing thermistors packet"));

    reading_number += 1;
}

void Thermistors_Manager::print_deque_content(void){
  Serial.print(F("thermistors buffer contains ")); Serial.print(thermistors_packets_buffer.size()); Serial.print(F(" entries, out of max "));
    Serial.print(thermistors_packets_buffer.max_size()); Serial.println();
  int index = 0;
  for (auto & crrt_entry : thermistors_packets_buffer){
    Serial.print(F("print entry ")); Serial.println(index);
    print_thermistors_packet(crrt_entry);
    index += 1;
  }
}

size_t Thermistors_Manager::write_message_to_buffer(etl::ivector<unsigned char>& buffer, size_t max_nbr_packets){
  // message format:
  // T  [nbr packets so far] P [message] P [message] E

  buffer.push_back('T');
  buffer.push_back((unsigned char)(reading_number%256));

  // how many packets to write
  size_t nbr_packets_to_write {0};
  nbr_packets_to_write = min(max_nbr_thermistor_packets, thermistors_packets_buffer.size());
  Serial.print(F("write ")); Serial.print(nbr_packets_to_write); Serial.println(F(" thermistor packets for now"));

  for (int packet_nbr=0; packet_nbr<nbr_packets_to_write; packet_nbr++){
    buffer.push_back('P');

    Thermistors_Packet crrt_thermistors_packet = thermistors_packets_buffer[thermistors_packets_buffer.size() - 1 - packet_nbr];
    unsigned char * start_of_crrt_packet = (unsigned char *) & crrt_thermistors_packet;

    for (int crrt_byte=0; crrt_byte<sizeof(Thermistors_Packet); crrt_byte++){
      buffer.push_back(*(start_of_crrt_packet+crrt_byte));
    }
  }

  buffer.push_back('E');

  return nbr_packets_to_write;
}

void Thermistors_Manager::clear_number_sent_packets(size_t number_of_packets_to_clear){
  for (size_t ind_to_clean=0; ind_to_clean<number_of_packets_to_clear; ind_to_clean++){
    thermistors_packets_buffer.pop_back();
  }
}

bool Thermistors_Manager::time_to_measure_thermistors(void) const{
  long modulo_time_result = (board_time_manager.get_posix_timestamp() % interval_between_thermistors_measurements_seconds);
  bool is_within = (modulo_time_result < tolerance_seconds_start_thermistors_measurements) || 
                       (interval_between_wave_spectra_measurements - modulo_time_result < tolerance_seconds_start_thermistors_measurements);
  return(is_within);
}

OneWire one_wire_thermistors(THERMISTORS_ONE_WIRE_PIN);
Thermistors_Manager board_thermistors_manager;


