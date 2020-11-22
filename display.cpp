#include "display.h"

// default "terminal" font is 53 * 30 (1590) characters to fill screen

TFT_eSPI tft = TFT_eSPI();

const uint8_t TFT_LED = 26;
const int8_t TFT_LED_CH = 0;
const int16_t TFT_LED_FREQ = 5000;
const int8_t TFT_LED_RES = 8;

uint8_t display_index = 0;
uint16_t pos = 0;
uint8_t display_maxMenuItem;


TFT_eSprite menuSprite = TFT_eSprite(&tft);
TFT_eSprite entrySprite = TFT_eSprite(&tft);
TFT_eSprite headerSprite = TFT_eSprite(&tft);

TFT_eSprite vuSprite = TFT_eSprite(&tft);

void display_init(){
	display_initBacklight();
	tft.init();
	tft.setRotation(1);
//	tft.setTextWrap(false, false);
//	tft.setCursor(3, 3);
	display_setTermMode();

	menuSprite.setColorDepth(16);
	entrySprite.setColorDepth(16);
	headerSprite.setColorDepth(16);
	vuSprite.setColorDepth(16);

	display_maxMenuItem = 10;

//	tft.println("esp pod");
//	tft.printf("line height : %i\n", tft.fontHeight());

	for(uint8_t i = 0; i < 250; ++i){
		ledcWrite(TFT_LED_CH, i);
		delay(5);
	}
}

void display_update(){

}

void display_initBacklight(){
	ledcSetup(TFT_LED_CH, TFT_LED_FREQ, TFT_LED_RES);
	ledcAttachPin(TFT_LED, TFT_LED_CH);
	ledcWrite(TFT_LED_CH, 0);
}

void display_setBackLight(uint8_t value){
	ledcWrite(TFT_LED_CH, value);
}

uint8_t display_getMaxMenuItem(){
	return display_maxMenuItem;
}

void display_setTermMode(){
	tft.unloadFont();
	tft.setTextSize(1);
	tft.fillScreen(TFT_BLACK);
	tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

void display_setRunningMode(){
	tft.fillScreen(COLOR_BG);
	tft.setTextColor(COLOR_TXT, COLOR_BG);
//	tft.loadFont(NotoSansBold16);
//	tft.printf("line height : %i\n", tft.fontHeight());
//	String filename = "system/fonts/NotoSansMono14";
//	tft.loadFont(filename, SD_MMC);
}

void display_clearDisplay(){
	tft.fillRect(0, 20, 320, 220, COLOR_BG);	
}

void display_clearAll(){
	tft.fillScreen(COLOR_BG);
}

void display_makeHeader(const char *header){
	headerSprite.loadFont(NotoSansBold15);
	headerSprite.createSprite(320, 22);
	headerSprite.fillSprite(COLOR_HEADER);
	headerSprite.setTextPadding(0);
	headerSprite.setTextColor(COLOR_TXT, COLOR_HEADER);
	headerSprite.setTextDatum(ML_DATUM);
	headerSprite.drawString(header, 2, 11);
	headerSprite.pushSprite(0, 0);
	headerSprite.unloadFont();
	headerSprite.deleteSprite();
//	tft.drawRect(0, 0, 320, 20, COLOR_HEADER);
//	tft.fillRect(0, 0, 320, 20, COLOR_HEADER);
}

void display_pushToMenu(const char *name, bool active){
	entrySprite.loadFont(NotoSansBold16);
	entrySprite.createSprite(320, 22);
	entrySprite.fillSprite(active ? COLOR_BG_SELECT : COLOR_BG);
	entrySprite.setTextPadding(0);
	entrySprite.setTextColor(active ? COLOR_TXT_SELECT : COLOR_TXT, active ? COLOR_BG_SELECT : COLOR_BG);
	entrySprite.setTextDatum(ML_DATUM);
	entrySprite.drawString(name, 1, 11);
	entrySprite.pushSprite(0, pos);
	entrySprite.unloadFont();
	entrySprite.deleteSprite();
	pos += 22;
	display_index++;
}

void display_makeMenu(const char *name){
//	tft.fillRect(0, 20, 320, 220, COLOR_BG);
//	menuSprite.createSprite(320, 220);
//	menuSprite.fillSprite(COLOR_BG);
	display_makeHeader(name);
	pos = 22;
	display_index = 0;
}

void display_updateMenu(){
//	tft.fillRect(0, 20, 320, 220, COLOR_BG);
//	menuSprite.pushSprite(0, 22);
//	menuSprite.deleteSprite();
	for(uint8_t i = display_index; i < display_maxMenuItem; ++i){
		display_pushToMenu("", 0);
	}
}

void display_makePlayer(){

}

void display_vuMeter(float value, uint16_t x, uint16_t y, uint16_t width = 200){
	if(value < 0) value = 0;
	if(value > 1) value = 1;
	uint16_t dspValue = value * width;
	vuSprite.createSprite(width, 16);
	vuSprite.fillSprite(COLOR_BG);
	vuSprite.drawRoundRect(0, 0, width, 16, 3, TFT_DARKGREEN);
	vuSprite.fillRoundRect(1, 1, dspValue - 2, 14, 2, COLOR_BG_SELECT);
	vuSprite.pushSprite(x, y);

}
