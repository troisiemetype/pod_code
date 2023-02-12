#include "esPod.h"

void sandbox(){
	delay(2000);
	tft.fillScreen(TFT_BLACK);
	tft.setCursor(0, 0);
//	tft.println("esPod could not be init...");
	for(;;);
}


void audioLoop(void *params){
	TickType_t last;
	last = xTaskGetTickCount();
	const TickType_t freq = 1 / portTICK_PERIOD_MS;
	for(;;){
//		Serial.println("al");
		audio_update();
		vTaskDelayUntil(&last, freq);
	}
}

void displayLoop(void *params){
	TickType_t last;
	last = xTaskGetTickCount();
	const TickType_t freq = 10 / portTICK_PERIOD_MS;
	for(;;){
		display_update();
		vTaskDelayUntil(&last, freq);
	}
	
}

void setup(){

	Serial.begin(115200);
	Serial.printf("setup running on core %i\n", xPortGetCoreID());

	log_d("starting...");
	log_d("Total heap: %d", ESP.getHeapSize());
	log_d("Free heap: %d", ESP.getFreeHeap());
	// no PSRAM on ESP32-WROOM !
//	log_d("Total PSRAM: %d", ESP.getPsramSize());
//	log_d("Free PSRAM: %d", ESP.getFreePsram());
	hw_init();
	io_init();
//	util_scanI2C();

	log_d("io init'd.");
	log_d("Free heap: %d", ESP.getFreeHeap());

	display_init();

	tft.println("Starting...");


	log_d("display init'd.");
	log_d("Free heap: %d", ESP.getFreeHeap());

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
	log_d("SD init'd.");
	log_d("Free heap: %d", ESP.getFreeHeap());

	// First need to create the song node of the menu, in which data will push every song in database.
	menu_init();
	log_d("menu init'd.");
	log_d("Free heap: %d", ESP.getFreeHeap());

	// We then need to init audio, because data parser will need the tag parsing for creating new database entry.
	audio_init();
	log_d("audio init'd.");
	log_d("Free heap: %d", ESP.getFreeHeap());

	// Parse database for listed, new and removed songs.
	data_init();
	log_d("data init'd.");
	log_d("Free heap: %d", ESP.getFreeHeap());

	// Then create the menu
	menu_makeMenu();

	hw_setBacklight(192);

	log_d("menu made.");
	log_d("enf of setup.");
	log_d("Total heap: %d", ESP.getHeapSize());
	log_d("Free heap: %d", ESP.getFreeHeap());


	attachInterrupt(39, io_updateReadButtons, FALLING);

	xTaskCreate(audioLoop, "audioLoop", 10000, NULL, 12, NULL);
//	xTaskCreate(displayLoop, "displayLoop", 10000, NULL, 11, NULL);
}

void loop(){
	io_update();
	display_update();
}
