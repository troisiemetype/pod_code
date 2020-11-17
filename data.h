#ifndef ESPOD_DATA_H
#define ESPOD_DATA_H

#include <Arduino.h>
#include "esPod.h"

enum dataError{
	DATA_OK = 0,
	DATA_FILE_ERR,
};

struct levelState_t{
	tinyxml2::XMLElement *firstNode = NULL;
	tinyxml2::XMLElement *firstDispNode = NULL;
	tinyxml2::XMLElement *activeNode = NULL;
	tinyxml2::XMLElement *lastNode = NULL;
	tinyxml2::XMLElement *lastDispNode = NULL;
};


/*
 *	Defining classes for handling menus. This should be a library.
 *		Needs for a menu list :			name
 *										list of children ()
 *										Active child
 *									display options :
 *										first child
 *		Basic need for a menu item :	name
 *										parent
 *										function 	(what to do when the menu is entered)
 *										item / filename / etc.
 *										
 */
struct MenuItem;

struct MenuList {
	char *name;
	MenuItem *children;
	MenuItem *activeChild;
	MenuItem *firstDisplayedChild;
};

struct MenuItem{
	char *name;
	MenuList *parent;
	// filename
	// Callback function
};

bool data_initXML();

bool data_makeDatabase();
bool data_listFiles();
bool data_populateArtists(tinyxml2::XMLElement *node, tinyxml2::XMLElement *ref, const char *filter);
bool data_sortMenu();
void data_parseFolder(fs::File *file, uint8_t lvl = 0);

void data_menuSetLevel(tinyxml2::XMLElement* parent, bool update = true);

void data_menuWrite();
void data_menuEnter();
void data_menuExit();
void data_menuNext();
void data_menuPrev();

void data_getFileTags(void *cbData, const char *type, bool isUnicode, const char *string);

#endif