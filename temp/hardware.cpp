#include "pod.h"

// HP amplifier is at adress 96 / 0x60. It's set in the class.

#define BAT_FULL_TENSION				4.1
#define BAT_EMPTY_TENSION				3.0
#define BAT_VOLTAGE_DIVIDER				0.7826			// That's 100k & 360k
#define BAT_TENSION_TO_STEP				(BAT_VOLTAGE_DIVIDER * 4096 / 3.3)
#define BAT_READ_HIGH					(BAT_FULL_TENSION * BAT_TENSION_TO_STEP)
#define BAT_READ_LOW					(BAT_EMPTY_TENSION * BAT_TENSION_TO_STEP)
#define BAT_READ_DELTA					(BAT_READ_HIGH - BAT_READ_LOW)

const uint8_t TFT_LED = 24;				//GPIO 24 is PWM4A
const int8_t TFT_LED_CH = 0;
const int16_t TFT_LED_FREQ = 5000;
const int8_t TFT_LED_RES = 9;

const uint8_t SPDIF_LED = 29;

volatile uint8_t blLevel = 0;
volatile uint8_t blGoal = 0;
uint8_t blDefault = 255;
int8_t blInc = 0;
volatile bool blChanging = false;

const uint8_t BL_TIMER_INC = 5;

repeating_timer blTimer;

TaskHandle_t *dimHandle;



const uint8_t PIN_BAT = 26;				//ADC0
// const uint8_t PIN_BAT_STAT = 36;		//not accessible, but would be on a custom RP2040 implemntation.
const uint8_t PIN_VUSB = 25;

bool batStat = false;
int32_t batRaw = 0;
float batLevel = 0;

uint32_t playerUpdateTime = 1000;
uint32_t volumeUpdateTime = 5000;

uint8_t volume = 0;

// What if we init a 10ms timer that is always running, from which periodic functions (diming, display time) are called ?
void hw_init(){
/*
	pinMode(SPDIF_LED, OUTPUT);

	for(uint8_t i = 0; i < 16; ++i){
		digitalWrite(SPDIF_LED, i % 2);
		delay(200);
	}
*/
//	hw_initBacklight();

//	hw_initHPAmp();

//	xTaskCreate(_hw_taskUpdateBattery, "update battery", 1024, NULL, 1, NULL);

}

void hw_update(){

}

// init the PWM for the display backlight.
void hw_initBacklight(){
	// attach PWM to the led GPIO.
	gpio_set_function(TFT_LED, GPIO_FUNC_PWM);
	// Find the slice (which timer) and channel for this GPIO
	uint8_t slice = pwm_gpio_to_slice_num(TFT_LED);
	uint8_t channel = pwm_gpio_to_channel(TFT_LED);
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

//	todo : change this delay to user-set one.
void hw_startDelayDiming(){
/*
	timerRestart(sleepTimer);
	timerAttachInterrupt(sleepTimer, _hw_intDimBacklight, true);
	timerAlarmWrite(sleepTimer, 5000000, false);
	timerAlarmEnable(sleepTimer);
*/
}

void hw_setBacklight(uint8_t value = blDefault){
//	pwm_set_gpio_level(TFT_LED, value);
/*
	if(blChanging) return;
	blChanging = true;
	blGoal = value;
	xTaskCreate(_hw_taskBacklight, "backlight", 512, NULL, 1, dimHandle);
*/	

	blGoal = value;
	if(blGoal > blLevel) blInc = 1;
	else if(blGoal < blLevel) blInc = -1;
	else blInc = 0;

	if(blChanging) return;
	blChanging = true;
	add_repeating_timer_ms(BL_TIMER_INC, _hw_backlightCB, NULL, &blTimer);

/*
	hardware_alarm_claim(0);
	hardware_alarm_set_callback(0, _hw_backlightCB);
	hardware_alarm_set_target(0, make_timeout_time_ms(BL_TIMER_INC));
*/
}

bool _hw_backlightCB(repeating_timer *timer){
//void _hw_backlightCB(uint timer){

//	if(1){
	if(blGoal != blLevel){
		blLevel += blInc;
		pwm_set_gpio_level(TFT_LED, blLevel);
//		hardware_alarm_set_target(timer, make_timeout_time_ms(BL_TIMER_INC));

		return true;
	} else {
//		blLevel = 0;
//		blInc = 0;
//		blChanging = false;
		cancel_repeating_timer(timer);
//		hardware_alarm_set_callback(timer, NULL);
//		hardware_alarm_unclaim(timer);
		return false;
	}
}

void _hw_taskBacklight(void *param){
	int8_t inc = 0;
	uint8_t value = blLevel;
	if(blGoal > blLevel) inc = 1;
	else if(blGoal < blLevel) inc = -1;
//	log_d("backlight, actual : %i, goal : %i, increment : %i\n", blLevel, blGoal, inc);
	for(;;){
		value += inc;
		pwm_set_gpio_level(TFT_LED, value);
		if(value == blGoal){
			blLevel = value;
			blChanging = false;
//			taskStackSize = uxTaskGetStackHighWaterMark(NULL)
//			log_d("backlight leveled, killing dim task");
//			vTaskDelete(NULL);
			break;
		}
//		vTaskDelay(5 / portTICK_PERIOD_MS);
//		vTaskDelay(pdMS_TO_TICKS(5));
	}
}

void _hw_intDimBacklight(){
//	Serial.println("lowering");
//	Serial.printf("dim task stack size : %i\n", uxTaskGetStackHighWaterMark(taskHandleDim));
	hw_setBacklight(8);
/*
	if(audio_getState() == AUDIO_IDLE){
//		hw_startDelaySleep();
	}
*/
}

//	Change this delay to a user-set one.
void hw_startDelaySleep(){
//	timerAttachInterrupt(sleepTimer, _hw_intSystemSleep, true);
	// Here is a 10 minutes delay before sleeping.
//	timerAlarmWrite(sleepTimer, 600000000, false);
	// 30 seconds for testing.
//	timerAlarmWrite(sleepTimer, 30000000, false);
//	timerAlarmEnable(sleepTimer);
}

void _hw_intSystemSleep(){

}

void _hw_taskUpdateBattery(void *param){
	// measures : bat 3.3V -> analog input 2.55V -> batLevel == 2890
/*	The battery voltage is measured trough a voltage divider with resistances of value 100k & 360k
 *	The analog input tension should thus be
 *	3.28V / 4079 when battery is fully charged at 4.2V,
 *	
 *	2.82V / 3496 when battery is decharged at 3.6V
 */

	const int8_t filterRatio = 30;
	batRaw = analogRead(PIN_BAT);

	for(;;){
//		batStat = digitalRead(PIN_BAT_STAT);

		int32_t read = analogRead(PIN_BAT);
		batRaw = read * filterRatio + batRaw * (0xff - filterRatio);
		batRaw /= 0xff;
		batLevel = batRaw - BAT_READ_LOW;
		batLevel /= BAT_READ_DELTA;
//		Serial.printf("battery level : %f\n", batLevel);
//		display_pushBattery(batLevel);
//		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
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

