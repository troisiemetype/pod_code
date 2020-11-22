#ifndef ESPOD_DISPLAY_H
#define ESPOD_DISPLAY_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "esPod.h"

// line height == 
#define FONT_DEFAULT NotoSansBold15
#define FONT_MENU

#define COLOR_HEADER 		TFT_SILVER
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

void display_init();

void display_update();
void display_initBacklight();
void display_setBackLight(uint8_t value);

uint8_t display_getMaxMenuItem();

void display_setTermMode();
void display_setRunningMode();

void display_clearDisplay();
void display_clearAll();

void display_makeHeader(const char *header);
void display_pushToMenu(const char *name, bool active = false);
void display_makeMenu(const char *name);
void display_updateMenu();

void display_makePlayer();

void display_vuMeter(float value, uint16_t x, uint16_t y, uint16_t width);

#endif