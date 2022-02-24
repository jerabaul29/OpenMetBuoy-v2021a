# Instructions for building the waves in ice instruments

A few high level rules:

- protect metal with duct tape, to avoid any possibility for short circuit if things move around
- wait to fix things with strong glue like epoxy until everything is tested; until everything is tested, fix things with duct tape, this is strong enough, and add in addition to the duct tape a bit of epoxy fixation after testing
- XXTODO: power supply to electronics, never 2 sources at the same time

## Preparation of the power supply, part 1: making the 3 battery holders in parallel ready

XXTODO: add hand written schematic

This describes the case when 3 D-cell batteries are used inside some battery holders. If you use some batteries with soldering tags, you can drop the battery holders and solder the wires directly on the soldering tags.

- tape 3 D-cell battery holders in the same direction (to be used in parallel)
- some D-cell battery holders have bad contacts; put 1 cell in each battery holder, and check that the voldate is transmitted to the solder pins; if not, fix with some solder; when done, remove the cells (avoid having any form of batteries or power supply connected when building / soldering things)
- connect 3 D-cell battery holders in parallel (follow usual conventions: + is red, - / ground is black)
- add 1 red (middle long) and 1 black (long) wire coming out of the 3 batteries assembly
- fix the battery holders at the bottom of the box with some duct tape (by making a "duct tape loop")

At this point, things should look like on the image below:

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/illustration_3_holders.jpg" width="400" />

## Preparation of the power supply, part 2: mounting the magnetic switch, checking its good functioning, connecting to the pololy 3.3V power regulator

XXTODO: add hand written schematic

This describes how to set up a magnetic (also known as reed) switch. If you know that you can put / remove the batteries until the deployment time, this can be skipped and you can just connect the battery wires directly to the 3.3V power regulator. Be **careful**, the reed switches are very easy to break. Make sure to always i) hold a switch leg close to the glass, ii) bend the same leg from further away on the leg, i.e., make sure that all the stress is only on the metal, and never gets applied to the metal package.

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
- connect the pololu 3.3V power regulator: the wire coming from the reed switch goes to pololu Vin (unregulated battery power in); connect a small black wire from the pololu GND to the GND wire from the battery, keeping enough metal wire to the open air to be able to solder an additional GND wire that will power the Artemis; connect a red wire from the Vout, that will be used to power the Artemis.

At this point, things should look like on the image (just note that the black wire from the battery is not yet connected to the small black wire from the pololu):

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/pololu_reed_switch.jpg" width="400" />

- wrap the pololu with duct tape to avoid naked metal
- test that all works well: put a battery in one of the holders, and check that: i) if there is no magnet, 3.3V is obtained between the red and the black wires coming out of the pololu, ii) if there is a magnet, the voltage between the 2 wires is 0V (in practise, due to the capacitors on the regulator, it will be a small, dropping voltage that converges to 0V if you wait enough).

This confirms that you have working power supply: the 3 batteries in parallel are being regulated to 3.3V and switched on / off by the reed switch

## Plastic plate with mounting holes

- prepare plastic plate with mounting holes

## Preparation of the electronics boards, part I: Artemis global tracker main part

- program the Artemis
- cut 1 LED pad Artemis
- solder power in
- connect antenna
- prepare plastic plate with mounting holes

## Preparing of the electronics, part II: wave measurement hardware

- cut 2 LED pads qwiic switch
- connect Artemis to qwiic switch
- connect qwiic switch to 9dof
- put in position on the plastic plate

## Final test and assembly

- register on Rock7
- perform a full test outside
  - notes about the colors of the LEDs
- check receive the messages and can decode
- long enough to check all fine, messages are buffered etc: 1 day
- glue inside
- glue outside

