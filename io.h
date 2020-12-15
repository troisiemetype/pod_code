#ifndef ESPOD_IO_H
#define ESPOD_IO_H

// #include <Arduino.h>
#include "esPod.h"
// #include "PushButton.h"
// #include "wheel.h"

void io_init();
void io_initIO();

void io_initButtons();
void io_initWheel();

void io_update();

void io_updateButtons();
void io_updateWheel();

void io_updateBattery();

void io_deattachAllCB();

void io_attachCBEnter(void (*fn)());
void io_attachCBUp(void (*fn)());
void io_attachCBDown(void (*fn)());
void io_attachCBRight(void (*fn)());
void io_attachCBLeft(void (*fn)());

void io_attachCBEnterLong(void (*fn)());
void io_attachCBUpLong(void (*fn)());
void io_attachCBDownLong(void (*fn)());
void io_attachCBRightLong(void (*fn)());
void io_attachCBLeftLong(void (*fn)());

void io_attachCBWheelClockwise(void (*fn)());
void io_attachCBWheelCounterClockwise(void (*fn)());

void io_deattachCBEnter();
void io_deattachCBUp();
void io_deattachCBDown();
void io_deattachCBRight();
void io_deattachCBLeft();

void io_deattachCBEnterLong();
void io_deattachCBUpLong();
void io_deattachCBDownLong();
void io_deattachCBRightLong();
void io_deattachCBLeftLong();

void io_deattachCBWheelClockwise();
void io_deattachCBWheelCounterClockwise();

void _io_cbEnter();
void _io_cbUp();
void _io_cbDown();
void _io_cbRight();
void _io_cbLeft();

void _io_cbEnterLong();
void _io_cbUpLong();
void _io_cbDownLong();
void _io_cbRightLong();
void _io_cbLeftLong();

void _io_cbWheelClockwise();
void _io_cbWheelCounterClockwise();

#endif