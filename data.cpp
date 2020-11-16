#include "data.h"

// using namespace tinyxml2;

extern AudioGenerator *player;

fs::File *menuFile = NULL;
tinyxml2::XMLDocument menuTree;
tinyxml2::XMLElement *root = NULL;
tinyxml2::XMLElement *currentNode = NULL;

const uint8_t maxLevel = 6;
uint8_t level = 0;
levelState_t state[maxLevel];

uint8_t maxMenuItem = 10;

bool data_initXML(){
	if(menuTree.LoadFile("/system/data/menu.xml")){
		return data_makeDatabase();
	} else {
		tft.println("ok");
	}
	root = menuTree.RootElement();
	data_menuSetLevel(root, true);
	return 0;
}

bool data_makeDatabase(){
	if(menuTree.LoadFile("/system/data/base_menu.xml")){
		tft.println("/system/data/base_menu.xml does not exit.");
		tft.println("Check SD card.");
		return 1;
	} else {
		tft.fillScreen(TFT_BLACK);
		tft.setCursor(1, 0);
		tft.println("Constructing database");
	}

	tinyxml2::XMLDocument songs;
	fs::File music = SD_MMC.open("/music");
	if(!music){
		tft.println("/music does not exit.");
		tft.println("Check SD card.");
		return 1;
	}

	currentNode = songs.NewElement("songs");

	data_parseFolder(&music);
	music.close();

	songs.SaveFile("/system/data/songs.xml");

	delay(2000);

	return 1;
}

void data_parseFolder(fs::File *folder, uint8_t lvl){
	for(uint8_t i = 0; i < lvl; ++i){
		tft.print("  ");
	}
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
			tft.fillRect(0, 9, 320, 231, TFT_BLACK);
			tft.setCursor(1, 9);
			tft.println(name);

			if(!audio_getTag(&file)){
				currentNode = currentNode->InsertNewChildElement("song");
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


void data_menuSetLevel(tinyxml2::XMLElement* parent, bool update){
	if(!parent){
		Serial.println("no parent");
		return;
	}
/*
	Serial.print("level : ");
	Serial.println(level);
*/
	if(!update) return;

	levelState_t *s = &state[level];
	s->firstNode = parent->FirstChildElement();
	if(s->firstNode){
		s->activeNode = s->firstNode;
		s->firstDispNode = s->firstNode;
		s->lastNode = parent->LastChildElement();
	} else {
		Serial.println("no child node");
		return;
	}
}

void data_menuWrite(){
	display_makeMenuBG();
	bool active = 0;
	levelState_t *s = &state[level];
	tinyxml2::XMLElement *entry = s->firstDispNode;
	for(uint8_t i = 0; i < maxMenuItem; ++i){
		if(entry == s->activeNode){
			active = 1;
		} else {
			active = 0;
		}
		display_makeMenuEntry(entry->Attribute("name"), active);
		if(entry == s->lastNode) break;
		entry = entry->NextSiblingElement();
	}
}

void data_menuEnter(){
	levelState_t *s = &state[level];
	const char *type = s->activeNode->Attribute("type");
	const char *ref = "menu";
	if(strcmp(type, ref) != 0){
		Serial.println("type is not menu");
		return;
	}
	++level;
	bool update = true;
	if(s->activeNode->FirstChildElement() == state[level].firstNode) update = false;
	data_menuSetLevel(s->activeNode, update);
	data_menuWrite();
}

void data_menuExit(){
	levelState_t *s = &state[level];
	if(s->activeNode->Parent() == root){
		Serial.println("menu root");
		return;
	}
	level--;
	data_menuSetLevel((tinyxml2::XMLElement*)s->activeNode->Parent()->Parent(), false);
	data_menuWrite();
}

void data_menuNext(){
	levelState_t *s = &state[level];
	if(s->activeNode != s->lastNode){
		s->activeNode = s->activeNode->NextSiblingElement();
		data_menuWrite();
	}
}

void data_menuPrev(){
	levelState_t *s = &state[level];
	if(s->activeNode != s->firstNode){
		s->activeNode = s->activeNode->PreviousSiblingElement();
		data_menuWrite();
	}
}

void data_getFileTags(void *cbData, const char *type, bool isUnicode, const char *string){
	tft.print(type);tft.print(" : ");tft.println(string);
	
	if(type == (const char*)"eof"){
		((AudioFileSourceFS*)cbData)->close();
//		player->stop();
		currentNode = (tinyxml2::XMLElement*)currentNode->Parent();
		return;
	}

	currentNode->SetAttribute(type, string);
}