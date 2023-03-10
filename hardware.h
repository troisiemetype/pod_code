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

void hw_setDimTimeout();
int64_t _hw_dimCB(int32_t value, void* data);

void hw_setVolume(uint8_t vol);
uint8_t hw_getVolume();
uint8_t hw_volumeUp();
uint8_t hw_volumeDown();


class ExternalInput{
public:
	enum I2C_REG{
		
	}

protected:

};


#endif