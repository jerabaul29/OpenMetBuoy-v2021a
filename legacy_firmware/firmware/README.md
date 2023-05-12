## Pre compiled firmwares

A disclaimer first:

- these firmwares work well, but they are messy; I have had very little time to improve code quality / re-factor, so things have grown up "as things went", features were added continuously, and the firmware was developed as an experiment rather than a well engineered piece of software

- the different versions of the firmware are largely copies of each other with small changes; this is of course not ideal (it violates the "dont repeat yoursel principle", but it works well enough for now, and I have no time at the moment to make things better).

The main firmware versions:

- **plain_gps_drifter**: a plain gps drifter, no wave measurements
- **standard_gps_waves_drifter**: a standard gps and waves drifter, with both drift and waves measurements
- **steval_gps_waves_drifter**: the gps and waves drifter adapted to the 6dof sensor, ie the replacement chip for when the Adafruit 9dof is not available, using the ISM330DHCX 6dof sensor; basically, the same code, but without the magnetometer
- **steval_LSM6DSOX_gps_waves_drifter**: the gps and waves drifter adapted to the 6dof sensor, ie the replacement chip for when the Adafruit 9dof is not available, using the LSM6DSOX 6dof sensor; basically, the same code, but without the magnetometer
- **standard_gps_waves_thermistors_drifter**: a standard gps and waves and thermistor drifter, with up to 6 DS18B20 thermistors
- **two_ways_gps_waves_drifter**: the gps and waves drifter, with the 2-ways communication added to be able to change the frequency of GPS / waves measurements.
- **functionality_test_mode**: the gps and waves drifter, set up in such a way that it tests that the GPS, IMU, and Iridium components are well working and showing it on the serial output.
- **steval_gps_waves_pcbtraces_2ways**: the gps and waves drifter, set up for the ISM330DHCX IMU, using the AGT SDA SCL pins / PCB traces for the I2C port to the IMU (ie shared with the GPS with current AGT configuration), and the 2 ways communications; this is a slightly less version by myself, but some partners have used it in depth; sidenote: the code was copied from the private repo: https://github.com/tnn77/tracker_and_waves_instrument/tree/feat/waves_in_ice_ISM330 .

Of course, all versions of the firmware can be tuned to your needs; see in particular the **params.h** file. If you want specific firmware version compiled, feel free to open an issue, specify your need, and I will compile it for you and put it here.

## Compiling your own firmwares in the docker environment

In case you want to compile your own firmware versions yourself, the simplest way is to do so in Docker, to make sure you have you environment set up correctly including all tools and dependencies.

The provided Dockerfile (https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/legacy_firmware/firmware/Dockerfile credits @gauteh , many thanks for developing this!) allows to reliably and reproducibely build the firmwares, by setting all the dependencies and tooling in exactly the way specified in the Dockerfile, and using this environmen to build. See any tutorial about docker online for more information. The instructions below should ensure that you are able to build even if you are not familiar with docker.

- make sure you have docker (or podman, or singularity, these should be drop in replaceable) installed on your machine; see the latest instructions from the tool you want to use. I will assume docker in the following (though docker is on its way to being deprecated in favor of podman and singularity). If you use singularity or podman, just replace "docker" by the corresponding tool in the commands.

- build the container image (that may take a few minutes) and check it is here:

```
~/Desktop/Git/OpenMetBuoy-v2021a/legacy_firmware/firmware> docker build . -t omb_compiler:latest
~/Desktop/Git/OpenMetBuoy-v2021a/legacy_firmware/firmware> docker image list
REPOSITORY                       TAG           IMAGE ID       CREATED          SIZE
omb_compiler                     latest        d46d60db6332   31 minutes ago   2.38GB
```

- start an interactive terminal in a container spinned from the image; you are now working inside a container built from your omb image:

```
~/Desktop/Git/OpenMetBuoy-v2021a/legacy_firmware/firmware> docker run -it omb_compiler:latest /bin/bash
root@9ecf3c47682e:/work#
```

- go to the firmware version you want to use, rename the ```tracker.ino``` file as the name of the folder containing it (ArduinoIDE assumes / needs that the main .ino file has the same name as the folder containing it to compile):

```
root@9ecf3c47682e:/work# cd OpenMetBuoy-v2021a/legacy_firmware/firmware/steval_lsm6DSOX_gps_waves_drifter/
root@9ecf3c47682e:/work/OpenMetBuoy-v2021a/legacy_firmware/firmware/steval_lsm6DSOX_gps_waves_drifter# mv tracker.ino steval_lsm6DSOX_gps_waves_drifter.ino
```

