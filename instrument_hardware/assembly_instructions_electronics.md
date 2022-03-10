# Instructions for building the waves in ice instruments

A few high level rules:

- Protect "naked" metal with duct tape, to avoid any possibility for short circuit if things move around in the box.
- Wait to fix things with strong glue like epoxy until everything is tested; until everything is tested, fix things with duct tape, this is strong enough, and add in addition to the duct tape a bit of epoxy fixation after testing.
- Never feed power to the main board with 2 sources at the same time (for example, USB and direct power in). This means, if you turn the instrument on, make sure first that the USB is disconnected; if you want to connect the instrument to the USB, make sure first that the instrument is turned off.
- Electronics are sensitive to static discharges; if possible, work on an ESD-safe workplace. If you do not have access to an ESD-safe workplace, try to avoid static electricity as much as possible (avoid synthetic clothes), and try to "static-electricity-discharge" yourself regularly (touch a large piece of metal connected to the ground, such as a sink or similar).
- condensation and humidity is the ennemy of electronics; make sure to i) work in a room that is as dry as possible, ii) use desiccant bags
- sudden temperature cycling is not good for electronics as i) it is a factor creating condensation, ii) repeated, sudden temperature changes put strains on solders. Try, if possible, to avoid too many harsh temperature cyclings (for example, if possible, avoid taking the instrument in from a +20 degrees temperature outside to a -20 degrees temperature too many times; ideally, let the instrument cool down and warm up slowly).
- never try to use the GPS or the Iridium without antenna, to avoid RF feedback back into the GPS / Iridium chips. This means in practice, never feed power to the AGT without having connected an antenna to it first.

## General schematic

The general schematic is as follows. The section belows are detailed instructions / tips and tricks to assemble it:

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/general_schematic.jpg" width="800" />

The finished buoy should in the end look like:

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/from_side.jpg" width="600" />

## Preparation of the power supply, part 1: making the 3 battery holders in parallel ready

The goal is to get a circuit that looks like the following drawing:

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/schematic_3cells.jpg" width="400" />

This describes the case when 3 D-cell batteries are used inside some battery holders. If you use some batteries with soldering tags, you can drop the battery holders and solder the wires directly on the soldering tags.

- tape 3 D-cell battery holders together in the same direction (to be used in parallel) to form a "pack"
- some D-cell battery holders may have bad contacts; put 1 cell in each battery holder, and check that the voltage is transmitted to the solder pins; if not, fix with some solder; when done, remove the cells (avoid having any form of batteries or power supply connected when building / soldering things)
- connect 3 D-cell battery holders in parallel (follow usual conventions: + is red, - / ground is black)
- add 1 red (middle long) and 1 black (long) wire coming out of the 3 batteries assembly
- fix the battery holders at the bottom of the box with some duct tape (by making a "duct tape loop" and putting it at the bottom of the box)

At this point, things should look like on the image below:

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/illustration_3_holders.jpg" width="400" />

## Preparation of the power supply, part 2: mounting the magnetic switch, checking its good functioning

The goal is to get a circuit that looks like the following drawing:

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/reed_schematic.jpg" width="400" />

This describes how to set up a magnetic (also known as reed) switch. If you know that you can put / remove the batteries until the deployment time, this can be skipped and you can just connect the battery wires directly to the 3.3V power regulator. Be **careful**, the reed switches are very easy to break. Make sure to always i) hold a switch leg close to the glass, ii) bend the same leg from further away on the leg, i.e., make sure that all the stress is only on the metal, and never gets applied to the metal package. The reed magnetic switch works in the following way:

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/reed_function.jpg" width="400" />

Make sure to use the right orientation when i) fixing the reed switch on the box wall (so that the reed switch blade moves perpendicular to the box wall), ii) orient the magnet correctly, as shown under:

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/reed_orientation.jpg" width="400" />
<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/magnet_orientation.jpg" width="400" />

- carefully separate a bit the 2 legs of the reed switch
- use a multimeter to determine which legs are connected when there is no magnet
- cut the leg that is not connected when there is no magnet / connected when there is a magnet; do not cut it too close to the glass part, and make sure to not damage the glass part when cutting it
- protect the remaining of the cut leg by carefully wrapping some duct tape around
- the reed switch is a small blade that gets bent when applying a magnetic field; the displacement of the blade should be perpendicular to the wall of the box; bend the leg so that, when taping the reed switch on the box, its moving blade will move perpendicular to the box wall
- solder the 2 legs of the reed switch: 1 connects to the + red wire from the battery pack, the other one connects to a relatively short red wire that will be used to power the 3.3V regulator
- tape the reed switch on the box wall
- mark with a marker the position of the reed switch on the exterior of the box
- use put 2 magnets together; they will naturally match with one N and one S; mark the outwards facing N and S with some marker; the magnetic field is strongest coming out of these faces; these are the faces that should be face the reed switch to change its state
- check with a multimeter that, without magnet, the battery pack + is connected with the wire coming out of the reed switch, and than when the magnet is on, the 2 are not connected; notice that when the reed switch state is changed by putting / pulling the magnet, a small noise can be heard.

## Preparation of the power supply, part 3: connecting to the pololy 3.3V power regulator

The goal is to get a circuit that looks like:

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/pololu_schematic.jpg" width="400" />

