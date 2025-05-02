#!/usr/bin/python3
# before I used #!/bin/python3 , but some users were experiencing problems

import binascii
import struct
import datetime
import time
import os
from dataclasses import dataclass
import click
from icecream import ic
import math
import numpy as np
import matplotlib.pyplot as plt
import pprint
import scipy.signal as signal

ic.configureOutput(prefix='', outputFunction=print)
p_print = pprint.PrettyPrinter(indent=2).pprint

"""
A bit of nomenclature:
- a message is a string sent / received over iridium
- a message is composed of one or more packets, where a packet is a dumped struct containing data
- each message may have metadata of its own separately from any packet
- a message is then composed of 3 parts: kind (which kind of data is in the message), metadata, and list of packets
"""

#--------------------------------------------------------------------------------
# make sure we are all UTC

os.environ["TZ"] = "UTC"
time.tzset()

#--------------------------------------------------------------------------------
# a few module constants

_BD_VERSION_NBR = "2.1"

####################
# derived properties of the GNSS packets
# 14; 1 byte start, posix, 2 longs, 1 byte end
_BD_GNSS_PACKET_LENGTH = 14

####################
# properties of the wave spectra packets with 2048 FFT_LEN
_BD_YWAVE_PACKET_MIN_BIN = 9
_BD_YWAVE_PACKET_MAX_BIN = 64
_BD_YWAVE_PACKET_SCALER = 65000
_BD_YWAVE_PACKET_SAMPLING_FREQ_HZ = 10.0
_BD_YWAVE_PACKET_NBR_SAMPLES_PER_SEGMENT = 2**11

# derived properties of the wave packer
_BD_YWAVE_NBR_BINS = _BD_YWAVE_PACKET_MAX_BIN - _BD_YWAVE_PACKET_MIN_BIN
_BD_YWAVE_PACKET_FRQ_RES = _BD_YWAVE_PACKET_SAMPLING_FREQ_HZ / _BD_YWAVE_PACKET_NBR_SAMPLES_PER_SEGMENT

# 138 in total length; 1 byte start, posix, int, 4 floats, uint36 array, 2 bytes for alignment, 1 byte end
_BD_YWAVE_PACKET_LENGTH = \
    1 + 4 + 4 + 4*4 + _BD_YWAVE_NBR_BINS * 2 + 2 + 1
LENGTH_FROM_SERIAL_OUTPUT = 138
assert _BD_YWAVE_PACKET_LENGTH == LENGTH_FROM_SERIAL_OUTPUT, "the arduino printout indicates that wave packets have length {}".format(LENGTH_FROM_SERIAL_OUTPUT)

####################
# properties of the default (4) thermistors packets
_BD_THERM_MSG_FIXED_LENGTH = 3  # start byte, byte metadata nbr packets, byte end
_BD_THERM_MSG_NBR_THERMISTORS = 4
_BD_THERM_PACKET_NBR_BYTES_PER_THERMISTOR = 3
_BD_THERM_PACKET_LENGTH = 1 + 1*4 + _BD_THERM_PACKET_NBR_BYTES_PER_THERMISTOR * _BD_THERM_MSG_NBR_THERMISTORS * 1
# these are from the params of the tracker
# _BD_THERM_ROLL_FLOAT_TO_INT8_FACTOR = 0.7
# _BD_THERM_PITCH_FLOAT_TO_INT8_FACTOR = 1.4
# factor for converting bin 12 bits signed int to temperature; from thermistor datasheet
_BD_THERM_12BITS_TO_FLOAT_TEMPERATURE_FACTOR = 1.0 / 16.0

####################
# properties of the thermistors16 packets
_BD_THERM16_MSG_FIXED_LENGTH = 3  # start byte, byte metadata nbr packets, byte end
_BD_THERM16_MSG_NBR_THERMISTORS = 16
_BD_THERM16_PACKET_NBR_BYTES_PER_THERMISTOR = 3
_BD_THERM16_PACKET_LENGTH = 1 + 1*4 + _BD_THERM16_PACKET_NBR_BYTES_PER_THERMISTOR * _BD_THERM16_MSG_NBR_THERMISTORS * 1
# these are from the params of the tracker
# _BD_THERM_ROLL_FLOAT_TO_INT8_FACTOR = 0.7
# _BD_THERM_PITCH_FLOAT_TO_INT8_FACTOR = 1.4
# factor for converting bin 12 bits signed int to temperature; from thermistor datasheet
_BD_THERM16_12BITS_TO_FLOAT_TEMPERATURE_FACTOR = 1.0 / 16.0

####################
# properties of the 3 thermistors + mlx packets
_BD_THERMMLX_MSG_FIXED_LENGTH = 3  # start byte, byte metadata nbr packets, byte end
_BD_THERMMLX_MSG_NBR_THERMISTORS = 3
_BD_THERMMLX_PACKET_NBR_BYTES_PER_THERMISTOR = 3
_BD_THERMMLX_PACKET_LENGTH = 1 + 4 + 5 + _BD_THERMMLX_PACKET_NBR_BYTES_PER_THERMISTOR * _BD_THERMMLX_MSG_NBR_THERMISTORS + 2
# these are from the params of the tracker
# _BD_THERM_ROLL_FLOAT_TO_INT8_FACTOR = 0.7
# _BD_THERM_PITCH_FLOAT_TO_INT8_FACTOR = 1.4
# factor for converting bin 12 bits signed int to temperature; from thermistor datasheet
_BD_THERM_12BITS_TO_FLOAT_TEMPERATURE_FACTOR = 1.0 / 16.0

####################
# properties of the 27 thermistors + mlx packets
_BD_27THERM1MLX_MSG_FIXED_LENGTH = 3  # start byte, byte metadata nbr packets, byte end
_BD_27THERM1MLX_MSG_NBR_THERMISTORS = 27
_BD_27THERM1MLX_PACKET_NBR_BYTES_PER_THERMISTOR = 3
_BD_27THERM1MLX_PACKET_LENGTH = 1 + 4 + 5 + _BD_27THERM1MLX_PACKET_NBR_BYTES_PER_THERMISTOR * _BD_27THERM1MLX_MSG_NBR_THERMISTORS + 2
# these are from the params of the tracker
# _BD_THERM_ROLL_FLOAT_TO_INT8_FACTOR = 0.7
# _BD_THERM_PITCH_FLOAT_TO_INT8_FACTOR = 1.4
# factor for converting bin 12 bits signed int to temperature; from thermistor datasheet
_BD_THERM_12BITS_TO_FLOAT_TEMPERATURE_FACTOR = 1.0 / 16.0


#--------------------------------------------------------------------------------
# misc


def get_version():
    return _BD_VERSION_NBR

#--------------------------------------------------------------------------------
# helper functions for unpacking binary data


def byte_to_char(crrt_byte):
    return(chr(crrt_byte))


