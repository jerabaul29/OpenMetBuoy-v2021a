// TODO: fill

//////////////////////////////////////////////////////////////////////////////////////////
// keeping track of what is on and when

// current implementation ideas

// TODO: move to a specific class

// For now we use a very simple system: use a set of unsigned short to keep track of which
// peripherals are on or not. If a peripheral_enabled entry is 0, the corresponding
// peripheral is not in use; if it is superior to 0, it is in use by as many "things" as
// the value.

// If programming for a multi threaded or multi cpu program / mcu, this should be atomic;
// for now, interrupts are our only source of concurrency, so it will be enough to have
// simple quantities; still, put an atomic on it, just to be on the safe side in the
// future.

// for now: start all I2Cs, Serials, SPIs etc when waking up, and stop all of these when
// done and ready to sleep... This way, no questions and no problems. Likely too many peripherals
// will be on, but this has quite limited consumption anyways... Another solution would be to
// turn on only what is needed when it is needed, but this is complicated to get right when sharing
// buses for several components for example... A possibility may be to derive from serial / i2c
// and count active peripherals, and use this to decide when to actually switch on / off.

// need to:
// switch_on_if_needed
// switch_off_if_needed
// force_switch_off
// reset
// have access to underlying raw interface
