#include "esPod.h"

// #include "display.h"

// default "terminal" font is 53 * 30 (1590) characters to fill screen

TFT_eSPI tft = TFT_eSPI();

uint8_t display_index = 0;
uint8_t display_topIndex = 0;
bool display_forceUpdate = true;
uint16_t pos = 0;
uint8_t display_maxMenuItem;

lineData_t *menuBuffer = NULL;

displayState_t displayState;

displayQueue_t displayQueue;
displayBuffer_t displayBuffer[DISPLAY_QUEUE_SIZE];
headerData_t displayHeaderData;
lineData_t displayMenuData[DISPLAY_MAX_MENU_ITEM * 2];
lineData_t *currentMenuData = displayMenuData;
playerData_t displayPlayerData;
timeData_t displayTimeData[3];
vuData_t displayVuData;

float displayBattery = 0;

TFT_eSprite menuSprite = TFT_eSprite(&tft);
TFT_eSprite entrySprite = TFT_eSprite(&tft);
TFT_eSprite thinEntrySprite = TFT_eSprite(&tft);
TFT_eSprite headerSprite = TFT_eSprite(&tft);

TFT_eSprite vuSprite = TFT_eSprite(&tft);
TFT_eSprite timeSprite = TFT_eSprite(&tft);

uint16_t *sprPtr = NULL;

theme_t theme = {
	0x528A,
	TFT_LIGHTGREY,
	TFT_DARKGREY,
	0xFC20,
	TFT_LIGHTGREY,
	TFT_BLACK,
	0x528A,
	0xFC20,
	0xD360,
	TFT_RED,
	0xD360,
	TFT_DARKGREEN,
	false
};

void display_init(){
	tft.init();
	tft.invertDisplay(1);
	tft.setRotation(1);
	tft.initDMA();
//	For memory only : TFT is the only peripheral on the SPI bus, with its CS pin permanentely tied to GND.
//	tft.startWrite();		

//	tft.setTextWrap(false, false);
//	tft.setCursor(3, 3);
	display_setTermMode();

	memset(&displayQueue, 0, sizeof(displayQueue_t));
	memset(displayBuffer, 0, sizeof(displayBuffer_t) * DISPLAY_QUEUE_SIZE);

	memset(displayMenuData, 0, sizeof(lineData_t) * DISPLAY_MAX_MENU_ITEM);
	memset(&displayHeaderData, 0, sizeof(headerData_t));
	memset(&displayPlayerData, 0, sizeof(playerData_t));
	memset(displayTimeData, 0, sizeof(timeData_t) * 3);
	memset(&displayVuData, 0, sizeof(vuData_t));

	displayQueue.buffer = displayBuffer;
	displayQueue.start = displayBuffer;
	displayQueue.end = displayBuffer;
	displayQueue.available = DISPLAY_QUEUE_SIZE;

	uint8_t limit = DISPLAY_QUEUE_SIZE - 1;

	for(uint8_t i = 0; i < limit; ++i){
		displayBuffer[i].next = &displayBuffer[i + 1];
	}
	displayBuffer[limit].next = displayBuffer;

	limit = (DISPLAY_MAX_MENU_ITEM * 2) - 1;
	for(uint8_t i = 0; i < limit; ++i){
		displayMenuData[i].next = &displayMenuData[i + 1];
	}
	displayMenuData[limit].next = displayMenuData;


	menuBuffer = new lineData_t[DISPLAY_MAX_MENU_ITEM];

	displayState = IDLE;
/*
	theme.headerBg = COLOR_HEADER;
	theme.headerTxt = COLOR_TXT;
	theme.bg = COLOR_BG;
	theme.bgSelect = COLOR_BG_SELECT;
	theme.txt = COLOR_TXT;
	theme.txtSelect = COLOR_TXT_SELECT;
	theme.vuBg = TFT_DARKGREY;
	theme.vuFg = COLOR_BG_SELECT;
	theme.vuCtr = TFT_DARKGREEN;
	theme.gradient = false;

	uint16_t orange = 0xFC20;
	uint16_t grey = 0x528A;
	uint16_t darkorange = 0xD360;
	theme.headerBg = grey;
	theme.headerTxt = TFT_LIGHTGREY;
	theme.bg = TFT_DARKGREY;
	theme.bgSelect = orange;
	theme.txt = TFT_LIGHTGREY;
	theme.txtSelect = TFT_BLACK;
	theme.vuBg = grey;
	theme.vuFg = orange;
	theme.vuCtr = darkorange;
	theme.batEmpty = TFT_RED;
	theme.batHalf = darkorange;
	theme.batFull = TFT_DARKGREEN;
	theme.gradient = false;
*/
	display_initSprites();

//	tft.println("esp pod");
//	tft.printf("line height : %i\n", tft.fontHeight());
//	hw_setBacklight(255);

/*
	for(uint8_t i = 0; i < 250; ++i){
		ledcWrite(TFT_LED_CH, i);
		delay(5);
	}
*/
}

