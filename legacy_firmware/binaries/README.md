# Pre-compiled binary firmware

For explanations of how to upload, see: https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/development_environment/setup_arduino_v1-8_environment/Instructions.md section "Use of pre-compiled firmware binaries".

## Quick description of the firmwares

### plain_gps_tracker.bin

plain_gps_tracker.bin: only tracking with GPS. Taking a GPS fix every 30 minutes. Sending at least 4 GPS fixes per iridium message.

### plain_gps_tracker_15minsresolution_1hrlatency.bin

plain_gps_tracker_15minsresolution_1hrlatency.bin: only tracking with GPS. Taking a GPS fix every 15 minutes. Sending at least 4 GPS fixes per iridium message. Same as plain_gps_tracker.bin, except for the higher time resolution.

### standard_gps_waves_drifter.bin

standard_gps_waves_drifter.bin: perform tracking with GPS, and wave measurements. The instrument MUST include an Adafruit ISM330DHCX + LIS3MDL FeatherWing or similar, ie both the ISM330DHCX and the LISM3MDL, over a qwiick switch, if not it will not work. Perform GPS measurement every 30 minutes, and waves measuremens every 3 hours.

### standard_gps_waves_thermistors_drifter.bin

standard_gps_waves_thermistors_drifter.bin: perform tracking with GPS, and wave measurements, and measurement of the temperature string composed of several DS18B20 temperature sensors. The instrument MUST include an Adafruit ISM330DHCX + LIS3MDL FeatherWing or similar, ie both the ISM330DHCX and the LISM3MDL, over a qwiick switch, if not it will not work. Perform GPS measurement and temperature measurement every 30 minutes, and waves measuremens every 3 hours.

### steval_gps_waves_drifter.bin

steval_gps_waves_drifter.bin: perform tracking with GPS, and wave measurements. The instrument is compatible with the STEVAL breakout boards, ie the ISM330DHCX alone, without the extra magnetometer chip, over a qwiick switch. Perform GPS measurement every 30 minutes, and waves measuremens every 3 hours.

### two_ways_gps_waves_drifter.bin

two_ways_gps_waves_drifter.bin: perform tracking with GPS, and wave measurements. The instrument MUST include an Adafruit ISM330DHCX + LIS3MDL FeatherWing or similar, ie both the ISM330DHCX and the LISM3MDL, over a qwiick switch, if not it will not work. Perform GPS measurement every 30 minutes, and waves measuremens every 3 hours. In addition, includes the functionality for 2-ways communications to update the measurement rate of the GPS and wave statistics.
