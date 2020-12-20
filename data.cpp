// #include "data.h"
#include "esPod.h"

extern 	AudioGenerator *player;

const char *musicDir = "/music";
const char *dataDir = "/system/data";
const char *dbDir = "/system/data/DB";
char dbFilename[22];

AudioTrackData *track = NULL;
char dbName[] = "0000";
uint16_t dbNameValue = 0;


uint32_t fileID = 0;

uint32_t counter = 0;
uint32_t totalSize = 0;

bool tagEOF = false;

/*
 * TODO : xml files quickly take a tremendous amount of space on the heap, so the database system must be rewritten
 *
 *		First check every sub-folder fo music.
 *			Compare the last modification date of folders against the one present in database.
 *
 *
 * 			One main database, listing sub-databases.
 * 			sub-database list files, up until it reaches a max size (lets say around 32)
 *			We then need to has a main function that :
 *				Open the database
 *				Loop into the sub-databases
 *					Open the sub-database
 *					gather songs info for menu and player
 * 					close the sub database.
 *					check for new songs
 * 					check for deleted songs
 */

/*
 *	The solution to XML file size would maybe simply be to have a data file for every song file,
 *	in which the AudioTrackData object is saved.
 *		On startup, open the folder containing those object-files.
 *		check if the file still exists.
 *			Add it to the running data, or
 *			erase it from the data.
 *		Check for new files.
 *			Create a new object-file if needed, add it to the running data.
 */

void data_init(){
	if(!SD_MMC.exists(musicDir)) SD_MMC.mkdir(musicDir);
	if(!SD_MMC.exists(dbDir)) SD_MMC.mkdir(dbDir);

	fs::File database = SD_MMC.open(dbDir);
	data_readDatabase(&database);

	data_checkNewFiles();
}

void data_readDatabase(fs::File *dir){
	for(;;){
		fs::File file = dir->openNextFile();
		if(!file) break;

		track = new AudioTrackData();
		data_readAudioTrackData(&file, track);
		// check if the file still exists in /music
		if(SD_MMC.exists(track->getFilename())){
			// if it exists, push it to menu.
			menu_pushSong(track);
			file.close();
		} else {
			// If it doesn't, we delete the entry in database, and the track info just created.
			delete track;
			file.close();
			SD_MMC.remove(file.name());
		}
	}
}

void data_checkNewFiles(){
	fs::File dir= SD_MMC.open(musicDir);
	data_parseFolder(&dir, 0);
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
			// Recursively dive into every folder we find.
			data_parseFolder(&file, lvl);
		} else {
			// If the file is a song, we check it.
			data_checkSong(&file);
			file.close();
		}
	}
}

void data_checkSong(fs::File *file){
	// Get filename, then extract extension to check for file validity
	// We only (for now i hope) want to deal with mp3 files.
	const char *name = file->name();
	const char *ext = strrchr(name, '.');

//	Serial.printf("file %s ; extension %s\n", name, ext);
	if(strcmp(ext, ".mp3") != 0) return;

	// We check if menu has already this song (which has already been loaded from database)
	if(menu_hasSong(name)) return;

//	log_d("getting tag");
//	Serial.printf("getting tags for  %s\n", name);
	if(audio_getTag(file)){
		tagEOF = false;
		totalSize += file->size();

		track = new AudioTrackData();
		track->setFilename(name);

		while(!tagEOF){
			player->loop();
		}
		player->stop();
		data_writeAudioTrackData(track);
		menu_pushSong(track);
//		Serial.println("song added to menu and in database.");
//		Serial.printf("added\t%s\n", name);
	}

}

void data_readAudioTrackData(fs::File *file, AudioTrackData *track){
//	Serial.printf("read data from %s\n", file->name());
	// make a clone copy of the file, byte fo byte, to the AudioTrackData instance.
	file->read((uint8_t*)track, sizeof(AudioTrackData) / sizeof(uint8_t));

	// then copy the values for name, artist, album and filename, that are appended at the end of the file.
	// We have to expressly copy these values since they are pointer in the class instance.
	char name[128];
	char c;
	uint8_t index = 0;
	do{
		c = file->read();
		name[index++] = c;
	}while(c);

	track->setName(name);

	index = 0;
	do{
		c = file->read();
		name[index++] = c;
	}while(c);

	track->setArtist(name);

	index = 0;
	do{
		c = file->read();
		name[index++] = c;
	}while(c);

	track->setAlbum(name);

	index = 0;
	do{
		c = file->read();
		name[index++] = c;
	}while(c);

	track->setFilename(name);
}

void data_writeAudioTrackData(AudioTrackData *track){
	// Check for existing name !
	for(;;){
		sprintf(dbName, "%04X", dbNameValue);
		strcpy(dbFilename, dbDir);
		strcat(dbFilename, "/");
		strcat(dbFilename, dbName);
		if(SD_MMC.exists(dbFilename)){
			dbNameValue++;
		} else {
			break;
		}
	}

	fs::File file = SD_MMC.open(dbFilename, "w");
	if(!file) return;

	size_t b = 0;
	b += file.write((uint8_t*)track, sizeof(AudioTrackData) / sizeof(uint8_t));
	b += file.write((uint8_t*)track->getName(), strlen(track->getName()) + 1);
	b += file.write((uint8_t*)track->getArtist(), strlen(track->getArtist()) + 1);
	b += file.write((uint8_t*)track->getAlbum(), strlen(track->getAlbum()) + 1);
	b += file.write((uint8_t*)track->getFilename(), strlen(track->getFilename()) + 1);

//	Serial.printf("%i bytes written to file %s\n", b, dbName);
}

// Populate XML based on SD card content.
// callback function passed to the ID3 parser.
void data_getFileTags(void *cbData, const char *type, bool isUnicode, const char *string){
//	tft.print(type);tft.print(" : ");tft.println(string);
	
	if(type == (const char*)"Title"){
		track->setName(string);
	} else if(type == (const char*)"Album"){
		track->setAlbum(string);
	} else if(type == (const char*)"Performer"){
		track->setArtist(string);
	} else if(type == (const char*)"Year"){
		track->setYear(strtol(string, NULL, 10));
	} else if(type == (const char*)"Track"){
		track->setTrack(strtol(string, NULL, 10));
	} else if(type == (const char*)"Set"){
		track->setSet(strtol(string, NULL, 10));
	} else if(type == (const char*)"Popularimeter"){
		track->setPop(strtol(string, NULL, 10));
	} else if(type == (const char*)"Compilation"){
		track->setCompilation(strtol(string, NULL, 10));
	} else if(type == (const char*)"eof"){
		tagEOF = true;
		return;
	}
}

/*
theme_t* data_getTheme(theme_t *theme){
	return NULL;
}

void data_updateTheme(const char *name){

}
*/