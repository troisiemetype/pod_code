#ifndef ESPOD_AUDIO_H
#define ESPOD_AUDIO_H

// #include <Arduino.h>
// #include "menu.h"
#include "esPod.h"
// #include <FS.h>
#include "AudioFileSource.h"
#include "AudioFileSourceID3.h"
#include "AudioFileSourceFS.h"
#include "AudioGeneratorWAV.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

#define AUDIO_BUFFER_SIZE				29192

void audio_init();

bool audio_update();
void audio_int();

void audio_playTrack(MenuItem *item);

void audio_stop();

void audio_updateDisplay();

void audio_nextTrack();
void audio_prevTrack();
void audio_pause();

void audio_muteThread(void *params);
void audio_mute();
void audio_unmute();

bool audio_getTag(fs::File *file);

#endif