def one_byte_to_int(crrt_byte):
    return(struct.unpack('B', bytes(crrt_byte))[0])


def one_byte_to_signed_int(crrt_byte):
    return(struct.unpack('b', bytes(crrt_byte))[0])


def two_bytes_to_int(crrt_two_bytes):
    return(struct.unpack('h', bytes(crrt_two_bytes)))[0]


def four_bytes_to_long(crrt_four_bytes):
    res = struct.unpack('<l', bytes(crrt_four_bytes))
    return res[0]


def four_bytes_to_int(crrt_four_bytes):
    res = struct.unpack('<i', bytes(crrt_four_bytes))
    return res[0]


def four_bytes_to_unsignedint(crrt_four_bytes):
    res = struct.unpack('<I', bytes(crrt_four_bytes))
    return res[0]


def four_bytes_to_float(crrt_four_bytes):
    res = struct.unpack('<f', bytes(crrt_four_bytes))
    return res[0]

#--------------------------------------------------------------------------------
# custom data classes to store data packets

@dataclass
class Spectral_Moments:
    m0: float
    m2: float
    m4: float


@dataclass
class GNSS_Packet:
    datetime_fix: datetime.datetime
    latitude: float
    longitude: float
    is_valid: bool


@dataclass
class GNSS_Metadata:
    nbr_gnss_fixes: int


@dataclass
class Waves_Packet:
    # the decoded part: this is just decoding / textbook operations
    datetime_fix: datetime.datetime
    spectrum_number: int
    Hs: float
    Tz: float
    Tc: float
    _array_max_value: float
    _array_uint16: float
    list_frequencies: list
    list_acceleration_energies: list
    frequency_resolution: float
    list_elevation_energies: list
    wave_spectral_moments: Spectral_Moments
    is_valid: bool
    # the processed part: this relies on some more advanced operations
    processed_list_frequencies: list
    processed_list_elevation_energies: list
    processed_wave_spectral_moments: Spectral_Moments
    processed_Hs: float
    processed_Tz: float
    processed_Tc: float
    low_frequency_index_cutoff: int
    # processed_quality_index: str  # todo: add a quality explanation string


@dataclass
class Waves_Metadata:
    None


@dataclass
class Thermistors_Reading:
    mean_temperature: float
    range_temperature: float
    probe_id: int


@dataclass
class Thermistors_Packet:
    datetime_packet: datetime.datetime
    thermistors_readings: list


@dataclass
class ThermistorsMLX_Packet:
    datetime_packet: datetime.datetime
    thermistors_readings: list
    ir_target_temp: float  # the ir-measured temperature
    ir_sensor_temp: float  # the "ambient" temperature, of the sensor itself
    ir_target_temp_range: float


@dataclass
class Thermistors_Metadata:
    nbr_thermistors_measurements: int


# --------------------------------------------------------------------------------
# data quality and processing utils


def find_low_frequency_cutoff(list_frequencies, list_elevation_energies):
    """Find the low frequency cutoff to avoid double integration noise contamination
    for IMU measurements of waves, similar to what is discussed in Fig. 7 of
    https://www.mdpi.com/2076-3263/12/3/110 .

    Inputs:
        list_frequencies: the list of frequencies at which spectrum is provided
        list_elevation_energies: the wave spectrum elevation energies at the corresponding
            frequencies
    Output:
        index_low_frequency_cutoff: the index to use to cut off low frequency double
            integration noise contamination; i.e., use only indexes > to the output
            to consider the valid part of the spectrum.
    """

    assert isinstance(list_frequencies, list)
    assert isinstance(list_elevation_energies, list)
    assert len(list_frequencies) == len(list_elevation_energies)

    # findpeaks:
    # we look for minima (peaks looks for maxima, so -)
    # we want peaks that are local peaks in their neighborhood and not just "super local minima": require distance = 3
    # we want only peaks that are clear enough: normalize the spectrum so that the maximum is always 1.0, and want a prominence of at least 0.05
    normalized_spectrum = -np.array(list_elevation_energies) / np.max(list_elevation_energies)
    peaks_output = signal.find_peaks(normalized_spectrum, distance=3, prominence=0.05)

    peaks = list(peaks_output[0])
    if len(peaks) == 0:
        peaks = [0]

    # isolate the peak we want
    # we are only interested in the first minimum, and it has to be low enough that it does correspond to a low freq. threshold
    first_peak = peaks[0]
    if list_frequencies[first_peak] > 0.10:  # the 0.1 value is somewhat arbitrary, but in practice we always have some energy at least there or before
        first_peak = 0  # we keep all indexes if there was no clear minimum on the left (ie no low energy noise)

    # we do not want to flag out valid parts of the spectrum when the spectrum is "really clean"
    # in cases where the spectrum is "really clean", can happen that the first minimum is a local minimum after the first valid peak
    # detect these cases and set the full spectrum as valid then
    if (list_elevation_energies[first_peak] > ((list_elevation_energies[0] + list_elevation_energies[1]) / 2.0)):
        first_peak = 0

    index_low_frequency_cutoff = first_peak

    return index_low_frequency_cutoff


# --------------------------------------------------------------------------------
# packets and messages decoding


def hex_to_bin_message(hex_string_message, print_info=False):
    if print_info:
        ic(hex_string_message)
    bin_msg = binascii.unhexlify(hex_string_message)
    return bin_msg


def message_kind(bin_msg):
    first_char = byte_to_char(bin_msg[0])
    valid_first_chars = ["G", "Y", "T", "U", "V", "W"]
    assert first_char in valid_first_chars, "unknown first_char message kind: got {}, valids are {}".format(first_char, valid_first_chars)
    return first_char


def decode_gnss_packet(bin_packet, print_decoded=False, print_debug_information=False):
    assert len(bin_packet) == _BD_GNSS_PACKET_LENGTH, "GNSS packets with start and end byte have 14 bytes, got {} bytes".format(len(bin_packet))

    char_first_byte = byte_to_char(bin_packet[0])

    assert char_first_byte == 'F', "GNSS packets must start with a 'F', got {}".format(char_first_byte)

    posix_timestamp_fix = four_bytes_to_long(bin_packet[1:5])
    datetime_fix = datetime.datetime.utcfromtimestamp(posix_timestamp_fix)

    latitude_long = four_bytes_to_long(bin_packet[5:9])
    latitude = latitude_long / 1.0e7

    longitude_long = four_bytes_to_long(bin_packet[9:13])
    longitude = longitude_long / 1.0e7

    if print_debug_information:
        print("------ START PRINT GNSS DEBUG INFO -----")
        ic(posix_timestamp_fix)
        ic(datetime_fix)
        ic(latitude_long)
        ic(latitude)
        ic(longitude_long)
        ic(longitude)
        print("------ END PRINT GNSS DEBUG INFO -----")

    if print_decoded:
        print("-------------------- decoded GNSS packet ---------------------")
        print("fix at posix {}, i.e. {}".format(posix_timestamp_fix, datetime_fix))
        print("latitude {}, i.e. {}".format(latitude_long, latitude))
        print("longitude {}, i.e. {}".format(longitude_long, longitude))
        print("--------------------------------------------------------------")

    char_next_byte = byte_to_char(bin_packet[13])

    assert char_next_byte == 'E' or char_next_byte == 'F', "either end ('E') or fix ('F') expected at the end, got {}".format(char_next_byte)

    decoded_packet = GNSS_Packet(
        datetime_fix=datetime_fix,
        latitude=latitude,
        longitude=longitude,
        is_valid=True
    )

    return decoded_packet