void display_initSprites(){
	headerSprite.setColorDepth(16);
	headerSprite.loadFont(NotoSansBold15);
	headerSprite.createSprite(SCREEN_WIDTH, DISPLAY_HEADER_HEIGHT);
	headerSprite.fillSprite(theme.headerBg);
	headerSprite.setTextPadding(0);
	headerSprite.setTextColor(theme.txt, theme.headerBg);
	headerSprite.setTextDatum(ML_DATUM);

	menuSprite.setColorDepth(16);

	entrySprite.setColorDepth(16);
	entrySprite.loadFont(NotoSansBold16);
	entrySprite.createSprite(SCREEN_WIDTH, DISPLAY_MENU_LINE_HEIGHT);
	entrySprite.setTextPadding(0);
	entrySprite.setTextDatum(ML_DATUM);

	thinEntrySprite.setColorDepth(16);
	thinEntrySprite.loadFont(NotoSans16);
	thinEntrySprite.createSprite(SCREEN_WIDTH, DISPLAY_MENU_LINE_HEIGHT);
	thinEntrySprite.fillSprite(theme.bg);
	thinEntrySprite.setTextColor(theme.txt, theme.bg);
	thinEntrySprite.setTextPadding(0);
	thinEntrySprite.setTextDatum(ML_DATUM);

	vuSprite.setColorDepth(16);
//	vuSprite.createSprite(DISPLAY_VU_WIDTH, DISPLAY_VU_HEIGHT);
	vuSprite.fillSprite(theme.bg);
	vuSprite.drawRoundRect(0, 0, DISPLAY_VU_WIDTH, DISPLAY_VU_HEIGHT, 3, TFT_DARKGREEN);

	timeSprite.setColorDepth(16);
	timeSprite.createSprite(DISPLAY_TIME_WIDTH, DISPLAY_TIME_HEIGHT);
	timeSprite.loadFont(NotoSans15);
	timeSprite.setTextColor(theme.txt, theme.bg);
	timeSprite.setTextPadding(0);

}

void display_setState(displayState_t state){
	displayState = state;
}

theme_t display_getTheme(){
	return theme;
}

void display_setTheme(theme_t newTheme){
	theme = newTheme;
}

void display_update(){
	if(tft.dmaBusy()) return;

	tft.startWrite();
	_display_consumeBuffer();
//	tft.endWrite();
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
	tft.fillScreen(theme.bg);
	tft.setTextColor(theme.txt, theme.bg);
//	tft.loadFont(NotoSansBold16);
//	tft.printf("line height : %i\n", tft.fontHeight());
//	String filename = "system/fonts/NotoSansMono14";
//	tft.loadFont(filename, SD_MMC);
}

void display_pushClearDisplay(){
	display_pushMenuPadding(0);
}

void display_pushClearAll(){

}

void display_pushHeader(const char *header){
	displayHeaderData.name = header;
	_display_populateBuffer(_display_updateHeader, reinterpret_cast<void*>(&displayHeaderData));
	display_pushBattery(displayBattery);
}

// TODO : Make a circular buffer for menu entry as well.
void display_pushMenu(const char *name, bool active, uint8_t index){
//	lineData_t *bf = &displayMenuData[index];
	currentMenuData = currentMenuData->next;
	lineData_t *bf = currentMenuData;
	bf->name = name;
	bf->active = active;
	bf->pos = DISPLAY_MENU_LINE_HEIGHT * (index + 1);

//	Serial.printf("pushing menu %s, active : %i; pos : %i\n", bf->name, bf->active, bf->pos);

	_display_populateBuffer(_display_updateLine, reinterpret_cast<void*>(bf));
}

void display_pushMenuPadding(uint8_t index){
	for(uint8_t i = index; i < DISPLAY_MAX_MENU_ITEM; ++i){
		display_pushMenu("\0", 0, i);
	}
}

