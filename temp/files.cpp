// #include "files.h"
#include "piPod.h"

// SD_MMC sdCard = SD_MMC();


void files_init(){	
	if(!SD_MMC.begin("")){

		tft.println(F("failed to open SD"));

	} else {
		tft.println(F("SD opened"));
	}

}