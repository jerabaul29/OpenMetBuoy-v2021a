# LabMaker OMB instructions

A few user instructions adapted specifically for users who bought a LabMaker OMB: https://www.labmaker.org/collections/earth-and-ecology/products/openmetbuoy .

## High level guidelines

Keep in mind the following to avoid damaging your OMB:

- Never feed power from 2 sources at the same time: either have batteries inserted, or connect the OMB to your computer with USB-C, but never do both at the same time
- Never turn on the OMB without having the antenna connected: RF modules turned on without an antenna may be damaged by energy reflections
- Be careful about Electro Static Discharge (ESD): even small electrostatic discharges that you do not feel as a human may be enough to damage electronics. Ideally use an ESD-safe workplace (as is typically found in electronics workshops), or at least try to avoid using clothes that create a lot of static electricity, and make sure to touch a large grounded metal mass (such as a sink) before touching electronics

## Extras needed

You will need a couple of extras for using the OMB from Labmaker:

- batteries are not included; you will need SAFT LSH20 batteries (2x). Other D-cell-format, Lithium 3.6V batteries may work, but this comes without guarantee from us (and make sure if you use another kind / supplier of Li batteries to use batteries that these are 3.6V, and can provide enough current - at least up to around 200mA in bursts)
- to make the OMB fully watertight, we recommend using "bathroom silicon" to seal the box once the batteries are inserted
- we recommend inserting "dessiccant bags" to make sure there is no humidity / condensation happening when there are temperature changes.

## Instructions

Please read the following to get a general overview of the OMB use:

https://github.com/jerabaul29/OpenMetBuoy-v2021a/tree/main/end_user_manual

Quick walkthrough:

- upload the exact firmware version you want
  - the instrument should come from LabMaker with the correct firmware. However, we recommend uploading the firmware yourself, to make sure the correct version is used. For this:
    - install the GUI programming tool: see https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/development_environment/setup_arduino_v1-8_environment/Instructions.md#use-of-pre-compiled-firmware-binaries for instructions and / or instructions at https://github.com/sparkfun/Artemis-Firmware-Upload-GUI . (depending on your platform, you may need a bit of extra setup, such as registering to the ```dialout``` group on Linux: see : https://arduino.stackexchange.com/questions/74714/arduino-dev-ttyusb0-permission-denied-even-when-user-added-to-group-dialout-o .
    - remove batteries from the OMB
    - connect the OMB to your computer over USB-C
    - upload the firmware you want; we recommend using https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/legacy_firmware/binaries/steval_gps30mins_waves2hr_drifter.bin ; if you want other parameters (frequency of GPS / wave logging), feel free to ask for it in a new issue at https://github.com/jerabaul29/OpenMetBuoy-v2021a/issues .
    - test the instrument: put it outside after setting up the Rock7 account (see next point), or just let it running connected over USB-C and check the serial output (see https://github.com/jerabaul29/OpenMetBuoy-v2021a/tree/main/end_user_manual#checking-operation-through-serial )
- prepare the Rock7 account and data collection: see https://github.com/jerabaul29/OpenMetBuoy-v2021a/tree/main/end_user_manual#receiving-the-iridium-messages .
- test and seal: we recommend to i) install batteries and put a dessiccant bag, ii) screw the lid, iii) put outside and do a test for a day to check messages are received as expected and the instrument works. Once this is done and it is confirmed that the instrument works well, seal it with bathroom silicon to make it 100% watertight; put the magnet and make sure the instrument is switched off.
- deploy: to deploy, all you need to do is remove the magnet and the instrument will turn on. Remember to have an active Rock7 subscription and enough credits!

## Taking contact

In case of any question, open an issue on the present GH repository to get help: https://github.com/jerabaul29/OpenMetBuoy-v2021a/issues .
