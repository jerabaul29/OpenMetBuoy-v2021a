**A question? => open an issue; An idea of improvement? => open an issue and / or submit a pull request.**

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

![caribbeans_deployment](https://user-images.githubusercontent.com/8382834/155947239-19cf49fe-5649-4f91-b513-fb7ae9e2e0ad.jpg)

Using the standard box and components, and 3 Li LSH20 batteries, the box floats quite well and works just fine, and will behave quite similar to an iSphere surface drifter for example (note that you will have to add some floatability if you use alkaline batteries, that are much heavier):

![box_no_extra_buoy](https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/floating_3LSH20_box.jpg)

We also describe how to build a standard, upper ocean drifter buoy, which allows to assemble a complete instrument+buoy setup looking like the following picture (to give a sense of scale, the PVC tube is about 1m long); this will drift more like the 1m depth current:

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/buoy_with_instrument.jpg" width="600" />

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

## Quirks / errors

This is a large code base, that I largely developed on my own with a very limited time budget, so there are some quirks / errors. I will fix the ones that lead to wrong results, but minor quirks may not be fixed fully. This is to list such quirks.

- **dyslexia frequency vs. period**: the instrument, internally, computes the zero crossing and the mean *frequency*, not *period* as the code says (i.e. there is a relation by ```f(x)=1/x``` between what is named in the code and what is actually computed). This would be error prone to fix in all firmwares, so I choose to i) keep it "as it is" in the firmware, and ii) fix this a posteriori in the decoder by taking the inverse there. For more discussions, see: https://github.com/jerabaul29/OpenMetBuoy-v2021a/issues/36

## Just for fun - a not so serious corner of the Readme :)

### Floatenstein, the ugly drifter

A buoy that needed to fly by plane (so Li batteries not allowed) had to be equipped with 3 Alkaline D cells in series instead. Alkaline batteries are heavier than Li batteries, so it was not floating. The solution was to add two chunks of styrofoam, wrapped in duct tape, fixed with cable nilon strips, all of it tightened with bathroom silicon. That made for an ugly, sticky, stinky instrument, but meh, it crossed the Caribbeans without any issue, so don't worry about how your instruments look like - as long as they work! :) .

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/floatenstein_light.jpg" width="600" />

Picture credits: me :) .

### The Arctic is tough, even if you're expensive

Instrumentation has a tendency to fail early and "randomly" in the Arctic - and harsh field conditions do not care about how much money you cost! This is an example where the telemetry from a Datawell Mk II deployed outside of Longyearbyen, Svalbard, was lost after just 2 hours. We were worried that the mooring had failed, or the buoy had sunk. In reality, due to the conditions (1 deg C water, -15 deg C air and some wind and waves), the Mk II had got iced and tilted. So next time your ocean buoy fails in the Arctic, don't take it too hard on yourself - it is likely not your fault, and it does not matter how much your instrument cost - stick to the OpenMetBuoy.

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/IMG_1099(1).JPG" width="400" />

Picture credits Atle Jensen.

### There is nothing as tasty as a free lunch

If you work in the Arctic, don't use bright colors, blinking LEDs, and chunky flags that make your instruments visible from far away - for a polar bear, there is nothing as tasty as a free lunch, even if it tastes plastic. According with some friends who are experts on Arctic expeditions, polar bears have a particular affinity for round instruments that they can play football with, large instruments they can use as a trampoline, and soft plastic / rubber that they can use to sharpen their teeths. Oh, and cables too! Record to break according to my friend: 250kUSD destruction of scientific instrumentation within 12 minutes...

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/687px-Polar_bears_chewing_cables.jpg" width="600" />

Picture credits: this is actually from wikimedia commons, I did not have a good picture lying around: https://commons.wikimedia.org/wiki/File:Polar_bears_chewing_cables.jpg .