def decode_gnss_message(bin_msg, print_decoded=True, print_debug_information=False):
    if print_decoded:
        print("----------------------- START DECODE GNSS MESSAGE -----------------------")

    assert message_kind(bin_msg) == 'G'
    expected_message_length = int(1 + (len(bin_msg) - 2 - _BD_GNSS_PACKET_LENGTH) / (_BD_GNSS_PACKET_LENGTH - 1))

    if print_decoded:
        print("expected number of packets based on message length: {}".format(expected_message_length))

    nbr_gnss_fixes = one_byte_to_int(bin_msg[1: 2])
    message_metadata = GNSS_Metadata(nbr_gnss_fixes=nbr_gnss_fixes)

    if print_decoded:
        print("number of fixes since boot at message creation: {}".format(nbr_gnss_fixes))

    crrt_packet_start = 2
    list_decoded_packets = []

    while True:
        crrt_byte_start = byte_to_char(bin_msg[crrt_packet_start])
        assert crrt_byte_start == "F"

        crrt_decoded_packet = decode_gnss_packet(bin_msg[crrt_packet_start: crrt_packet_start+_BD_GNSS_PACKET_LENGTH], print_decoded=print_decoded, print_debug_information=print_debug_information)
        list_decoded_packets.append(crrt_decoded_packet)

        trailing_char = byte_to_char(bin_msg[crrt_packet_start + _BD_GNSS_PACKET_LENGTH - 1])
        assert trailing_char in ["E", "F"]
        if trailing_char == "E":
            break
        else:
            crrt_packet_start += _BD_GNSS_PACKET_LENGTH - 1

    assert expected_message_length == len(list_decoded_packets)

    if print_decoded:
        print("----------------------- DONE DECODE GNSS MESSAGE -----------------------")

    return message_metadata, list_decoded_packets


def decode_ywave_packet(bin_packet, print_decoded=False, print_debug_information=False):
    assert len(bin_packet) == _BD_YWAVE_PACKET_LENGTH

    char_first_byte = byte_to_char(bin_packet[0])
    assert char_first_byte == "Y"

    char_last_byte = byte_to_char(bin_packet[-1])
    assert char_last_byte == "E"

    crrt_start_data_field = 1

    posix_timestamp = four_bytes_to_long(bin_packet[crrt_start_data_field: crrt_start_data_field+4])
    crrt_start_data_field += 4
    datetime_packet = datetime.datetime.utcfromtimestamp(posix_timestamp)

    spectrum_number = four_bytes_to_int(bin_packet[crrt_start_data_field: crrt_start_data_field+4])
    crrt_start_data_field += 4

    Hs = four_bytes_to_float(bin_packet[crrt_start_data_field: crrt_start_data_field+4])
    crrt_start_data_field += 4

    Tz = 1.0 / four_bytes_to_float(bin_packet[crrt_start_data_field: crrt_start_data_field+4])
    crrt_start_data_field += 4

    Tc = 1.0 / four_bytes_to_float(bin_packet[crrt_start_data_field: crrt_start_data_field+4])
    crrt_start_data_field += 4

    _array_max_value = four_bytes_to_float(bin_packet[crrt_start_data_field: crrt_start_data_field+4])
    crrt_start_data_field += 4

    nbr_bytes_uint16_array = _BD_YWAVE_NBR_BINS * 2
    _array_uint16 = struct.unpack('<' + _BD_YWAVE_NBR_BINS * "H", bin_packet[crrt_start_data_field: crrt_start_data_field+nbr_bytes_uint16_array])
    crrt_start_data_field += nbr_bytes_uint16_array

    assert crrt_start_data_field+1+2 == _BD_YWAVE_PACKET_LENGTH  # the +2 is due to struct alignment issues

    if Hs > 1e-5:
        is_valid = True
    else:
        is_valid = False
        print("WARNING: is_valid is False; that probably indicates an IMU misfunction")

    if print_debug_information:
        print("----- YWAVE START DEBUG INFORMATION -----")
        ic(posix_timestamp)
        ic(spectrum_number)
        ic(Hs)
        ic(Tz)
        ic(Tc)
        ic(_array_max_value)
        ic(_array_uint16)
        ic(is_valid)
        print("----- YWAVE END DEBUG INFORMATION -----")

    list_frequencies = []
    list_acceleration_energies = []

    for ind, crrt_uint16 in enumerate(_array_uint16):
        list_frequencies.append((_BD_YWAVE_PACKET_MIN_BIN + ind) * _BD_YWAVE_PACKET_FRQ_RES)
        list_acceleration_energies.append(crrt_uint16 * _array_max_value / _BD_YWAVE_PACKET_SCALER)

    list_omega = [2.0 * math.pi * crrt_freq for crrt_freq in list_frequencies]
    list_omega_4 = [math.pow(crrt_omega, 4) for crrt_omega in list_omega]
    list_elevation_energies = [crrt_acceleration_energy / crrt_omega_4 for (crrt_acceleration_energy, crrt_omega_4) in zip(list_acceleration_energies, list_omega_4)]

    def compute_spectral_moment(list_frequencies, list_elevation_energies, order):
        list_to_integrate = [
            math.pow(crrt_freq, order) * crrt_energy for (crrt_freq, crrt_energy) in zip(list_frequencies, list_elevation_energies)
        ]

        moment = np.trapz(list_to_integrate, list_frequencies)

        return moment

    m0 = compute_spectral_moment(list_frequencies, list_elevation_energies, 0)
    m2 = compute_spectral_moment(list_frequencies, list_elevation_energies, 2)
    m4 = compute_spectral_moment(list_frequencies, list_elevation_energies, 4)

    # ic(list_frequencies)
    # ic(list_elevation_energies)
    # ic(m0)

    spectral_moments = Spectral_Moments(
        m0,
        m2,
        m4
    )

    # add the post processed part

    low_frequency_index_cutoff = find_low_frequency_cutoff(list_frequencies, list_elevation_energies)

    processed_list_frequencies = list_frequencies[low_frequency_index_cutoff:]
    processed_list_elevation_energies = list_elevation_energies[low_frequency_index_cutoff:]

    processed_m0 = compute_spectral_moment(processed_list_frequencies, processed_list_elevation_energies, 0)
    processed_m2 = compute_spectral_moment(processed_list_frequencies, processed_list_elevation_energies, 2)
    processed_m4 = compute_spectral_moment(processed_list_frequencies, processed_list_elevation_energies, 4)

    # ic(processed_list_frequencies)
    # ic(processed_list_elevation_energies)
    # ic(processed_m0)

    processed_wave_spectral_moments = Spectral_Moments(
        processed_m0,
        processed_m2,
        processed_m4,
    )

    processed_Hs = 4 * math.sqrt(processed_m0)
    processed_Tz = 1.0 / math.sqrt(processed_m2 / processed_m0)
    processed_Tc = 1.0 / math.sqrt(processed_m4 / processed_m2)

    processed_list_frequencies = list_frequencies
    processed_list_elevation_energies = low_frequency_index_cutoff * [math.nan] + processed_list_elevation_energies

    decoded_packet = Waves_Packet(
        datetime_packet,
        spectrum_number,
        Hs,
        Tz,
        Tc,
        _array_max_value,
        _array_uint16,
        list_frequencies,
        list_acceleration_energies,
        _BD_YWAVE_PACKET_FRQ_RES,
        list_elevation_energies,
        spectral_moments,
        is_valid,
        processed_list_frequencies,
        processed_list_elevation_energies,
        processed_wave_spectral_moments,
        processed_Hs,
        processed_Tz,
        processed_Tc,
        low_frequency_index_cutoff,
    )

    if print_decoded:
        print("----- YWAVE START PRINT DECODED -----")
        ic(datetime_packet)
        ic(spectrum_number)
        ic(is_valid)
        ic(Hs)
        ic(Tz)
        ic(Tc)
        print("acceleration energy spectrum:")
        for freq, energy in zip(list_frequencies, list_acceleration_energies):
            print("  acceleration spectrum energy at {:.8f} Hz : {:16.8f}".format(freq, energy))
        print("----- YWAVE END PRINT DECODED -----")

        print("----- YWAVE START PRINT DERIVED -----")
        ic(m0)
        ic(m2)
        ic(m4)
        print("wave stats from moments")
        ic(4 * math.sqrt(m0))
        ic(math.sqrt(m2 / m0))
        ic(math.sqrt(m4 / m2))
        print("wave stats from processed moments")
        ic(processed_Hs)
        ic(processed_Tz)
        ic(processed_Tc)
        print("elevation energy spectrum:")
        for freq, energy in zip(list_frequencies, list_elevation_energies):
            print("  elevation spectrum energy at {:.8f} Hz : {:16.8f}".format(freq, energy))
        print("processed elevation energy spectrum:")
        for freq, energy in zip(processed_list_frequencies, processed_list_elevation_energies):
            print("  processed elevation spectrum energy at {:.8f} Hz : {:16.8f}".format(freq, energy))
        if not is_valid:
            print("*** CAUTIOUS: THIS IS INVALID PACKET!!! ***")
        print("----- YWAVE END PRINT DERIVED -----")

    return decoded_packet


