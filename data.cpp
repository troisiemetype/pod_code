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
 *	After having tried to use XML files, both on SD card for long term memory and internally for programm runtime,
 *	another solution has been adopted :
 *	For every song in the data base, there is small binary file which sumarizes the file :
 *		- Info needed for the menu, and
 *		- path to the file itself.
 *	(note : iPod uses something similar, excpet that all this informations are stored in ONE binary file which is parsed at startup)
 *	Those binary files are handle like that :
 *		On startup, open the folder containing those binary files.
 *		check if the audio file still exists in database, then :
 *			Add it to the running data, or
 *			erase it from the data.
 *		Check for new files.
 *			Create a new binary file if needed, add it to the running data.
 */

void data_init(){
	if(!SD_MMC.exists(musicDir)) SD_MMC.mkdir(musicDir);
	if(!SD_MMC.exists(dbDir)) SD_MMC.mkdir(dbDir);

	fs::File database = SD_MMC.open(dbDir);
	data_readDatabase(&database);

	data_checkNewFiles();
}

void data_readDatabase(fs::File *dir){
	log_d("reading database");
	for(;;){
		fs::File file = dir->openNextFile();
		if(!file) break;
		log_d("next file : %s", file.name());

		track = new AudioTrackData();
		data_readAudioTrackData(&file, track);

		log_d("\t reading DB : %s", track->getFilename());
		// check if the file still exists in /music
		if(SD_MMC.exists(track->getFilename())){
			// if it exists, push it to menu.
			log_d("\tpush track to menu");

			menu_pushSong(track);
			file.close();
		} else {
			// If it doesn't, we delete the entry in database, and the track info just created.
			delete track;
			log_d("\tno audio file in DB for this entry, removing it.");
			file.close();
			SD_MMC.remove(file.path());
		}
	}
}

void data_checkNewFiles(){
	fs::File dir= SD_MMC.open(musicDir);
	data_parseFolder(&dir, 0);
}

// Parse folder /music/, to recursively list every audio file on the SD card.
void data_parseFolder(fs::File *folder, uint8_t lvl){
	log_d("\tparsing folder %s", folder->name());
	++lvl;
	for(;;){
		fs::File file = folder->openNextFile();
		if(!file){
			break;
		}
//		log_d("new file : %s", file.name());
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
//	char *name = (char*)malloc(sizeof(char) * strlen(file->name()) + 1);
//	*name = '/';
//	strcat(name, file->name());
/*
	char *name = (char*)calloc(128, sizeof(char));
	*name = '/';
	strcat(name, file->name());
	log_d("name : %s", name);
*/
	const char *path = file->path();
	const char *ext = strrchr(path, '.');

//	log_d("file %s ; extension %s", name, ext);
	if(strcmp(ext, ".mp3") != 0) return;

	// We check if menu has already this song (which has already been loaded from database)
	if(menu_hasSong(path)) return;

//	log_d("getting tag");
	log_d("getting tags for  %s", path);
	if(audio_getTag(file)){
		tagEOF = false;
		totalSize += file->size();

		track = new AudioTrackData();
		track->setFilename(path);

		while(!tagEOF){
			player->loop();
		}
		player->stop();
//		Serial.println("song added to menu and in database.");
//		Serial.printf("added\t%s\n", name);
	}
	data_writeAudioTrackData(track);
	menu_pushSong(track);
//	data_getTrackLength(file, track);

}

void data_readAudioTrackData(fs::File *file, AudioTrackData *track){
	log_d("read data from %s", file->name());
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

void data_getTrackLength(fs::File *file, AudioTrackData *track){
	Serial.printf("getting length from %s\n", file->name());
	uint32_t size = 0;
	uint8_t c;
	// We read thewhole file to count frame headers.
	while(file->available()){
		// We are looking for 11 bits set in a row.
		// If we found them, it's a new frame header, so we increase the number of frames for the track.
		c = file->read();
		if((c == 0xFF) && (file->peek() & 0xE0)){
			size ++;
		}
	}
	track->setDuration(size * 26);
	Serial.printf("%i frames / %ims\n", size, size * 26);
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