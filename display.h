#ifndef POD_DISPLAY_H
#define POD_DISPLAY_H

#include <Arduino.h>
#include "pod.h"

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
extern TFT_eSprite consoleSprite;

void display_init();
void display_initSprites();

void display_setTermMode();
void display_setRunningMode();



#endif