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
	Serial.printf("setup running on core %i\n", xPortGetCoreID());

	audioLogger = &Serial;

	log_d("Total heap: %d", ESP.getHeapSize());
	log_d("Free heap: %d", ESP.getFreeHeap());
	log_d("Total PSRAM: %d", ESP.getPsramSize());
	log_d("Free PSRAM: %d", ESP.getFreePsram());

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

	log_d("Total heap: %d", ESP.getHeapSize());
	log_d("Free heap: %d", ESP.getFreeHeap());
	audio_init();
	data_init();
//	log_d("data init");
	if(menu_init()){
		sandbox();
	}
	log_d("Total heap: %d", ESP.getHeapSize());
	log_d("Free heap: %d", ESP.getFreeHeap());

	data_empty();

	log_d("Total heap: %d", ESP.getHeapSize());
	log_d("Free heap: %d", ESP.getFreeHeap());

//	xTaskCreatePinnedToCore(audioLoop, "audioLoop", 10000, NULL, 1, NULL, 0);
	xTaskCreate(audioLoop, "audioLoop", 10000, NULL, 1, NULL);

}

void loop(){
//	audio_update();
	io_update();
	display_update();
}

void audioLoop(void *params){
	for(;;){
//		Serial.println("al");
		audio_update();
		vTaskDelay(1 / portTICK_PERIOD_MS);
	}
}

void displayLoop(void *params){
	
}