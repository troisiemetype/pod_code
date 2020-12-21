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

#define DISPLAY_QUEUE_SIZE			32
#define DISPLAY_MAX_MENU_ITEM		10

#define DISPLAY_HEADER_HEIGHT		22
#define DISPLAY_MENU_LINE_HEIGHT	22

#define DISPLAY_VU_WIDTH 			200
#define DISPLAY_VU_HEIGHT			16

#define DISPLAY_TIME_WIDTH			50
#define DISPLAY_TIME_HEIGHT			22

extern TFT_eSPI tft;

enum displayState_t{
	IDLE = 0,
	MENU,
	PLAYER,
	VOLUME,
};

enum timeType_t{
	TIME = 0,
	CURRENT,
	REMAINING,
	TOTAL,
};

struct theme_t{
	uint16_t headerBg;
	uint16_t headerTxt;
	uint16_t bg;
	uint16_t bgSelect;
	uint16_t txt;
	uint16_t txtSelect;
	uint16_t vuBg;
	uint16_t vuFg;
	uint16_t vuCtr;
	uint16_t batEmpty;
	uint16_t batHalf;
	uint16_t batFull;
	bool gradient;
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

struct lineData_t{
	const char *name;
	uint8_t index;
	bool active;
	bool update;
	uint16_t pos;
	lineData_t *next;
};

struct timeData_t{
	uint16_t current;
	uint16_t total;
	timeType_t type;
	uint16_t x;
	uint16_t y;
};

struct playerData_t{
	const char *name;
	const char *album;
	const char *artist;
	uint8_t track;
	uint8_t volume;
};

struct vuData_t{
	float value;
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
};

void display_init();

void display_initSprites();

void display_initBacklight();
void display_setBackLight(uint8_t value);

void display_setState(displayState_t state);

theme_t display_getTheme();
void display_setTheme(theme_t newTheme);

void display_update();

uint8_t display_getMaxMenuItem();

void display_setTermMode();
void display_setRunningMode();

void display_pushClearDisplay();
void display_pushClearAll();
void display_pushHeader(const char *header);
void display_pushMenu(const char *name, bool active, uint8_t index);
void display_pushMenuPadding(uint8_t index);
void display_pushPlayer(const char *artist, const char *album, const char *song, uint8_t track);
void display_pushPlayerProgress(uint16_t current, uint16_t total);
void display_pushPlayerVolume(uint8_t volume);
void display_pushVuMeter(float value, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void display_pushBattery(float value);

void _display_updateClearDisplay(void *data);
void _display_updateClearAll(void *data);
void _display_updateHeader(void *data);
void _display_updateLine(void *data);
void _display_updateLineThin(void *data);
void _display_updatePlayerProgress(void *data);
void _display_updatePlayerVolume(void *data);
void _display_updateTime(void *data);
void _display_updateVuMeter(void *data);
void _display_updateBattery(void *data);

void _display_populateBuffer(void (*fn)(void*), void *data);
void _display_consumeBuffer();

void display_battery(float value);

#endif