- compile and exit the container; note the location where the binary firmware was generated (here, the ```/tmp/arduino/sketches/705AC3464EF059C0473A6EBFB8A15A77/steval_lsm6DSOX_gps_waves_drifter.ino.axf``` folder)

```
root@9ecf3c47682e:/work/OpenMetBuoy-v2021a/legacy_firmware/firmware/steval_lsm6DSOX_gps_waves_drifter# arduino-cli compile -v -b SparkFun:apollo3:artemis .
[...] lots of compile logs
/root/.arduino15/packages/SparkFun/tools/arm-none-eabi-gcc/8-2018-q4-major/bin/arm-none-eabi-size -A /tmp/arduino/sketches/705AC3464EF059C0473A6EBFB8A15A77/steval_lsm6DSOX_gps_waves_drifter.ino.axf
Sketch uses 118168 bytes (12%) of program storage space. Maximum is 960000 bytes.
Global variables use 157124 bytes of dynamic memory.

Used library                                Version Path
WDT                                         0.1     /root/.arduino15/packages/SparkFun/hardware/apollo3/1.2.1/libraries/WDT
Time                                        1.6.1   /root/Arduino/libraries/Time
Embedded Template Library ETL               20.22.0 /root/Arduino/libraries/Embedded_Template_Library_ETL
Wire                                        1.0     /root/.arduino15/packages/SparkFun/hardware/apollo3/1.2.1/libraries/Wire
SparkFun Qwiic Power Switch Arduino Library 1.0.0   /root/Arduino/libraries/SparkFun_Qwiic_Power_Switch_Arduino_Library
SparkFun u-blox GNSS Arduino Library        2.0.2   /root/Arduino/libraries/SparkFun_u-blox_GNSS_Arduino_Library
IridiumSBDi2c                               3.0.5   /root/Arduino/libraries/IridiumSBDi2c
Adafruit LSM6DS                             4.3.1   /root/Arduino/libraries/Adafruit_LSM6DS
Adafruit BusIO                              1.7.2   /root/Arduino/libraries/Adafruit_BusIO
SPI                                         1.0     /root/.arduino15/packages/SparkFun/hardware/apollo3/1.2.1/libraries/SPI
Adafruit Unified Sensor                     1.1.9   /root/Arduino/libraries/Adafruit_Unified_Sensor
Adafruit AHRS                               2.2.4   /root/Arduino/libraries/Adafruit_AHRS
OneWire                                     2.3.6   /root/Arduino/libraries/OneWire

Used platform    Version Path
SparkFun:apollo3 1.2.1   /root/.arduino15/packages/SparkFun/hardware/apollo3/1.2.1
root@9ecf3c47682e:/work/OpenMetBuoy-v2021a/legacy_firmware/firmware/steval_lsm6DSOX_gps_waves_drifter# exit
exit
~/Desktop/Git/OpenMetBuoy-v2021a/legacy_firmware/firmware>
```

- extract the firmware that just got compiled from the docker container into your host local folder; note that you need to use the ID of the container (that you can see in the command line information from above, in this case ```9ecf3c47682e```), and the path where the binary was generated (from the previous step):

```
~/Desktop/Git/OpenMetBuoy-v2021a/legacy_firmware/firmware> docker cp 9ecf3c47682e:/tmp/arduino/sketches/705AC3464EF059C0473A6EBFB8A15A77/steval_lsm6DSOX_gps_waves_drifter.ino.bin custom_firmware.bin
~/Desktop/Git/OpenMetBuoy-v2021a/legacy_firmware/firmware> ls -lrth
-rwxr-xr-x 1 jrmet jrmet 116K mai   12 11:54 custom_firmware.bin
```

- you can now upload the ```custom_firmware.bin``` to the AGT as any other firmware pre compiled binary (see the instructions at https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/development_environment/setup_arduino_v1-8_environment/Instructions.md ). You can also clean up unused containers to save space on your machine:

```
~/Desktop/Git/OpenMetBuoy-v2021a/legacy_firmware/firmware> docker container list --all
CONTAINER ID   IMAGE                 COMMAND                  CREATED        STATUS                         PORTS     NAMES
9ecf3c47682e   omb_compiler:latest   "/bin/bash"              2 hours ago    Exited (0) About an hour ago             upbeat_edison
~/Desktop/Git/OpenMetBuoy-v2021a/legacy_firmware/firmware> docker container rm 9ecf3c47682e
```

- Remember to **always test** new firmware binaries, to make sure that they work well, that you have set the right parameters, that the modifications you applied are working, etc. I recomment testing a full buoy for a couple of days typically when using a new firmware, to make sure everything works as intended.

