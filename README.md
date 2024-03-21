**A question? => open an issue; An idea of improvement? => open an issue and / or submit a pull request.**

# OpenMetBuoy-v2021a (OMB / OMB-v21a): an easy to build, affordable, customizable, open source instrument for oceanographic measurements - that you can build yourself of buy commercially

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

*NOTE*: following reviewer feedback during the review process, we had to cut some technical discussions in the published manuscripts, as these were seen as "engineering" and not science. We believe some of these discussions were important (and in fact we have been asked several times about technical points related to the cut content), so we recommend to read the preprint: https://www.researchgate.net/publication/357712696_OpenMetBuoy-v2021_an_easy-to-build_affordable_customizable_open_source_instrument_for_oceanographic_measurements_of_drift_and_waves_in_sea_ice_and_the_open_ocean . In particular, the methodology for the power consumption is available there in Table 1 legend.

You can either build the OMB yourself using the resources available on this repository (total cost of the parts: around 550-600USD in late 2022), or buy it as a turn-key (but still open source and hackable) product from LabMaker Gmbh for a reasonable cost (around 1200USD in late 2022): https://www.labmaker.org/collections/earth-and-ecology/products/openmetbuoy (note: the screenshot under is from Sept. 2022, price and conditions at LabMaker may vary, and I am not affiliated to Labmaker nor getting any money from LabMaker).

![OMB_Labmaker](https://user-images.githubusercontent.com/8382834/191990617-ad533b96-0d03-4e05-a78f-1240b57d4700.png)

The fully assembled instrument gets space into a box 12x12x9 cm, including 3 D-size battery holders (at the bottom, empty on the picture, should use typically 3 SAFT LSH20 D-size batteries), all the components, and the 9dof sensor:

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/from_side.jpg" width="600" />

We are also working on a PCB, but this is still a bit a work in progress. If you want access to the PCB for easier assembly, take contact directly here in an issue.

This is a typical example of dataset collected by an instrument that drifted in the Caribbeans: the track of the instrument and the wave measurements are indicated by the black curve, while satellite measurements of significant wave height (from the Sentinel satellites in particular) are shown on intersecting transsects with colored lines:

![caribbeans_deployment](https://user-images.githubusercontent.com/8382834/155947239-19cf49fe-5649-4f91-b513-fb7ae9e2e0ad.jpg)

Using the standard box and components, and 3 Li LSH20 batteries, the box floats quite well and works just fine, and will behave quite similar to an iSphere surface drifter for example (note that you will have to add some floatability if you use alkaline batteries, that are much heavier):

![box_no_extra_buoy](https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/floating_3LSH20_box.jpg)

We also describe how to build a standard, upper ocean drifter buoy, which allows to assemble a complete instrument+buoy setup looking like the following picture (to give a sense of scale, the PVC tube is about 1m long); this will drift more like the 1m depth current:

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/buoy_with_instrument.jpg" width="600" />

The OMB when used on the sea ice can be either put directly on sea ice, or on top of a PVC tube that is frozen over some depth in the sea ice, or on top of, for example, a fender buoy drilled a bit into the ice for stability, and equipped with a netting to attach the buoy, and a heavy chain at the bottom to keep it upright (photo and method credits for the fender buoy method: Ruth Mottram, Andrea Gierisch, and colleagues, Danish Meteorological Institute, NCKF/DMI, with help from Greenlandic hunters to develop and build the setup):

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/on_fender.jpg" width="500" />

The deployment method used may depend on the expected wind, precipitation (in particular if heavy snowfalls are expected and the OMB should be a bit elevated to avoid being buried and loosing satellite communications), and ice breaking conditions. A note though: polar bears **love** big, flashy colors, chewy stuff, and they will happily chew / play football with such a buoy: consider painting it white to make it less attractive if you are working in an area with a lot of polar bear activity!

## Key data

These are explained in more details in the paper, see in particular Tables 1, 2, 3, and the associated text, for more details.

- total cost of the parts: 562USD (Nov. 2021 price).
- typical assembly time: around 1-1.5hr per buoy when producing a small series efficiently.
- cost of the iridium communications: 42USD / month if using only GPS, up to 108USD / month when using GPS and high frequency wave measurements (Nov. 2021 price).
- battery autonomy: i) 4.6 months using 2 Li D-cells, with GPS and wave measurements activated. ii) Over 1 year using 2 Li D-cells and GPS tracking only. iii) Battery life scales linearly with the number of D-cells included in the instrument.
- typical detection threshold for waves in ice: 0.5cm at 16s period, even better detection threshold is obtained at higher frequency due to using an IMU to perform the measurements.

## Overview of the content and organisation of the repository

- https://github.com/jerabaul29/OpenMetBuoy-v2021a/tree/main/development_environment : this contains the instructions on how to set up your development environment to write firmwares, either for the "legacy" (Arduino 1.8-based), or the "rewrite" (PlatformIO-based) firmware.
- https://github.com/jerabaul29/OpenMetBuoy-v2021a/tree/main/instrument_hardware : this describes the hardware needed, and the parts to source, for building different versions of the instrument, and includes the assembly instructions. We also have a PCB (not released yet, take contact by issue or email to get access to it)
- https://github.com/jerabaul29/OpenMetBuoy-v2021a/tree/main/legacy_firmware : this contains the legacy firmware (version IDE 1.8), which at the moment is the only one ready to use. It provides both the source code (in different versions), and some pre-compiled binaries with default parameters.

## Coding environment

