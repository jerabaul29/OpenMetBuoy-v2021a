The STEVAL-MKI207V1 contains the exact same 6dof (ISM330DHCX accelerometer + gyroscope) chip that is used on the Adafruit 9dof breakout. A few connections are necessary to use it:

- cut one connector end of a qwiic cable
- cut some prototyping PCB to the good size so that you can solder the breakout on it and connect the necessary pins
- connect qwiic red (3.3V) to the following pins of the eval breakout: VDD, VDDIO, SCX, SDX, CS
- connect qwiic black (GND) to the following pins: GND, SDO
- connect qwiic blue (SDA) to: SDA
- connect qwiic yellow (SCL) to: SCL
- protect the rear part of the prototype PCB with duct tape (no naked metal policy)

Things should look like the following:

<img src="https://github.com/jerabaul29/OpenMetBuoy-v2021a/blob/main/instrument_hardware/jpg/STEval_PCB.jpg" width="400" />

The qwiic cable coming out of the module can be connected to the qwiic switch OUT port instead of the default Adafruit 9dof breakout. Use a firmware version without the magnetometer.
