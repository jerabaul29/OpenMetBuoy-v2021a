#include "wave_analyzer.h"

WaveAnalyzer board_wave_analyzer;

void print_wave_packet(const Wave_Packet &packet){
  Serial.println(F("start print wave packet..."));
  Serial.print(F("posix_timestamp: ")); Serial.println(packet.posix_timestamp); delay(5);
  Serial.print(F("spectrum number: ")); Serial.println(packet.spectrum_number); delay(5);
  Serial.print(F("Hs: ")); Serial.println(packet.Hs); delay(5);
  Serial.print(F("Tz: ")); Serial.println(packet.Tz); delay(5);
  Serial.print(F("Tc: ")); Serial.println(packet.Tc); delay(5);
  Serial.print(F("max_value: ")); Serial.println(packet.max_value); delay(5);
  Serial.println(F("start of spectrum content..."));
  Serial.println(F("entries are power (quadratic) spectral densities of accel down, estimated with Welch averaging"));
  for (int ind=0; ind<nbr_wave_packet_freqs; ind++){
    Serial.print(F("ind ")); serial_print_int_width_4(ind);
    Serial.print(F(" | frq: ")); serial_print_float_width_16_prec_8((ind + welch_bin_min) * welch_frequency_resolution);
    Serial.print(F(" | entry uint_16: ")); serial_print_uint16_width_5(packet.array_pwelch[ind]);
    Serial.print(F(" | entry float: ")); serial_print_float_width_24_prec_8(static_cast<float>(packet.array_pwelch[ind]) * packet.max_value / 65000.0f);
    Serial.println();
    delay(5);
  }
  Serial.println(F("end of spectrum content"));
  Serial.println(F("end print wave packet")); delay(5);
}

void WaveAnalyzer::print_deque_content(void) const{
  Serial.print(F("wave_packet_buffer deque has size: ")); Serial.println(wave_packet_buffer.size());
  for (size_t i=0; i<wave_packet_buffer.size(); i++){
    Serial.print(F("print deque entry ")); Serial.println(i);
    print_wave_packet(wave_packet_buffer[i]);
    Serial.println(F("done"));
  }
}

void WaveAnalyzer::gather_and_analyze_wave_data(void){
  gather_imu_data();
  perform_welch_analysis_imu_data();
}

bool WaveAnalyzer::gather_imu_data(void){
  if (!use_dummy_imu_data){
    Serial.println(F("use true IMU data"));
    Serial.println(F("just testing functionality; we will only take a few measurements..."));

    // clean the data manager content
    board_data_manager.clear();

    // start the IMU
    board_imu_manger.start_IMU();

    // sample the full data manager needed content

    float acc_N;
    float acc_E;
    float acc_D;

    float yaw__;
    float pitch;
    float roll_;

    for (int sample=0; sample<100+99; sample++){
      wdt.restart();
      if (!board_imu_manger.get_new_reading(acc_N, acc_E, acc_D, yaw__, pitch, roll_)){
        Serial.println(F("ERROR gather_imu_data"));
        board_data_manager.clear();
        // we do not want to loose all what we have in RAM; just put a dummy entry...
        // TODO: consider having smarter retry methods
        for (int sample=0; sample<total_number_of_samples+99; sample++){
          board_data_manager.push_back_entry(DataEntry{0});
        }
        return false;
      }
      board_data_manager.push_back_entry(DataEntry{acc_D});

      Serial.print(millis()); Serial.print(F("ms | nmeas "));
      Serial.print(sample); Serial.print(F(" | "));
      Serial.print(acc_D); Serial.print(" ");
      Serial.println();
    }
    
    for (int sample=100+99; sample<total_number_of_samples+99; sample++){
      board_data_manager.push_back_entry(DataEntry{0});
    }

    // stop the IMU
    board_imu_manger.stop_IMU();
  }
  else {
    Serial.println(F("WARNING: use test dummy acceleration signal"));

    board_imu_manger.start_IMU();

    float acc_D;

    for (int sample=0; sample<total_number_of_samples+99; sample++){
      wdt.restart();

      acc_D = dummy_imu_accel_constant_component +  // DC
                dummy_imu_accel_amplitude * cos(2.0f * pi * dummy_imu_accel_frequency * sample * 0.1f);  // cos(2 pi f * n dt)

      board_data_manager.push_back_entry(DataEntry{acc_D});

      Serial.print(millis()); Serial.print(F("ms | nmeas "));
      Serial.print(sample); Serial.print(F(" | "));
      Serial.print(acc_D); Serial.print(" ");
      Serial.println();

      delay(1);
    }

    board_imu_manger.stop_IMU();
  }

  return true;
}

