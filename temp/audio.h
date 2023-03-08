#ifndef ESPOD_AUDIO_H
#define ESPOD_AUDIO_H

// #include <Arduino.h>
// #include "menu.h"
#include "piPod.h"
// #include <FS.h>

//#include "Audio.h"

#define AUDIO_BUFFER_SIZE				29192

enum audioState_t{
	AUDIO_IDLE = 0,
	AUDIO_PARSING_TAGS,
	AUDIO_PLAY,
	AUDIO_MUTING,
	AUDIO_MUTE,
	AUDIO_UNMUTING,
	AUDIO_STOPPING,
};

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

audioState_t audio_getState();

#endif