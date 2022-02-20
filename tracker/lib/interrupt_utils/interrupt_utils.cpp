#include "interrupt_utils.h"

uint32_t disable_interrupts(void){
    // return 1 if interrupts were previously disabled, 0 otherwise.
    return am_hal_interrupt_master_disable();
}

void reverse_disable_interrupts(uint32_t disable_result){
    if (disable_interrupts == 0){
        am_hal_interrupt_master_enable();
    }
}
