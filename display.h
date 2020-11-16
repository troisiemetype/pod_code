#ifndef ESPOD_DISPLAY_H
#define ESPOD_DISPLAY_H

#include <Arduino.h>
#include "esPod.h"

// line height == 
#define FONT_DEFAULT NotoSansBold15
#define FONT_MENU

#define COLOR_HEADER 		TFT_LIGHTGREY
#define COLOR_BG 			TFT_WHITE
#define COLOR_TXT 			TFT_BLACK
#define COLOR_BG_SELECT 	TFT_DARKGREEN
#define COLOR_TXT_SELECT 	TFT_WHITE

extern TFT_eSPI tft;

struct displaySetting_t{
	uint16_t backgroundColor;
	uint16_t fontColor;
	uint16_t fontSize;
	String font;
};

void display_initBacklight();
void display_setBackLight(uint8_t value);
void display_init();
void display_setTermMode();
void display_setRunningMode();
void display_makeHeader();
void display_makeMenuBG();
void display_makeMenuEntry(const char *name, bool active = false);

#endif