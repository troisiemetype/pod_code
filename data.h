#ifndef ESPOD_DATA_H
#define ESPOD_DATA_H

#include <Arduino.h>
#include <FS.h>
#include "esPod.h"

void data_init();

bool data_listFiles();
void data_parseFolder(fs::File *file, uint8_t lvl = 0);

tinyxml2::XMLElement* data_getSongList();

void data_getFileTags(void *cbData, const char *type, bool isUnicode, const char *string);


#endif