# End user manual

This is the "minimum" manual for the end user - there is no need to be familiar with all the details about the instrument to just operate it :) .

## Note about abrupt temperature changes

Sudden, violent temperature cycling is not good for electronics as i) it is a factor creating condensation even when dessiccant bags are used inside the electronics enclosure, ii) repeated, sudden temperature changes put strains on solders and weaken / break some connections. Try, if possible, to avoid too many harsh temperature cyclings (for example, if possible, avoid taking the instrument brutally in from a +20 degrees temperature inside to outside at a -20 degrees temperature too many times; ideally, let the instrument cool down and warm up slowly, and / or have intermediate steps, for example, by putting the instrument in an "acclimatation" room at around 0 degrees before moving back and forth between a -20 and a +20 degrees environment). This will not be an issue when deployed in the field, as weather-related temperature changes are relatively slow, but may be an issue when taking instruments back and forth outside of a building / ship.

## Checking operation through serial

You can check the operation of an instrument through the serial USB-C connector output: all instruments, if connected to a computer over USB, will output log information through serial-over-USB at baudrate 1000000. This can be useful to troubleshoot, check that an instrument is working as it should, do some development etc. To do so:

- make sure that the instrument is powered off and that there is no power input from the battery (ie "never feed in power from 2 different power sources at the same time" rule)
- connect the instrument to your computer over USB
- monitor the serial output over the USB port. The baudrate is 1000000 (1 million), and you can either use the serial monitor built in inside the Arduino IDE or the PlatformIO editor, or on the linux command line:

  - find which port the instrument is connected to (you can, depending on how your machine is set, use some combination of ```lsusb```, ```dmesg | grep tty```, ```ls ttyUSB*``` etc).
  - set the baudrate and log the serial output as it is received:

```
stty -F /dev/ttyUSB0 1000000  # set baudrate
tail -f /dev/ttyUSB0  # read from tty
od -x < /dev/ttyUSB0  # or read in octal, xxd can also be used etc
```

This should provide a lot of information about the setup of the firware version, what is going on on the instrument, etc.

## Switching on and off

### General principle

The magnetic switch is used to switch the instrument on and off.

- if the magnet IS PRESENT at the right location, the instrument is OFF
- if the magnet IS ABSENT, the instrument is ON

### About the placement of the magnet

Note that:

- The exact location of the magnet relatively to the box wall is quite important: it has to actually activate a magnetic switch inside the box; insert the magnet at the marked location (usually a black dot of some kind).
- The orientation of the magnet is important: you need to have one of the poles of the magnet facing the box wall, having the magnet "sideways" may not be enough to reliably activate the magnetic switch. To find the pole of the magnet, simply put 2 magnets together - they always end up attracting each other most strongly when they face "pole to pole".

### Procedure to turn on and off and check that the action is effective

In order to check that the switching on is effective, from the state of power down:

- remove the magnet
- then the blue LED should turn on

This allows to make sure that the instrument does is turned on, by checking that the blue LED gets active at once.

In order to check that the power off is effective, from the state of power up:

- add the magnet to disable the instrument, and keep it disabled for a few seconds
- remove the magnet and check that the blue led turns on (ie that the instrument was just forced to restart after it was stopped)
- add again the magnet and check that the blue led turns off immediately.

This allows to make sure that the instrument does is turned off, by checking that the blue led is abruptly turned off when the magnet is (correctly) placed.

In case you do not follow these procedures, you may end up believing that you have switched the instrument on or off without having actually done so (especially true for the "turn off" procedure: if the instrument is turned off wrongly (ie bad magnet placement) while it was sleeping, you will not get any indication - as the LEDs were already off and will stay so, except for an occasional blinking).

### Notes

As a side note: when the magnetic switch is added / removed, you should hear a small "tick" sound; this is an evidence that the magnetic switch has well changed state.

## LED colors and patterns

The LEDs have the following meanings:

- blue LED on: the GPS module is currently active. Should never happen while the orange or red LED is on.
- orange LED on: the supercapacitors for the iridium modem are being charged.
- red LED on: the iridium modem is on and attempting to contact the satellites. Should never happen while the blue LED is on.
- while LED: no particular meaning (or, rather, dependent on the context, and firmware-controlled); for now, it just blinks now and then to give a visual indication that things are going on well, when no other LEDs are active. During sleep, it will blink once in a while (a double blink every 2 minutes or so); when the IMU is active, it will blink a bit more often (every few seconds or so). It will also blink a few times briefly at boot.

## Typical activation sequences

The instrument typically (this may depend on the exact firmware setup):

### startup phase

- 1 blinks the white LED a few time when turned on

- 2 tries to get a GPS fix for a pre-determined time duration (typically 3 to 5 minutes depending on firmware setup)
- 2a if no GPS fix is obtained in 2, the instrument deduces that it has been turned on by mistake while in transit in a container / in a boat etc. To save battery, it will then go to sleep for typically 2 hours (controlled by firmware) before going back to state 2
- 2b if GPS fix is obtained in 2, the instrument deduces that it is rightly turned on; it sets up the UTC time on its internal clock from the GPS fix, and continues operating (going to step 3)

