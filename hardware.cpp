#include "pod.h"

const uint8_t PIN_BL = 24;				//GPIO 24 is PWM4A

const uint8_t PIN_SPDIF = 29;

volatile uint8_t blLevel = 0;
volatile uint8_t blGoal = 0;
uint8_t blDefault = 255;
int8_t blInc = 0;
volatile bool blChanging = false;

const uint8_t BL_TIMER_INC = 10;
repeating_timer blTimer;

void hw_init(){
	hw_initBacklight();
}

// init the PWM for the display backlight.
void hw_initBacklight(){
	// attach PWM to the led GPIO.
	gpio_set_function(PIN_BL, GPIO_FUNC_PWM);
	// Find the slice (which timer) and channel for this GPIO
	uint8_t slice = pwm_gpio_to_slice_num(PIN_BL);
	uint8_t channel = pwm_gpio_to_channel(PIN_BL);
	// set the wrap (top) value for the counter
	pwm_set_wrap(slice, 255);
	// set the compare level (duty cycle value)
	pwm_set_chan_level(slice, channel, blLevel);
	// finally start the timer
	pwm_set_enabled(slice, true);

	hw_setBacklight(blDefault);
}

void hw_initHPAmp(){
	// implement the digital reosthat control
}

void hw_setBacklight(uint8_t value){
	blGoal = value;
	if(blGoal > blLevel) blInc = 1;
	else if(blGoal < blLevel) blInc = -1;
	else return;

	if(blChanging) return;
	blChanging = true;
	add_repeating_timer_ms(BL_TIMER_INC, _hw_backlightCB, NULL, &blTimer);
}

bool _hw_backlightCB(repeating_timer *timer){
	if(blGoal != blLevel){
		blLevel += blInc;
		pwm_set_gpio_level(PIN_BL, blLevel);
		return true;
	} else {
		blChanging = false;
		blInc = 0;
//		cancel_repeating_timer(timer);
		return false;
	}

}
