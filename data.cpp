#include "data.h"

// using namespace tinyxml2;

extern AudioGenerator *player;

fs::File *menuFile = NULL;
tinyxml2::XMLDocument menuTree;
tinyxml2::XMLElement *root = NULL;
tinyxml2::XMLElement *currentNode = NULL;

int32_t *fileID = NULL;

const uint8_t maxLevel = 6;
uint8_t level = 0;

uint8_t maxMenuItem = 10;

MenuList *menu;

// Create main menu, call subroutines to parse levels.
bool data_initMenuStructure(){
	if(!SD_MMC.exists("/system/data/songs.xml")){
		data_listFiles();
	}

	tft.println("creating menu");

	menu = new MenuList();

	menu->setDisplaySize(maxMenuItem);
	menu->setName("esPod");
	menu->attachCallback(data_menuListCB, NULL);

	MenuItem *i = new MenuItem();
	i->setName("playing");
	menu->addChild(i);

	MenuList *music = new MenuList();
	music->attachCallback(data_menuListCB, NULL);
	music->setName("music");
	menu->addChild(music);

	MenuList *artists = new MenuList();
	artists->setName("artists");
	music->addChild(artists);

	MenuList *albums = new MenuList();
	albums->setName("albums");
	music->addChild(albums);

	MenuList *songs = new MenuList();
	songs->attachCallback(data_menuListCB, NULL);
	songs->setName("songs");
	music->addChild(songs);

	MenuList *compilations = new MenuList();
	compilations->setName("compilations");
	music->addChild(compilations);

	MenuList *playlists = new MenuList();
	playlists->setName("playlists");
	music->addChild(playlists);


	MenuList *settings = new MenuList();
	settings->attachCallback(data_menuListCB, NULL);
	settings->setName("settings");
	menu->addChild(settings);

	MenuList *brightness = new MenuList();
	brightness->setName("brightness");
	settings->addChild(brightness);

	MenuList *maxvolume = new MenuList();
	maxvolume->setName("maxvolume");
	settings->addChild(maxvolume);

	MenuList *legal = new MenuList();
	legal->setName("legal");
	settings->addChild(legal);

	MenuList *update = new MenuList();
	update->setName("update");
	settings->addChild(update);

	MenuList *equalizer = new MenuList();
	equalizer->setName("equalizer");
	settings->addChild(equalizer);

	tinyxml2::XMLDocument songData;
	if(songData.LoadFile("/system/data/songs.xml")){
		return 1;
	}
	tinyxml2::XMLElement *currentNode = songData.RootElement()->FirstChildElement();
//	tinyxml2::XMLHandle *handle = new tinyxml2::XMLHandle(currentNode);

	data_createMenuMusicSongs(songs, currentNode);
/*
	tft.printf("size of MenuItem : %i\n", sizeof(*i));
	tft.printf("size of MenuList : %i\n", sizeof(*menu));
	delay(3000);
*/

//	return 1;

	display_setRunningMode();
	display_makeHeader();

	data_menuWrite(menu);
	return 0;

}

const char* data_getXMLField(tinyxml2::XMLElement *node, const char *field){
	const char *name = node->FirstChildElement(field)->GetText();
	if(name) return name;
	return " ";
}

// Create the music/song menu, that will be the base for the other ones.
bool data_createMenuMusicSongs(MenuList *list, tinyxml2::XMLElement *currentNode){
	tft.println("parsing songs database");
	MenuSong *song;
	for(;;){
//		const char *name = currentNode->FirstChildElement("name")->GetText();
//		const char *artist = currentNode->FirstChildElement("artist")->GetText();
//		const char *album = currentNode->FirstChildElement("album")->GetText();
//		const char *filename = currentNode->FirstChildElement("filename")->GetText();
//		uint16_t year = currentNode->FirstChildElement("year")->GetText()
//		uint8_t track = currentNode->FirstChildElement("track")->GetText();
//		uint8_t set = currentNode->FirstChildElement("set")->GetText();

//		if(name){
//			tft.printf("parsing song : %s\n", name);
//			Serial.printf("parsing song : %s\n", name);		
		song = new MenuSong();
		song->setName(data_getXMLField(currentNode, "name"));
		song->setArtist(data_getXMLField(currentNode, "artist"));
		song->setAlbum(data_getXMLField(currentNode, "album"));
		song->attachCallback(data_menuSongCB, NULL);
		list->addChild(song);
//		}


		currentNode = currentNode->NextSiblingElement();

		if(currentNode == NULL) break;
	}

	list->sort(&MenuItem::getName);

/*	list->setFirst();
	item = list->enter();

	for(uint32_t i = 0; i < list->getSize(); ++i){
		tft.fillScreen(TFT_BLACK);
		tft.setCursor(0, 0);
		tft.printf("item n°%i : %s\n", i, item->getName());
		delay(200);
		item = item->getNextItem();
	}
*/
	return 0;
}

