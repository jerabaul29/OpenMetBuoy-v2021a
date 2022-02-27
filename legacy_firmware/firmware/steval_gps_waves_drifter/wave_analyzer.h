#ifndef WAVE_ANALYZER_H
#define WAVE_ANALYZER_H

#include "Arduino.h"

#include "arm_math.h"
#include "ard_supers/avr/dtostrf.h"

#include "etl.h"
#include "etl/vector.h"
#include "etl/deque.h"

#include "params.h"
#include "imu_manager.h"
#include "data_manager.h"
#include "watchdog_manager.h"
#include "time_manager.h"
#include "helpers.h"

// take FFT with Welch method
// put in buffer
// for each segment
// load segment
// cpmpute FFT
// add to the Welch accumulator
// normalize Welch accumulator
// encode it
// put in buffer

// methods for:
// processing the data currently in the data manager and push so the buffer
// check how many available in the buffer
// push / pop spectra
// note: transmit one or several spectra at a time?


// we use 50% overlap when taking FFT

// take one FFT
// 

// TODO: have some error codes etc; clean the implementations; decide what belongs to who in terms of data and tasks to perform

struct Wave_Packet{
    long posix_timestamp;
    unsigned int spectrum_number;
    float Hs;
    float Tz;
    float Tc;
    float max_value;
    uint16_t array_pwelch[welch_bin_max - welch_bin_min];
};

void print_wave_packet(Wave_Packet const & packet);

class WaveAnalyzer{
    public:
        void gather_and_analyze_wave_data(void);

        // gather the needed data in the data analyzer and make things ready to perform a wave analysis
        bool gather_imu_data(void);

        // perform wave analysis, and push the result into the buffer of Wave_Packet data
        void perform_welch_analysis_imu_data(void);

        // buffering of the wave packet data
        etl::deque<Wave_Packet, size_wave_packet_buffer> wave_packet_buffer;

        void print_deque_content(void) const;

        void write_message_to_buffer(etl::ivector<unsigned char>& buffer);
        
        bool time_to_measure_waves(void) const;
    private:
        // ------------------------------------------------------------------------------------------

        // prepare the twiddle factors etc
        void init_fast_instance(void);

        // perform the rfft from fft_input to fft_output
        void perform_fft(void);

        // various buffer and variable cleaning
        void clear_fft_input_output(void);
        void clear_working_welch_spectrum(void);
        void clear_working_wave_packet(void);

        // ------------------------------------------------------------------------------------------
        // FFT properties
        // admissible rfft lengths as well as some documentation are available at
        // https://www.keil.com/pack/doc/CMSIS/DSP/html/group__RealFFT.html#gac5fceb172551e7c11eb4d0e17ef15aa3
        // so the length used in the params.h should be within these values
        static constexpr uint8_t forward_fft = 0;
        static constexpr uint8_t backward_fft = 1;

        // FFT CMSIS magics
        // the twiddle factors and other precomputed params; this can either be filled in the code (as is done here), or use a precomputed table (need arm_const_structs.h).
        arm_rfft_fast_instance_f32 crrt_arm_rfft_fast_instance_f32;
        // getting our status back
        arm_status crrt_arm_status;

        // FFT buffers
        // buffers for: input and output; these will be used and modified by the fft function calls
        // for RFFT, there are a bit of "hacks", see the discussion at: https://github.com/ARM-software/CMSIS_5/issues/1091
        // input: an array of samples, all real, nothing special, N samples [r_sample[0], r_samples[1], ...]
        float32_t fft_input[fft_length];
        // output: there is a bit of a hack here; the FT of a real signal is conjugate symmetric, i.e. Hermitian, i.e. FFT(i) = conj(FFT(N-i)), so it is enough
        // to just output the coefficients from and included to 0 to N/2; in addition, FFT(0) and FFT(N/2) are real, not complex; the CMSIS RFFT takes advantage
        // of that, so that the output is: [FFT(0), FFT(N/2), real(FFT(1)), img(FFT(1)), real(FFT(2)), img(FFT(2)), ...]
        float32_t fft_output[fft_length];

        // working wave spectrum
        float32_t working_welch_spectrum[welch_bin_max-welch_bin_min];

        // working wave packet
        Wave_Packet working_wave_packet;

        unsigned int spectrum_number {0};
};

extern WaveAnalyzer board_wave_analyzer;

#endif
