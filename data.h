#ifndef ESPOD_DATA_H
#define ESPOD_DATA_H

#include <Arduino.h>
#include <FS.h>
#include "esPod.h"

void data_init();

bool data_listFiles();
void data_parseFolder(fs::File *file, uint8_t lvl = 0);

bool data_checkExistingFiles();
bool data_checkNewFiles();

void data_addSong(fs::File *file);

tinyxml2::XMLElement* data_getSongList();

void data_getFileTags(void *cbData, const char *type, bool isUnicode, const char *string);


#endif