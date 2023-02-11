#include "esPod.h"

// HP amplifier is at adress 96 / 0x60

#define BAT_FULL_TENSION				4.1
#define BAT_EMPTY_TENSION				3.0
#define BAT_VOLTAGE_DIVIDER				0.7826			// That's 100k & 360k
#define BAT_TENSION_TO_STEP				(BAT_VOLTAGE_DIVIDER * 4096 / 3.3)
#define BAT_READ_HIGH					(BAT_FULL_TENSION * BAT_TENSION_TO_STEP)
#define BAT_READ_LOW					(BAT_EMPTY_TENSION * BAT_TENSION_TO_STEP)
#define BAT_READ_DELTA					(BAT_READ_HIGH - BAT_READ_LOW)

const uint8_t TFT_LED = 26;
const int8_t TFT_LED_CH = 0;
const int16_t TFT_LED_FREQ = 5000;
const int8_t TFT_LED_RES = 9;

hw_timer_t *sleepTimer = NULL;
portMUX_TYPE sleepTimerMux = portMUX_INITIALIZER_UNLOCKED;
TaskHandle_t *taskHandleDim = NULL;

uint8_t blLevel = 0;
uint8_t blGoal = 0;
uint8_t blDefault = 255;
bool blChanging = false;

const uint8_t LED_CH = 0;

uint32_t taskStackSize = 0;

const uint8_t PIN_BAT = 34;
const uint8_t PIN_BAT_STAT = 36;

bool batStat = false;
int32_t batRaw = 0;
float batLevel = 0;

// hw_timer_t *hwTimer = NULL;

//volatile hwTimerFlags_t hwTimer = 0;

uint32_t playerUpdateTime = 1000;
uint32_t volumeUpdateTime = 5000;

uint8_t volume = 0;


void hw_init(){
	hw_initBacklight();

	sleepTimer = timerBegin(1, 80, true);
	timerStop(sleepTimer);

	xTaskCreate(_hw_taskUpdateBattery, "update battery", 1024, NULL, 1, NULL);
/*
	hwTimer = timerBegin(0, 80, true);
	timerAttachInterrupt(hwTimer, _hw_timerInt, true);
	timerAlarmWrite(hwTimer, 1000, true);
*/
}

void hw_update(){

}

void hw_initBacklight(){
	ledcSetup(TFT_LED_CH, TFT_LED_FREQ, TFT_LED_RES);
	ledcAttachPin(TFT_LED, TFT_LED_CH);
	ledcWrite(TFT_LED_CH, 0);
}

void hw_initHPAmp(){

}

//	todo : change this delay to user-set one.
void hw_startDelayDiming(){
//	sleepTimer = timerBegin(1, 80, true);
	timerRestart(sleepTimer);
	timerAttachInterrupt(sleepTimer, _hw_intDimBacklight, true);
	timerAlarmWrite(sleepTimer, 5000000, false);
	timerAlarmEnable(sleepTimer);
}

//	Todo : enable changing backlight when there's already a change running, to avoid getting dimer when user clicks during a diming.
void hw_setBacklight(uint8_t value = blDefault){
//	Serial.printf("task handle : %i\n", (int)taskHandleDim);
/*	if(taskHandleDim != NULL){
		vTaskDelete(taskHandleDim);
	}
*/	if(blChanging) return;
	blChanging = true;
	blGoal = value;
	xTaskCreate(_hw_taskBacklight, "backlight", 8024, NULL, 1, taskHandleDim);
}

void _hw_taskBacklight(void *param){
	int8_t inc = 0;
	uint8_t value = blLevel;
	if(blGoal > blLevel) inc = 1;
	else if(blGoal < blLevel) inc = -1;
	Serial.printf("backlight, actual : %i, goal : %i, increment : %i\n", blLevel, blGoal, inc);
	for(;;){
		value += inc;
//		Serial.printf("%i : level : %i\n", xTaskGetTickCount(), value);
		ledcWrite(LED_CH, value);
		if(value == blGoal){
			blLevel = value;
			blChanging = false;
//			taskStackSize = uxTaskGetStackHighWaterMark(NULL)
			vTaskDelete(NULL);
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

void _hw_intDimBacklight(){
//	Serial.println("lowering");
//	Serial.printf("dim task stack size : %i\n", uxTaskGetStackHighWaterMark(taskHandleDim));
	hw_setBacklight(8);
	if(audio_getState() == AUDIO_IDLE){
//		hw_startDelaySleep();
	}
}

//	Change this delay to a user-set one.
void hw_startDelaySleep(){
	timerAttachInterrupt(sleepTimer, _hw_intSystemSleep, true);
	// Here is a 10 minutes delay before sleeping.
//	timerAlarmWrite(sleepTimer, 600000000, false);
	// 30 seconds for testing.
	timerAlarmWrite(sleepTimer, 30000000, false);
	timerAlarmEnable(sleepTimer);
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
		batStat = digitalRead(PIN_BAT_STAT);

		int32_t read = analogRead(PIN_BAT);
		batRaw = read * filterRatio + batRaw * (0xff - filterRatio);
		batRaw /= 0xff;
		batLevel = batRaw - BAT_READ_LOW;
		batLevel /= BAT_READ_DELTA;
//		Serial.printf("battery level : %f\n", batLevel);
		display_pushBattery(batLevel);
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}

void hw_setVolume(uint8_t vol){
	if(vol >= 64) vol = 63;
	volume = vol;
}

uint8_t hw_getVolume(){
	return volume;
}

uint8_t hw_volumeUp(){
	if(volume < 63){
		volume += 1;
	}
	return volume;
}

uint8_t hw_volumeDown(){
	if(volume > 0){
		volume -= 1;
	}
	return volume;
}

