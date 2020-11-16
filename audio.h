#ifndef ESPOD_AUDIO_H
#define ESPOD_AUDIO_H

#include <Arduino.h>
#include "esPod.h"

extern TFT_eSPI tft;
void audio_tagCB(void *cbData, const char *type, bool isUnicode, const char *string);

bool audio_getTag(fs::File *file);

#endif