void display_pushPlayer(const char *artist, const char *album, const char *song, uint8_t track){
/*
	displayPlayerData.artist = artist;
	displayPlayerData.album = album;
	displayPlayerData.name = song;
	displayPlayerData.track = track;
*/
	currentMenuData = currentMenuData->next;
	lineData_t *bf = currentMenuData;
	bf->name = song;
	bf->active = 0;
	bf->pos = 44;
//	Serial.printf(" pushing %s\n", bf->name);
	_display_populateBuffer(_display_updateLine, reinterpret_cast<void*>(bf));

	currentMenuData = currentMenuData->next;
	bf = currentMenuData;
	bf->name = album;
	bf->active = 0;
	bf->pos = 66;
//	Serial.printf(" pushing %s\n", bf->name);
	_display_populateBuffer(_display_updateLineThin, reinterpret_cast<void*>(bf));

	currentMenuData = currentMenuData->next;
	bf = currentMenuData;
	bf->name = artist;
	bf->active = 0;
	bf->pos = 88;
//	Serial.printf(" pushing %s\n", bf->name);
	_display_populateBuffer(_display_updateLineThin, reinterpret_cast<void*>(bf));

	currentMenuData = currentMenuData->next;
	bf = currentMenuData;
// Find a way to display the track number !
	bf->name = "\0";
	bf->active = 0;
	bf->pos = 110;
	_display_populateBuffer(_display_updateLineThin, reinterpret_cast<void*>(bf));
}

void display_pushPlayerProgress(uint16_t current, uint16_t total){
	if(displayState != PLAYER) return;
	timeData_t *bf = &displayTimeData[1];
	bf->current = current;
	bf->total = total;
	bf->type = CURRENT;
	bf->x = 0;
	bf->y = 200;
	_display_populateBuffer(_display_updateTime, reinterpret_cast<void*>(bf));

	bf = &displayTimeData[2];
	bf->current = current;
	bf->total = total;
	bf->type = REMAINING;
	bf->x = 270;
	bf->y = 200;
	_display_populateBuffer(_display_updateTime, reinterpret_cast<void*>(bf));

	displayVuData.value = (float)current / total;
	displayVuData.x = 50;
	displayVuData.y = 202;
	displayVuData.width = 220;
	displayVuData.height = 15;

	_display_populateBuffer(_display_updateVuMeter, reinterpret_cast<void*>(&displayVuData));
}

void display_pushPlayerVolume(uint8_t volume){
	if(displayState != PLAYER) return;
	displayPlayerData.volume = volume;
	_display_populateBuffer(_display_updatePlayerVolume, reinterpret_cast<void*>(&displayPlayerData));

}

void display_pushVUMeter(float value, uint16_t x, uint16_t y, uint16_t width, uint16_t height){
	displayVuData.value = value;
	displayVuData.x = x;
	displayVuData.y = y;
	displayVuData.width = width;
	displayVuData.height = height;

	_display_populateBuffer(_display_updateVuMeter, reinterpret_cast<void*>(&displayVuData));
}

void display_pushBattery(float value){
	displayBattery = value;
	_display_populateBuffer(_display_updateBattery, reinterpret_cast<void*>(&displayBattery));
}

