#include "esPod.h"
// #include "menu.h"

using namespace tinyxml2;

MenuList *menu;

uint8_t maxMenuItem;

float testValue = 0.5;

// dynamic : 19044 bytess (5%)
// 100 songs : 21044 bytes (6%)
// 1000 songs : 39044 bytes (11%)
// 5000 songs : 119044 bytes (36%)
// 7000 songs : compilation error
// 100 empty MenuItem = 2000 bytes
// 100 empty MenuSong = 4000 bytes
// MenuSong *songs = (MenuSong*)ps_malloc(100);

// Create main menu, call subroutines to parse levels.
bool menu_init(){
//	tft.println("creating menu");

	maxMenuItem = display_getMaxMenuItem();

	menu = new MenuList();

	menu->setDisplaySize(maxMenuItem);
	menu->setName("esPod");
	menu->attachCallback(menu_cbList, NULL);

	MenuItem *i = new MenuItem();
	i->attachCallback(menu_cbPlayer, NULL);
	i->setName("playing");
	menu->addChild(i);

	MenuList *music = new MenuList();
	music->attachCallback(menu_cbList, NULL);
	music->setName("music");
	menu->addChild(music);

	MenuList *artists = new MenuList();
	artists->attachCallback(menu_cbList, NULL);
	artists->setName("artists");
	music->addChild(artists);

	MenuList *albums = new MenuList();
	albums->attachCallback(menu_cbList, NULL);
	albums->setName("albums");
	music->addChild(albums);

	MenuList *songs = new MenuList();
	songs->attachCallback(menu_cbList, NULL);
	songs->setName("songs");
	music->addChild(songs);

	MenuList *compilations = new MenuList();
	compilations->attachCallback(menu_cbList, NULL);
	compilations->setName("compilations");
	music->addChild(compilations);

	MenuList *playlists = new MenuList();
	playlists->attachCallback(menu_cbList, NULL);
	playlists->setName("playlists");
	music->addChild(playlists);


	MenuList *settings = new MenuList();
	settings->attachCallback(menu_cbList, NULL);
	settings->setName("settings");
	menu->addChild(settings);

	MenuItem *test = new MenuItem();
	test->attachCallback(menu_cbTest, NULL);
	test->setName("test");
	menu->addChild(test);

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

	XMLElement *currentNode = data_getSongList();

//	Serial.println("create songs");
	menu_createMusicSongs(songs, currentNode);
//	Serial.println("songs created");
	Serial.println("create other lists");
	menu_createMusic(artists, albums, songs);
	Serial.println("lists ok");


//	menu_createMusicAlbums(albums, songs);
//	menu_createMusicArtists(artists, songs);

	songs->sort();
/*
	tft.printf("size of MenuItem : %i\n", sizeof(*i));
	tft.printf("size of MenuList : %i\n", sizeof(*menu));
	delay(3000);
*/

//	return 1;
//	menu_write(menu);
	menu_cbList(menu);
	return 0;

}

// Create the music/song menu, that will be the base for the other ones.
bool menu_createMusicSongs(MenuList *list, XMLElement *currentNode){
//	tft.println("parsing songs database");
	AudioTrackData *song;
	MenuItem *songEntry;
	for(;;){
		songEntry = new MenuItem();
		song = new AudioTrackData();
		song->setFilename(menu_getXMLTextField(currentNode, "filename"));
		song->setName(menu_getXMLTextField(currentNode, "name"));
		song->setArtist(menu_getXMLTextField(currentNode, "artist"));
		song->setAlbum(menu_getXMLTextField(currentNode, "album"));
		song->setTrack(menu_getXMLNumberField(currentNode, "track"));
		song->setSet(menu_getXMLNumberField(currentNode, "set"));
		song->setYear(menu_getXMLNumberField(currentNode, "year"));
//		Serial.printf("song %s\t", song->getName());
		songEntry->attachCallback(menu_cbSong, song);
//		Serial.printf("callback\t");
		songEntry->attachData(reinterpret_cast<void*>(song));
//		Serial.printf("data\t");
		songEntry->attachName(song->getName());
//		Serial.printf("name\t");
		list->addChild(songEntry);
//		Serial.printf("ok\n");

		currentNode = currentNode->NextSiblingElement();
		if(currentNode == NULL) break;
	}

	list->sort();
	return 0;
}

