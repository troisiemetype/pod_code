#include "esPod.h"
// #include "display.h"

// default "terminal" font is 53 * 30 (1590) characters to fill screen

TFT_eSPI tft = TFT_eSPI();

const uint8_t TFT_LED = 26;
const int8_t TFT_LED_CH = 0;
const int16_t TFT_LED_FREQ = 5000;
const int8_t TFT_LED_RES = 8;

uint8_t display_index = 0;
uint8_t display_topIndex = 0;
bool display_forceUpdate = true;
uint16_t pos = 0;
uint8_t display_maxMenuItem;

struct menuEntry_t{
	bool active;
	bool update;
	char *name = NULL;
};

menuEntry_t *menuBuffer = NULL;

enum displayState_t{
	IDLE = 0,
	UPDATE_MENU,
	UPDATE_PLAYING_TIME,
	UPDATE_VOLUME,
	UPDATE_VOLUME_DELAY,
} displayState;


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

	menuBuffer = new menuEntry_t[display_maxMenuItem];

	displayState = IDLE;

//	tft.println("esp pod");
//	tft.printf("line height : %i\n", tft.fontHeight());

	for(uint8_t i = 0; i < 250; ++i){
		ledcWrite(TFT_LED_CH, i);
		delay(5);
	}
}

void display_update(){
	switch (displayState){
		case UPDATE_MENU:
			display_updateMenu();
			break;
		case UPDATE_PLAYING_TIME:
			break;
		case UPDATE_VOLUME:
			break;
		case UPDATE_VOLUME_DELAY:
			break;
		case IDLE:
		default:
			break;
	}

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
	display_forceUpdate = true;
}

