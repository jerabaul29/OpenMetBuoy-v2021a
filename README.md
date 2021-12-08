# OpenMetBuoy-v2021a: an easy to build, affordable, customizable, open source instrument for oceanographic measurements.

Our pledge: many fields within Geosciences are relying on in-situ data collection. Traditionally, collecting in-situ data is expensive, with commercial instruments easily reaching several thousands of USDs even if only quite simple functionalities are provided. However, the recent emergence of open source communities within micro controllers and low level firmware for these opens new possibilities for developing low-cost, high performance instruments based on widely available and affordable components. This repository is focusing on developing an oceanic buoy based on such open source code and low cost components. Our solution is around 10 times cheaper than the least expensive similar commercial instrument we know of, and is actually superior to the commercial instrument in a number of regards, for example, for measurement in the polar regions. We hope that the present open source release, beyond sharing the design of our specific instrument, will encourage many more actors to share their developments within in-situ instrumentation and participate in building an open source community around geophysical instrumentation.

**We will populate this repository and publish all our materials as open source upon acceptance of our paper in the peer reviewed literature (ongoing).**

XX:TODOs:UponPaperRelease
- add pointer to preprint, if useful please refer to our work etc
- populate with full legacy code when paper published
- explain legacy vs "bleeding edge"

## Overview of the content and organisation of the repository

- **/legacy** : all the materials about the "legacy" instrument hardware, firware based on Arduino IDE-1.8, decoder, building instructions etc; this is the material that was used in our paper. This works perfectly fine, but we are now working on a better architectured re-write using the experience we gained building the "legacy" instrument. **At the moment, the "bleeding edge rewrite" is a WIP, and this is the one only ready-to-use instrument and code; this will change in the future.**

- **/decoder_iridium_messages** : the "bleeding edge" iridium messages decoder. **At the moment, this is WIP.**
- **/instrument_firmware** : the "bleeding edge" firmware for the instrument, based on VSC+PlatformIO. **At the mommentm this is WIP.**
- **/instrument_hardware** : the "bleeding edge" hardware description, PCBs etc. **At the moment, this is WIP.**

- **/doc** : various documentation pages.

## Coding environments

I am using only Linux in the form of Ubuntu LTS distributions. Though the instructions here can likely be adapted to other distributions / OSes (possibly with the Windows Linux Subsystem or similar), if you want to use something else than Ubuntu 20.04 LTS or another modern Ubuntu LTS, you are on your own!

### Current instrument

The coding environment for the "current" instrument available here is based on:

- C++ with an Arduino flavor,
- Visual Studio Code and PlatformIO,
- the SparkFun Ambiq Apollo3 Arduino core, in "bare metal" (i.e. no RTOS), flavor.

For installation instructions and setup of the coding environment, see: https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/feat/platformio-tracker/doc/setup_platformio_environment/Instructions.md .

### "Legacy" instrument

The coding environment for the "legacy" instrument was based on Arduino IDE-1.8 and the SparkFun Ambiq Apollo3 Arduino core, in a slightly older version of the "bare metal" core. See the insructions at https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/doc/setup_arduino_v1.8_environment/Instructions.md .

### A note about other programming languages

C++ has some pros and cons, and it is possible to use other languages for low level programming. You can find a project that i) targets similar MCUs and development boards, ii) uses Rust-Lang as an alternative to C++, at: https://github.com/gauteh/sfy .

## Contribution policy

In case of any question, please use the Github issues tracker system.

All materials on this repo are released under the MIT license, unless explicitly stated otherwise. Nothing on this repository comes with any warranty.

