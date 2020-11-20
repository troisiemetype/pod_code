#ifndef ESPOD_AUDIO_H
#define ESPOD_AUDIO_H

#include <Arduino.h>
#include "esPod.h"

extern TFT_eSPI tft;

bool audio_getTag(fs::File *file);

#endif