# I recommend building and then executing interactively, see the attached readme

FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y curl git build-essential ffmpeg arduino-mk && apt-get clean

RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=/usr/local/bin sh

RUN arduino-cli config init

RUN arduino-cli config add board_manager.additional_urls https://raw.githubusercontent.com/sparkfun/Arduino_Apollo3/main/package_sparkfun_apollo3_index.json

RUN arduino-cli core update-index
RUN arduino-cli core install "Sparkfun:apollo3@1.2.1"

RUN arduino-cli lib update-index

RUN arduino-cli lib install "Embedded Template Library ETL"
RUN touch /root/Arduino/libraries/Embedded_Template_Library_ETL/src/etl.h
RUN arduino-cli lib install Time
RUN arduino-cli lib install "SparkFun Qwiic Power Switch Arduino Library"
RUN arduino-cli lib install "SparkFun u-blox GNSS Arduino Library@2.0.2"
RUN arduino-cli lib install IridiumSBDi2c
RUN arduino-cli lib install "Adafruit LSM6DS@4.3.1"
RUN arduino-cli lib install "Adafruit BusIO@1.7.2"
RUN arduino-cli lib install "Adafruit LIS3MDL"
RUN arduino-cli lib install "Adafruit AHRS@2.2.4"
RUN arduino-cli lib install "OneWire@2.3.6"

# Get Jean's fixed libs
WORKDIR /root/Arduino/libraries
RUN rm -rf Adafruit_BusIO
RUN git clone -b "fix/SPI_with_Artemis" --depth=1 https://github.com/jerabaul29/Adafruit_BusIO.git

RUN rm -rf Adafruit_LSM6DS
RUN git clone -b "feat/propagate_bool_flags" --depth=1 https://github.com/jerabaul29/Adafruit_LSM6DS.git

RUN rm -rf OneWire
RUN git clone -b "feat/Artemis_compatible" --depth=1 https://github.com/jerabaul29/OneWire.git

WORKDIR /work

RUN git clone --depth=1 https://github.com/jerabaul29/OpenMetBuoy-v2021a.git

# example of how to compile a specific firmware version; see the Readme
# you need to:
# - go to the dir with the version of the firmware you want
# WORKDIR /work/OpenMetBuoy-v2021a/legacy_firmware/firmware/steval_gps_waves_drifter
# - if relevant, edit the firmware, in particular its params
# - rename the tracker.ino into the name of the folder, since ArduinoIDE assumes the name of the main executable is the name of the containing folder
# RUN mv tracker.ino steval_gps_waves_drifter.ino
# - compile
# RUN arduino-cli compile -v -b SparkFun:apollo3:artemis .
# - get the .bin firmware from the container to your host, to allow using it further

