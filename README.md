# OpenMetBuoy-v2021a

An easy to build, affordable, customizable, open source instrument for oceanographic measurements.

XX:TODO:"manifesto", why, main objectives, how to use.

**We will populate this repository and publish all our materials as open source upon acceptance of our paper in the peer reviewed literature.**

XX:TODOs:
- add pointer to preprint, if useful please refer to our work
- populate with full legacy code when paper published

## Overview of the content and organisation of the repository

XX:TODO

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

C++ has some pros and cons, and it is possible to use other languages for low level programming. You can find a project that i) targets similar MCUs and development boards, ii) uses Rust-Lang, at: https://github.com/gauteh/sfy .
