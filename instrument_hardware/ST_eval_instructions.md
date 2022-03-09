The STEVAL-MKI207V1 contains the exact same 6dof (ISM330DHCX accelerometer + gyroscope) chip that is used on the Adafruit 9dof breakout. A few connections are necessary to use it:

- cut one connector end of a qwiic cable
- cut some prototyping PCB to the good size so that you can solder the breakout on it and connect the necessary pins
- connect qwiic red (3.3V) to the following pins of the eval breakout: VDD, VDDIO, SCX (the one on the side of VDD and VDDIO, see note under), SDX (the one on  the side of VDD and VDDIO, see note under), CS
- connect qwiic black (GND) to the following pins: GND, SDO
- connect qwiic blue (SDA) to: SDA
- connect qwiic yellow (SCL) to: SCL
- protect the rear part of the prototype PCB with duct tape (no naked metal policy)

This should correspond to the following wiring (using the correct SCx and SDx, see the note!!) (credits and many thanks to @gauteh for providing the wiring schematic from a KiCAD model :) ) (NB: to cut on assembly time, we do not put the resistor R1, we just feed to 3.3V; this seems to work well enough, but this is not what the datasheet says, so if you design a PCB in the future, add it!):

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/ISM330DHCX_wireup.png" width="400" />

Things should look like the following:

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/STEval_PCB.jpg" width="400" />

The qwiic cable coming out of the module can be connected to the qwiic switch OUT port instead of the default Adafruit 9dof breakout. Use a firmware version without the magnetometer.

## NOTE

The STEval breakout pinout is a bit confusing: there are 2 pins marked SCx and 2 pins marked SDx. However, it seems that these are NOT equivalent. 1 SCx is connected to the chip through a resistor (the one on the VDD / VDDIO side), while the other one (on the SCL / SDA side) is "connected" through an air jump, i.e. not connected, to the chip. Same for the SDx. So, **always connect the 3.3V to the SCx and SDx on the side of the VDD and VDDIO, never on the SCx and SDx on the side of the SCL / SDA**. For more information, link to discussions and datasheets, see: https://github.com/jerabaul29/OpenMetBuoy-v2021a/issues/25 . This corresponds to the wiring shown in the picture (though it is a bit hard to see, open the picture and zoom on it :) ).
