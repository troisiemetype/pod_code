#include "menu.h"

// using namespace tinyxml2;

uint8_t maxMenuItem = 10;

MenuList *menu;

// Create main menu, call subroutines to parse levels.
bool menu_init(){
//	tft.println("creating menu");

	menu = new MenuList();

	menu->setDisplaySize(maxMenuItem);
	menu->setName("esPod");
	menu->attachCallback(menu_listCB, NULL);

	MenuItem *i = new MenuItem();
	i->setName("playing");
	menu->addChild(i);

	MenuList *music = new MenuList();
	music->attachCallback(menu_listCB, NULL);
	music->setName("music");
	menu->addChild(music);

	MenuList *artists = new MenuList();
	artists->attachCallback(menu_listCB, NULL);
	artists->setName("artists");
	music->addChild(artists);

	MenuList *albums = new MenuList();
	albums->attachCallback(menu_listCB, NULL);
	albums->setName("albums");
	music->addChild(albums);

	MenuList *songs = new MenuList();
	songs->attachCallback(menu_listCB, NULL);
	songs->setName("songs");
	music->addChild(songs);

	MenuList *compilations = new MenuList();
	compilations->attachCallback(menu_listCB, NULL);
	compilations->setName("compilations");
	music->addChild(compilations);

	MenuList *playlists = new MenuList();
	playlists->attachCallback(menu_listCB, NULL);
	playlists->setName("playlists");
	music->addChild(playlists);


	MenuList *settings = new MenuList();
	settings->attachCallback(menu_listCB, NULL);
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

	tinyxml2::XMLElement *currentNode = data_getSongList();

	menu_createMusicSongs(songs, currentNode);

	menu_createMusic(artists, albums, songs);

//	menu_createMusicAlbums(albums, songs);
//	menu_createMusicArtists(artists, songs);

	songs->sortExternal(menu_sortByName);
/*
	tft.printf("size of MenuItem : %i\n", sizeof(*i));
	tft.printf("size of MenuList : %i\n", sizeof(*menu));
	delay(3000);
*/

//	return 1;

	display_setRunningMode();
	display_makeHeader();

	menu_write(menu);
	return 0;

}

// Create the music/song menu, that will be the base for the other ones.
bool menu_createMusicSongs(MenuList *list, tinyxml2::XMLElement *currentNode){
//	tft.println("parsing songs database");
	MenuSong *song;
	for(;;){
		song = new MenuSong();
		song->setFilename(menu_getXMLTextField(currentNode, "filename"));
		song->setName(menu_getXMLTextField(currentNode, "name"));
		song->setArtist(menu_getXMLTextField(currentNode, "artist"));
		song->setAlbum(menu_getXMLTextField(currentNode, "album"));
		song->setTrack(menu_getXMLNumberField(currentNode, "track"));
		song->setSet(menu_getXMLNumberField(currentNode, "set"));
		song->setYear(menu_getXMLNumberField(currentNode, "year"));
		song->attachCallback(menu_songCB, NULL);
		list->addChild(song);

		currentNode = currentNode->NextSiblingElement();
		if(currentNode == NULL) break;
	}

	list->sortExternal(menu_sortByName);
	return 0;
}

