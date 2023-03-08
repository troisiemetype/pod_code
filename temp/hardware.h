#ifndef POD_HARDAWARE_H
#define POD_HARDAWARE_H

#include "pod.h"

#include "hardware/pwm.h"
//#include "hardware/time.h"
#include "hardware/timer.h"
#include "hardware/irq.h"
#include <FreeRTOS.h>
#include <task.h>

/*
 *	Here is where the global hardware is handled.
 *		screen backlight control
 *		battery state
 *		headphones amplifier control
 */

/*
 *	Note on timer use : several functions need to be called in a regular way. But not all these functions need to be precise.
 *	We can sort these function by the precision needed :
 *
 *	precise timing :
 *		Audio counter for time of playing.
 *
 *	less prices timing :
 *		Battery polling.
 *		Smooth backlight diming.
 *		Last user interaction, for dimed backlight.
 *		Last user unteraction, for putting the system to light / deep sleep if not playing.
 *
 *	There are several ways to achieve a timing, which are :
 *		Use a hardware timer.
 *			Pros :
 *				Really precise
 *				Timer can be started or stopped, fire once or loop, re-init, delay can be changed, isr function can be changed as well.
 *			Cons :
 *				Only four available
 *		Use a task (thread).
 *			Pro :
 *				quite precise too, unless another task with greater priority takes over.
 *				has its own life, nothing to take care of once it's created.
 *				Tasks can use queues and receive custom parameters.
 *			Cons :
 *				Variable access need to be cared of, as a tack can interrupt the normal program flow.
 *				Usefull for a recurring task (even ponctually), but heavy for a one of, particularly if it has to be re-init.
 *		Use a timer library based on millis();
 *			Pro :
 *				ease of use : set a timer with given delay, then start it one shot or looping.
 *			Cons :
 *				Needs to be polled in the main loop.
 *		Use Ticker class !!
 */

/*
 *	Based on this, here is how the different needs are implemented :
 *
 *	Audio counter for elapsed time of play:
 *		Timer 0
 *
 *	Battery poling :
 *		task
 *
 *	Smooth backlight diming :
 *		task
 *
 *	Last user interaction :
 *		task (one for low backlight and one for putting the system to sleep)
 *		This one could use a hardware timer, for ease of use. With a first delay for backlight diming, and then a second one for system sleep.
 */

void hw_init();
void hw_update();

void hw_initBacklight();

void hw_initHPAmp();

void hw_startDelayDiming();
void hw_setBacklight(uint8_t value);
bool _hw_backlightCB(repeating_timer *timer);
//void _hw_backlightCB(uint timer);
void _hw_taskBacklight(void *param);
void _hw_intDimBacklight();

void hw_startDelaySleep();
void _hw_intSystemSleep();

void _hw_taskUpdateBattery(void *param);

void hw_setVolume(uint8_t vol);
uint8_t hw_getVolume();
uint8_t hw_volumeUp();
uint8_t hw_volumeDown();

#endif