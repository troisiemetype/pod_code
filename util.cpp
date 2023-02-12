#include "esPod.h"

void util_scanI2C(){
	Wire.begin();

	Serial.println("start scanning I2C");
	for(uint8_t i = 0; i < 128; ++i){
		Wire.beginTransmission(i);
		if(Wire.endTransmission() == 0){
			Serial.print("Device at address: ");
		}
		Serial.printf("%i\t%20X\n", i, i);
	}
	Serial.println();
}