bool menu_createMusic(MenuList *artists, MenuList *albums, MenuList *ref){
	ref->sortExternal(menu_sortByTrack);
	ref->sortExternal(menu_sortByAlbum);
	ref->sortExternal(menu_sortByArtist);
	MenuSong *lastSong = (MenuSong*)ref->getLast();
	MenuSong *song = (MenuSong*)(ref->getFirst());
	MenuList *listArtists = NULL;
	MenuList *listArtistsAlbums = NULL;
	MenuList *listAlbums = NULL;

//		Serial.printf("%i\t | %s\t\t | %s\t\t | %s\t\t\n", song->getTrack(), song->getArtist(), song->getAlbum(), song->getName());
	// artists parsing
	for(;;){
		Serial.printf("%s\n", song->getArtist());
		listArtists = new MenuList();
		listArtists->setName(song->getArtist());
		listArtists->attachCallback(menu_listCB, NULL);
		artists->addChild(listArtists);

		// albums parsing
		for(;;){
			Serial.printf("\t%s\n", song->getAlbum());
			listAlbums = new MenuList();
			listAlbums->setName(song->getAlbum());
			listAlbums->attachCallback(menu_listCB, NULL);
			albums->addChild(listAlbums);

			listArtistsAlbums = new MenuList();
			listArtistsAlbums->setName(song->getAlbum());
			listArtistsAlbums->attachCallback(menu_listCB, NULL);
			listArtists->addChild(listArtistsAlbums);

			// songs parsing
			for(;;){
				Serial.printf("\t\t%s\n", song->getName());
				listAlbums->addChild(new MenuSong(*song));
				listArtistsAlbums->addChild(new MenuSong(*song));

				lastSong = song;
				song = (MenuSong*)ref->getNext();
				if((song == NULL) || (strcmp(song->getAlbum(), lastSong->getAlbum()))){
					listAlbums->sortExternal(menu_sortByTrack);
					listArtistsAlbums->sortExternal(menu_sortByTrack);
					break;
				}
			}
			if((song == NULL) || (strcmp(song->getArtist(), lastSong->getArtist()))){
				listArtists->sortExternal(menu_sortByName);
				break;
			}
		}
		if(song == NULL){
			break;
		}
		albums->sortExternal(menu_sortByName);
	}
	return 1;
}

int16_t menu_sortByName(MenuItem *a, MenuItem *b){
	return strcmp(a->getName(), b->getName());
}

int16_t menu_sortByArtist(MenuItem *a, MenuItem *b){
	return strcmp(((MenuSong*)a)->getArtist(), ((MenuSong*)b)->getArtist());
}

int16_t menu_sortByAlbum(MenuItem *a, MenuItem *b){
	return strcmp(((MenuSong*)a)->getAlbum(), ((MenuSong*)b)->getAlbum());
}

int16_t menu_sortByYear(MenuItem *a, MenuItem *b){
	return (((MenuSong*)a)->getYear() - ((MenuSong*)b)->getYear());
}

int16_t menu_sortByTrack(MenuItem *a, MenuItem *b){
	return (((MenuSong*)a)->getTrack() - ((MenuSong*)b)->getTrack());
}

int16_t menu_sortBySet(MenuItem *a, MenuItem *b){
	return (((MenuSong*)a)->getSet() - ((MenuSong*)b)->getSet());
}

int16_t menu_sortRank(MenuItem *a, MenuItem *b){
	return (((MenuSong*)a)->getPop() - ((MenuSong*)b)->getPop());
}

// Display the current menu on screen.
// TODO : see if using a sprite could avoid flickering.
void menu_write(MenuList *list){
//	Serial.printf("making menu %s\n", list->getName());
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
void menu_enter(){
	menu->getFocus()->exec();
}

// back to the previous menu level.
// TODO : transtyping this way is not good practice, maybe a callback should be implemented here.
void menu_exit(){
	menu = (MenuList*)menu->exit();
	menu_write(menu);

}

// Go a step down.
void menu_next(){
	if(!menu->focusNextItem()) return;
	menu_write(menu);
}

// Go a step up.
void menu_prev(){
	if(!menu->focusPreviousItem()) return;
	menu_write(menu);
}

// The callback function to be attached to MenuList objects, when selecting them.
void menu_listCB(void* list){
	menu = (MenuList*)list;
	menu_write(menu);
}

// The callback function to be attached to MenuItem objects, when selecting them.
void menu_songCB(void *data){
//	tft.print("song called");
}

const char* menu_getXMLTextField(tinyxml2::XMLElement *node, const char *field){
	const char *name = node->FirstChildElement(field)->GetText();
	if(name) return name;
	return " ";
}

int16_t menu_getXMLNumberField(tinyxml2::XMLElement *node, const char *field){
	int value = 0;
	node->FirstChildElement(field)->QueryIntText(&value);
//	value = node->IntAttribute(field);
	return value;
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
