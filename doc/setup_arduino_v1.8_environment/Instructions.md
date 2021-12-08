At present, I do not recommend to use the Arduino IDE 1.8 for newer developments. However, the "old, first, prototype" version of the code was developed in Arduino IDE 1.8, so if you want to use it, you will need the Arduino IDE 1.8. The steps needed are (as usual, on Ubuntu 20.04):

- get the Arduino IDE 1.8: see https://www.arduino.cc/en/software ,
- on Ubuntu, all what is needed to run the Arduino 1.8 IDE is i) extract the tar, ii) navigate to arduino-1.8.16-linux64/arduino-1.8.16 within the extracted files, iii) run the arduino executable,
- there is a bit of setup at this point, see https://learn.sparkfun.com/tutorials/artemis-development-with-arduino/all . tldr: in the arduino IDE, preferences, add the external URL: https://raw.githubusercontent.com/sparkfun/Arduino_Apollo3/main/package_sparkfun_apollo3_index.json ,
- install the Sparkfun Apollo3 board, in v1.2.3 (NOT in v2.x.x, since v1.2.3 is the bare metal core, while v2.x.x is RTOS-based)

From there, you will need to install by hand the libraries in the correct version, by github cloning and checking out the right repos into the ~/Arduino/libraries folder. Some explanations are also available in the corresponding "legacy/tracker_prototype" folder.
