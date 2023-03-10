#include "../../pod.h"

bool state = 1;

Logger console = Logger();


void setup(){
	
	display_init();
	console.attachOutput(&consoleSprite);
	console.init(2000);

	delay(250);

	display_setTermMode();


//	digitalWrite(24, 1);
	hw_init();
	hw_setBacklight(255);
	hw_setDimTimeout();

	console.println("configuring SD card");

	SDFSConfig sdConf;
	sdConf.setAutoFormat(false);
	sdConf.setCSPin(13);
	sdConf.setSPISpeed(SD_SCK_MHZ(50));
	sdConf.setSPI(SPI1);
	// What is this one ? partitions we want to read, if there are several ones ?
//	sdConf.setPart(uint8_t);

	SDFS.setConfig(sdConf);

/*
	SPI1.setRX(8);
//	SPI1.setCS(9);
	SPI1.setSCK(10);
	SPI1.setTX(11);
*/
	SPI1.setRX(12);
//	SPI1.setCS(13);
	SPI1.setSCK(14);
	SPI1.setTX(15);

	if(SDFS.begin()){
//	if(SD.begin(13, SPI1)){
		console.println("SD mounted successfully");
	} else {
		console.println("SD mounted failed");
		for(;;);
	}

	console.println("parsing SD card");

	fs::File dir = SD.open("/", "r");
	parseFolder(&dir, 0);
	

	console.println("started !");
}

void loop(){

}

void parseFolder(fs::File *folder, uint8_t level){
	for(;;){
		for(uint8_t i = 0; i < level; ++i){
			console.print("  ");
		}
		fs::File file = folder->openNextFile();
		if(!file){
			console.println();
			break;
		}

		console.print(file.name());

		if(file.isDirectory()){
			console.println();
			parseFolder(&file, level + 1);
		} else {

		}
		console.println();
	}
}
