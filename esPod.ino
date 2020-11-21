#include "esPod.h"

TFT_eSPI tft = TFT_eSPI();

AudioFileSourceFS *audioFile;
AudioFileSourceID3 *audioTags;
AudioGenerator *player;
AudioOutputI2S *audioOutput;

void sandbox(){
	delay(2000);
	tft.fillScreen(TFT_BLACK);
	tft.setCursor(0, 0);
	tft.println("esPod could not be init...");
	for(;;);
}

void setup(){

	io_initIO();

	Serial.begin(115200);
//	audioLogger = &tft;

	display_initBacklight();
	display_init();
//	display_setRunningMode();
	tft.println("starting...");

	tft.println("init display");

	tft.println("init SD card");

	if (!SD_MMC.begin("")) {
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

	tft.print("SD Card Type: ");
	if (cardType == CARD_MMC) {
		tft.println("MMC");
	} else if (cardType == CARD_SD) {
		tft.println("SDSC");
	} else if (cardType == CARD_SDHC) {
		tft.println("SDHC");
	} else {
		tft.println("UNKNOWN");
	}

	uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
	tft.printf("SD Card Size: %lluMB\n", cardSize);



	tft.println("looking for xml file on SD");
/*	menuFile = new fs::File();
	*menuFile = SD_MMC.open("/system/menu/menu.xml");
	if(!menuFile){
		tft.println("failed to find menu xml file");
	} else {
		tft.println("ok");
	}
*/

//	tft.println("init music vars");
	audioOutput = new AudioOutputI2S();
	audioOutput->SetPinout(16, 22, 21);
	audioFile = new AudioFileSourceFS(SD_MMC);
	audioTags = new AudioFileSourceID3(audioFile);
	player = new AudioGeneratorMP3();

//	audiometa_getTag("/music/tetris/tetris-gameboy-02.mp3");
	io_initButtons();
	data_init();
	if(menu_init()){
		sandbox();
	}
//	tft.println("esPod initialized");
	delay(500);
//	display_setRunningMode();
//	display_makeHeader();
//	menu_menuWrite();
}

void loop(){
	/*
	if(player->isRunning()){
		if(!player->loop()) player->stop();
	}
	*/
	uint8_t state = io_updateButtons();

	if(state & (1 << 1)){
		if(io_justReleased(1)){
			menu_prev();
//			menu_menuWrite();
		}
	} else if(state & (1 << 2)){
		if(io_justReleased(2)){
			menu_enter();
//			menu_menuWrite();
		}
	} else if(state & (1 << 3)){
		if(io_justReleased(3)){
			menu_next();
//			menu_menuWrite();
		}
	} else if(state & (1 << 4)){
		if(io_justReleased(4)){
			menu_exit();
//			menu_menuWrite();
		}
	}
}
