This contains the "legacy" code, i.e. the first firmware version and related tooling, developed on Arduino IDE-1.8, before we did the major rewrite and change to PlatformIO. This is working fine though, and all the instruments reported in the paper used this code - but, as time goes, the newer code based on PlatformIO will get much better :) .

- **binaries**: pre-compiled firmware binaries, ready to upload (see instructions at https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/development_environment/setup_arduino_v1-8_environment/Instructions.md )
- **decoder**: the python3 decoder for converting raw binary iridium messages into python objects that contain the scientific data
- **firmware**: the code for the firmware (in different versions)
- **utils**: various utils, for i) interacting with the Rock7 API and automatically retrieving the iridium messages, ii) processing data dumps from Rock7.

A few notes about the firmware:

- the baudrate for the serial output is 1000000 ; this is higher than is usual for Arduinos, but works well with the Artemis
- you can test features and good functioning easily by enabling debugging / test runs, for example:

https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/8edfdab28c1fd6196a0b6bd81b148be4b010553f/legacy_firmware/firmware/standard_gps_waves_thermistors_drifter/tracker.ino#L49-L76

- to send commands to update frequencies etc, see the documentation at:

https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/8edfdab28c1fd6196a0b6bd81b148be4b010553f/legacy_firmware/firmware/two_ways_gps_waves_drifter/iridium_manager.h#L46-L70