bool data_createMenuMusicAlbums(MenuList* list){
	return 1;
}

bool data_createMenuMusicArtists(MenuList* list){
	return 1;
}

bool data_createMenuMusicArtistsAlbums(MenuList* list){
	return 1;
}

// List file from SD card, then save them to a raw XML containing metadata.
// This file will serve as a data base.
bool data_listFiles(){
	tinyxml2::XMLDocument songs;
	fs::File music = SD_MMC.open("/music");
	if(!music){
		tft.println("/music does not exit.");
		tft.println("Check SD card.");
		return DATA_FILE_ERR;
	}

	tft.println("Listing songs");
	currentNode = songs.NewElement("songs");
	currentNode = (tinyxml2::XMLElement*)songs.InsertFirstChild(currentNode);

	fileID = new int32_t(0);
	data_parseFolder(&music);
	music.close();
	delete fileID;
	tft.println("Saving songs listing");
	songs.SaveFile("/system/data/songs.xml");

	return DATA_OK;
}

// Parse folder /music/, to recursively list every audio file on the SD card.
void data_parseFolder(fs::File *folder, uint8_t lvl){
/*
	for(uint8_t i = 0; i < lvl; ++i){
		tft.print("  ");
	}
*/
/*	
	String name = folder->name();
	int8_t index = name.lastIndexOf('/');
	tft.println(name.substring(index + 1));
*/
//	tft.println(folder->name());
	++lvl;
	for(;;){
		fs::File file = folder->openNextFile();
		if(!file){
			break;
		}

		if(file.isDirectory()){
//			tft.printf("folder %s \n\t\tsize : %i\n", file.name(), file.size());
			data_parseFolder(&file, lvl);
		} else {
/*
			for(uint8_t i = 0; i < lvl; ++i){
				tft.print("  ");
			}
*/
			String name = file.name();
/*
			index = name.lastIndexOf('/');
			tft.println(name.substring(index + 1));
*/
/*			tft.fillRect(0, 9, 320, 231, TFT_BLACK);
			tft.setCursor(1, 9);
			tft.println(name);
*/
			if(!audio_getTag(&file)){
				currentNode = currentNode->InsertNewChildElement("song");
				currentNode->InsertNewChildElement("id")->SetText((*fileID)++);
				currentNode->InsertNewChildElement("name");
				currentNode->InsertNewChildElement("filename")->InsertNewText(file.name());
				currentNode->InsertNewChildElement("album");
				currentNode->InsertNewChildElement("artist");
				currentNode->InsertNewChildElement("track");
				currentNode->InsertNewChildElement("set");
				currentNode->InsertNewChildElement("year");
				currentNode->InsertNewChildElement("compilation");
				currentNode->InsertNewChildElement("popmeter");

				while(player->isRunning()){
//					tft.print('.');
					player->loop();
				}
			}
/*			
			char nameChar[name.length() + 1];
			name.toCharArray(nameChar, name.length() + 1);
			audiometa_getTag(nameChar);
*/
		}
	}
}

// Display the current menu on screen.
// TODO : see if using a sprite could avoid flickering.
void data_menuWrite(MenuList *list){
	Serial.printf("making menu %s\n", list->getName());
	display_makeMenuBG();
	MenuItem *item = list->getFirstDisplay();
//	Serial.printf("menu @ %i\n", (int32_t)list);
//	Serial.printf("focus @ %i\n", (int32_t)list->getFocus());
	for(uint8_t i = 0; i < maxMenuItem; ++i){
//		Serial.printf("item @ %i, named %s, focus : %i\n", (int32_t)item, item->getName(), item->hasFocus());
		display_makeMenuEntry(item->getName(), item->hasFocus());
		item = list->getNext();
		if(item == NULL) break;
	}
}

