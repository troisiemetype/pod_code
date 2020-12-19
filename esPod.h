#ifndef ESPOD_H
#define ESPOD_H

#include <Arduino.h>
#include "TFT_eSPI.h"
#include <FS.h>
#include <SD_MMC.h>
#include <tinyxml2.h>
#ifdef ESP32
	#include <SPIFFS.h>
#endif
#include "Menu.h"
#include "PushButton.h"
#include "wheel.h"


#include "AudioTrackData.h"
#include "io.h"
#include "fonts.h"
#include "display.h"
#include "logger.h"
#include "files.h"
#include "data.h"
#include "menu.h"
#include "audio.h"

#endif