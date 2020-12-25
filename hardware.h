#ifndef ESPOD_HARDAWARE_H
#define ESPOD_HARDAWARE_H

#include "esPod.h"

void hw_init();

void hw_initBacklight();

void hw_startDelayDiming();
void hw_setBacklight(uint8_t value);
void _hw_taskBacklight(void *param);
void _hw_dimBacklight();

#endif