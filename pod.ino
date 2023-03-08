#include "pod.h"

bool state = 1;

Logger console = Logger();

void setup(){
	pinMode(29, OUTPUT);
	pinMode(24, OUTPUT);
	
	display_init();
	console.attachOutput(&consoleSprite);
	console.init(2000);

	display_setTermMode();

	delay(250);

	digitalWrite(24, 1);
/*
	for(uint8_t i = 0; i < 50; ++i){

		console.print("line ");
		console.println(i);
	}

	console.clear();
*/
/*
	for(uint8_t i = 0; i < 45; ++i){

		console.print("line ");
		console.println(i);
	}

	for(uint8_t i = 0; i < 31; ++i){

		console.progress(i, 30);
	}

	for(uint8_t i = 10; i < 20; ++i){

		console.print("line ");
		console.println(i);
	}
*/
	console.println("started !");

}

void loop(){

	digitalWrite(29, state);
	state ^= 1;
	delay(50);
	

}
