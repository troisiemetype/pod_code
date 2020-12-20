#ifndef ESPOD_DATA_H
#define ESPOD_DATA_H

// #include <Arduino.h>
// #include <FS.h>
#include "esPod.h"

void data_init();

void data_readDatabase(fs::File *dir);
void data_checkNewFiles();
void data_parseFolder(fs::File *file, uint8_t lvl = 0);
void data_checkSong(fs::File *file);

void data_readAudioTrackData(fs::File *file, AudioTrackData *track);
void data_writeAudioTrackData(AudioTrackData *track);

void data_getFileTags(void *cbData, const char *type, bool isUnicode, const char *string);
/*
theme_t* data_getTheme(theme_t *theme);
void data_updateTheme(const char *name);
*/

#endif