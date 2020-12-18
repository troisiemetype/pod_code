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

menuData_t *menuBuffer = NULL;

displayState_t displayState;

displayQueue_t displayQueue;
displayBuffer_t displayBuffer[DISPLAY_QUEUE_SIZE];
headerData_t displayHeaderData;
menuData_t displayMenuData[DISPLAY_MAX_MENU_ITEM];
playerData_t displayPlayerData;

TFT_eSprite menuSprite = TFT_eSprite(&tft);
TFT_eSprite entrySprite = TFT_eSprite(&tft);
TFT_eSprite headerSprite = TFT_eSprite(&tft);

TFT_eSprite vuSprite = TFT_eSprite(&tft);

uint16_t *sprPtr = NULL;

void display_init(){
	display_initBacklight();
	tft.init();
	tft.setRotation(1);
	tft.initDMA();
//	For memory only : TFT is the only peripheral on the SPI bus, with its CS pin permanentely tied to GND.
//	tft.startWrite();		

//	tft.setTextWrap(false, false);
//	tft.setCursor(3, 3);
	display_setTermMode();

	memset(&displayQueue, 0, sizeof(displayQueue_t));
	memset(displayBuffer, 0, sizeof(displayBuffer_t) * DISPLAY_QUEUE_SIZE);

	memset(displayMenuData, 0, sizeof(menuData_t) * DISPLAY_MAX_MENU_ITEM);
	memset(&displayHeaderData, 0, sizeof(headerData_t));
	memset(&displayPlayerData, 0, sizeof(playerData_t));

	displayQueue.buffer = displayBuffer;
	displayQueue.start = displayBuffer;
	displayQueue.end = displayBuffer;
	displayQueue.available = DISPLAY_QUEUE_SIZE;

	uint8_t limit = DISPLAY_QUEUE_SIZE - 1;

	for(uint8_t i = 0; i < limit; ++i){
		displayBuffer[i].next = &displayBuffer[i + 1];
	}
	displayBuffer[limit].next = displayBuffer;


	menuBuffer = new menuData_t[DISPLAY_MAX_MENU_ITEM];

	displayState = IDLE;

	display_initSprites();

//	tft.println("esp pod");
//	tft.printf("line height : %i\n", tft.fontHeight());

	for(uint8_t i = 0; i < 250; ++i){
		ledcWrite(TFT_LED_CH, i);
		delay(5);
	}
}

void display_initSprites(){
	headerSprite.setColorDepth(16);
	headerSprite.loadFont(NotoSansBold15);
	headerSprite.createSprite(SCREEN_WIDTH, DISPLAY_HEADER_HEIGHT);
	headerSprite.fillSprite(COLOR_HEADER);
	headerSprite.setTextPadding(0);
	headerSprite.setTextColor(COLOR_TXT, COLOR_HEADER);
	headerSprite.setTextDatum(ML_DATUM);

	menuSprite.setColorDepth(16);

	entrySprite.setColorDepth(16);
	entrySprite.loadFont(NotoSansBold16);
	entrySprite.createSprite(SCREEN_WIDTH, DISPLAY_MENU_LINE_HEIGHT);
	entrySprite.setTextPadding(0);
	entrySprite.setTextDatum(ML_DATUM);

	vuSprite.setColorDepth(16);
	vuSprite.createSprite(DISPLAY_VU_WIDTH, DISPLAY_VU_HEIGHT);
	vuSprite.fillSprite(COLOR_BG);
	vuSprite.drawRoundRect(0, 0, DISPLAY_VU_WIDTH, DISPLAY_VU_HEIGHT, 3, TFT_DARKGREEN);

}

void display_initBacklight(){
	ledcSetup(TFT_LED_CH, TFT_LED_FREQ, TFT_LED_RES);
	ledcAttachPin(TFT_LED, TFT_LED_CH);
	ledcWrite(TFT_LED_CH, 0);
}

void display_setBackLight(uint8_t value){
	ledcWrite(TFT_LED_CH, value);
}

void display_update(){
	if(tft.dmaBusy()) return;

	tft.startWrite();
	_display_consumeBuffer();
//	tft.endWrite();
}

void display_updateClearDisplay(void *data){

}

void display_clearAll(void *data){

}

void display_updateHeader(void *data){
	sprPtr = reinterpret_cast<uint16_t*>(headerSprite.getPointer());

	headerSprite.fillSprite(COLOR_HEADER);
	headerSprite.setTextColor(COLOR_TXT, COLOR_HEADER);
	headerSprite.drawString(displayHeaderData.name, 2, 11);
	tft.pushImageDMA(0, 0, SCREEN_WIDTH, DISPLAY_HEADER_HEIGHT, sprPtr);

}

