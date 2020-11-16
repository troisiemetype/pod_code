#ifndef ESPOD_IO_H
#define ESPOD_IO_H

#include <Arduino.h>
#include "esPod.h"
#include "PushButton.h"

void io_initIO();

void io_initButtons();

uint8_t io_updateButtons();
bool io_isPressed(uint8_t btn);
bool io_justReleased(uint8_t btn);



#endif