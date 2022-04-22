List of components needed to build the instrument. Note that additional functionalities could be used. For example, it would be possible to add some pressure / temperature / humidity / wind sensors, or to switch to a solar panel and rechargeable battery power source. Note that alternative providers may be available if the default provider is sold out (check, for example, Mouser, Digikey, RS-Online, Elfa Distrelec, Farnell, Adafruit, Alibaba, etc). Also note that prices can be significantly reduced if buying larger volumes, and that shipping, toll, and taxes may apply in addition. Note that other regional providers may be better / faster / cheaper if you live in another part of the world than I do.

### Main board

- 1 x Artemis Global Tracker: 400USD: https://www.sparkfun.com/products/18712

### Extension cable

- 1 x SMA Female to SMA Male 25cm: 5.5USD : https://www.sparkfun.com/products/12861

### Antenna

- 1 x Iridium / GPS / Glonass passive antenna: 65USD: https://www.sparkfun.com/products/16838

### Box

- 1 x RND 455-01082 - Plastic Enclosure 120x120x90mm Polycarbonate: 15USD: https://www.elfadistrelec.no/en/plastic-enclosure-120x120x90mm-light-grey-polycarbonate-ip67-rnd-components-rnd-455-01082/p/30129934

or

- 1 x Hammond 1554P2GYCL - Watertight Enclosure Smoked Lid, Polycarbonate, 120x120x80mm: 23USD: https://www.elfadistrelec.no/en/watertight-enclosure-smoked-lid-polycarbonate-120x120x80mm-clear-light-grey-hammond-1554p2gycl/p/30100709?q=&pos=2&origPos=52&origPageSize=50&track=true

or any other box you like (size may be adapted to number of battery cells, additional functionality that requires additional small PCBs, etc).

### Non rechargable batteries

I recommend trying to get batteries with solder tags, and drop the battery holders alltogether, but it is not always easy to find batteries with solder tags for a good price:

- 3 x Saft LSH20 3.6V batteries: 45USD:  https://eu.nkon.nl/saft-lsh-20-lithium-battery-3-6v.html

or

- 3 x Tadiran SL-2780 / D - 3.6V: 45USD: https://eu.nkon.nl/disposable-batteries/tadiran-sl-2780-sl-780-d-lithium-battery-3-6v.html

### Battery holders

Any D-cell battery holder will do. Careful, some cheap ones are of bad quality and will create bad contacts. I recommend using batteries with soldering tags if possible, and drop the battery holders altogether:

- 3 x BX0037 D-cell battery holder: 10USD: https://no.farnell.com/bulgin/bx0037/holder-battery-d-cell/dp/301097 (these have been of consistent good quality in my experience)

### Magnetic switch

Any magnetic switch that of the same family that either is "normally closed" or has "3 legs" will do.

- 1 x MDRR-DT-25-30-F magnetic reed switch: 5USD: https://no.mouser.com/ProductDetail/Littelfuse/MDRR-DT-25-30-F?qs=nyo4TFax6New9CGWZwJeTA%3D%3D

### Magnet

- 1 x Magnet Square - 0.25": 2USD: https://www.sparkfun.com/products/8643

### Power regulator

- 1 x Pololu 3.3V Step-Up/Step-Down Voltage Regulator S7V8F3: 10USD: https://www.pololu.com/product/2122

### Qwiic cables

- 2 x Flexible Qwiic Cable - 100mm: 3USD: https://www.sparkfun.com/products/17259

### Qwiic switch

- 1 x Qwiic Power Switch: 7USD: https://www.sparkfun.com/products/16740

### 9-dof sensor

This part has a tendency to be sold out. At present, we only use the accelerometer and gyroscope data (6dof), i.e. the ISM330DHCX part, out of the accelerometer and gyroscope and magnetometer that are present on the default Adafruit breakout. The reference part is the Adafruit FeatherWing. I list some alternatives without the magnetometer (which is currently not in use) under:

- 1 x Adafruit ISM330DHCX + LIS3MDL FeatherWing - High Precision 9-DoF IMU: 22USD: https://www.adafruit.com/product/4569 (this is the default part; though it uses only the ISM330DHCX, the default firmware checks that the LIS3MDL is present, so using default firmware without the LIS3MDL will not work).

In case the part above is sold out, you can buy one of the alternatives, that is only 6-dof (no magnetometer). For now, since the magnetometer is not in use, this does not affect functionality, but you will need to use a special firmware for this to work (the "STEval_" firmware versions):

- 1 x Adafruit ISM330DHCX - 6 DoF IMU - Accelerometer and Gyroscope - STEMMA QT / Qwiic: 20USD: https://www.adafruit.com/product/4502
- 1 x STEVAL-MKI207V1 - STMicroelectronics ISM330DHCX Adapter Board: 20USD: https://no.rs-online.com/web/p/sensor-development-tools/1961491
- any other breakout or board that has an ISM330DHCX build in

In adition, it is possible to use some other 6dof models; one such model is the LSM6DSOX for example. Though it has lower accuracy than the ISM330DHCX, it is still fine to use for large waves (typically, in the ocean). To use LSM6DOSX, you need another custom flavor of the firmware (see the "STEval_lsm6DSOX" firmware versions). Any breakout featuring the LSM6DSOX chip will then be compatible with the corresponding firwmare flavor.

### Temperature sensors

- 1 x DS18B20 waterproof: 10USD: https://www.sparkfun.com/products/11050

### Small extras

You will also need the usual "small extras" when doing electronics and assembling instruments that should go into the water:

- electronics cables, typically some 20-22 AWG cables in a variety of colors do just fine for connections that do not carry much current: https://uk.rs-online.com/web/p/hook-up-wire/8724530 . For carrying more current (main power supply, etc), you can consider using some slightly thicker wires, for example 16-18AWG such as: https://www.elfadistrelec.no/en/stranded-wire-pvc-8mm-tinned-copper-black-3055-30-5m-alpha-wire-3055-bk005/p/30083071?trackQuery=18+awg+wire&pos=31&origPos=40&origPageSize=50&track=true , though these are harder to cut, solder, and handle, and the current limitations implemented in the AGT (no more than typically 150-250mA peak are used when the iridium modem is in use) mean that 20 AWG is good enough also for the power supply part. If you switch to a board using more current at peak, i.e. typically over 250mA , thicker cables (ie lower AWG) are then needed to avoid voltage drops.
- soldering tools: soldering iron (I recommend the Pinecil https://pine64.com/product/pinecil-smart-mini-portable-soldering-iron/ ), soldering wire, soldering brass sponge, ...
- plastic plate to fix the electronics: something like this works fine, 2mm thickness is enough, up to (included) 4 is fine, but more than 4 is a bit hard to cut: https://www.biltema.no/bygg/platematerialer/plastglass/plastglass-2000030030 plastglass.
- desiccant bags
- duct tape
- epoxy glue
- bathroom silicon
- plastic "pop / snap" rivets: I recommend the ones with diameter 3mm, length 9mm, though may be possible to do with a bit different rivets: https://no.rs-online.com/web/p/rivets/0280824 Richco Nylon Snap Rivet, 3mm diameter, 9mm length
- various tools (cutter, drill, etc)

