This will guide you in case you want to upload firmware using the Arduino 1.8 IDE, or compile the code from the "legacy" firmware coded in Arduino 1.8 IDE. You can either use "out of the box" one of the firmware binaries I provide, or compile your own with custom setups / modifications.

# Use of pre-compiled firmware binaries

I provide a few pre-compiled firware binaries, https://github.com/jerabaul29/OpenMetBuoy-v2021a/tree/main/legacy_firmware/binaries folder. These come with some default firmware configuration. If you want another firmware configuration, either request so in an issue, or compile it yourself (see instructions under for compiling yourself).

To upload the binaries:

- install the necessary dependencies:
```
~$ sudo apt update
~$ sudo apt-get install python3-pyqt5
~$ sudo apt-get install python3-pyqt5.qtserialport python3-serial
```
- download the sparkfun tool: https://github.com/sparkfun/Artemis-Firmware-Upload-GUI , either by cloning the repository, or by downloading the zip.
- the executable does not seem to work out of the box due to some broken dependencies, but this is not an issue, as the python executable works just fine; for example, from the downloaded zip:
```
~/Downloads/Artemis-Firmware-Upload-GUI-master/tools$ python3 artemis_firmware_uploader_gui.py 
```
- in the GUI, choose the firmware file to upload, the COM port of the Artemis, the baudrate (I recommend 921600), and click "Upload Firmware". This will flash the given firmware on the Artemis.

# Compiling the firware yourself

The "legacy" version of the firmware was developed in Arduino IDE 1.8, so if you want to compile it, you will need the Arduino IDE 1.8. The steps needed are (as usual, on Ubuntu 20.04):

## Getting the Arduino 1.8 IDE

- get the Arduino IDE 1.8: see https://www.arduino.cc/en/software ,
- on Ubuntu, all what is needed to run the Arduino 1.8 IDE is i) extract the tar, ii) navigate to arduino-1.8.16-linux64/arduino-1.8.16 within the extracted files, iii) run the arduino executable,
- there is a bit of setup at this point, see https://learn.sparkfun.com/tutorials/artemis-development-with-arduino/all . tldr: in the arduino IDE, preferences, add the external URL: https://raw.githubusercontent.com/sparkfun/Arduino_Apollo3/main/package_sparkfun_apollo3_index.json ,
- install the Sparkfun Apollo3 board, in v1.2.3 (NOT in v2.x.x, since v1.2.3 is the bare metal core, while v2.x.x is RTOS-based)

From there, you will need to install by hand the libraries in the correct version, by github cloning and checking out the right repos into the ~/Arduino/libraries folder. For more information about which library versions to use, see the sections under.

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

- I am also using an "updated" OneWire library to talk to temperature sensors: see https://github.com/jerabaul29/OneWire/tree/feat/Artemis_compatible .

## Compiling and uploading to the Artemis Global Tracker

- Use the board: "Sparkfun Artemis Module". Other Artemis boards have different pinouts / definitions for I2C ports etc and will not work (some will compile but use wrong pins; some will not compile at all due to missing I2C port definitions).
- Use the "Sparkfun Variable Loader (Recommended)" to upload to the AGT.

## Keeping the binaries for later use (if you want)

Once the binaries got compiled, you can find it in a tmp folder. For example, when I compile the code in the Arduino 1.8 IDE, I get towards the end of compilation the following output (the exact tmp folder will change):

```
/home/jrmet/.arduino15/packages/SparkFun/tools/arm-none-eabi-gcc/8-2018-q4-major/bin/arm-none-eabi-objcopy -O binary /tmp/arduino_build_434342/tracker.ino.axf /tmp/arduino_build_434342/tracker.ino.bin
```

This means that a full binary of the firmware, that can be saved and uploaded later on, is available at **/tmp/arduino_build_434342/tracker.ino.bin** in my case.