def decode_ywave_message(bin_msg, print_decoded=True, print_debug_information=False):
    if print_decoded:
        print("----------------------- START DECODE YWAVES MESSAGE -----------------------")

    assert message_kind(bin_msg) == "Y"
    assert byte_to_char(bin_msg[_BD_YWAVE_PACKET_LENGTH-1]) == "E"

    message_metadata = Waves_Metadata()

    list_decoded_packets = []
    crrt_packet = decode_ywave_packet(bin_msg, print_decoded=print_decoded, print_debug_information=print_debug_information)
    list_decoded_packets.append(crrt_packet)

    if print_decoded:
        print("----------------------- DONE DECODE GNSS MESSAGE -----------------------")

    return message_metadata, list_decoded_packets


def decode_thermistor_reading(crrt_thermistor_bin, print_debug_information=False):

    # quite a bit of tweaking...
    # this is the inverse operation of the binary encoding done in the thermistor manager C++ code

    # TODO: actually, this can be made with clear syntax a la n & 0xffffffff too, fixme

    id_6_bits = one_byte_to_int(crrt_thermistor_bin[0: 1]) // 4

    reading_2_higher_bits = one_byte_to_int(crrt_thermistor_bin[0: 1]) % 4
    reading_2_higher_bits_lower = reading_2_higher_bits % 2
    reading_2_higher_bits_higher = (reading_2_higher_bits - reading_2_higher_bits_lower) // 2
    reading_8_middle_bits = one_byte_to_int(crrt_thermistor_bin[1: 2])
    reading_2_lower_bits = one_byte_to_int(crrt_thermistor_bin[2: 3]) // 64

    reading_reconstructed_bin = reading_2_lower_bits + (2**2) * reading_8_middle_bits + (2**10) * reading_2_higher_bits_lower
    if reading_2_higher_bits_higher:
        reading_reconstructed_bin = reading_reconstructed_bin - 2**11 - 1

    range_6_bits_bin = one_byte_to_int(crrt_thermistor_bin[2:3]) % 64

    if print_debug_information:
        ic(id_6_bits)

        ic(reading_2_higher_bits)
        ic(reading_2_higher_bits_lower)
        ic(reading_2_higher_bits_higher)
        ic(reading_8_middle_bits)
        ic(reading_2_lower_bits)
        ic(reading_reconstructed_bin)

        ic(range_6_bits_bin)

    reading_reconstructed = reading_reconstructed_bin * _BD_THERM_12BITS_TO_FLOAT_TEMPERATURE_FACTOR
    range_temperature = range_6_bits_bin * _BD_THERM_12BITS_TO_FLOAT_TEMPERATURE_FACTOR

    crrt_thermistor_reading = Thermistors_Reading(
        mean_temperature=reading_reconstructed,
        range_temperature=range_temperature,
        probe_id=id_6_bits
    )

    return crrt_thermistor_reading


def print_thermistor_reading(crrt_thermistor_reading):
    ic(crrt_thermistor_reading.probe_id)
    ic(crrt_thermistor_reading.mean_temperature)
    ic(crrt_thermistor_reading.range_temperature)


def print_thermistor_packet(crrt_thermistor_packet):
    ic(crrt_thermistor_packet.datetime_packet)

    for ind, crrt_thermistor_reading in enumerate(crrt_thermistor_packet.thermistors_readings):
        print("--- thermistor reading nbr {}".format(ind))
        print_thermistor_reading(crrt_thermistor_reading)


def print_thermistormlx_packet(crrt_thermistor_packet):
    ic(crrt_thermistor_packet.datetime_packet)
    ic(crrt_thermistor_packet.ir_target_temp)
    ic(crrt_thermistor_packet.ir_sensor_temp)
    ic(crrt_thermistor_packet.ir_target_temp_range)

    for ind, crrt_thermistor_reading in enumerate(crrt_thermistor_packet.thermistors_readings):
        print("--- thermistor reading nbr {}".format(ind))
        print_thermistor_reading(crrt_thermistor_reading)


