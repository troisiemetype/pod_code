// #include "io.h"
#include "piPod.h"

const uint8_t NB_BUTTONS = 5;
/*
const uint8_t BTN_CENTER = 0;
const uint8_t BTN_UP = 34;
const uint8_t BTN_RIGHT = 36;
const uint8_t BTN_DOWN = 35;
const uint8_t BTN_LEFT = 39;
*/
const uint8_t WHEEL_0 = 4;
const uint8_t WHEEL_1 = 13;
const uint8_t WHEEL_2 = 12;
const uint8_t WHEEL_3 = 27;
const uint8_t WHEEL_4 = 32;
const uint8_t WHEEL_5 = 33;

const uint8_t BATTERY_LEVEL = 34;
const uint8_t BATTERY_STAT = 36; // SenVP

void (*_io_cbfnEnter)(void) = NULL;
void (*_io_cbfnUp)(void) = NULL;
void (*_io_cbfnDown)(void) = NULL;
void (*_io_cbfnRight)(void) = NULL;
void (*_io_cbfnLeft)(void) = NULL;
void (*_io_cbfnEnterLong)(void) = NULL;
void (*_io_cbfnUpLong)(void) = NULL;
void (*_io_cbfnDownLong)(void) = NULL;
void (*_io_cbfnRightLong)(void) = NULL;
void (*_io_cbfnLeftLong)(void) = NULL;

void (*_io_cbfnWheelClk)(void) = NULL;
void (*_io_cbfnWheelCClk)(void) = NULL;

PushButton button[NB_BUTTONS];
uint8_t buttonBit[NB_BUTTONS] = {0, 1, 2, 4, 3};

TouchWheel wheel = TouchWheel(WHEEL_0, WHEEL_1, WHEEL_2, WHEEL_3, WHEEL_4, WHEEL_5);

uint32_t _timerWheelUpdate = 0;
const uint16_t WHEEL_UPDATE = 2;

uint32_t cnt = 0;
/*
uint16_t batLevel = 0;
bool batStat = 0;
*/

uint16_t touch = 0;
uint16_t minTouch = 0;
uint16_t maxTouch = 0;

volatile bool ioInt = 0;

void io_init(){
	io_initIO();
	io_initButtons();
	io_initPortExpander();
//	__touchInint();
//	touchSetCycles(uint16_t measure, uint16_t sleep);
	// measure is the time spent measuring the capacitance of the pin, in 8MHz cycles
	// sleep is the time sleeping before starting measure ? "sleep cycles for timer"
	touchSetCycles(0x1000, 0x1000);

 	wheel.init();
	wheel.setSteps(16);
//	wheel.reverse();
//	for(uint8_t i = 0; i < 16; ++i)	touch = minTouch = maxTouch = touchRead(WHEEL_0);

//	sendData.attach(0.5, io_data);
}

void io_data(){
	Serial.printf("touch\t: %i\n", touch);
	Serial.printf("min touch : %i\n", minTouch);
	Serial.printf("max touch : %i\n\n", maxTouch);
}

void io_initIO(){
	// Enable pullups on SD lines
	// Change those pullups as SD is operated in one bit mode.
	pinMode(2, INPUT_PULLUP);
	pinMode(14, INPUT_PULLUP);
	pinMode(15, INPUT_PULLUP);
	// Pullup on TCA9534 INT pin.
	// No internal pullup on pins 36 and 39, so one is added on the board. (PCB next version)
	pinMode(39, INPUT);
}

void io_initPortExpander(){

}

void io_initButtons(){
/*
	for(uint8_t i = 0; i < NB_BUTTONS; ++i){
		button[i] = new PushButton();
	}
*/
	// Buttons are mapped like following on the IO expander :
	//		center 		0
	//		up 			1
	// 		down 		2
	// 		right 		4
	// 		left 		3
	// This happens when the front face is designed as back face of the pcb. :D

	button[0].begin(PULLUP);
	button[1].begin(PULLUP);
	button[2].begin(PULLUP);
	button[3].begin(PULLUP);
	button[4].begin(PULLUP);
}

void io_update(){
//	cnt = millis();
	io_updateButtons();
//	log_d("buttons update : %i", (millis() - cnt));
/*
	touch = touchRead(WHEEL_0);
	if(touch < minTouch) minTouch = touch;
	if(touch > maxTouch) maxTouch = touch;
*/
	// temp return for letting touch aside.
//	return;

	if(wheel.update()){
//		hw_setBacklight(192);
		hw_startDelayDiming();
		int8_t steps = wheel.getStep();
		if(steps > 0){
			_io_cbWheelClockwise();
		} else if(steps < 0){
			_io_cbWheelCounterClockwise();
		}

//		Serial.printf("position : %i\n", wheel.getPosition());
//		Serial.printf("step : %i\n", wheel.getStep());
	}
}