// Enter selected menu item.
// MenuItem::exec() is volatile, and will call the appropriate callback for each derivate of the class.
void data_menuEnter(){
	menu->getFocus()->exec();
}

// back to the previous menu level.
// TODO : transtyping this way is not good practice, maybe a callback should be implemented here.
void data_menuExit(){
	menu = (MenuList*)menu->exit();
	data_menuWrite(menu);

}

// Go a step down.
void data_menuNext(){
	if(!menu->focusNextItem()) return;
	data_menuWrite(menu);
}

// Go a step up.
void data_menuPrev(){
	if(!menu->focusPreviousItem()) return;
	data_menuWrite(menu);
}

// The callback function to be attached to MenuList objects, when selecting them.
void data_menuListCB(void* list){
	menu = (MenuList*)list;
	data_menuWrite(menu);
}

// The callback function to be attached to MenuItem objects, when selecting them.
void data_menuSongCB(void *data){
//	tft.print("song called");
}

// Populate XML based on SD card content.
// callback function passed to the ID3 parser.
void data_getFileTags(void *cbData, const char *type, bool isUnicode, const char *string){
//	tft.print(type);tft.print(" : ");tft.println(string);
	
	if(type == (const char*)"Title"){
		currentNode->FirstChildElement("name")->InsertNewText(string);
	} else if(type == (const char*)"Album"){
		currentNode->FirstChildElement("album")->InsertNewText(string);
	} else if(type == (const char*)"Performer"){
		currentNode->FirstChildElement("artist")->InsertNewText(string);
	} else if(type == (const char*)"Year"){
		currentNode->FirstChildElement("year")->InsertNewText(string);
	} else if(type == (const char*)"Track"){
		currentNode->FirstChildElement("track")->InsertNewText(string);
	} else if(type == (const char*)"Set"){
		currentNode->FirstChildElement("set")->InsertNewText(string);
	} else if(type == (const char*)"Popularimeter"){
		currentNode->FirstChildElement("popmeter")->InsertNewText(string);
	} else if(type == (const char*)"Compilation"){
		currentNode->FirstChildElement("compilation")->InsertNewText(string);
	} else if(type == (const char*)"eof"){
		((AudioFileSourceFS*)cbData)->close();
//		player->stop();
		currentNode = (tinyxml2::XMLElement*)currentNode->Parent();
		return;
	}
}



// class MenuSong : public MenuItem
// public
MenuSong::MenuSong(){
	_filename = NULL;
	_artist = NULL;
	_album = NULL;
}

MenuSong::~MenuSong(){
	free(_name);
	free(_filename);
	free(_artist);
	free(_album);
}


void MenuSong::setFilename(const char *name){
	allocateChar(name, &_filename);
}

const char* MenuSong::getFilename(){
	return _filename;
}

void MenuSong::setArtist(const char *name){
	allocateChar(name, &_artist);
}

const char* MenuSong::getArtist(){
	return _artist;
}

void MenuSong::setAlbum(const char *name){
	allocateChar(name, &_album);
}

const char* MenuSong::getAlbum(){
	return _album;
}

void MenuSong::setYear(uint16_t year){
	_year = year;
}

uint16_t MenuSong::getYear(){
	return _year;
}

void MenuSong::setTrack(uint8_t track){
	_track = track;
}

uint8_t MenuSong::getTrack(){
	return _track;
}

void MenuSong::setSet(uint8_t set){
	_set = set;
}

uint8_t MenuSong::getSet(){
	return _set;
}

void MenuSong::setCompilation(bool compilation){
	_compilation = compilation;
}

bool MenuSong::getCompilation(){
	return _compilation;
}

void MenuSong::setPop(uint8_t rank){
	_rank = rank;
}

uint8_t MenuSong::getPop(){
	return _rank;
}

void MenuSong::exec(){

}