def decode_thermistors_packet(bin_packet, print_decoded=False, print_debug_information=False):
    if print_debug_information:
        print("----- START DECODE THERM PACKET -----")

    assert len(bin_packet) == _BD_THERM_PACKET_LENGTH

    char_first_byte = byte_to_char(bin_packet[0])
    assert char_first_byte == "P"

    crrt_start_field = 1

    posix_timestamp = four_bytes_to_long(bin_packet[crrt_start_field: crrt_start_field+4])
    datetime_packet = datetime.datetime.utcfromtimestamp(posix_timestamp)
    crrt_start_field += 4

    list_thermistors_readings = []

    for crrt_thermistor in range(_BD_THERM_MSG_NBR_THERMISTORS):
        crrt_thermistor_bin = bin_packet[crrt_start_field: crrt_start_field+3]
        crrt_start_field += 3
        crrt_thermistor_reading = decode_thermistor_reading(crrt_thermistor_bin, print_debug_information=print_debug_information)
        assert isinstance(crrt_thermistor_reading, Thermistors_Reading)
        list_thermistors_readings.append(crrt_thermistor_reading)

    assert crrt_start_field == _BD_THERM_PACKET_LENGTH

    crrt_thermistor_packet = Thermistors_Packet(
        datetime_packet=datetime_packet,
        thermistors_readings=list_thermistors_readings,
    )

    if print_decoded:
        print_thermistor_packet(crrt_thermistor_packet)

    if print_debug_information:
        print("----- DONE DECODE THERM PACKET -----")

    return crrt_thermistor_packet


def decode_thermistors16_packet(bin_packet, print_decoded=False, print_debug_information=False):
    if print_debug_information:
        print("----- START DECODE THERM PACKET -----")

    assert len(bin_packet) == _BD_THERM16_PACKET_LENGTH

    char_first_byte = byte_to_char(bin_packet[0])
    assert char_first_byte == "P"

    crrt_start_field = 1

    posix_timestamp = four_bytes_to_long(bin_packet[crrt_start_field: crrt_start_field+4])
    datetime_packet = datetime.datetime.utcfromtimestamp(posix_timestamp)
    crrt_start_field += 4

    list_thermistors_readings = []

    for crrt_thermistor in range(_BD_THERM16_MSG_NBR_THERMISTORS):
        crrt_thermistor_bin = bin_packet[crrt_start_field: crrt_start_field+3]
        crrt_start_field += 3
        crrt_thermistor_reading = decode_thermistor_reading(crrt_thermistor_bin, print_debug_information=print_debug_information)
        assert isinstance(crrt_thermistor_reading, Thermistors_Reading)
        list_thermistors_readings.append(crrt_thermistor_reading)

    assert crrt_start_field == _BD_THERM16_PACKET_LENGTH

    crrt_thermistor_packet = Thermistors_Packet(
        datetime_packet=datetime_packet,
        thermistors_readings=list_thermistors_readings,
    )

    if print_decoded:
        print_thermistor_packet(crrt_thermistor_packet)

    if print_debug_information:
        print("----- DONE DECODE THERM PACKET -----")

    return crrt_thermistor_packet


def decode_thermistorsmlx_packet(bin_packet, print_decoded=False, print_debug_information=False):
    if print_debug_information:
        print("----- START DECODE THERMMLX PACKET -----")

    assert len(bin_packet) == _BD_THERMMLX_PACKET_LENGTH

    char_first_byte = byte_to_char(bin_packet[0])
    assert char_first_byte == "P"

    crrt_start_field = 1

    posix_timestamp = four_bytes_to_long(bin_packet[crrt_start_field: crrt_start_field+4])
    datetime_packet = datetime.datetime.utcfromtimestamp(posix_timestamp)
    crrt_start_field += 4

    ir_target_temp = two_bytes_to_int(bin_packet[crrt_start_field: crrt_start_field+2])
    ir_target_temp = ir_target_temp / 100.0
    crrt_start_field += 2

    ir_sensor_temp = two_bytes_to_int(bin_packet[crrt_start_field: crrt_start_field+2])
    ir_sensor_temp = ir_sensor_temp / 100.0
    crrt_start_field += 2

    ir_target_temp_range = one_byte_to_int(bin_packet[crrt_start_field: crrt_start_field+1])
    ir_target_temp_range = ir_target_temp_range / 20.0
    crrt_start_field += 1

    list_thermistors_readings = []

    for crrt_thermistor in range(_BD_THERMMLX_MSG_NBR_THERMISTORS):
        crrt_thermistor_bin = bin_packet[crrt_start_field: crrt_start_field+3]
        crrt_start_field += 3
        crrt_thermistor_reading = decode_thermistor_reading(crrt_thermistor_bin, print_debug_information=print_debug_information)
        assert isinstance(crrt_thermistor_reading, Thermistors_Reading)
        list_thermistors_readings.append(crrt_thermistor_reading)

    # some padding at the end... this may depend on the number of thermistors...
    crrt_start_field += 2
    
    assert crrt_start_field == _BD_THERMMLX_PACKET_LENGTH

    crrt_thermistormlx_packet = ThermistorsMLX_Packet(
        datetime_packet=datetime_packet,
        thermistors_readings=list_thermistors_readings,
        ir_target_temp=ir_target_temp,
        ir_sensor_temp=ir_sensor_temp,
        ir_target_temp_range=ir_target_temp_range,
    )

    if print_decoded:
        print_thermistormlx_packet(crrt_thermistormlx_packet)

    if print_debug_information:
        print("----- DONE DECODE THERMMLX PACKET -----")

    return crrt_thermistormlx_packet


