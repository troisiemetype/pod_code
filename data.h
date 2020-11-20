#ifndef ESPOD_DATA_H
#define ESPOD_DATA_H

#include <Arduino.h>
#include <FS.h>
#include "esPod.h"

#include "Menu.h"

bool data_initMenuStructure();
const char* data_getXMLField(tinyxml2::XMLElement *node, const char *field);
bool data_createMenuMusicSongs(MenuList* list, tinyxml2::XMLElement *currentNode);
bool data_createMenuMusicAlbums(MenuList* list, tinyxml2::XMLElement *currentNode);
bool data_createMenuMusicArtists(MenuList* list, tinyxml2::XMLElement *currentNode);
bool data_createMenuMusicArtistsAlbums(MenuList* list, tinyxml2::XMLElement *currentNode);

bool data_listFiles();
void data_parseFolder(fs::File *file, uint8_t lvl = 0);

void data_menuWrite(MenuList *list);
void data_menuEnter();
void data_menuExit();
void data_menuNext();
void data_menuPrev();

void data_menuListCB(void* list);
void data_menuSongCB(void *data);
void data_getFileTags(void *cbData, const char *type, bool isUnicode, const char *string);

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

	virtual void exec();

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