// On a button push, this function is 28ms long !! These are 1234 samples at 44100Hz !
// TODO : need to find a way to either buffer data, or speedup buttons !
void io_updateButtons(){
//	Serial.print("io :");
		ioInt = 0;
//		False int due to unknown reason !
		hw_setBacklight(192);
	}
	for(uint8_t i = 0; i < NB_BUTTONS; ++i){
//		state |= (button[i].update() << i);
//			hw_setBacklight(192);
			hw_startDelayDiming();
			if(button[i].isLongPressed()){
				switch(i){
					case 0:
						_io_cbEnterLong();
						break;
					case 1:
						_io_cbUpLong();
						break;
					case 2:
						_io_cbDownLong();
						break;
					case 3:
						_io_cbRightLong();
						break;
					case 4:
						_io_cbLeftLong();
						break;
				}
			} else if(button[i].justReleased()){
				switch(i){
					case 0:
						_io_cbEnter();
						break;
					case 1:
						_io_cbUp();
						break;
					case 2:
						_io_cbDown();
						break;
					case 3:
						_io_cbRight();
						break;
					case 4:
						_io_cbLeft();
						break;
				}
			}
		}
	}
//	Serial.println();
	return;
}

// called by an interrupt in main loop.
// Maybe the interrupt should be called from io_, or from hw_.
void io_updateReadButtons(){
	ioInt = 1;
}


void io_updateWheel(){

}

// To be moved to hardware
void io_updateBattery(){
//	batStat = digitalRead(BATTERY_STAT);
//	batLevel = analogRead(BATTERY_LEVEL);
/*
	if((millis() - cnt) > 1000){
		cnt = millis();
		if(++batLevel > 4095) batLevel = 0;
		display_pushBattery((float)batLevel / 4095);
	}
*/
}

void io_deattachAllCB(){
	io_deattachCBEnter();
	io_deattachCBUp();
	io_deattachCBDown();
	io_deattachCBRight();
	io_deattachCBLeft();
	io_deattachCBEnterLong();
	io_deattachCBUpLong();
	io_deattachCBDownLong();
	io_deattachCBRightLong();
	io_deattachCBLeftLong();
	io_deattachCBWheelClockwise();
	io_deattachCBWheelCounterClockwise();
}

void io_attachCBEnter(void (*fn)())			{_io_cbfnEnter = fn;}
void io_attachCBUp(void (*fn)())			{_io_cbfnUp = fn;}
void io_attachCBDown(void (*fn)())			{_io_cbfnDown = fn;}
void io_attachCBRight(void (*fn)())			{_io_cbfnRight = fn;}
void io_attachCBLeft(void (*fn)())			{_io_cbfnLeft = fn;}

void io_attachCBEnterLong(void (*fn)())		{_io_cbfnEnterLong = fn;}
void io_attachCBUpLong(void (*fn)())		{_io_cbfnUpLong = fn;}
void io_attachCBDownLong(void (*fn)())		{_io_cbfnDownLong = fn;}
void io_attachCBRightLong(void (*fn)())		{_io_cbfnRightLong = fn;}
void io_attachCBLeftLong(void (*fn)())		{_io_cbfnLeftLong = fn;}

void io_attachCBWheelClockwise(void (*fn)())		{_io_cbfnWheelClk = fn;}
void io_attachCBWheelCounterClockwise(void (*fn)())	{_io_cbfnWheelCClk = fn;}

void io_deattachCBEnter()		{_io_cbfnEnter = NULL;}
void io_deattachCBUp()			{_io_cbfnUp = NULL;}
void io_deattachCBDown()		{_io_cbfnDown = NULL;}
void io_deattachCBRight()		{_io_cbfnRight = NULL;}
void io_deattachCBLeft()		{_io_cbfnLeft = NULL;}

void io_deattachCBEnterLong()	{_io_cbfnEnterLong = NULL;}
void io_deattachCBUpLong()		{_io_cbfnUpLong = NULL;}
void io_deattachCBDownLong()	{_io_cbfnDownLong = NULL;}
void io_deattachCBRightLong()	{_io_cbfnRightLong = NULL;}
void io_deattachCBLeftLong()	{_io_cbfnLeftLong = NULL;}

void io_deattachCBWheelClockwise()			{_io_cbfnWheelClk = NULL;}
void io_deattachCBWheelCounterClockwise()	{_io_cbfnWheelCClk = NULL;}

void _io_cbEnter()		{if(_io_cbfnEnter) 	(*_io_cbfnEnter)();}
void _io_cbUp()			{if(_io_cbfnUp) 	(*_io_cbfnUp)();}
void _io_cbDown()		{if(_io_cbfnDown) 	(*_io_cbfnDown)();}
void _io_cbRight()		{if(_io_cbfnRight) 	(*_io_cbfnRight)();}
void _io_cbLeft()		{if(_io_cbfnLeft) 	(*_io_cbfnLeft)();}

void _io_cbEnterLong()	{if(_io_cbfnEnterLong)	(*_io_cbfnEnterLong)();}
void _io_cbUpLong()		{if(_io_cbfnUpLong)		(*_io_cbfnUpLong)();}
void _io_cbDownLong()	{if(_io_cbfnDownLong)	(*_io_cbfnDownLong)();}
void _io_cbRightLong()	{if(_io_cbfnRightLong) 	(*_io_cbfnRightLong)();}
void _io_cbLeftLong()	{if(_io_cbfnLeftLong)	(*_io_cbfnLeftLong)();}

void _io_cbWheelClockwise()			{if(_io_cbfnWheelClk)	(*_io_cbfnWheelClk)();}
void _io_cbWheelCounterClockwise()	{if(_io_cbfnWheelCClk)	(*_io_cbfnWheelCClk)();}