def decode_27thermistors1mlx_packet(bin_packet, print_decoded=False, print_debug_information=False):
    if print_debug_information:
        print("----- START DECODE THERMMLX PACKET -----")

    assert len(bin_packet) == _BD_27THERM1MLX_PACKET_LENGTH

    char_first_byte = byte_to_char(bin_packet[0])
    assert char_first_byte == "P"

    crrt_start_field = 1

    posix_timestamp = four_bytes_to_long(bin_packet[crrt_start_field: crrt_start_field+4])
    datetime_packet = datetime.datetime.utcfromtimestamp(posix_timestamp)
    crrt_start_field += 4

    ir_target_temp = two_bytes_to_int(bin_packet[crrt_start_field: crrt_start_field+2])
    ir_target_temp = ir_target_temp / 100.0
    crrt_start_field += 2

    ir_sensor_temp = two_bytes_to_int(bin_packet[crrt_start_field: crrt_start_field+2])
    ir_sensor_temp = ir_sensor_temp / 100.0
    crrt_start_field += 2

    ir_target_temp_range = one_byte_to_int(bin_packet[crrt_start_field: crrt_start_field+1])
    ir_target_temp_range = ir_target_temp_range / 20.0
    crrt_start_field += 1

    list_thermistors_readings = []

    for crrt_thermistor in range(_BD_27THERM1MLX_MSG_NBR_THERMISTORS):
        crrt_thermistor_bin = bin_packet[crrt_start_field: crrt_start_field+3]
        crrt_start_field += 3
        crrt_thermistor_reading = decode_thermistor_reading(crrt_thermistor_bin, print_debug_information=print_debug_information)
        assert isinstance(crrt_thermistor_reading, Thermistors_Reading)
        list_thermistors_readings.append(crrt_thermistor_reading)

    # some padding at the end... this may depend on the number of thermistors...
    crrt_start_field += 2
    
    assert crrt_start_field == _BD_27THERM1MLX_PACKET_LENGTH

    crrt_thermistormlx_packet = ThermistorsMLX_Packet(
        datetime_packet=datetime_packet,
        thermistors_readings=list_thermistors_readings,
        ir_target_temp=ir_target_temp,
        ir_sensor_temp=ir_sensor_temp,
        ir_target_temp_range=ir_target_temp_range,
    )

    if print_decoded:
        print_thermistormlx_packet(crrt_thermistormlx_packet)

    if print_debug_information:
        print("----- DONE DECODE THERMMLX PACKET -----")

    return crrt_thermistormlx_packet


def decode_thermistors_message(bin_msg, print_decoded=False, print_debug_information=False):
    if print_decoded:
        print("----------------------- START DECODE THERMISTORS MESSAGE -----------------------")

    assert message_kind(bin_msg) == "T"
    assert byte_to_char(bin_msg[-1]) == "E"

    if (print_debug_information):
        print("received message of length: {}".format(len(bin_msg)))

    expected_message_length = int( (len(bin_msg) - _BD_THERM_MSG_FIXED_LENGTH) / _BD_THERM_PACKET_LENGTH )
    assert expected_message_length * _BD_THERM_PACKET_LENGTH + _BD_THERM_MSG_FIXED_LENGTH == len(bin_msg)

    nbr_thermistors_measurements = one_byte_to_int(bin_msg[1: 2])
    message_metadata = Thermistors_Metadata(nbr_thermistors_measurements=nbr_thermistors_measurements)

    if print_decoded:
        print(message_metadata)

    crrt_packet_start = 2
    list_decoded_packets = []

    while True:
        if print_decoded:
            print("----- START PACKET -----")

        crrt_byte_start = byte_to_char(bin_msg[crrt_packet_start])
        assert crrt_byte_start == "P"

        # decode
        crrt_decoded_packet = decode_thermistors_packet(bin_msg[crrt_packet_start: crrt_packet_start+_BD_THERM_PACKET_LENGTH], print_decoded=print_decoded, print_debug_information=print_debug_information)
        list_decoded_packets.append(crrt_decoded_packet)

        trailing_char = byte_to_char(bin_msg[crrt_packet_start + _BD_THERM_PACKET_LENGTH])
        assert trailing_char in ["P", "E"]
        if trailing_char == "E":
            break
        else:
            crrt_packet_start += _BD_THERM_PACKET_LENGTH

        if print_decoded:
            print("----- END PACKET -----")

    assert expected_message_length == len(list_decoded_packets)

    if print_decoded:
        print("----------------------- DONE DECODE THERMISTORS MESSAGE -----------------------")

    return message_metadata, list_decoded_packets

def decode_thermistorsmlx_message(bin_msg, print_decoded=False, print_debug_information=False):
    if print_decoded:
        print("----------------------- START DECODE THERMISTORS MESSAGE -----------------------")

    assert message_kind(bin_msg) == "U"
    assert byte_to_char(bin_msg[-1]) == "E"

    expected_message_length = int( (len(bin_msg) - _BD_THERMMLX_MSG_FIXED_LENGTH) / _BD_THERMMLX_PACKET_LENGTH )

    if (print_debug_information):
        print("received message of length: {}".format(len(bin_msg)))
        print("number of packets inside (expected_message_length): {}".format(expected_message_length))
        print("total expected message length: {}".format(expected_message_length * _BD_THERMMLX_PACKET_LENGTH + _BD_THERMMLX_MSG_FIXED_LENGTH))

    assert expected_message_length * _BD_THERMMLX_PACKET_LENGTH + _BD_THERMMLX_MSG_FIXED_LENGTH == len(bin_msg)

    nbr_thermistors_measurements = one_byte_to_int(bin_msg[1: 2])
    message_metadata = Thermistors_Metadata(nbr_thermistors_measurements=nbr_thermistors_measurements)

    if print_decoded:
        print(message_metadata)

    crrt_packet_start = 2
    list_decoded_packets = []

    while True:
        if print_decoded:
            print("----- START PACKET -----")

        crrt_byte_start = byte_to_char(bin_msg[crrt_packet_start])
        assert crrt_byte_start == "P"

        # decode
        crrt_decoded_packet = decode_thermistorsmlx_packet(bin_msg[crrt_packet_start: crrt_packet_start+_BD_THERMMLX_PACKET_LENGTH], print_decoded=print_decoded, print_debug_information=print_debug_information)
        list_decoded_packets.append(crrt_decoded_packet)

        trailing_char = byte_to_char(bin_msg[crrt_packet_start + _BD_THERMMLX_PACKET_LENGTH])
        assert trailing_char in ["P", "E"]
        if trailing_char == "E":
            break
        else:
            crrt_packet_start += _BD_THERMMLX_PACKET_LENGTH

        if print_decoded:
            print("----- END PACKET -----")

    assert expected_message_length == len(list_decoded_packets)

    if print_decoded:
        print("----------------------- DONE DECODE THERMISTORS MESSAGE -----------------------")

    return message_metadata, list_decoded_packets


