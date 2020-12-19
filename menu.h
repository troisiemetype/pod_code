#ifndef ESPOD_MENU_H
#define ESPOD_MENU_H

// #include <Arduino.h>
// #include <FS.h>
#include "esPod.h"

// #include "Menu.h"

bool menu_init();
bool menu_createMusicSongs(MenuList *list, tinyxml2::XMLElement *currentNode);
bool menu_createMusic(MenuList *list, MenuList *album, MenuList *ref);

int16_t menu_sortByName(MenuItem *a, MenuItem *b);
int16_t menu_sortByArtist(MenuItem *a, MenuItem *b);
int16_t menu_sortByAlbum(MenuItem *a, MenuItem *b);
int16_t menu_sortByYear(MenuItem *a, MenuItem *b);
int16_t menu_sortByTrack(MenuItem *a, MenuItem *b);
int16_t menu_sortBySet(MenuItem *a, MenuItem *b);
int16_t menu_sortRank(MenuItem *a, MenuItem *b);

void menu_write(MenuList *list);
void menu_update();
void menu_enter();
void menu_exit();
void menu_next();
void menu_prev();

void menu_test();
void menu_testExit();
void menu_testValuePlus();
void menu_testValueMinus();
void menu_testValueMiddle();
void menu_cbTest(void *empty);

void menu_cbList(void *list);
void menu_cbSong(void *data);
void menu_cbPlayer(void *data);

const char* menu_getXMLTextField(tinyxml2::XMLElement *node, const char *field);
int16_t menu_getXMLNumberField(tinyxml2::XMLElement *node, const char *field);

class MenuSong : public MenuItem{
public:
	MenuSong();
	~MenuSong();

	void setFilename(const char *name);
	const char* getFilename();

	void setArtist(const char *name);
	const char* getArtist();

	void setAlbum(const char *name);
	const char* getAlbum();

	void setYear(uint16_t year);
	uint16_t getYear();

	void setTrack(uint8_t track);
	uint8_t getTrack();

	void setSet(uint8_t set);
	uint8_t getSet();

	void setCompilation(bool compilation);
	bool getCompilation();

	void setPop(uint8_t rank);
	uint8_t getPop();

//	virtual void exec();

private:
	char *_filename;
	char *_artist;
	char *_album;
	uint16_t _year;
	uint8_t _track;
	uint8_t _set;
	bool _compilation;
	uint8_t _rank;

};

enum dataError{
	DATA_OK = 0,
	DATA_FILE_ERR,
};


#endif