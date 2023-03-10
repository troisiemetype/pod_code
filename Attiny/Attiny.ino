/*
 *
 *	This program is for controlling an Attiny set as an I2C peripheral,
 *	which continuousely monitors buttons and capacitive touch pads.
 *
 *	As any I2C peripheral device, it can be accessed by the controller through registers, and can also signal any input change.
 *
 *	For buttons, we need to access :
 *		the state they are in (released, pushed, clicked, long clicked)
 *		Their settings (debounce delay, long click delay, double click delay)
 *	
 *	For capacitive touch, we need to access :
 *		is it touched or not.
 *		Position.
 *		step (+ or -, regarding previous position)
 */

#include <Wire.h>
#include "buttons.h"
#include "PushButton.h"

// Pinout of the Attiny :

// touch pads
const uint8_t TOUCH_1 = 0;				// PA4, pin 2
const uint8_t TOUCH_2 = 1;				// PA5, pin 3
const uint8_t TOUCH_3 = 2;				// PA6, pin 4
const uint8_t TOUCH_4 = 3;				// PA7, pin 5
const uint8_t TOUCH_5 = 13;				// PC3, pin 15
const uint8_t TOUCH_6 = 12;				// PC2, pin 14
const uint8_t TOUCH_7 = 11;				// PC1, pin 13
const uint8_t TOUCH_8 = 10;				// PC0, pin 12

// I2C pins :
const uint8_t PIN_SDA = 8;				// PB1, pin 10
const uint8_t PIN_SCL = 9;				// PB0, pin 11
const uint8_t PIN_INT = 16;				// PA3, pin 19


enum wireReg_t{
	// buttons registers
	BTN_CHANGED =  				0x00,		// register + 1 byte to controller
	BTN_STATE =					0x08,		// register | button number + 1 byte to controller

	BTN_SET_DEBOUNCE =  		0x10,		// register + 2 bytes from controller.
	BTN_SET_LONG_CLICK =		0x11,		// register + 2 bytes from controller.
	BTN_SET_DOUBLE_CLICK =		0x12,		// register + 2 bytes from controller.

	// touch registers
}

enum wireState_t{
	I2C_IDLE = 0,
	I2C_WAIT_FOR_RECEIVE,
	I2C_WAIT_FOR_SEND,
};

wireState_t wireState = I2C_IDLE;

uint8_t command = 0;


void initTouchPads(){

}

// handle received bytes.
void receiveCB(int numBytes){
	// Received bytes can be of two kinds : 
	// It can be the register at the beginning of a transmission :
	if(wireState == I2C_IDLE){
		command = Wire.read();
	// Or it can be some data sent by the controller.
	} else if(wireState == I2C_WAIT_FOR_RECEIVE){
		uint8_t b = 0;
		uint16_t data = 0;
		switch(command){
			case BTN_SET_DEBOUNCE:
				data = Wire.read();
				b = Wire.read();
				data |= (uint16_t)b << 8;
				buttons_setDebounce(data);
				break;
			case BTN_SET_LONG_CLICK:
				data = Wire.read();
				b = Wire.read();
				data |= (uint16_t)b << 8;
				buttons_setLong(data);
				break;
			case SET_DOUBLE_CLICK:
				data = Wire.read();
				b = Wire.read();
				data |= (uint16_t)b << 8;
				buttons_setDouble(data);
				break;
		}
		wireState = I2C_IDLE;
	}
}

// Handle requests from controller
void requestCB(){
	// When we are requested bytes, we can be in one only case : a register address has been sent, and we have byte to send back.
	if(wireState != I2C_WAIT_FOR_SEND){
		wireState = I2C_IDLE;
		return
	}
	if(command == BTN_CHANGED){
		wire.write(buttons_getFlags());
	} else if(command & 0xF8){
		uint8_t button = command & 0x07;
		wire.write(buttoons_getButton(button));
	}

}

void setup(){
	buttons_init();

	// Set the pin used for signaling changes (INT) to the controller.
	// Int pins are almost always active low, so we set it high at init.
	pinMode(PIN_INT, OUTPUT);
	digitalWrite(PIN_INT, 1);

	Wire.begin(0x0A);

	Wire.onReceive(receiveCB);
	Wire.onRequest(requestCB);
}

void updateTouchPads(){

}

void loop(){
	if(buttons_update()) digitalWrite(PIN_INT, 0);
}