#ifndef ESPOD_DATA_H
#define ESPOD_DATA_H

#include <Arduino.h>
#include "esPod.h"

struct levelState_t{
	tinyxml2::XMLElement *firstNode = NULL;
	tinyxml2::XMLElement *firstDispNode = NULL;
	tinyxml2::XMLElement *activeNode = NULL;
	tinyxml2::XMLElement *lastNode = NULL;
	tinyxml2::XMLElement *lastDispNode = NULL;
};

bool data_initXML();

bool data_makeDatabase();
void data_parseFolder(fs::File *file, uint8_t lvl = 0);

void data_menuSetLevel(tinyxml2::XMLElement* parent, bool update = true);

void data_menuWrite();
void data_menuEnter();
void data_menuExit();
void data_menuNext();
void data_menuPrev();

void data_getFileTags(void *cbData, const char *type, bool isUnicode, const char *string);

#endif