#include "pod.h"


TFT_eSPI tft = TFT_eSPI();

TFT_eSprite consoleSprite = TFT_eSprite(&tft);

void display_init(){
	tft.init();
	tft.invertDisplay(1);
	tft.setRotation(2);
	tft.initDMA();

	display_initSprites();

}

void display_initSprites(){
	consoleSprite.setColorDepth(1);
	consoleSprite.createSprite(TFT_WIDTH, TFT_HEIGHT);

}

void display_setTermMode(){
	tft.unloadFont();
	tft.setTextSize(1);
	tft.fillScreen(TFT_BLACK);
	tft.setBitmapColor(TFT_WHITE, TFT_BLACK);
	tft.setTextColor(TFT_WHITE, TFT_BLACK);

}

void display_setRunningMode(){

}


