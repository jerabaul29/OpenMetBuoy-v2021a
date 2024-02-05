This is just an ugly, quick test bench code for checking a thermistor ID. Do not use for any deployment or the likes!

```
- the binary is available in the binaries folder
- to program, use the "Artemis module" target, even if you use a Red Board Artemis

- thermistor labeling test bench: description: need
-- SF RB Art
-- USB-C cable
-- breadboard
-- 3 jumpers RBW
-- 3 crocodie jumpers RBW
-- 3 resistors: 2x50, 1x4.7k
-- duct tape
-- waterproof marker

- wiring: (with AGT vs. RBA)
-- RBA GND - black jumper - black croco
-- RBA 3 / AGT D4 - red jumper - 50 - red croco
-- RBA 2 / AGT AD35 - white jumper - 50 - white croco
-- red croco - 4.7k - white croco

- thermistors labeling test bench: procedure
-- program RBA (even if using RBA, still use the "Sparkfun Module" as a compilation target with the present sketch)
-- start RBA
-- step *: connect: GND, SGN, PWR: same color as the thermistors
-- boot button press
-- note 6 bits ID
-- disconnect: PWR, SGN, GND, connect next thermistor (step *)

Notes:
    // we know that:
    // the first byte is the "kind", should be 0x28
    // the next 6 bytes are the serial number
    // the LS bytes are first; then end is still 0x00 0x00 (as production has not
    // gone so far) the last byte is the CRC so, use the 6 LSBs of the LS byte of
    // the serial number (these are the ones that "rotate" fastest)
    // i.e. ID: 28 5F D9 76 E 0 0 EC
    // has LSB of rolling ID 5F, which taking 6 LSBs and in decimal is 31

    // the pictures include a full test bench, including qwiic switch + 6dof, though these two latest are not in use
```

See the pics for more illustration of how to wire.
