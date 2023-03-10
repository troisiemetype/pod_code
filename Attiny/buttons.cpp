#include "buttons.h"

// buttons :
const uint8_t BTN_UP = 4;				// PB5, pin 6
const uint8_t BTN_RIGHT = 5;			// PB4, pin 7
const uint8_t BTN_DOWN = 6;				// PB3, Pin 8
const uint8_t BTN_LEFT = 7;				// PB2, PIN 9
const uint8_t BTN_CENTER = 14;			// PA1, PIN 17
const uint8_t BTN_HOLD = 15;			// PA2, PIN 18

const uint8_t NUM_BTN = 6;

// Array of buttons,easier to handle than by independant variables.
PushButton button[NUM_BTN];

// This tracks which button has changed.
// Set by button update, cleared by controller reading. One bit flag for each button.
// bit 7 	|	bit 6 	|	bit 5 	|	bit 4 	|	bit 3 	|	bit 2 	|	bit 1 	|	bit 0
// UD 	 	|	UD 		|	HOLD	|	CENTER	|  	LEFT	|   DOWN	| 	RIGHT 	| 	UP
uint8_t buttonChanged = 0;
// describe the button state :
// bit 7 	|	bit 6 	|	bit 5 	|	bit 4 	|	bit 3 	|	bit 2 	|	bit 1 	|	bit 0
// flag 	|	flag	|	flag	|	flag	| jst rel.	|   lg rel.	| 	lg pr. 	| 	pressed
// Each flag describes which of the lower bits have changed. Flags cleared by controller reading.
uint8_t buttonState[NUM_BTN];


// init each button.
// Pin are init'ed by the PushButton class.
void buttons_init(){
	button[0].begin(BTN_UP, INPUT_PULLUP);
	button[1].begin(BTN_RIGHT, INPUT_PULLUP);
	button[2].begin(BTN_DOWN, INPUT_PULLUP);
	button[3].begin(BTN_LEFT, INPUT_PULLUP);
	button[4].begin(BTN_CENTER, INPUT_PULLUP);
	button[5].begin(BTN_HOLD, INPUT_PULLUP);	
}

bool buttons_update(){
	bool update = false;
	for(uint8_t i = 0; i < NUM_BTN; ++i){
		if(button[i].update()){
			update = true;
			// Setting a flag for this button.
			buttonChanged |= (1 << i);

			// storing the previous state of the button.
			uint8_t state = buttonState[i];

			// constructing the current state of the button.
			buttonState[i] = 0;

			if(button[i].isPressed()) buttonState[i] |= (1 << 0);
			if(button[i].isLongPressed()) buttonState[i] |= (1 << 1);
			if(button[i].isLongReleased()) buttonState[i] |= (1 << 2);
			if(button[i].justReleased()) buttonState[i] |= (1 << 3);

			// Set the flags for this button.
			uint8_t flags = state ^ buttonState[i];
			buttonState[i] |= flags << 4;
		}
	}

	return update;
}

void buttons_setDebounce(uint16_t debounce){
	for(uint8_t i = 0; i < NUM_BTN; ++i){
		button[i].setDebounceDelay(debounce);
	}
}

void buttons_setLong(uint16_t longDelay){
	for(uint8_t i = 0; i < NUM_BTN; ++i){
		button[i].setLongDelay(longDelay);
	}
}

void buttons_setDouble(uint16_t doubleDelay){
	for(uint8_t i = 0; i < NUM_BTN; ++i){
		button[i].setDoubleDelay(doubleDelay);
	}
}

uint8_t buttons_getFlags(){
	return buttonChanged;
}

uint8_t buttons_getbutton(uint8_t button){
	return buttonState[button];
}
