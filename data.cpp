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
	return DATA_OK;
}

bool data_makeDatabase(){
	if(menuTree.LoadFile("/system/data/base_menu.xml")){
		tft.println("/system/data/base_menu.xml does not exit.");
		tft.println("Check SD card.");
		return DATA_FILE_ERR;
	}
	tft.fillScreen(TFT_BLACK);
	tft.setCursor(1, 0);
	tft.println("Constructing database");

	bool result = data_listFiles();
	if(result){
		return result;
	}

	tinyxml2::XMLDocument menu;
	menuTree.DeepCopy(&menu);

	tinyxml2::XMLDocument songs;
	songs.LoadFile("/system/data/songs.xml");

	tft.println("Creating whole menu");
	delay(500);

	currentNode = menu.FirstChildElement("artists");
	if(!currentNode) tft.print("'artist' node not found");
	delay(500);
	currentNode = currentNode->FirstChildElement("content");
	if(!currentNode) tft.print("content node not found");
	delay(500);

	data_populateArtists(currentNode, songs.RootElement(), "artist");
	delay(4000);
	menu.SaveFile("system/data/menu.xml");


	return 1;
}

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

bool data_populateArtists(tinyxml2::XMLElement *node, tinyxml2::XMLElement *ref, const char *filter){
	ref = ref->FirstChildElement();
	if(!ref) tft.print("ref node problem !");
	delay(500);
	uint16_t i = 0;
	for(;;){
		const char *name = ref->FirstChildElement(filter)->GetText();
		tft.printf("node %i, name : %s\n", i, name);
		if(!(node->FirstChildElement(name))){
			node = node->InsertNewChildElement("menu");
			node->InsertNewChildElement("name")->InsertNewText(name);
			node->InsertNewChildElement("type")->InsertNewText("menu");
			ref = ref->NextSiblingElement();
			if(!ref) break;
			++i;
		}
	}
	tft.printf("end of songs\n");
	return 0;
}

bool data_sortMenu(){
	return 0;
}

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
		display_makeMenuEntry(entry->FirstChildElement("name")->GetText(), active);
		if(entry == s->lastNode) break;
		entry = entry->NextSiblingElement();
	}
}

void data_menuEnter(){
	levelState_t *s = &state[level];
	const char *type = s->activeNode->FirstChildElement("type")->GetText();
	const char *ref = "menu";
	if(strcmp(type, ref) != 0){
		Serial.println("type is not menu");
		return;
	}
	++level;
	bool update = true;
	if(s->activeNode->FirstChildElement("content")->FirstChildElement("menu") == state[level].firstNode) update = false;
	data_menuSetLevel(s->activeNode->FirstChildElement("content"), update);
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