void WaveAnalyzer::perform_welch_analysis_imu_data(void){
  spectrum_number += 1;

  // check that enough accel data available
  Serial.print(F("data available has size ")); Serial.print(board_data_manager.size());
  if (board_data_manager.size() < total_number_of_samples + 75){
    Serial.println(F("too little data, abort!"));
    return;
  }

  wdt.restart();

  clear_working_welch_spectrum();
  clear_working_wave_packet();

  working_wave_packet.posix_timestamp = board_time_manager.get_posix_timestamp();
  working_wave_packet.spectrum_number = spectrum_number;

  // perform the FFT on the segments and add output to the current working spectrum
  size_t start_current_segment = 50;  // we got 100 extra points, start at half of that to have margin on both sides
  size_t end_current_segment = start_current_segment + fft_length;

  // go through the segments
  for (int crrt_segment_nbr=0; crrt_segment_nbr<number_welch_segments; crrt_segment_nbr++){
    wdt.restart();

    Serial.print(F("Segment between points number ")); Serial.print(start_current_segment); Serial.print(F(" - ")); Serial.println(end_current_segment);
    // clear and fill fft input output buffers
    clear_fft_input_output();

    for (int crrt_fft_input_index=0; crrt_fft_input_index<fft_length; crrt_fft_input_index++){
      fft_input[crrt_fft_input_index] = board_data_manager.get_entry(crrt_fft_input_index + start_current_segment).accel_down - 9.81f;
    }

    // apply windowing
    if (use_hanning_window){
      Serial.println(F("apply hanning window"));
      // from: https://en.wikipedia.org/wiki/Hann_function and https://community.sw.siemens.com/s/article/window-correction-factors
      float energy_scaling_factor = 1.63f;
      for (int crrt_fft_input_index=0; crrt_fft_input_index<fft_length; crrt_fft_input_index++){
        float scaling_sin = sin(pi * static_cast<float>(crrt_fft_input_index) / static_cast<float>(fft_length));
        fft_input[crrt_fft_input_index] *= scaling_sin * scaling_sin * energy_scaling_factor;
      }
    }

    if (use_hamming_window){
      Serial.println(F("apply hamming window"));
      // from: https://en.wikipedia.org/wiki/Window_function#Hamming_window and https://community.sw.siemens.com/s/article/window-correction-factors
      float energy_scaling_factor = 1.59f;
      for (int crrt_fft_input_index=0; crrt_fft_input_index<fft_length; crrt_fft_input_index++){
        float cos_part = cos(2.0f * pi * static_cast<float>(crrt_fft_input_index) / static_cast<float>(fft_length));
        fft_input[crrt_fft_input_index] *= energy_scaling_factor * (0.54f + 0.46f * cos_part);
      }
    }
    
    // perform FFT
    perform_fft();

    // accumulated welch output with the energy content in each bin, scaled by the number of segments
    // for how the output of the rfft with CMSIS looks like, see https://github.com/jerabaul29/Artemis_MbedOS_recipes/blob/main/recipes/recipe_CMSIS_FFT_fft_init/recipe_CMSIS_FFT_fft_init.ino: 
    for (int wave_packet_entry_index=0; wave_packet_entry_index<nbr_wave_packet_freqs; wave_packet_entry_index++){
      size_t fft_output_frequency_nbr = wave_packet_entry_index + welch_bin_min;
      float crrt_energy = fft_output[2 * fft_output_frequency_nbr] * fft_output[2 * fft_output_frequency_nbr] +
                          fft_output[2 * fft_output_frequency_nbr + 1] * fft_output[2 * fft_output_frequency_nbr + 1];
      crrt_energy /= static_cast<float>(number_welch_segments);
      working_welch_spectrum[wave_packet_entry_index] += 2.0f * crrt_energy / fft_length / fft_length / welch_frequency_resolution;  // there is a bit of scaling involved, see the recipe sketch for testing around that
    }

    // prepare for the next segment
    start_current_segment += fft_overlap;
    end_current_segment += fft_overlap;
  }

  // generate packet and push to the list of packets to use
  
  // find the max entry in the welch spectrum
  wdt.restart();

  Serial.println(F("find welch spectrum max entry; welch spectrum content is:"));

  float welch_spectrum_max_entry = 0.0f;
  float sqrt_m0 = 0.0f;  // sqrt_mp should be equal to the standard deviation of the elevation value
  float sqrt_m2 = 0.0f;
  float sqrt_m4 = 0.0f;
  float crrt_frequency = 0.0f;
  float crrt_frequency_2 = 0.0f;
  float crrt_frequency_4 = 0.0f;
  float omega_4 = 0.0f;

  for (int crrt_welch_ind=0; crrt_welch_ind<nbr_wave_packet_freqs; crrt_welch_ind++){
    crrt_frequency = (crrt_welch_ind + welch_bin_min) * welch_frequency_resolution;
    crrt_frequency_2 = crrt_frequency * crrt_frequency;
    crrt_frequency_4 = crrt_frequency_2 * crrt_frequency_2;
    omega_4 = pow(2.0f * pi * crrt_frequency, 4);
    // this shows quite a bit of conversion: 1) divide by omega**4 to go from acceleration to elevation spectrum, 2) multiply by df to compute the m0 integral
    sqrt_m0 += working_welch_spectrum[crrt_welch_ind] / omega_4 * welch_frequency_resolution;
    sqrt_m2 += crrt_frequency_2 * working_welch_spectrum[crrt_welch_ind] / omega_4 * welch_frequency_resolution;
    sqrt_m4 += crrt_frequency_4 * working_welch_spectrum[crrt_welch_ind] / omega_4 * welch_frequency_resolution;

    if (working_welch_spectrum[crrt_welch_ind] > welch_spectrum_max_entry){
      welch_spectrum_max_entry = working_welch_spectrum[crrt_welch_ind];
    }

    // and maybe do a bit of print...
    if (true){
      Serial.print("ind: "); Serial.print(crrt_welch_ind); Serial.print(F(" | f [Hz] ")); Serial.print(crrt_frequency); Serial.print(F(" | Pxx ")); Serial.println(working_welch_spectrum[crrt_welch_ind]);
      delay(5);
      wdt.restart();
    }
  }

  Serial.println(F("end of welch spectrum content"));

  sqrt_m0 = sqrt(sqrt_m0);
  Serial.print(F("sqrt_m0 ")); Serial.println(sqrt_m0);
  float Hs = 4 * sqrt_m0;

  sqrt_m2 = sqrt(sqrt_m2);
  sqrt_m4 = sqrt(sqrt_m4);

  float tz = sqrt_m2 / sqrt_m0;
  float tc = sqrt_m4 / sqrt_m2;

  working_wave_packet.Hs = Hs;
  Serial.print(F("Hs ")); Serial.println(Hs);

  working_wave_packet.Tz = tz;
  working_wave_packet.Tc = tc;

  Serial.print(F("Tz ")); Serial.println(tz, 4);
  Serial.print(F("Tc ")); Serial.println(tc, 4);

  // write the max value and the discretized spectrum
  Serial.println(F("fill wave packet with output data"));
  working_wave_packet.max_value = welch_spectrum_max_entry;

  for (int crrt_welch_ind=0; crrt_welch_ind<nbr_wave_packet_freqs; crrt_welch_ind++){
    uint16_t crrt_discretized = static_cast<uint16_t>(working_welch_spectrum[crrt_welch_ind] / welch_spectrum_max_entry * 65000.0f);  // note that we use only 65000 to avoid rounding error issues
    working_wave_packet.array_pwelch[crrt_welch_ind] = crrt_discretized;
  }

  wdt.restart();

  Serial.println(F("push wave packet"));
  // put the new spectrum packet in the deque
  // if already full, need to make some space
  if (wave_packet_buffer.full()){
      Serial.println(F("warning: wave packet deque was full; pop_front"));
      wave_packet_buffer.pop_front();
  }

  // push at the end the last fix
  wave_packet_buffer.push_back(working_wave_packet);

  Serial.println(F("analysis success!"));
}

