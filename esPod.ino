#include "esPod.h"

void sandbox(){
	delay(2000);
	tft.fillScreen(TFT_BLACK);
	tft.setCursor(0, 0);
//	tft.println("esPod could not be init...");
	for(;;);
}

void setup(){

	Serial.begin(115200);

	io_init();
	display_init();

	if (!SD_MMC.begin("", true)) {
//	if (!SD_MMC.begin("")) {
		tft.println("Card Mount Failed");
		sandbox();
		return;
	}
	uint8_t cardType = SD_MMC.cardType();

	if (cardType == CARD_NONE) {
		tft.println("No SD card attached");
		sandbox();
		return;
	}

	audio_init();
	data_init();
	if(menu_init()){
		sandbox();
	}
}

void loop(){
	audio_update();
	io_update();
}