def decode_thermistors16_message(bin_msg, print_decoded=False, print_debug_information=False):
    if print_decoded:
        print("----------------------- START DECODE THERMISTORS MESSAGE -----------------------")

    assert message_kind(bin_msg) == "V"
    assert byte_to_char(bin_msg[-1]) == "E"

    if (print_debug_information):
        print("received message of length: {}".format(len(bin_msg)))

    expected_message_length = int( (len(bin_msg) - _BD_THERM16_MSG_FIXED_LENGTH) / _BD_THERM16_PACKET_LENGTH )
    assert expected_message_length * _BD_THERM16_PACKET_LENGTH + _BD_THERM16_MSG_FIXED_LENGTH == len(bin_msg)

    nbr_thermistors_measurements = one_byte_to_int(bin_msg[1: 2])
    message_metadata = Thermistors_Metadata(nbr_thermistors_measurements=nbr_thermistors_measurements)

    if print_decoded:
        print(message_metadata)

    crrt_packet_start = 2
    list_decoded_packets = []

    while True:
        if print_decoded:
            print("----- START PACKET -----")

        crrt_byte_start = byte_to_char(bin_msg[crrt_packet_start])
        assert crrt_byte_start == "P"

        # decode
        crrt_decoded_packet = decode_thermistors16_packet(bin_msg[crrt_packet_start: crrt_packet_start+_BD_THERM16_PACKET_LENGTH], print_decoded=print_decoded, print_debug_information=print_debug_information)
        list_decoded_packets.append(crrt_decoded_packet)

        trailing_char = byte_to_char(bin_msg[crrt_packet_start + _BD_THERM16_PACKET_LENGTH])
        assert trailing_char in ["P", "E"]
        if trailing_char == "E":
            break
        else:
            crrt_packet_start += _BD_THERM16_PACKET_LENGTH

        if print_decoded:
            print("----- END PACKET -----")

    assert expected_message_length == len(list_decoded_packets)

    if print_decoded:
        print("----------------------- DONE DECODE THERMISTORS MESSAGE -----------------------")

    return message_metadata, list_decoded_packets


def decode_27thermistors1mlx_message(bin_msg, print_decoded=False, print_debug_information=False):
    if print_decoded:
        print("----------------------- START DECODE THERMISTORS MESSAGE -----------------------")

    assert message_kind(bin_msg) == "W"
    assert byte_to_char(bin_msg[-1]) == "E"

    expected_message_length = int( (len(bin_msg) - _BD_27THERM1MLX_MSG_FIXED_LENGTH) / _BD_27THERM1MLX_PACKET_LENGTH )

    if (print_debug_information):
        print("received message of length: {}".format(len(bin_msg)))
        print("number of packets inside (expected_message_length): {}".format(expected_message_length))
        print("total expected message length: {}".format(expected_message_length * _BD_27THERM1MLX_PACKET_LENGTH + _BD_27THERM1MLX_MSG_FIXED_LENGTH))

    assert expected_message_length * _BD_27THERM1MLX_PACKET_LENGTH + _BD_27THERM1MLX_MSG_FIXED_LENGTH == len(bin_msg)

    nbr_thermistors_measurements = one_byte_to_int(bin_msg[1: 2])
    message_metadata = Thermistors_Metadata(nbr_thermistors_measurements=nbr_thermistors_measurements)

    if print_decoded:
        print(message_metadata)

    crrt_packet_start = 2
    list_decoded_packets = []

    while True:
        if print_decoded:
            print("----- START PACKET -----")

        crrt_byte_start = byte_to_char(bin_msg[crrt_packet_start])
        assert crrt_byte_start == "P"

        # decode
        crrt_decoded_packet = decode_27thermistors1mlx_packet(bin_msg[crrt_packet_start: crrt_packet_start+_BD_27THERM1MLX_PACKET_LENGTH], print_decoded=print_decoded, print_debug_information=print_debug_information)
        list_decoded_packets.append(crrt_decoded_packet)

        trailing_char = byte_to_char(bin_msg[crrt_packet_start + _BD_27THERM1MLX_PACKET_LENGTH])
        assert trailing_char in ["P", "E"]
        if trailing_char == "E":
            break
        else:
            crrt_packet_start += _BD_27THERM1MLX_PACKET_LENGTH

        if print_decoded:
            print("----- END PACKET -----")

    assert expected_message_length == len(list_decoded_packets)

    if print_decoded:
        print("----------------------- DONE DECODE THERMISTORS MESSAGE -----------------------")

    return message_metadata, list_decoded_packets


def decode_message(hex_string_message, print_decoded=True, print_debug_information=False):
    bin_msg = hex_to_bin_message(hex_string_message)

    kind = message_kind(bin_msg)

    if kind == "G":
        message_metadata, list_decoded_packets = decode_gnss_message(bin_msg, print_decoded=print_decoded, print_debug_information=print_debug_information)
    elif kind == "Y":
        message_metadata, list_decoded_packets = decode_ywave_message(bin_msg, print_decoded=print_decoded, print_debug_information=print_debug_information)
    elif kind == "T":
        message_metadata, list_decoded_packets = decode_thermistors_message(bin_msg, print_decoded=print_decoded, print_debug_information=print_debug_information)
    elif kind == "U":
        message_metadata, list_decoded_packets = decode_thermistorsmlx_message(bin_msg, print_decoded=print_decoded, print_debug_information=print_debug_information)
    elif kind == "V":
        message_metadata, list_decoded_packets = decode_thermistors16_message(bin_msg, print_decoded=print_decoded, print_debug_information=print_debug_information)
    elif kind == "W":
        message_metadata, list_decoded_packets = decode_27thermistors1mlx_message(bin_msg, print_decoded=print_decoded, print_debug_information=print_debug_information)
        
    else:
        raise RuntimeError("Unknown message kind: {}".format(kind))

    return (kind, message_metadata, list_decoded_packets)


def auto_test():
    # TODO: assert all results for correct values
    print("------------------------------ START AUTO TEST ------------------------------")

    hex_in = "4704469cb62a6007160f2ef5581e1246c2b62a60d3150f2e1b5b1e1246e8b62a6009110f2e2e8a1e1245"
    ic(hex_in)
    decode_gnss_message(hex_to_bin_message(hex_in), print_decoded=True, print_debug_information=True)
    decode_message(hex_in)

    hex_in = "59a016b363010000003e329b3c24dcce3df785203e122d1c398803dd0443057904e005f40d7c541748af0ad504a3036204aa0679073008fc07870e211b351cdf27b9933ccb40da03ca209a7586ce944625f70a6f0ae80aba0c9407cd0505088407c3089f0dce0f6f20954189cce8fd6737d9150b2bbd427640562d5153fe656656a83b9e2ea470000045"
    ic(hex_in)
    decode_ywave_message(hex_to_bin_message(hex_in), print_decoded=True, print_debug_information=True)
    decode_message(hex_in)

    hex_in = "54035086000000CC6FD7806E5A8074D92063C25048000000CC769B8061C1806F922063C15009000000CC71578061C18062C120640245"
    ic(hex_in)
    decode_thermistors_message(hex_to_bin_message(hex_in), print_decoded=True, print_debug_information=True)
    decode_message(hex_in)

    print("------------------------------ END AUTO TEST ------------------------------")


def plot_wave_packet(list_wave_packet_in):
    wave_packet_in = list_wave_packet_in[0]

    plt.figure()
    plt.plot(wave_packet_in.list_frequencies, wave_packet_in.list_elevation_energies)
    plt.xlabel("frq [Hz]")
    plt.ylabel("Seta(f) [m2/Hz]")
    title_str = "{}, Hs={:05.2f}m, Tz={:05.2f}s".format(wave_packet_in.datetime_fix, wave_packet_in.Hs, wave_packet_in.Tz)
    plt.title(title_str)
    plt.tight_layout()
    plt.show()


