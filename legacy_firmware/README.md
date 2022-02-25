This contains the "legacy" code, i.e. the first firmware version and related tooling, developed on Arduino IDE-1.8, before we did the major rewrite and change to PlatformIO. This is working fine though, and all the instruments reported in the paper used this code - but, as time goes, the newer code based on PlatformIO will get much better :) .

- **binaries**: pre-compiled firmware binaries, ready to upload (see instructions at https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/development_environment/setup_arduino_v1-8_environment/Instructions.md )
- **decoder**: the python3 decoder for converting raw binary iridium messages into python objects that contain the scientific data
- **firmware**: the code for the firmware (in different versions)
- **utils**: various utils, for i) interacting with the Rock7 API and automatically retrieving the iridium messages, ii) processing data dumps from Rock7.
