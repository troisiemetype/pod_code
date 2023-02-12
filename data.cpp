// #include "data.h"
#include "esPod.h"

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
//			file.close();
		} else {
			// If it doesn't, we delete the entry in database, and the track info just created.
			delete track;
			log_d("\tno audio file in DB for this entry, removing it.");
			// file.path() doesn't exist once the file has been closed.
//			file.close();
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
	const char *path = file->path();
	const char *ext = strrchr(path, '.');

//	log_d("file %s ; extension %s", name, ext);

	// We check if menu has already this song (which has already been loaded from database)
	if(menu_hasSong(path)) return;	

//	log_d("getting tag");
	log_d("getting tags for  %s", path);

	track = new AudioTrackData();
	track->setFilename(path);

	tagEOF = false;

	// each file type has its own parser.
	if(strcmp(ext, ".mp3") == 0){
		if(data_getTagMp3(file)){
			log_d("no tags for this file");
			delete track;
			return;
		}
	} else {
		// If we don't have a parser for the type, it's not supported.
		delete track;
		return;
	}

	totalSize += file->size();

	data_writeAudioTrackData(track);
	menu_pushSong(track);
//	data_getTrackLength(file, track);

}

void data_readAudioTrackData(fs::File *file, AudioTrackData *track){
	log_d("read data from %s", file->name());
	// make a clone copy of the file, byte for byte, to the AudioTrackData instance.
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


	log_d("%i bytes written to file %s\n", b, dbName);
}

void data_clearDB(void *empty){
	fs::File database = SD_MMC.open(dbDir);
	for(;;){
		fs::File file = database.openNextFile();
		if(!file) break;

		log_d("suppressing file %s", file.name());
		SD_MMC.remove(file.path());
	}
	database.close();
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

/*
 *	Reminder of a mp3 ID3 header :
 *
 *	Header-header. At the top of the file.
 *		Bytes		Content
 *		0-2			TAG identifier. Contains the string "ID3"
 *		3-4			TAG version.
 *		5 			Flags.
 *					note : formated abc00000 with a = unsynchronization, b = extended header, c = experimental indicator
 *		6-9 		Header size.
 *					the MSB of each byte is 0, so each byte is encoded on 7 bits.
 *
 *	Frames
 *		Bytes		Content
 *		0-3			Frame identifier	3 or 4 char word describing the type of info following
 *		4-7			Size
 *		8-9			Flags
 *
 */
bool data_getTagMp3(fs::File* file){
	char header[10];
	file->read((uint8_t*)header, 10);
	log_d("header = %s", header);
	if(header[0] != 'I' || header[1] != 'D' || header[2] != '3'){
		// no header, return. Or see what to do.
		return 1;
	}
	log_d("version %i %i", header[3], header[4]);

	// Skipping flags.

	// Determinig header size.
	uint32_t size = 0;
	for(uint8_t i = 6; i < 10; i++){
//		log_d("byte : %i", header[i + 6]);
		size <<= 7;
		uint32_t add = header[i];
		size += add;
	}
	log_d("header size : %i", size);

	int32_t consummed = size;

	// parsing header until size bytes have been read.
	while(consummed){
		// getting identifier.
		char frameIdentifier[5];
		file->read((uint8_t*)frameIdentifier, 4);
		consummed -= 4;
		frameIdentifier[4] = 0;

		// account for 0-padding in tags
		if(frameIdentifier[0] == 0 && frameIdentifier[1] == 0 && frameIdentifier[2] == 0 && frameIdentifier[3] == 0){
			log_d("padding");

			data_storeTag("eof", "");
/*
			char c = 0;
			while(c == 0){
				c = file->read();
			}
*/
			return 0;
		}

		log_d("id : %s", frameIdentifier);

		// Determining frame size.
		uint32_t frameSize = 0;
		for(uint8_t i = 0; i < 4; i++){
	//		log_d("byte : %i", header[i + 6]);
			frameSize <<= 8;
			uint32_t add = (uint32_t)file->read();
			consummed--;
//			log_d("byte %i", add);
			frameSize += add;
		}
		log_d("frame size : %i", frameSize);


		// Skipping flags for now.
		file->read();
		file->read();
		consummed -=2;
		// We do nothing with empty tags.
		if(frameSize == 0) continue;
		// Skipping the 0 byte as well
		file->read();
		consummed--;

		// retriving the frame content
		// FrameSize has already been consumed by one : the 0 byte above.
		frameSize--;
		char frameContent[frameSize];
		file->read((uint8_t*)frameContent, frameSize);
		consummed -= frameSize;
		frameContent[frameSize] = 0;

		log_d("content : %s", frameContent);

		data_storeTag(frameIdentifier, frameContent);
	}

	data_storeTag("eof", "");
	return 0;
}


// Populate database binary file based on SD card content.
void data_storeTag(const char* id, const char* value){
	log_d("storing data : %s %s", id, value);
	
	if(strcmp(id, "TIT2") == 0){
		track->setName(value);
	} else if(strcmp(id, "TALB") == 0){
		track->setAlbum(value);
	} else if(strcmp(id, "TPE1") == 0){
		track->setArtist(value);
	} else if(strcmp(id, "TYER") == 0){
		track->setYear(strtol(value, NULL, 10));
	} else if(strcmp(id, "TRCK") == 0){
		track->setTrack(strtol(value, NULL, 10));
	} else if(strcmp(id, "TPOS") == 0){
		track->setSet(strtol(value, NULL, 10));
	} else if(strcmp(id, "POPM") == 0){
		track->setPop(strtol(value, NULL, 10));
	} else if(strcmp(id, "TCMP") == 0){
		track->setCompilation(strtol(value, NULL, 10));
	} else if(strcmp(id, "eof") == 0){
		tagEOF = true;
	}


	// here we populate the artist, album and name if they were not in the tag.
	// Not sure if it's better to do it here, or during the parsing of database.
	if(tagEOF){
//		log_d("tag parsed");
//		log_d("name : %s", track->getName());
//		log_d("artist : %s", track->getArtist());
//		log_d("album : %s", track->getAlbum());
//		log_d("name : %x", *track->getName());
//		log_d("artist : %x", *track->getArtist());
//		log_d("album : %x", *track->getAlbum());

//		if(track->getName() == (const char*)""){
		if(track->getName() == NULL){

			log_d("file has no name");

			const char *filename = (track->getFilename());
			char *name = (char*)malloc(strlen(filename));
			if(name == NULL) return;

			memcpy(name, filename, strlen(filename));
			name = strrchr(name, '/');
			const char *ext = strrchr(name, '.');
			memmove(name, (name + 1), (strlen(name) - strlen(ext) - 1));
			*(name + (strlen(name) - strlen(ext) - 1)) = '\0';

			log_d("name : %s", name);
			track->setName(name);

			// free(name) makes esp reboot.See Why (There was the same problem when dealing with memory freeing in linked classes)
//			free(name);
			
		}

//		if(track->getAlbum() == (const char*)""){
		if(track->getAlbum() == NULL){

//			log_d("file has no album");
			track->setAlbum("unknown album");
		}

//		if(track->getArtist() == (const char*)""){
		if(track->getArtist() == NULL){
//			log_d("file has no artist");
			track->setArtist("unknown artist");
		}
	}
}


/*
theme_t* data_getTheme(theme_t *theme){
	return NULL;
}

void data_updateTheme(const char *name){

}
*/