#include "esPod.h"


AudioFileSourceFS *audioFile;
AudioFileSourceID3 *audioTags;
AudioGenerator *player;
AudioOutputI2S *audioOutput;

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

//	audioLogger = &tft;

	display_init();
//	display_setRunningMode();
//	tft.println("starting...");

//	tft.println("init display");

//	tft.println("init SD card");
/*
	for(uint8_t j = 0; j < 30; ++j){
		for(uint8_t i = 0; i < 53; ++i){
			tft.print('#');
		}
	}
*/
	// Check this for 1-bit mode :
	// sdmmc_slot_config_t slot = SDMMC_SLOT_CONFIG_DEFAULT();
	// slot.width = 1;
	// slot_config.width = 1;
//	sdmmc_host_set_bus_width(SDMMC_HOST_SLOT_1, 1);
//	sdmmc_slot_config_t slot;
//	slot.width = 0;
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

/*	tft.print("SD Card Type: ");
	if (cardType == CARD_MMC) {
		tft.println("MMC");
	} else if (cardType == CARD_SD) {
		tft.println("SDSC");
	} else if (cardType == CARD_SDHC) {
		tft.println("SDHC");
	} else {
		tft.println("UNKNOWN");
	}
*/
//	uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
//	tft.printf("SD Card Size: %lluMB\n", cardSize);



//	tft.println("looking for xml file on SD");
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
	data_init();
	if(menu_init()){
		sandbox();
	}
//	display_setRunningMode();
//	tft.println("esPod initialized");
//	delay(500);
//	display_setRunningMode();
//	display_makeHeader();
//	menu_menuWrite();
}

void loop(){
	io_update();
}
