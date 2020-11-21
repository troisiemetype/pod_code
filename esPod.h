#ifndef ESPOD_H
#define ESPOD_H

#include <Arduino.h>
#include "TFT_eSPI.h"
#include "FS.h"
#include <SD_MMC.h>
#include <tinyxml2.h>
#ifdef ESP32
	#include "SPIFFS.h"
#endif

#include "AudioFileSource.h"
#include "AudioFileSourceID3.h"
#include "AudioFileSourceFS.h"
#include "AudioGeneratorWAV.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"


#include "io.h"
#include "fonts.h"
#include "display.h"
#include "files.h"
#include "data.h"
#include "menu.h"
#include "music.h"
#include "audio.h"

#endif