void display_updateMenu(){
	menuEntry_t *entry = &menuBuffer[display_index];
//	menuEntry_t *entry = menuBuffer + display_index;
	if(display_forceUpdate || entry->update){
		display_makeMenuEntry(entry->name, entry->active);
	}
	pos += 22;
	display_index++;

	if(display_index > display_topIndex){
		tft.fillRect(0, pos, 320, 240-pos, COLOR_BG);
		display_forceUpdate = false;
		displayState = IDLE;
	}
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

// this is the function called from Menu.
// It stores the menu to build, so subsequent calls to display_update can build it one block at a time.
void display_pushToMenu(const char *name, bool active, uint8_t index){
	log_d("pushing to menu");
	// First check that index is not out of bounds.
	if(index > display_maxMenuItem) return;
	display_topIndex = index;

	menuEntry_t *entry = &menuBuffer[index];
//	log_d("local pointer done");
//	menuEntry_t *entry = menuBuffer + index;
	// Then compare the new menu entry name with the previous one, so we dont do unnecessary copying.
	if(entry->name == name){
		log_d("same name");
		if(entry->active != active){
			entry->active = active;
			entry->update = 1;
		} else {
			entry->update = 0;
		}
	} else {
		log_d("populate buffer");
		entry->name = (char*)name;
		entry->active = active;
		entry->update = 1;
	}
}

void display_makeMenuEntry(const char *name, bool active){
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
/*
	pos += 22;
	display_index++;
*/
}

void display_makeMenu(const char *name){
//	tft.fillRect(0, 20, 320, 220, COLOR_BG);
//	menuSprite.createSprite(320, 220);
//	menuSprite.fillSprite(COLOR_BG);
	display_makeHeader(name);
	pos = 22;
	display_index = 0;
	displayState = UPDATE_MENU;

}

void display_fillMenu(){
//	tft.fillRect(0, 20, 320, 220, COLOR_BG);
//	menuSprite.pushSprite(0, 22);
//	menuSprite.deleteSprite();
	for(uint8_t i = display_index; i < display_maxMenuItem; ++i){
		display_pushToMenu("", 0, i);
	}
}

void display_makePlayer(const char *artist, const char *album, const char *song, uint8_t track){
	display_clearAll();
	display_makeHeader("playing");

	entrySprite.loadFont(NotoSansBold16);
	entrySprite.createSprite(320, 22);
	entrySprite.fillSprite(COLOR_BG);
	entrySprite.setTextPadding(0);
	entrySprite.setTextColor(COLOR_TXT, COLOR_BG);
	entrySprite.setTextDatum(ML_DATUM);
	entrySprite.drawString(song, 1, 11);
	entrySprite.pushSprite(0, 44);

	entrySprite.loadFont(NotoSans16);
//	entrySprite.createSprite(320, 22);
	entrySprite.fillSprite(COLOR_BG);
//	entrySprite.setTextPadding(0);
//	entrySprite.setTextColor(COLOR_TXT, COLOR_BG);
//	entrySprite.setTextDatum(ML_DATUM);
	entrySprite.drawString(album, 1, 11);
	entrySprite.pushSprite(0, 66);

//	entrySprite.createSprite(320, 22);
	entrySprite.fillSprite(COLOR_BG);
//	entrySprite.setTextPadding(0);
//	entrySprite.setTextColor(COLOR_TXT, COLOR_BG);
//	entrySprite.setTextDatum(ML_DATUM);
	entrySprite.drawString(artist, 1, 11);
	entrySprite.pushSprite(0, 88);

//	entrySprite.loadFont(NotoSans16);
//	entrySprite.createSprite(320, 22);
	entrySprite.fillSprite(COLOR_BG);
//	entrySprite.setTextPadding(0);
//	entrySprite.setTextColor(COLOR_TXT, COLOR_BG);
//	entrySprite.setTextDatum(ML_DATUM);
	// Check how to display values.
	entrySprite.drawNumber(track, 1, 11);
	entrySprite.pushSprite(0, 110);

	entrySprite.unloadFont();
	entrySprite.deleteSprite();

}

void display_playerProgress(uint16_t current, uint16_t total){
	uint8_t minutes = current / 60;
	uint8_t seconds = current % 60;
	uint8_t rMinutes = (total - current) / 60;
	uint8_t rSeconds = (total - current) % 60;

	String time = String(minutes) + String(':') + String(seconds / 10) + String(seconds % 10);

	entrySprite.loadFont(NotoSans15);
	entrySprite.createSprite(50, 22);
	entrySprite.fillSprite(COLOR_BG);
	entrySprite.setTextPadding(0);
	entrySprite.setTextColor(COLOR_TXT, COLOR_BG);
	entrySprite.setTextDatum(MR_DATUM);
	entrySprite.drawString(time, 37, 11);
	entrySprite.pushSprite(0, 200);

	display_vuMeter(((float)current / total), 50, 201, 220);

	time = String('-') + String(rMinutes) + String(':') + String(rSeconds / 10) + String(rSeconds % 10);

	entrySprite.fillSprite(COLOR_BG);
	entrySprite.setTextPadding(0);
	entrySprite.setTextColor(COLOR_TXT, COLOR_BG);
	entrySprite.setTextDatum(ML_DATUM);
	entrySprite.drawString(time, 3, 11);
	entrySprite.pushSprite(280, 200);

	entrySprite.unloadFont();
	entrySprite.deleteSprite();
}


// Todo : make a vu meter dedicated to battery, or change this one so the height is also customizable.
void display_vuMeter(float value, uint16_t x, uint16_t y, uint16_t width = 200){
	if(value < 0) value = 0;
	if(value > 1) value = 1;
	uint16_t dspValue = value * width;
	vuSprite.createSprite(width, 16);
	vuSprite.fillSprite(COLOR_BG);
	vuSprite.drawRoundRect(0, 0, width, 16, 3, TFT_DARKGREEN);
	vuSprite.fillRoundRect(1, 1, dspValue - 2, 14, 2, COLOR_BG_SELECT);
	vuSprite.pushSprite(x, y);

	vuSprite.deleteSprite();
}

void display_battery(float value){
	uint8_t width = 30;
	if(value < 0) value = 0;
	if(value > 1) value = 1;

	uint16_t dspValue = value * (width - 3);
	uint16_t color = tft.alphaBlend((value * 255), TFT_GREEN, TFT_RED);

	vuSprite.createSprite(width, 11);
	vuSprite.fillSprite(COLOR_HEADER);
	vuSprite.fillRoundRect(0, 0, width - 2, 11, 2, TFT_DARKGREY);
	vuSprite.drawRoundRect(0, 0, width - 2, 11, 2, TFT_DARKGREEN);
	vuSprite.drawFastVLine(width - 1, 3, 6, TFT_DARKGREEN);
	vuSprite.fillRoundRect(1, 1, dspValue, 9, 1, color);
	vuSprite.pushSprite(280, 5);

	vuSprite.deleteSprite();
}
