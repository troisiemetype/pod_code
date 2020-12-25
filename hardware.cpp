#include "esPod.h"

const uint8_t TFT_LED = 26;
const int8_t TFT_LED_CH = 0;
const int16_t TFT_LED_FREQ = 5000;
const int8_t TFT_LED_RES = 9;

hw_timer_t *dimingTimer = NULL;
portMUX_TYPE dimingTimerMux = portMUX_INITIALIZER_UNLOCKED;
TaskHandle_t *taskHandleDim = NULL;

uint8_t blLevel = 0;
uint8_t blGoal = 0;
uint8_t blDefault = 255;
bool blChanging = false;

const uint8_t LED_CH = 0;

uint32_t taskStackSize = 0;

void hw_init(){
	hw_initBacklight();
}

void hw_initBacklight(){
	ledcSetup(TFT_LED_CH, TFT_LED_FREQ, TFT_LED_RES);
	ledcAttachPin(TFT_LED, TFT_LED_CH);
	ledcWrite(TFT_LED_CH, 0);
}

void hw_startDelayDiming(){
	dimingTimer = timerBegin(1, 80, true);
	timerAttachInterrupt(dimingTimer, _hw_dimBacklight, true);
	timerAlarmWrite(dimingTimer, 5000000, false);
	timerAlarmEnable(dimingTimer);
}

void hw_setBacklight(uint8_t value = blDefault){
	if(blChanging) return;
	blChanging = true;
	blGoal = value;
	xTaskCreate(_hw_taskBacklight, "backlight", 1024, NULL, 1, taskHandleDim);
}

void _hw_taskBacklight(void *param){
	int8_t inc = 0;
	uint8_t value = blLevel;
	if(blGoal > blLevel) inc = 1;
	else if(blGoal < blLevel) inc = -1;
//	Serial.printf("backlight, actual : %i, goal : %i, increment : %i\n", blLevel, blGoal, inc);
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
		vTaskDelay(3 / portTICK_PERIOD_MS);
	}
}

void _hw_dimBacklight(){
	Serial.println("lowering");
//	Serial.printf("dim task stack size : %i\n", uxTaskGetStackHighWaterMark(taskHandleDim));
	hw_setBacklight(8);
}

