#ifndef ESPOD_DISPLAY_H
#define ESPOD_DISPLAY_H

// #include <Arduino.h>
// #include <TFT_eSPI.h>
#include "esPod.h"

// line height == 
#define FONT_DEFAULT NotoSansBold15
#define FONT_MENU

#define SCREEN_WIDTH				320
#define SCRENN_HEIGHT				240

#define COLOR_HEADER 				TFT_SILVER
#define COLOR_BG 					TFT_WHITE
#define COLOR_TXT 					TFT_BLACK
#define COLOR_BG_SELECT 			TFT_DARKGREEN
#define COLOR_TXT_SELECT 			TFT_WHITE

#define DISPLAY_QUEUE_SIZE			16
#define DISPLAY_MAX_MENU_ITEM		10

#define DISPLAY_HEADER_HEIGHT		22
#define DISPLAY_MENU_LINE_HEIGHT	22

#define DISPLAY_VU_WIDTH 			200
#define DISPLAY_VU_HEIGHT			16

extern TFT_eSPI tft;

enum displayState_t{
	IDLE = 0,
	UPDATE_MENU,
	UPDATE_PLAYING_TIME,
	UPDATE_VOLUME,
	UPDATE_VOLUME_DELAY,
};

struct displaySetting_t{
	uint16_t backgroundColor;
	uint16_t fontColor;
	uint16_t fontSize;
	String font;
};

struct displayBuffer_t{
	void (*cbFn)(void*);
	void *data;
	displayBuffer_t *next;
};

struct displayQueue_t{
	displayBuffer_t *buffer;
	displayBuffer_t *start;
	displayBuffer_t *end;
	uint8_t size;
	uint8_t available;
};

struct headerData_t{
	const char *name;
	uint8_t battery;
};

struct menuData_t{
	const char *name;
	bool active;
	bool update;
	uint16_t pos;
};

struct playerData_t{
	const char *artist;
	const char *album;
	const char *name;
	uint8_t track;
	uint8_t currentTime;
	uint8_t totalTime;
	uint8_t volume;
};

void display_init();

void display_initSprites();

void display_initBacklight();
void display_setBackLight(uint8_t value);

void display_update();
void display_updateClearDisplay(void *data);
void display_clearAll(void *data);
void display_updateHeader(void *data);
void display_updateMenu(void *data);
void display_updatePlayer(void *data);
void display_updatePlayerProgress(void *data);
void display_updatePlayerVolume(void *data);

uint8_t display_getMaxMenuItem();

void display_setTermMode();
void display_setRunningMode();

void display_pushHeader(const char *header);
void display_pushMenu(const char *name, bool active, uint8_t index);
void display_pushMenuPadding(uint8_t index);
void display_pushPlayer(const char *artist, const char *album, const char *song, uint8_t track);
void display_pushPlayerProgress(uint16_t current, uint16_t total);
void display_pushPlayerVolume(uint8_t volume);

void _display_populateBuffer(void (*fn)(void*), void *data);
void _display_consumeBuffer();
/*
void display_clearDisplay();
void display_clearAll();

void display_updateMenu();

void display_makeMenuEntry(const char *name, bool active);
void display_makeMenu(const char *name);
void display_fillMenu();

void display_makePlayer(const char *artist, const char *album, const char *song, uint8_t track);
void display_playerProgress(uint16_t current, uint16_t total);

void display_makePlayer();

void display_vuMeter(float value, uint16_t x, uint16_t y, uint16_t width);
void display_battery(float value);
*/
#endif