void display_updateMenu(void *data){
	menuData_t *bf = reinterpret_cast<menuData_t*>(data);
	sprPtr = reinterpret_cast<uint16_t*>(entrySprite.getPointer());

//	Serial.printf("displaying menu %s, active : %i; pos : %i\n", bf->name, bf->active, bf->pos);

	entrySprite.fillSprite(bf->active ? COLOR_BG_SELECT : COLOR_BG);
	entrySprite.setTextColor(bf->active ? COLOR_TXT_SELECT : COLOR_TXT, bf->active ? COLOR_BG_SELECT : COLOR_BG);
	entrySprite.drawString(bf->name, 1, 11);
	tft.pushImageDMA(0, bf->pos, SCREEN_WIDTH, DISPLAY_MENU_LINE_HEIGHT, sprPtr);

	/*

	if(display_index > display_topIndex){
		tft.fillRect(0, pos, 320, 240-pos, COLOR_BG);
		display_forceUpdate = false;
		displayState = IDLE;
	}
	*/
}

void display_updatePlayer(void *data){

}

void display_updatePlayerProgress(void *data){

}

void display_updatePlayerVolume(void *data){

}


uint8_t display_getMaxMenuItem(){
	return DISPLAY_MAX_MENU_ITEM;
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

void display_pushHeader(const char *header){
	displayHeaderData.name = header;
	_display_populateBuffer(display_updateHeader, reinterpret_cast<void*>(&displayHeaderData));
}

void display_pushMenu(const char *name, bool active, uint8_t index){
	menuData_t *bf = &displayMenuData[index];
	bf->name = name;
	bf->active = active;
	bf->pos = DISPLAY_MENU_LINE_HEIGHT * (index + 1);

	Serial.printf("pushing menu %s, active : %i; pos : %i\n", bf->name, bf->active, bf->pos);

	_display_populateBuffer(display_updateMenu, reinterpret_cast<void*>(bf));
}

void display_pushMenuPadding(uint8_t index){
	for(uint8_t i = index; i < DISPLAY_MAX_MENU_ITEM; ++i){
		display_pushMenu("", 0, i);
	}
}

void display_pushPlayer(const char *artist, const char *album, const char *song, uint8_t track){
	displayPlayerData.artist = artist;
	displayPlayerData.album = album;
	displayPlayerData.name = song;
	displayPlayerData.track = track;

	_display_populateBuffer(display_updatePlayer, reinterpret_cast<void*>(&displayPlayerData));
}

void display_pushPlayerProgress(uint16_t current, uint16_t total){
	displayPlayerData.currentTime = current;
	displayPlayerData.totalTime = total;

	_display_populateBuffer(display_updatePlayerProgress, reinterpret_cast<void*>(&displayPlayerData));
}

void display_pushPlayerVolume(uint8_t volume){
	displayPlayerData.volume = volume;
	_display_populateBuffer(display_updatePlayerVolume, reinterpret_cast<void*>(&displayPlayerData));

}

void _display_populateBuffer(void (*fn)(void*), void *data){
	if(displayQueue.available <= 0) return;
//	Serial.printf("populating buffer %i\n", displayQueue.size);

	displayBuffer_t *bf = displayQueue.end;
	bf->cbFn = fn;
	bf->data = data;
	displayQueue.end = bf->next;
	displayQueue.size++;
	displayQueue.available--;

}

void _display_consumeBuffer(){
	if(displayQueue.size <= 0) return;
//	Serial.printf("consuming buffer %i\n", displayQueue.size);

	displayBuffer_t *bf = displayQueue.start;
	bf->cbFn(bf->data);
	displayQueue.start = bf->next;
	displayQueue.size--;
	displayQueue.available++;
}



/*
void display_clearDisplay(){
	tft.fillRect(0, 20, 320, 220, COLOR_BG);	
}

void display_clearAll(){
	tft.fillScreen(COLOR_BG);
	display_forceUpdate = true;
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
//	log_d("pushing to menu");
	// First check that index is not out of bounds.
	if(index > display_maxMenuItem) return;
	display_topIndex = index;

	menuData_t *entry = &menuBuffer[index];
//	log_d("local pointer done");
//	menuData_t *entry = menuBuffer + index;
	// Then compare the new menu entry name with the previous one, so we dont do unnecessary copying.
	if(entry->name == name){
//		log_d("same name");
		if(entry->active != active){
			entry->active = active;
			entry->update = 1;
		} else {
			entry->update = 0;
		}
	} else {
//		log_d("populate buffer");
		entry->name = (char*)name;
		entry->active = active;
		entry->update = 1;
	}
}

void display_makeMenuEntry(const char *name, bool active){
	entrySprite.loadFont(NotoSansBold16);
	sprPtr = reinterpret_cast<uint16_t*>(entrySprite.createSprite(320, 22));
	entrySprite.fillSprite(active ? COLOR_BG_SELECT : COLOR_BG);
	entrySprite.setTextPadding(0);
	entrySprite.setTextColor(active ? COLOR_TXT_SELECT : COLOR_TXT, active ? COLOR_BG_SELECT : COLOR_BG);
	entrySprite.setTextDatum(ML_DATUM);
	entrySprite.drawString(name, 1, 11);
	tft.pushImageDMA(0, pos, 320, 22, sprPtr);
//	entrySprite.pushSprite(0, pos);
	entrySprite.unloadFont();
	entrySprite.deleteSprite();

//	pos += 22;
//	display_index++;

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
*/