#ifndef INTERRUPT_UTILS_H
#define INTERRUPT_UTILS_H

#include "Arduino.h"

// a couple of functions to disable interrupts and restore interrupt status to what it was
// before the disabling; 

// after calling this function, we are sure that interrupts are disabled
// returns 1 if interrupts were previously disabled, 0 otherwise.
uint32_t disable_interrupts(void);

// provided the return value of disable_interrupts, set the interrupts
// back in their initial state; if was disabled before, do not re enable;
// if was enabled before, re-enable.
void reverse_disable_interrupts(uint32_t disable_result);

#endif