- 3 the instrument attempts to transmit the GPS fix of step 2 by iridium, for typically 5 minutes at most; iridium transmissions are not always successful, so it will jump to the next step anyways

- 4a the instrument attempts to take a thermistors measurement if thermistors is activated
- 4b the instrument attempts to transmit the thermistors measurement if thermistors is activated

- 5a the instrument attempts to take a wave measurement (over 20 minutes) if wave measurement is activated
- 5b the instrument attempts to transmit the wave measurement if wave is activated

At this stage, the instrument knows that it is operating, has got an initial gps fix and is aware of the UTC time, and attempted to transmit initial "startup measurements". It is ready to further operate.

### activity phase

- 6 the instrument sleeps in low power mode until the next gps measurement should be performed, by comparing the UTC time it has in its internal clock to the gps wakeup pattern
- 7 the instrument wakes up: it is time to take a gps measurement

- 8 the instrument checks the wave measurement pattern to check if it should also perform a wave measurement (typically every 3 hours)
- 9 the instrument checks the thermistors measurement pattern to check if it should also perform a thermistors measurement (typically every 1 hour)

- 10 the instrument attempts to get a GPS fix (timeout typically 3 minutes)
- 10a if the instrument got a GPS fix, it starts collecting 30 GPS measurements in a buffer, at 1Hz; i.e., this will take about 30 seconds
- 10b once the 30 GPS measurements are collected, the instrument applies an outlier rejection + averaging filter, to determine the GPS position to transmit
- 10c the instrument pushes the averaged GPS position + timestamp to the GPS buffer

- 11a if there was successful GPS fix at step 10, the instrument enters iridium transmission modus (if there was no GPS fix, the view of the sky is bad, and there will be no iridium transmission attempt)
- 11b the instrument checks if enough GPS measurements are buffered (typically 4 or more), and if so, packs them in a message, and attempts to transmit these
- 11c the instrument checks if there are outgoing wave messages, and if so, attempts to transmit them
- 11d the instrument checks if there are outgoing thermistor messages, and if so, attempts to transmit them

- 12a if it is time to perform a thermistors measurement from step 9, do it
- 12b push the thermistors measurement to the corresponding buffer of messages to transmit
- 12c if the gps fix 10 went through, attempt to transmit the thermistor message

- 13a if it is time to perform a wave measurement from step 8, do it
- 13b push the wave measurement to the corresponding buffer of messages to transmit
- 13c if the gps fix 10 went through, attempt to transmit the wave message

- 14 end the activity phase, jump back to 6

## Notes about the implementation

- when transmitting messages that are buffered, newer messages are always attempted to transmit first
- if any malfunction happens, either hardware fault or software bug, the hardware watchdog reboots the instrument (starting again from step 1). This means that buffered measurements are lost, but that otherwise functionality is restored.

## Receiving the iridium messages

To receive iridium messages:

- set up an account at Rock7
- register your specific modem
- activate the modem line
- buy some iridium data credits

The messages will be received on the user email adress, and can also be retrieved through the website GUI or with custom http requests (see https://github.com/jerabaul29/OpenMetBuoy-v2021a/tree/main/legacy_firmware/utils ). Note that it may take a bit of time (up to a few hours) for the information that your modem is active to "diffuse" to all satellites. Note that it is quite common for iridium messages to fail to go through, so do not worry if you miss a message now and then - things are buffered by the instrument anyways.

If you have not activated the iridium line rentals, messages will be lost without any way to recover them. So don't forget to switch on (and renew!) the iridium line rental. An iridium modem without line rental activated will "think" that it can send messages (ie, the message will be considered as "sent" if it is actually received by the satellite, and the corresponding data removed from the buffers).

Note that it may be possible, for larger deployments, to set up some form of invoicing: see the last post in the discussion https://github.com/jerabaul29/OpenMetBuoy-v2021a/issues/28 .

## Decoding the iridium messages

The messages received from Rock7 are hex-encoded binary strings. Decode them with: https://github.com/jerabaul29/OpenMetBuoy-v2021a/tree/main/legacy_firmware/decoder .

## Estimating iridium costs

From https://www.researchgate.net/publication/357712696_OpenMetBuoy-v2021_an_easy-to-build_affordable_customizable_open_source_instrument_for_oceanographic_measurements_of_drift_and_waves_in_sea_ice_and_the_open_ocean : (note that this is from 2021, with 2021 GBP prices, and 2021 GBP/USD exchange rate: this may evolve with time):

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/end_user_manual/assets/expected_costs.png" width="600" />

## Estimating battery time

From https://www.researchgate.net/publication/357712696_OpenMetBuoy-v2021_an_easy-to-build_affordable_customizable_open_source_instrument_for_oceanographic_measurements_of_drift_and_waves_in_sea_ice_and_the_open_ocean :

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/end_user_manual/assets/expected_battery.png" width="600" />

## Getting help

If you need help / guidance, do not send a private email, but open an issue on this Github repository!

