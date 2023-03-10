#ifndef POD_ATTINY_BUTTONS_H
#define POD_ATTINY_BUTTONS_H

#include "PushButton.h"

void buttons_init();
bool buttons_update();

void buttons_setDebounce(uint16_t debounce);
void buttons_setLong(uint16_t longDelay);
void buttons_setDouble(uint16_t doubleDelay);

uint8_t buttons_getFlags();
uint8_t buttons_getbutton(uint8_t button);


#endif