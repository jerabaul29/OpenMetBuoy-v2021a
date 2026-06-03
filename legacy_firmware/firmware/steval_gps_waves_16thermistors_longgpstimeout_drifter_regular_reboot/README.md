# Getting the code to compile and upload

- The code for the tracker is provided as an Arduino project.
- The main file is the ```tracker.ino``` sketch.
- I strongly recommend using Ubuntu or similar OS; all the advices under apply to an Ubuntu system. I will not provide support / help for other systems. A recent Ubuntu may be needed (I am using Ubuntu 20.04 myself). However, seems like other OSes are working fine (know of people using the code on Windows without any issues).
- The most recent tracker version is currently available in the branch ```feat/waves_in_ice_corev1```.

## Setting up the Arduino IDE including Artemis core

- I use the Arduino IDE 1.8.13. At the time when I started with this project, only the Arduino IDE 1.8.x was stable enough for development. IDE 2.x may or may not be stable enough nowadays. Platformio is another attractive alternative but the Artemis boards were not supported well when starting this project. At the moment, I wait for some stable platformio support for Artemis core before switching to platformio.
- One needs to install the Sparkfun Artemis core by hand; this is done by following the instructions at ```https://learn.sparkfun.com/tutorials/artemis-development-with-arduino/all``` in particular for adding the external URL.
- I am using the core v1.X. This core is a "bare metal" core without any reliance on a RTOS or similar, but with all the conveniences of the Arduino environment (all the digital, analog, I2C, SPI, etc, stuff works, and arduino libraries usually work out of the box). This is the only core mature enough at the time of development of this code. The core v2 (with is based on the MBedOS RTOS) will not be compatible with the code. I am using the core v1.2.1, though more recent versions of the core 1.2.x may / should work. I think that I will stick to the core v1 for the foreseeable future, migrating to the core v2 would require quite an architectural re-design to take good advantage of it.

## Setting up the extra libraries

- I am using several extra libraries. These can be installed directly from the library manager. In case of missing library, the compiler will indicate which library needs to be installed; to get all the libraries you need: compile, fail, install missing new library, repeat... You can get an overview of the libraries I am using (note: this may get outdated) by looking at the output in the file ```INFO_compilation_output_lib_infos.txt``` in this folder.

- The etl / embedded template library requires a bit of user "tuning". On my machine (and most Ubuntu machines), it should be found (after installation) at: ```~/Arduino/libraries/etl```. I was initially using an old version of the library, which requested adding an empty file in the library ```src``` to compile. For the present code to compile, the etl folder should look like:

```
~/Arduino/libraries/etl/src$ ls
etl  etl.h
```

The ```etl.h``` file is not present by default and needs to be created. It can be let empty. Without it, you will not be able to import ```etl/SOME_HEADER.h``` headers and compilation will fail. On more modern versions of the etl, there is an ```Embedded_Template_Library.h``` header (with a few meta macros) that fulfills the same need for more recent projects. You can let this file alone and ignore it, but you still will need to create an empty ```etl.h```.

- I also encountered some difficulties compiling the "Adafruit_BusIO" due to some missing mapping that are expected by the Adafruit library. To fix this, use my fork, available at: ```https://github.com/jerabaul29/Adafruit_BusIO/tree/fix/SPI_with_Artemis```. To use the fork, rather than the default library, simply:

```
jrmet@L590 ~ $ cd Arduino/libraries/
jrmet@L590 ~/Arduino/libraries $ rm -rf Adafruit_BusIO/
jrmet@L590 ~/Arduino/libraries $ git clone git@github.com:jerabaul29/Adafruit_BusIO.git
Cloning into 'Adafruit_BusIO'...
remote: Enumerating objects: 895, done.
remote: Counting objects: 100% (127/127), done.
remote: Compressing objects: 100% (73/73), done.
remote: Total 895 (delta 70), reused 100 (delta 54), pack-reused 768
Receiving objects: 100% (895/895), 312.16 KiB | 1.26 MiB/s, done.
Resolving deltas: 100% (536/536), done.
jrmet@L590 ~/Arduino/libraries $ cd Adafruit_BusIO/
jrmet@L590 ~/Arduino/libraries/Adafruit_BusIO master|✓ $ git checkout fix/SPI_with_Artemis
Branch 'fix/SPI_with_Artemis' set up to track remote branch 'fix/SPI_with_Artemis' from 'origin'.
Switched to a new branch 'fix/SPI_with_Artemis'
```

- I am using some updated version of the 9-dof Adafruit library (see https://github.com/adafruit/Adafruit_LSM6DS/pull/27 and https://github.com/adafruit/Adafruit_LSM6DS/issues/26 ). I hope these will be integrated into the Adafruit library. In the meantime to be able to use the code, you will need to use my fork of the Adafruit library and install it locally (by removing the "old" adafruit version and cloning into your ```~/Adafruit/library``` library folder): https://github.com/jerabaul29/Adafruit_LSM6DS/tree/feat/propagate_bool_flags . Remember to clone and switch to the correct branch :) . Failure to use my updated library will result in compilation error due to the getEvent signature (void vs bool in the update).

## Compiling and uploading to the Artemis Global Tracker

- Use the board: "Sparkfun Artemis Module". Other Artemis boards have different pinouts / definitions for I2C ports etc and will not work (some will compile but use wrong pins; some will not compile at all due to missing I2C port definitions).
- Use the "Sparkfun Variable Loader (Recommended)" to upload to the AGT.

# Hardware

## Essentials

- Artemis Global Tracker
- Iridium and GNSS passive antenna
- SMA extension cable
- Qwiic switch
- Adafruit 9-dof (Adafruit ISM330DHCX + LIS3MDL FeatherWing - High Precision 9-DoF IMU, PRODUCT ID: 4569)
- 2 Qwiic cables
- LSH20 cells, 3 in parallel
- pololu 3.3V step up step down regulator

## Adds on

- thermistors if wanted, with mini PCB
- magnetic switch

## Assembly

- connect AGT - SMA extender - antenna
- connect AGT Qwiic - Qwiic switch in - Qwiic switch out - Adafruit 9 dof
- solder battery - 3.3V regulator - 3.3V pin
- cut the LED enable traces on the AGT (```PWR_LED```) and Qwiich switch (```LED_IN``` and ```LED_OUT```)

# Operating notes

Typical power consumption, with up to date code, LEDs cut:

- sleep is around 0.5mA
- GPS on is 30 mA but this is very short (as only need a few seconds to get a fix, once in a while)
- 9-dof on and calculation of Kalman filter is 10mA altogether.
- FFTs go so fast it is negligible. Generally the Artemis MCU uses very little energy when active (typically a mA at most)
- Iridium is difficult to measure but in practice not much energy, as it goes very fast and is used only now and then, though there may be some instantaneous power peaks.