void WaveAnalyzer::init_fast_instance(void){
  Serial.println(F("start init fast struct"));
  crrt_arm_status = arm_rfft_2048_fast_init_f32(&crrt_arm_rfft_fast_instance_f32);
  Serial.print(F("done init fast struct, status: ")); Serial.println(crrt_arm_status);
  wdt.restart();
}

void WaveAnalyzer::perform_fft(void){
  init_fast_instance();
  Serial.println(F("start perform FFT..."));
  arm_rfft_fast_f32(&crrt_arm_rfft_fast_instance_f32, fft_input, fft_output, forward_fft);
  Serial.println(F("done FFT"));
  wdt.restart();
}

void WaveAnalyzer::clear_fft_input_output(void){
  for (int i=0; i<fft_length; i++){
    fft_input[i] = 0.0f;
    fft_output[i] = 0.0f;
  }
  wdt.restart();
}

void WaveAnalyzer::clear_working_welch_spectrum(void){
  for (int i=0; i<nbr_wave_packet_freqs; i++){
    working_welch_spectrum[i] = 0.0f;
  }
  wdt.restart();
}

void WaveAnalyzer::clear_working_wave_packet(void){
  working_wave_packet.posix_timestamp = 0;
  working_wave_packet.spectrum_number = 0;
  working_wave_packet.Hs = -1.0f;
  working_wave_packet.max_value = -1.0f;
  for (int i=0; i<nbr_wave_packet_freqs; i++){
    working_wave_packet.array_pwelch[nbr_wave_packet_freqs] = 0;
  }
  wdt.restart();
}

void WaveAnalyzer::write_message_to_buffer(etl::ivector<unsigned char>& buffer){
  if (wave_packet_buffer.size() > 0){
    buffer.push_back('Y');

    // pointer to the start of the last entry in the wave_packet_buffer buffer, taken as raw data (unsigned char)
    unsigned char * start_crrt_wave_packet = (unsigned char *)&(wave_packet_buffer[wave_packet_buffer.size()-1]);

    for (size_t crrt_byte_ind=0; crrt_byte_ind<sizeof(Wave_Packet); crrt_byte_ind++){
      buffer.push_back(*(start_crrt_wave_packet + crrt_byte_ind));
    }

    buffer.push_back('E');

    wdt.restart();
  }
}

bool WaveAnalyzer::time_to_measure_waves(void) const{
  long modulo_time_result = (board_time_manager.get_posix_timestamp() % interval_between_wave_spectra_measurements);
  bool is_within = (modulo_time_result < tolerance_seconds_start_wave_measurements) || 
                       (interval_between_wave_spectra_measurements - modulo_time_result < tolerance_seconds_start_wave_measurements);
  return(is_within);
}

