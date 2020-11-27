#include "data.h"

extern 	AudioGenerator *player;

const char* database = "/system/data/songs.xml";

using namespace tinyxml2;

XMLDocument *songData = NULL;
XMLElement *currentNode = NULL;

uint32_t fileID = 0;

uint32_t counter = 0;
uint32_t totalSize = 0;


void data_init(){
	songData = new XMLDocument();
	if(!SD_MMC.exists("/music")) SD_MMC.mkdir("/music");
//	Serial.println("listing files.");
	data_checkNewFiles();
	data_checkDeletedFiles();
//	data_checkNewFiles();
}

// List file from SD card, then save them to a raw XML containing metadata.
// This file will serve as a data base.
bool data_checkNewFiles(){
//	XMLError loaded = songData->LoadFile(database);
	if(songData->LoadFile(database)){
		currentNode = songData->NewElement("songs");
		currentNode = (XMLElement*)songData->InsertFirstChild(currentNode);
	} else {
		currentNode = songData->RootElement();
	}

//	Serial.println("Listing songs");
	fileID = 0;
	fs::File music = SD_MMC.open("/music");
	counter = millis();
	data_parseFolder(&music);
	music.close();
//	uint32_t time = millis() - counter;
//	Serial.printf("%i files listed in %i milliseconds.\n", fileID, time);

//	tft.println("Saving songs listing");
	songData->SaveFile(database);

	return 0;
}

bool data_checkDeletedFiles(){
	songData->LoadFile(database);
	currentNode = songData->RootElement()->FirstChildElement();
	fileID = 0;

	XMLNode *nodeToClear = NULL;

//	Serial.printf("\nchecking for removed songs\n");
	counter = millis();
	for (;;){
		const char *name = currentNode->FirstChildElement("filename")->GetText();
//		const char *name = menu_getXMLTextField(currentNode, "name");
		// Check if file exists
		if(SD_MMC.exists(name)){
			// Add its size to the total size, and increment the files counter.
			// Maybe the total size can just be checked from the SD_MMC object.
//			fs::File file = SD_MMC.open(name);
//			totalSize += file.size();
			fileID++;
//			file.close();
		} else {
			// remove this file from the database.
			nodeToClear = currentNode;
//			Serial.printf("removed\t%s\n", name);
		}
		currentNode = currentNode->NextSiblingElement();
		if(nodeToClear){
			nodeToClear->Parent()->DeleteChild(nodeToClear);
			nodeToClear = NULL;
		}
		if(currentNode == NULL) break;
	}
//	uint32_t time = millis() - counter;
//	Serial.printf("%i files listed in %i milliseconds.\n", fileID, time);
	songData->SaveFile(database);
	return 1;
}

// Parse folder /music/, to recursively list every audio file on the SD card.
void data_parseFolder(fs::File *folder, uint8_t lvl){

	++lvl;
	for(;;){
		fs::File file = folder->openNextFile();
		if(!file){
			break;
		}
		if(file.isDirectory()){
			data_parseFolder(&file, lvl);
		} else {
			data_checkSong(&file);
		}
	}
}

void data_checkSong(fs::File *file){
	const char *name = file->name();

	currentNode = songData->RootElement()->FirstChildElement();

	for(;;){
		if(!currentNode) break;
		if(strcmp(currentNode->FirstChildElement("filename")->GetText(), name) == 0){
//			Serial.printf("checked\t%s\n", name);
			fileID++;
			return;
		}
		currentNode = currentNode->NextSiblingElement();
	}

	currentNode = songData->RootElement();
	if(!audio_getTag(file)){
		totalSize += file->size();
		currentNode = currentNode->InsertNewChildElement("song");
		currentNode->InsertNewChildElement("id")->SetText(fileID++);
		currentNode->InsertNewChildElement("name");
		currentNode->InsertNewChildElement("filename")->InsertNewText(file->name());
		currentNode->InsertNewChildElement("album");
		currentNode->InsertNewChildElement("artist");
		currentNode->InsertNewChildElement("track");
		currentNode->InsertNewChildElement("set");
		currentNode->InsertNewChildElement("year");
		currentNode->InsertNewChildElement("compilation");
		currentNode->InsertNewChildElement("popmeter");
		currentNode->InsertNewChildElement("size")->SetText(file->size());

		while(player->isRunning()){
			player->loop();
		}
//		Serial.printf("added\t%s\n", name);
	}

}


XMLElement* data_getSongList(){
	if(songData->LoadFile(database)){
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
		currentNode = (XMLElement*)currentNode->Parent();
		return;
	}
}