void _display_populateBuffer(void (*fn)(void*), void *data){
	if(displayQueue.available <= 0){
//		Serial.printf("buffer unavailable ! Losing %i", (uint32_t)(currentMenuData));
		return;
	}
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

void _display_updateClearDisplay(void *data){

}

void _display_updateClearAll(void *data){

}

void _display_updateHeader(void *data){
	sprPtr = reinterpret_cast<uint16_t*>(headerSprite.getPointer());

	headerSprite.fillSprite(theme.headerBg);
	headerSprite.setTextColor(theme.txt, theme.headerBg);
	headerSprite.drawString(displayHeaderData.name, 2, 11);
	tft.pushImageDMA(0, 0, SCREEN_WIDTH, DISPLAY_HEADER_HEIGHT, sprPtr);

}

void _display_updateLine(void *data){
	lineData_t *bf = reinterpret_cast<lineData_t*>(data);
	sprPtr = reinterpret_cast<uint16_t*>(entrySprite.getPointer());

//	Serial.printf("displaying menu %s, active : %i; pos : %i\n", bf->name, bf->active, bf->pos);
//	Serial.printf("new line : %s\n", bf->name);
	entrySprite.fillSprite(bf->active ? theme.bgSelect : theme.bg);
	entrySprite.setTextColor(bf->active ? theme.txtSelect : theme.txt, bf->active ? theme.bgSelect : theme.bg);
	entrySprite.drawString(bf->name, 1, 11);
	tft.pushImageDMA(0, bf->pos, SCREEN_WIDTH, DISPLAY_MENU_LINE_HEIGHT, sprPtr);
}

void _display_updateLineThin(void *data){
	lineData_t *bf = reinterpret_cast<lineData_t*>(data);
	sprPtr = reinterpret_cast<uint16_t*>(thinEntrySprite.getPointer());	

//	Serial.printf("new thin line : %s\n", bf->name);
	thinEntrySprite.fillSprite(theme.bg);
	thinEntrySprite.drawString(bf->name, 1, 11);
	tft.pushImageDMA(0, bf->pos, SCREEN_WIDTH, DISPLAY_MENU_LINE_HEIGHT, sprPtr);

}

void _display_updatePlayerProgress(void *data){

}

void _display_updatePlayerVolume(void *data){

}

void _display_updateTime(void *data){
	timeData_t *bf = reinterpret_cast<timeData_t*>(data);
	sprPtr = reinterpret_cast<uint16_t*>(timeSprite.getPointer());
	uint8_t minutes = bf->current / 60;
	uint8_t seconds = bf->current % 60;

	if(bf->type == REMAINING){
		minutes = (bf->total - bf->current) / 60;
		seconds = (bf->total - bf->current) % 60;
	} else if(bf->type == TOTAL){
		minutes = bf->total / 60;
		seconds = bf->total % 60;
	}

	String secs = String(seconds / 10) + String(seconds % 10);

	timeSprite.fillSprite(theme.bg);
	timeSprite.setTextDatum(MR_DATUM);
	if(bf->type == REMAINING) timeSprite.drawString("-", 8, 11);
	timeSprite.drawNumber(minutes, 23, 11);
	timeSprite.setTextDatum(MC_DATUM);
	timeSprite.drawString(":", 25, 11);
	timeSprite.setTextDatum(ML_DATUM);
	timeSprite.drawString(secs, 28, 11);
	tft.pushImageDMA(bf->x, bf->y, DISPLAY_TIME_WIDTH, DISPLAY_TIME_HEIGHT, sprPtr);
}

void _display_updateVuMeter(void *data){
	vuData_t *bf = reinterpret_cast<vuData_t*>(data);
	if(bf->value < 0) bf->value = 0;
	if(bf->value > 1) bf->value = 1;
	uint16_t dspValue = bf->value * bf->width;

	vuSprite.deleteSprite();

	sprPtr = reinterpret_cast<uint16_t*>(vuSprite.createSprite(bf->width, bf->height));	

	vuSprite.fillSprite(theme.bg);
	vuSprite.drawRoundRect(0, 0, bf->width, bf->height, 3, theme.vuCtr);
	vuSprite.fillRoundRect(1, 1, dspValue - 2, bf->height - 2, 2, theme.bgSelect);
	tft.pushImageDMA(bf->x, bf->y, bf->width, bf->height, sprPtr);
}

void _display_updateBattery(void *data){
	uint8_t width = 30;
	float value = displayBattery;
	if(value < 0) value = 0;
	if(value > 1) value = 1;

	uint16_t dspValue = value * (width - 3);
	uint16_t color;
	if(value < 0.25){
		value *= 4;
		color = tft.alphaBlend((value * 255), theme.batHalf, theme.batEmpty);
	} else {
		value -= 0.25;
		value *= 1.3;
		color = tft.alphaBlend((value * 255), theme.batFull, theme.batHalf);
	}

	vuSprite.deleteSprite();
	sprPtr = reinterpret_cast<uint16_t*>(vuSprite.createSprite(width, 11));	

	vuSprite.fillSprite(theme.headerBg);
	vuSprite.fillRoundRect(0, 0, width - 2, 11, 2, theme.vuBg);
	vuSprite.drawRoundRect(0, 0, width - 2, 11, 2, theme.vuCtr);
	vuSprite.drawFastVLine(width - 1, 3, 5, theme.vuCtr);
	vuSprite.fillRoundRect(1, 1, dspValue, 9, 1, color);
	tft.pushImageDMA(280, 5, width, 11, sprPtr);
}
