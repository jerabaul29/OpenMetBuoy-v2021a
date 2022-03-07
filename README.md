# OpenMetBuoy-v2021a: an easy to build, affordable, customizable, open source instrument for oceanographic measurements.

**Our pledge**: many fields within Geosciences are relying on in-situ data collection. Traditionally, collecting in-situ data is expensive, with commercial instruments easily reaching several thousands of USDs even if only quite simple functionalities are provided. However, the recent emergence of open source communities within micro controllers and low level firmware for these opens new possibilities for developing low-cost, high performance instruments based on widely available and affordable components. This repository is focusing on developing an oceanic buoy based on such open source code and low cost components. Our solution is around 10 times cheaper than the least expensive similar commercial instrument we know of, and is actually superior to the commercial instrument in a number of regards, for example, for measurement in the polar regions. We hope that the present open source release, beyond sharing the design of our specific instrument, will encourage many more actors to share their developments within in-situ instrumentation and participate in building an open source community around geophysical instrumentation.

Link to our paper, if you find our work useful and build up on it, please consider citing it:

```
Rabault, Jean, et al.
"OpenMetBuoy-V2021: an easy-to-build, affordable, customizable, open source instrument for
  oceanographic measurements of drift and waves in sea ice and the open ocean."
Geosciences (2022).
```

- As a preprint: https://www.researchgate.net/publication/357712696_OpenMetBuoy-v2021_an_easy-to-build_affordable_customizable_open_source_instrument_for_oceanographic_measurements_of_drift_and_waves_in_sea_ice_and_the_open_ocean , and as pdf on this repository (see **latest_preprint_MDPI_2022.pdf**),
- As a published paper (open access, CC-BY license): https://www.mdpi.com/2076-3263/12/3/110 .

The fully assembled instrument gets space into a box 12x12x9 cm, including 3 D-size battery holders (at the bottom, empty on the picture, should use typically 3 SAFT LSH20 D-size batteries), all the components, and the 9dof sensor:

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/from_side.jpg" width="600" />

This is a typical example of dataset collected by an instrument that drifted in the Caribbeans: the track of the instrument and the wave measurements are indicated by the black curve, while satellite measurements of significant wave height (from the Sentinel satellites in particular) are shown on intersecting transsects with colored lines:

![fig_for_linkedin](https://user-images.githubusercontent.com/8382834/155947239-19cf49fe-5649-4f91-b513-fb7ae9e2e0ad.jpg)

## Overview of the content and organisation of the repository

- https://github.com/jerabaul29/OpenMetBuoy-v2021a/tree/main/development_environment : this contains the instructions on how to set up your development environment to write firmwares, either for the "legacy" (Arduino 1.8-based), or the "rewrite" (PlatformIO-based) firmware.
- https://github.com/jerabaul29/OpenMetBuoy-v2021a/tree/main/instrument_hardware : this describes the hardware needed, and the parts to source, for building different versions of the instrument, and includes the assembly instructions.
- https://github.com/jerabaul29/OpenMetBuoy-v2021a/tree/main/legacy_firmware : this contains the legacy firmware (version IDE 1.8), which at the moment is the only one ready to use. It provides both the source code (in different versions), and some pre-compiled binaries with default parameters.

## Coding environment

I am using only Linux in the form of Ubuntu LTS distributions. Though the instructions here can likely be adapted to other distributions / OSes (possibly with the Windows Linux Subsystem or similar), if you want to use something else than Ubuntu 20.04 LTS or another modern Ubuntu LTS, you are on your own! Also, some of the PlatformIO building flags rely on using a Linux system. These could be adapted to also work on Windows, but I am not interesed in spending the time to get it to work at the moment on neither Windows not MacOS (but contributions can be welcome).

## About the "legacy" vs "rewrite" firmwares

### "Legacy" instrument firmware

The coding environment for the "legacy" instrument was based on Arduino IDE-1.8 and the SparkFun Ambiq Apollo3 Arduino core, in a slightly older version of the "bare metal" core. It is well working, but a bit messy as it was developed "as things went". For now, this is the only one that is ready for use (see "Instrument firmware under rework" section).

### Instrument firmware under rework

The "legacy" instrument firmware has been developed "as things were going". It works well and is robust, but the codebase needs a good rework to clean it. The "reworked firmware" is a work in progress, without any guarantees on when it will be done, and is available at: https://github.com/jerabaul29/OpenMetBuoy-v2021a/tree/feat/platformio-tracker and "related" branches. It is based on:

- C++ with an Arduino flavor,
- Visual Studio Code and PlatformIO,
- the SparkFun Ambiq Apollo3 Arduino core, in "bare metal" (i.e. no RTOS), flavor.

See the corresponding branch for more instructions.

If you want to develop a "cousin" instrument of our own, I would recommend that you base yourself on this development environment.

## A note about other programming languages

C++ has some pros and cons, and it is possible to use other languages for low level programming. You can find a project that i) targets similar MCUs and development boards, ii) uses Rust-Lang as an alternative to C++, at: https://github.com/gauteh/sfy . If you want to develop a new project from scratch, it may be worth considering if you want to use our C++ or the Rust firmware as a starting point :) (NOTE: I see the pont of Rust and I want to transition to it in the - hopefully - not so far future, but for now I am still working in C++, the code there is from a colleague :) ).

## Contribution policy

In case of any question / bug report, please use the Github issues tracker system. I will very likely **not** answer to private emails, as I want all discussions to be open, become part of the repo and, therefore, the "meta documentation" of the project, and serve the community.

All materials on this repo are released under the MIT license, unless explicitly stated otherwise. Nothing on this repository comes with any warranty. I do not guarantee any form of support.

