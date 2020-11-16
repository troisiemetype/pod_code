#include "display.h"

const uint8_t TFT_LED = 33;
const int8_t TFT_LED_CH = 0;
const int16_t TFT_LED_FREQ = 5000;
const int8_t TFT_LED_RES = 8;

uint16_t pos = 0;

void display_initBacklight(){
	ledcSetup(TFT_LED_CH, TFT_LED_FREQ, TFT_LED_RES);
	ledcAttachPin(TFT_LED, TFT_LED_CH);
	ledcWrite(TFT_LED_CH, 0);
}

void display_setBackLight(uint8_t value){
	ledcWrite(TFT_LED_CH, value);
}

void display_init(){
	tft.init();
	tft.setRotation(1);
	tft.setTextWrap(false, false);
	tft.setCursor(3, 3);
	display_setTermMode();

//	tft.println("esp pod");
//	tft.printf("line height : %i\n", tft.fontHeight());

	for(uint8_t i = 0; i < 250; ++i){
		ledcWrite(TFT_LED_CH, i);
		delay(5);
	}
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
	tft.loadFont(NotoSansBold16);
//	tft.printf("line height : %i\n", tft.fontHeight());
//	String filename = "system/fonts/NotoSansMono14";
//	tft.loadFont(filename, SD_MMC);
}

void display_makeHeader(){
	tft.drawRect(0, 0, 320, 20, COLOR_HEADER);
	tft.fillRect(0, 0, 320, 20, COLOR_HEADER);
}

void display_makeMenuBG(){
	tft.drawRect(0, 20, 320, 220, COLOR_BG);
	tft.fillRect(0, 20, 320, 220, COLOR_BG);
	pos = 20;
}

void display_makeMenuEntry(const char *name, bool active){
	if(active){
		tft.drawRect(0, pos, 320, 22, COLOR_BG_SELECT);
		tft.fillRect(0, pos, 320, 22, COLOR_BG_SELECT);
		tft.setTextColor(COLOR_TXT_SELECT, COLOR_BG_SELECT);
	}
	tft.drawString(name, 2, pos + 2);
	if(active){
		tft.setTextColor(COLOR_TXT, COLOR_BG);		
	}
	pos += 22;
}

