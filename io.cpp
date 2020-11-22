#include "io.h"

const uint8_t NB_BUTTONS = 5;

const uint8_t BTN_CENTER = 0;
const uint8_t BTN_UP = 34;
const uint8_t BTN_RIGHT = 36;
const uint8_t BTN_DOWN = 35;
const uint8_t BTN_LEFT = 39;

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

PushButton button[NB_BUTTONS];
/*
PushButton btnCenter;
PushButton btnUp;
PushButton btnRight;
PushButton btnDown;
PushButton btnLeft;
*/
void io_init(){
	io_initIO();
	io_initButtons();
}

void io_initIO(){
	// Enable pullups on SD lines
	pinMode(2, INPUT_PULLUP);
	pinMode(4, INPUT_PULLUP);
	pinMode(12, INPUT_PULLUP);
	pinMode(13, INPUT_PULLUP);
	pinMode(14, INPUT_PULLUP);
	pinMode(15, INPUT_PULLUP);
}

void io_initButtons(){
/*
	for(uint8_t i = 0; i < NB_BUTTONS; ++i){
		button[i] = new PushButton();
	}
*/
	button[0].begin(BTN_CENTER, PULLUP);
	button[1].begin(BTN_UP, PULLUP);
	button[2].begin(BTN_DOWN, PULLUP);
	button[3].begin(BTN_RIGHT, PULLUP);
	button[4].begin(BTN_LEFT, PULLUP);
}

uint8_t io_update(){
//	uint8_t state = 0;

	for(uint8_t i = 0; i < NB_BUTTONS; ++i){
//		state |= (button[i].update() << i);
		if(button[i].update()){
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

//	return state;
	return 0;
}

bool io_isPressed(uint8_t btn){
	return button[btn].justPressed();
}

bool io_justReleased(uint8_t btn){
	return button[btn].justReleased();
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

void io_deattachCBEnter(void (*fn)())		{_io_cbfnEnter = NULL;}
void io_deattachCBUp(void (*fn)())			{_io_cbfnUp = NULL;}
void io_deattachCBDown(void (*fn)())		{_io_cbfnDown = NULL;}
void io_deattachCBRight(void (*fn)())		{_io_cbfnRight = NULL;}
void io_deattachCBRLeft(void (*fn)())		{_io_cbfnLeft = NULL;}

void io_deattachCBEnterLong(void (*fn)())	{_io_cbfnEnterLong = NULL;}
void io_deattachCBUpLong(void (*fn)())		{_io_cbfnUpLong = NULL;}
void io_deattachCBDownLong(void (*fn)())	{_io_cbfnDownLong = NULL;}
void io_deattachCBRightLong(void (*fn)())	{_io_cbfnRightLong = NULL;}
void io_deattachCBLeftLong(void (*fn)())	{_io_cbfnLeftLong = NULL;}

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