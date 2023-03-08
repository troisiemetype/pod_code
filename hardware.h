#ifndef POD_HARDWARE_H
#define POD_HARDWARE_H

#include "pod.h"

#include "hardware/pwm.h"
#include "hardware/timer.h"
#include "hardware/irq.h"

void hw_init();

void hw_initBacklight();

void hw_initHPAmp();

void hw_setBacklight(uint8_t value);
bool _hw_backlightCB(repeating_timer *timer);


#endif