def plot_gnss_fixes(list_gnss_packets_in):
    list_lats = [crrt_packet.latitude for crrt_packet in list_gnss_packets_in]
    list_lons = [crrt_packet.longitude for crrt_packet in list_gnss_packets_in]


    fig, ax = plt.subplots()
    plt.plot(list_lons, list_lats)
    ax.annotate("Start", (list_lons[0], list_lats[0]))
    plt.scatter(list_lons[0], list_lats[0], s=80)
    plt.xlabel("lon [DD East]")
    plt.ylabel("lat [DD North]")
    title_str = "drift {} to {}".format(list_gnss_packets_in[0].datetime_fix, list_gnss_packets_in[-1].datetime_fix)
    plt.title(title_str)
    plt.tight_layout()
    plt.show()


def plot_decoded(message_kind, message_metadata, list_decoded_packets):
    if message_kind == "Y":
        plot_wave_packet(list_decoded_packets)
    elif message_kind == "G":
        plot_gnss_fixes(list_decoded_packets)
    else:
        raise RuntimeError("Unknown message_kind: {}".format(message_kind))

#--------------------------------------------------------------------------------
# simple CLI

@click.command(context_settings=dict(help_option_names=["-h", "--help"]))
@click.option('--example', '-e', is_flag=True, help="Example of installation and use")
@click.option('--module', '-m', is_flag=True, help="Show how to use as a module")
@click.option('--autotest', '-t', is_flag=True, help="Run a few autotests to check that the package works")
@click.option('--version', '-s', is_flag=True, help="Print the version number")
@click.option('--verbose', '-v', is_flag=True, help="Turn on verbosity")
@click.option('--verbose-debug', '-b', is_flag=True, help="Turn on verbosity to debug level")
@click.option('--decode-hex', '-d', default=None, help="Decode the provided hex message")
@click.option('--plot', '-p', is_flag=True, help="Plot the data contained in the message")
def cli(example, module, autotest, version, verbose, verbose_debug, plot, decode_hex):
    """
    Simple CLI for decoding tracker messages.
    This can be added as a command by copying into $HOME/bin and making executable.
    (you may need to $ source ~/.profile to activate)
    """

    if example:
        print("download messages directly from Rock7: https://rockblock.rock7.com/ > Messages > Export > Payload")
        print("for example decoding a GNSS message with 3 packets obtained there:")
        print("    $ ./decoder.py --verbose --decode-hex 4704469cb62a6007160f2ef5581e1246c2b62a60d3150f2e1b5b1e1246e8b62a6009110f2e2e8a1e1245")
        print("    ----------------------- START DECODE GNSS MESSAGE -----------------------")
        print("    expected number of packets based on message length: 3")
        print("    number of fixes since boot at message creation: 4")
        print("    -------------------- decoded GNSS packet ---------------------")
        print("    fix at posix 1613411996, i.e. 2021-02-15 17:59:56")
        print("    latitude 772740615, i.e. 77.2740615")
        print("    longitude 303978741, i.e. 30.3978741")
        print("    --------------------------------------------------------------")
        print("    -------------------- decoded GNSS packet ---------------------")
        print("    fix at posix 1613412034, i.e. 2021-02-15 18:00:34")
        print("    latitude 772740563, i.e. 77.2740563")
        print("    longitude 303979291, i.e. 30.3979291")
        print("    --------------------------------------------------------------")
        print("    -------------------- decoded GNSS packet ---------------------")
        print("    fix at posix 1613412072, i.e. 2021-02-15 18:01:12")
        print("    latitude 772739337, i.e. 77.2739337")
        print("    longitude 303991342, i.e. 30.3991342")
        print("    --------------------------------------------------------------")
        print("    ----------------------- DONE DECODE GNSS MESSAGE -----------------------")
        print("this can also be shortened into: $ ./decoder.py -vd 470146fa30dc602521ba23b4575d0645")
        print("i.e. the abbreviated form for verbose decode is '-vd'")

    if module:
        print("this can be used as a module after adding to PYTHONPATH")
        print("    $ python3")
        print("    >>> from decoder import *")
        print('    >>> kind, metadata, list_decoded = decode_message("4704469cb62a6007160f2ef5581e1246c2b62a60d3150f2e1b5b1e1246e8b62a6009110f2e2e8a1e1245")')
        print("    ----------------------- START DECODE GNSS MESSAGE -----------------------")
        print("    expected number of packets based on message length: 3")
        print("    number of fixes since boot at message creation: 4")
        print("    -------------------- decoded GNSS packet ---------------------")
        print("    fix at posix 1613411996, i.e. 2021-02-15 17:59:56")
        print("    latitude 772740615, i.e. 77.2740615")
        print("    longitude 303978741, i.e. 30.3978741")
        print("    --------------------------------------------------------------")
        print("    -------------------- decoded GNSS packet ---------------------")
        print("    fix at posix 1613412034, i.e. 2021-02-15 18:00:34")
        print("    latitude 772740563, i.e. 77.2740563")
        print("    longitude 303979291, i.e. 30.3979291")
        print("    --------------------------------------------------------------")
        print("    -------------------- decoded GNSS packet ---------------------")
        print("    fix at posix 1613412072, i.e. 2021-02-15 18:01:12")
        print("    latitude 772739337, i.e. 77.2739337")
        print("    longitude 303991342, i.e. 30.3991342")
        print("    --------------------------------------------------------------")
        print("    ----------------------- DONE DECODE GNSS MESSAGE -----------------------")
        print("    >>> kind")
        print("    'G'")
        print("    >>> metadata")
        print("    GNSS_Metadata(nbr_gnss_fixes=4)")
        print("    >>> for i in list_decoded:")
        print("    ...     print(i)")
        print("    ... ")
        print("    GNSS_Packet(datetime_fix=datetime.datetime(2021, 2, 15, 17, 59, 56), latitude=77.2740615, longitude=30.3978741)")
        print("    GNSS_Packet(datetime_fix=datetime.datetime(2021, 2, 15, 18, 0, 34), latitude=77.2740563, longitude=30.3979291)")
        print("    GNSS_Packet(datetime_fix=datetime.datetime(2021, 2, 15, 18, 1, 12), latitude=77.2739337, longitude=30.3991342)")

    if autotest:
        print("we will now run a few diagnostic tests to check that the module is working fine...")
        print("these tests should decode some test messages to check that all work well...")
        print()
        auto_test()

    if version:
        print("version {}".format(_BD_VERSION_NBR))

    if decode_hex:
        message_kind, message_metadata, list_decoded_packets = decode_message(decode_hex, print_decoded=verbose, print_debug_information=verbose_debug)

        if plot:
            plot_decoded(message_kind, message_metadata, list_decoded_packets)


if __name__ == "__main__":
    cli()