bool menu_createMusic(MenuList *artists, MenuList *albums, MenuList *ref){
	ref->sortExternal(menu_sortByTrack);
	ref->sortExternal(menu_sortByAlbum);
	ref->sortExternal(menu_sortByArtist);
//	Serial.println("sorted");
	MenuItem *lastItem = ref->getLast();
	MenuItem *item = ref->getFirst();
	MenuList *listArtists = NULL;
	MenuList *listArtistsAlbums = NULL;
	MenuList *listAlbums = NULL;
	AudioTrackData *song = reinterpret_cast<AudioTrackData*>(item->getData());
	AudioTrackData *lastSong = reinterpret_cast<AudioTrackData*>(lastItem->getData());

//		Serial.printf("%i\t | %s\t\t | %s\t\t | %s\t\t\n", song->getTrack(), song->getArtist(), song->getAlbum(), song->getName());
	// artists parsing
	for(;;){
//		Serial.printf("%s\n", song->getArtist());
		listArtists = new MenuList();
		listArtists->setName(song->getArtist());
		listArtists->attachCallback(menu_cbList, NULL);
		artists->addChild(listArtists);

		// albums parsing
		for(;;){
//			Serial.printf("\t%s\n", song->getAlbum());
			listAlbums = new MenuList();
			listAlbums->setName(song->getAlbum());
			listAlbums->attachCallback(menu_cbList, NULL);
			albums->addChild(listAlbums);

			listArtistsAlbums = new MenuList();
			listArtistsAlbums->setName(song->getAlbum());
			listArtistsAlbums->attachCallback(menu_cbList, NULL);
			listArtists->addChild(listArtistsAlbums);

			// songs parsing
			for(;;){
//				Serial.printf("\t\t%s\n", song->getName());
//				listAlbums->addChild(new MenuItem(*song));
				MenuItem *newItem = new MenuItem();
				newItem->attachData(song);
				newItem->attachName(song->getName());
				listAlbums->addChild(newItem);

				newItem = new MenuItem();
				newItem->attachData(song);
				newItem->attachName(song->getName());
				listArtistsAlbums->addChild(newItem);

				lastItem = item;
				item = ref->getNext();

				if((item == NULL) || (strcmp(song->getAlbum(), lastSong->getAlbum()))){
					listAlbums->sortExternal(menu_sortByTrack);
					listArtistsAlbums->sortExternal(menu_sortByTrack);
					break;
				}
				lastSong = song;
				song = reinterpret_cast<AudioTrackData*>(item->getData());
			}
			if((item == NULL) || (strcmp(song->getArtist(), lastSong->getArtist()))){
				listArtists->sort();
				break;
			}
		}
		if(item == NULL){
			break;
		}
		albums->sortExternal(menu_sortByName);
	}
	return 1;
}

int16_t menu_sortByName(MenuItem *a, MenuItem *b){
	AudioTrackData *_a = reinterpret_cast<AudioTrackData*>(a->getData());
	AudioTrackData *_b = reinterpret_cast<AudioTrackData*>(b->getData());
	return strcmp(_a->getName(), _b->getName());
}

int16_t menu_sortByArtist(MenuItem *a, MenuItem *b){
	AudioTrackData *_a = reinterpret_cast<AudioTrackData*>(a->getData());
	AudioTrackData *_b = reinterpret_cast<AudioTrackData*>(b->getData());
	return strcmp(_a->getArtist(), _b->getArtist());
}

int16_t menu_sortByAlbum(MenuItem *a, MenuItem *b){
	AudioTrackData *_a = reinterpret_cast<AudioTrackData*>(a->getData());
	AudioTrackData *_b = reinterpret_cast<AudioTrackData*>(b->getData());
	return strcmp(_a->getAlbum(), _b->getAlbum());
}

int16_t menu_sortByYear(MenuItem *a, MenuItem *b){
	AudioTrackData *_a = reinterpret_cast<AudioTrackData*>(a->getData());
	AudioTrackData *_b = reinterpret_cast<AudioTrackData*>(b->getData());
	return (_a->getYear() - _b->getYear());
}

