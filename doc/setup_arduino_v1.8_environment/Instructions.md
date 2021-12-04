At present, I do not recommend to use the Arduino IDE 1.8. If you really want to use it, the steps needed are (as usual, on Ubuntu 20.04):

- get the Arduino IDE 1.8: see https://www.arduino.cc/en/software ,
- on Ubuntu, all what is needed is i) extract the tar, ii) navigate to arduino-1.8.16-linux64/arduino-1.8.16 within the extracted files, iii) run the arduino executable,
- in the arduino IDE, preferences, add the external URL: https://raw.githubusercontent.com/sparkfun/Arduino_Apollo3/main/package_sparkfun_apollo3_index.json ,
- install the Sparkfun Apollo3 board, in v1.2.3 (NOT in v2.x.x, since v1.2.3 is the bare metal core, while v2.x.x is RTOS-based)

From there, you will need to install by hand the libraries in the correct version, by github cloning and checking out the right repos into the ~/Arduino/libraries folder.