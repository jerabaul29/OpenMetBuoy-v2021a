# OpenMetBuoy-v2021a: an easy to build, affordable, customizable, open source instrument for oceanographic measurements.

Our pledge: many fields within Geosciences are relying on in-situ data collection. Traditionally, collecting in-situ data is expensive, with commercial instruments easily reaching several thousands of USDs even if only quite simple functionalities are provided. However, the recent emergence of open source communities within micro controllers and low level firmware for these opens new possibilities for developing low-cost, high performance instruments based on widely available and affordable components. This repository is focusing on developing an oceanic buoy based on such open source code and low cost components. Our solution is around 10 times cheaper than the least expensive similar commercial instrument we know of, and is actually superior to the commercial instrument in a number of regards, for example, for measurement in the polar regions. We hope that the present open source release, beyond sharing the design of our specific instrument, will encourage many more actors to share their developments within in-situ instrumentation and participate in building an open source community around geophysical instrumentation.

Link to our paper, if you find our work useful and build up on it, please consider citing it:

**XXTODO**.
```

```

## Overview of the content and organisation of the repository

- 

## Coding environments

I am using only Linux in the form of Ubuntu LTS distributions. Though the instructions here can likely be adapted to other distributions / OSes (possibly with the Windows Linux Subsystem or similar), if you want to use something else than Ubuntu 20.04 LTS or another modern Ubuntu LTS, you are on your own!

## "Legacy" instrument firmware

The coding environment for the "legacy" instrument was based on Arduino IDE-1.8 and the SparkFun Ambiq Apollo3 Arduino core, in a slightly older version of the "bare metal" core. It is well working, but a bit messy as it was developed "as things went". For now, this is the only one that is ready for use (see "Instrument firmware under rework" section).

## Instrument firmware under rework

The "legacy" instrument firmware has been developed "as things were going". It works well and is robust, but the codebase needs a good rework to clean it. The "reworked firmware" is a work in progress, without any guarantees on when it will be done, and is available at: https://github.com/jerabaul29/OpenMetBuoy-v2021a/tree/feat/platformio-tracker . It is based on:

- C++ with an Arduino flavor,
- Visual Studio Code and PlatformIO,
- the SparkFun Ambiq Apollo3 Arduino core, in "bare metal" (i.e. no RTOS), flavor.

See the corresponding branch for more instructions.

## A note about other programming languages

C++ has some pros and cons, and it is possible to use other languages for low level programming. You can find a project that i) targets similar MCUs and development boards, ii) uses Rust-Lang as an alternative to C++, at: https://github.com/gauteh/sfy . If you want to develop a new project from scratch, it may be worth considering if you want to use our C++ or the Rust firmware as a starting point :) .

## Contribution policy

In case of any question / bug report, please use the Github issues tracker system. I will **not** answer to private emails, as I want all discussions to be open, become part of the repo and, therefore, the "meta documentation" of the project, and serve the community.

All materials on this repo are released under the MIT license, unless explicitly stated otherwise. Nothing on this repository comes with any warranty. I do not guarantee any form of support.