int16_t menu_sortByTrack(MenuItem *a, MenuItem *b){
	AudioTrackData *_a = reinterpret_cast<AudioTrackData*>(a->getData());
	AudioTrackData *_b = reinterpret_cast<AudioTrackData*>(b->getData());
	return (_a->getTrack() - _b->getTrack());
}

int16_t menu_sortBySet(MenuItem *a, MenuItem *b){
	AudioTrackData *_a = reinterpret_cast<AudioTrackData*>(a->getData());
	AudioTrackData *_b = reinterpret_cast<AudioTrackData*>(b->getData());
	return (_a->getSet() - _b->getSet());
}

int16_t menu_sortRank(MenuItem *a, MenuItem *b){
	AudioTrackData *_a = reinterpret_cast<AudioTrackData*>(a->getData());
	AudioTrackData *_b = reinterpret_cast<AudioTrackData*>(b->getData());
	return (_a->getPop() - _b->getPop());
}

// Display the current menu on screen.
// TODO : make the menu update line after line, to avoid audio buffer underrun.
void menu_write(MenuList *list){
//	Serial.printf("making menu %s\n", list->getName());
//	display_makeMenuBG();
//	display_makeMenu(list->getName());
	display_setState(MENU);
	display_pushHeader(list->getName());
	MenuItem *item = list->getFirstDisplay();
//	Serial.printf("menu @ %i\n", (int32_t)list);
//	Serial.printf("focus @ %i\n", (int32_t)list->getFocus());
	for(uint8_t i = 0; i < maxMenuItem; ++i){
//		Serial.printf("item @ %i, named %s, focus : %i\n", (int32_t)item, item->getName(), item->hasFocus());
		display_pushMenu(item->getName(), item->hasFocus(), i);
		item = list->getNext();
		if(item == NULL){
			if(i < (maxMenuItem - 1)) display_pushMenuPadding(i + 1);
			break;
		}
	}
//	display_fillMenu();
}

void menu_update(){
	menu_cbList(menu);
}

// Enter selected menu item.
// MenuItem::exec() is virtual, and will call the appropriate callback for each derivate of the class.
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

void menu_test(){
//	display_vuMeter(testValue, 40, 190, 240);
//	display_makePlayer("Toto", "le meilleur de", "prout prout tralala", 1);
//	display_playerProgress(25, 195);
}

void menu_testExit(){
	io_deattachCBUp();
	io_deattachCBDown();
	menu_cbList(menu);
}

void menu_testValuePlus(){
	testValue += 0.02;
	menu_test();
}

void menu_testValueMinus(){
	testValue -= 0.02;
	menu_test();
}

void menu_testValueMiddle(){
	testValue = 0.5;
	menu_test();
}

void menu_cbTest(void *empty){
//	io_deattachCBWheelClockwise();
//	io_deattachCBWheelCounterClockwise();
	io_attachCBUp(menu_testExit);
	io_attachCBDown(menu_testValueMiddle);
	io_attachCBRight(menu_testValuePlus);
	io_attachCBLeft(menu_testValueMinus);
//	display_clearDisplay();
	menu_test();
}

// The callback function to be attached to MenuList objects, when selecting them.
void menu_cbList(void* list){
	io_deattachAllCB();
//	io_attachCBWheelClockwise(menu_next);
//	io_attachCBWheelCounterClockwise(menu_prev);
	io_attachCBUp(menu_prev);
	io_attachCBDown(menu_next);
	io_attachCBRight(menu_enter);
	io_attachCBLeft(menu_exit);

	menu = (MenuList*)list;
	menu_write(menu);
}

// The callback function to be attached to MenuItem objects, when selecting them.
void menu_cbSong(void *data){
//	tft.print("song called");
	audio_playTrack((AudioTrackData*)data);
}

void menu_cbPlayer(void *data){
	audio_updateDisplay();
}

const char* menu_getXMLTextField(XMLElement *node, const char *field){
	const char *name = node->FirstChildElement(field)->GetText();
	if(name) return name;
	return " ";
}

int16_t menu_getXMLNumberField(XMLElement *node, const char *field){
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

/*
void MenuSong::exec(){

}
*/