I am using only Linux in the form of Ubuntu LTS distributions. Though the instructions here can likely be adapted to other distributions / OSes (possibly with the Windows Linux Subsystem or similar, see for example https://github.com/jerabaul29/OpenMetBuoy-v2021a/issues/53 ), if you want to use something else than Ubuntu 20.04 LTS or another modern Ubuntu LTS, you are on your own! Also, some of the PlatformIO building flags rely on using a Linux system. These could be adapted to also work on Windows, but I am not interesed in spending the time to get it to work at the moment on neither Windows not MacOS (but contributions can be welcome).

## About the "legacy" vs "rewrite" firmwares

### "Legacy" instrument firmware

The coding environment for the "legacy" instrument was based on Arduino IDE-1.8 and the SparkFun Ambiq Apollo3 Arduino core, in a slightly older version of the "bare metal" core. It is well working, but a bit messy as it was developed "as things went". For now, this is the only one that is ready for use (see "Instrument firmware under rework" section).

**NOTE**: in the end, I never had the time to rewrite the firmware, so this is still the main firmware version, and it works well enough :) . All published works use this firmware!

### Instrument firmware under rework

**NOTE**: in the end, I never had the time to rewrite the firmware; so, this is a nice "framework" and nicer way to code if you want to develop your own firmware from scratch, but the OMB-v2021 will likely remain focused on the "legacy" firmware, which works well enough :) .

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

## A few deployments I know about

If you deploy the OMB, feel free to let me know in an issue! These are so far the deployments I know of (no guarantee this is up to date). This is just to show that we do have solid experience with the OMB, and that we have had many successful depoyments:

- 2 test buoys: NABOS expedition, Arctic, September 2021: 1 in the open water in the MIZ, 1 on ice
- 1 test drifter: OneOcean cruise, Caribbeans, November 2021: open water, freely drifting
- 3 ice drifters: Japanese Antarctic program, Antarctica, February 2022: 3 instruments on the sea ice
- 2 ice drifters: Seal pups monitoring cruise 2022, East Greenland sea, March 2022: on the ice in the MIZ, 2 drifters
- 20 ice drifters: CIRFA 2022 cruise, East Greenland sea, April 2022: 20 instruments on the sea ice
- 15 drifting buoys: CIRFA 2022 cruise, Barents sea, April 2022: 15 instruments, drifting in open water
- 20 ice drifters: AWI summer cruise 2022, Barents sea: 20 instruments, on the sea ice
- 15 ice trackers: Hovercraft 2022 cruise, Yamal plateau / East Greenland sea, August 2022: 15 instruments on the sea ice

## Openly available data

I also try to keep a separate repository about the openly available waves in ice and sea ice drift data available, with data gathered both by the OMB and other buoys, at:

- https://github.com/jerabaul29/meta_overview_sea_ice_available_data

If you look for OMB data, go there to get some examples. If you have some OMB data you want to make available, feel free to contact me and / or release the data and ask on https://github.com/jerabaul29/meta_overview_sea_ice_available_data for "indexing" them :) .

## Just for fun - a not so serious corner of the Readme :)

If you have more fun / goofy / strange pictures or stories that can fit in here, just share them as an issue or open a pull request!

### The OMB is polar bears approved

Polar bears approve of the use of OMBs to help monitor, understand, and protect their ecosystem!

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/misc/OMB_Polar_Bear_Mario_Hoppmann.jpg" width="600" />

Picture credits Mario Hoppmann, AWI.

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

### Trashing planet Earth, one step at a time - I feel really bad about this one

Most (all?) of the floating trash we throw in the oceans, ultimately comes back on the shores - or get caught in the Vortex. Also floating scientific instruments. I feel really bad about this, but at least our instruments are much smaller than typical commercial instruments (so, for a given number of measurements, we release less volume and mass of trash, so less trash footprint), and, at least, this is for a good cause (advancing human knowledge and protecting the world oceans), and not a random sacrifice to the pagan god of over-consumption and fast fashion... Still, if you have ideas on how to reduce the environmental footprint of the OMB, open an issue and help us get better :) .

This picture is actually from one of my "old, v2018 instruments", that drifted all the way from the Arctic MIZ North of Svalbard, until it got stranded on the Northen coast of Iceland. Someone taking a walk on the beach was kind enough to open it, read the waterproof notice with our contact information, and let us know it was found. Pretty rusted!

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/electronics_trash.jpg" width="600" />

Credits: Picture Finn Plny, sharing Atle Jensen; we are going to pick up the box, and try to get the raw data from the SD card.

### Go big or go home

The aim of the OpenMetBuoy (OMB) is to increase the number of measurement points you can get at constant budget, NOT to reduce your overall field instrumentation budget! So, go big or go home! This is what you get for the cost of 4 or 5 classical commercial instruments: enough components to build 40 OMBs!

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/go_big_go_home.jpg" width="600" />

Credits: Takehiko Nose, preparing to assemble 50 OMBs.

### "Dolly" the search-and-rescue doll

The OMB has also been used to test search and rescue systems - hard time floating alone in the middle of the Norwegian sea! Credits: Forsvarets 330 Skv. Avd Sola.

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/search_and_rescue_doll" width="600" />

### OMB are small but tough - rounding 330 days on Antarctic drift ice

The OMB is small but though. So far our record is having an OMB survive the harsh Antarctic environment for 330 days, drifting over 3000 kilometers, before the ice under it got broken and melt away during a summer storm. Amazing quality of assembly and deployment by the University of Tokyo, who are proud users of the OMB design!

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/misc/OMB_UoT.jpeg" width="400" />

Credits: Takehiko Nose et. al., University of Tokyo.

### This is what deployment "en masse" looks like!

The reduced cost of the OMB allows to perform deployments "en masse": here, over 70 OMB trajectories, all gathered during the summer and fall 2022, around the Svalbard archipelago, by a range of contributors (University of Tokyo, Norwegian Meteorological Institute, University in Tromø, University in Bergen).

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/misc/all_drifters_outreach.png" width="400" />

