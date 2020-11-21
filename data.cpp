#include "data.h"

extern 	AudioGenerator *player;

tinyxml2::XMLDocument *songData;
tinyxml2::XMLElement *currentNode;

int32_t *fileID = NULL;


void data_init(){
	songData = new tinyxml2::XMLDocument();
	if(!SD_MMC.exists("/system/data/songs.xml")){
		data_listFiles();
	}
}

// List file from SD card, then save them to a raw XML containing metadata.
// This file will serve as a data base.
bool data_listFiles(){
	fs::File music = SD_MMC.open("/music");
	if(!music){
//		tft.println("/music does not exit.");
//		tft.println("Check SD card.");
		return 1;
	}

//	tft.println("Listing songs");

//	tinyxml2::XMLDocument songs;
	currentNode = songData->NewElement("songs");
	currentNode = (tinyxml2::XMLElement*)songData->InsertFirstChild(currentNode);

	fileID = new int32_t(0);
	data_parseFolder(&music);
	music.close();
	delete fileID;
//	tft.println("Saving songs listing");
	songData->SaveFile("/system/data/songs.xml");

	return 0;
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
/*			tft.fillRect(0, 8, 320, 232, TFT_BLACK);
			tft.setCursor(0, 8);
			tft.printf("folder %s \t\tsize : %i\n", file.name(), file.size());
			data_parseFolder(&file, lvl);
*/		} else {
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
*/			tft.println(name);

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

tinyxml2::XMLElement* data_getSongList(){
	if(songData->LoadFile("/system/data/songs.xml")){
		return NULL;
	}
	return songData->RootElement()->FirstChildElement();
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
