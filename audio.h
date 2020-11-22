#ifndef ESPOD_AUDIO_H
#define ESPOD_AUDIO_H

#include <Arduino.h>
#include "esPod.h"
#include <FS.h>
#include "AudioFileSource.h"
#include "AudioFileSourceID3.h"
#include "AudioFileSourceFS.h"
#include "AudioGeneratorWAV.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

bool audio_getTag(fs::File *file);

#endif