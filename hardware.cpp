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

alarm_id_t dimAlarm = -1;
bool isDimmed = false;

uint8_t volume = 0;

void hw_init(){
	console.println("initializing hardware");
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
	console.println("  backlight");
}

void hw_initHPAmp(){
	// implement the digital reosthat control
	console.println("  headphone amp");
}

// Launch a repeating timer for changing backlight
// On each fire of the callback function associated, we will change the PWM duty cycle of the led backlight.
void hw_setBacklight(uint8_t value){
	blGoal = value;
	if(blGoal > blLevel) blInc = 1;
	else if(blGoal < blLevel) blInc = -1;
	else return;

	if(blChanging) return;
	blChanging = true;
	add_repeating_timer_ms(BL_TIMER_INC, _hw_backlightCB, NULL, &blTimer);
}

// Callback for changing the dutycycle of backlight.
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

// Start a timeout for display diming after 
void hw_setDimTimeout(){
	// to cancel : 
//	cancelAlarm(dimAlarm);
	// delay, callback, data to pass, shall the delay be already elapsed : execute the callback
	dimAlarm = add_alarm_in_ms(30000, _hw_dimCB, NULL, false);
}

int64_t _hw_dimCB(int32_t value, void *data){
	hw_setBacklight(8);
	isDimmed = true;
	dimAlarm = -1;
	return value;
}


void hw_setVolume(uint8_t vol){
	if(vol >= 64) vol = 63;
	volume = vol;
//	hpAmp.volume(volume);
}

uint8_t hw_getVolume(){
	return volume;
}

uint8_t hw_volumeUp(){
	if(volume < 63){
		volume += 1;
	}
//	hpAmp.volume(volume);
	return volume;
}

uint8_t hw_volumeDown(){
	if(volume > 0){
		volume -= 1;
	}
//	hpAmp.volume(volume);
	return volume;
}
