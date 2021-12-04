# OpenMetBuoy-v2021a

An easy to build, affordable, customizable, open source instrument for oceanographic measurements.

**We will populate this repository and publish all our materials as open source upon acceptance of our paper in the peer reviewed literature.**

TODOs:
- add pointer to preprint
- populate with code

## Coding environment

I am using only Linux in the form of Ubuntu LTS distributions. Though the instructions here can likely be adapted to other distributions / OSes (possibly with the Windows Linux Subsystem or similar), if you want to use something else than Ubuntu 20.04, you are on your own!

The coding environment here is based on:

- C++ with an Arduino flavor,
- visual studio code and platformio,
- the SparkFun Ambiq Apollo3 Arduino core, in "bare metal" (i.e. no RTOS), flavor.

C++ has some pros and cons, and it is possible to use other languages for low level programming. You can find a project that i) targets similar MCUs and development boards, ii) uses Rust-Lang, at: https://github.com/gauteh/sfy .

For installation instructions and setup of the coding environment, see: **/doc/setup\_platformio\_environment/Instructions.md** .