- connect the pololu 3.3V power regulator: the wire coming from the reed switch goes to pololu Vin (unregulated battery power in); connect a small black wire from the pololu GND to the GND wire from the battery, keeping enough metal wire to the open air to be able to solder an additional GND wire that will power the Artemis; connect a red wire from the Vout, that will be used to power the Artemis.

At this point, things should look like on the image (just note that the black wire from the battery is not yet connected to the small black wire from the pololu):

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/pololu_reed_switch.jpg" width="400" />

- wrap the pololu with duct tape to avoid naked metal
- test that all works well: put a battery in one of the holders, and check that: i) if there is no magnet, 3.3V is obtained between the red and the black wires coming out of the pololu, ii) if there is a magnet, the voltage between the 2 wires is 0V (in practise, due to the capacitors on the regulator, it will be a small, dropping voltage that converges to 0V if you wait enough).

This confirms that you have working power supply: the 3 batteries in parallel are being regulated to 3.3V and switched on / off by the reed switch

## Plastic plate with mounting holes

I recommend using a thin plastic plate (for example, 2mm thick polycarbonate plate as one can get in Biltema or any similar hardware store). To fix the AGT on the plate (note: do not fix it yet, this will be done after the power supply of the AGT has been soldered; this is just for checking where mounting holes should be, and their diameter), use some nylon snap rivet; for example, rivets with diameter 3mm and length 9mm work perfectly).

- cut the plate so that it fits into the box (a guillotine cutter machine is in my experience what works best for cutting it fast)
- pre-position the AGT on the plate (and possibly extra components such as qwiic switch and IMU if these are to be used), making sure you have enough space for the qwiic connector at the bottom and the antenna at the top, and mark the position of the mounting holes
- drill the mounting holes
- check that the assembly works

Things should typically look like:

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/plast_plate.jpg" width="400" />
<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/all_in_box.jpg" width="400" />

## Preparation of the electronics boards, part I: Artemis global tracker main part

The power supply to the AGT is very simple:

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/AGT_power.jpg" width="400" />

- program the Artemis, see: https://github.com/jerabaul29/OpenMetBuoy-v2021a/tree/main/development_environment for example for uploading the (currently recommended) legacy firmware: https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/development_environment/setup_arduino_v1-8_environment/Instructions.md . The simplest solution is to upload a pre compiled binary, if one of these fits your needs. Note that, if you use only a 6dof sensor, you need a firmware version without the magnetometer.
- to save power, the always-on PWR LED on the AGT has to be cut; use a hobby knife to the "PWR\_LED" pad at the rear of the AGT (for explanations about cutting pads, see: https://learn.sparkfun.com/tutorials/how-to-work-with-jumper-pads-and-pcb-traces/cutting-a-trace-between-jumper-pads ).
- solder the power in to the AGT: the common GND from pololu and the battery to any GND pin; the 3.3V Vout coming out of the pololu regulator to any 3.3V pin
- connect the antenna, using the extension cable
- fix the AGT on the plastic plate using snap rivets

## Preparing of the electronics, part II: wave measurement hardware

This is only to be used if the wave measurement functionality should be part of the instrument (i.e., this is not requested for "pure GPS drifters"). In case you have a ST-eval kit instead of a the 9dof sensor because of chips shortage, see the instructions for connecting the ST-eval kit at: https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/ST_eval_instructions.md .

- cut the 2 LED pads on the qwiic switch (LED\_IN and LED\_OUT)
- connect AGT to qwiic switch using a qwiic cable; the AGT should connect to the IN qwiic switch port
- connect the qwiic switch to 9dof (or 6dof if only using a 6dof) using a qwiic cable; the 9dof should connect to the OUT qwiic switch port 
- put the components in position on the plastic plate and in the box; if possible, try to locate the 9dof sensor on the opposite side to the magnet. The orientation of the 9dof sensor does not play any role (as it will compute its orientation automatically and compensate for it), just make sure that it is stably fixed with some duct tape, snap river, or similar. I personally wrap it in tape (no naked metal policy), and tape it and add a dot of epoxy to keep it stable.
- if you want people to contact you when they find the box, print or write a piece of paper with your email, phone number etc, and tape it on the inside of the box lid, visible from outside
- add a desiccant silica gel bag into the box to make sure that there will be no issues with condensation

## Final test and assembly

The instrument should now look something like (this is a version without the wave measurements, so no qwiic switch and 9dof sensor):

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/instrument_ready.jpg" width="400" />

- register the modem on Rock7 and activate it (see https://rockblock.rock7.com and the GUI interface there)
- perform a full test outside; note that it may take a while for the registration of the modem to be effective, and that iridium messages do not always go through; i.e., do not worry if you do not get a message within a few minutes, and just let the instrument out for a few hours
- check receive the messages received and can decode them (see the decoder folder of the firmware you use)
- I would recommend, if possible, to let the instrument out for a day or so, to make sure that you have uploaded the right firmware, that the update rate is correct, etc
- when the instrument is fully tested, stop it, glue (in addition to the duct tape, no need to remove it) the different parts in place using epoxy, seal the top lid (screw it firmly so that the seal is tight), and add a layer of bathrool silicon (convenient, you can re open the instrument if needed) or epoxy glue (that will be harder to re open the instrument if needed) on the top and bottom holes of the screws and around the lid to make it fully waterproof over time.

