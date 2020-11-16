#include "io.h"

const uint8_t NB_BUTTONS = 5;

const uint8_t BTN_CENTER = 0;
const uint8_t BTN_UP = 34;
const uint8_t BTN_RIGHT = 36;
const uint8_t BTN_DOWN = 35;
const uint8_t BTN_LEFT = 39;

PushButton button[NB_BUTTONS];
/*
PushButton btnCenter;
PushButton btnUp;
PushButton btnRight;
PushButton btnDown;
PushButton btnLeft;
*/

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
	button[2].begin(BTN_RIGHT, PULLUP);
	button[3].begin(BTN_DOWN, PULLUP);
	button[4].begin(BTN_LEFT, PULLUP);
/*
	pinMode(BTN_CENTER, INPUT);
	pinMode(BTN_UP, INPUT);
	pinMode(BTN_RIGHT, INPUT);
	pinMode(BTN_DOWN, INPUT);
	pinMode(BTN_LEFT, INPUT);
*/
}

uint8_t io_updateButtons(){
	uint8_t state = 0;

	for(uint8_t i = 0; i < NB_BUTTONS; ++i){
		state |= (button[i].update() << i);
	}

	return state;
}

bool io_isPressed(uint8_t btn){
	return button[btn].justPressed();
}

bool io_justReleased(uint8_t btn){
	return button[btn].justReleased();
}