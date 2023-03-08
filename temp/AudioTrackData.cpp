#include "AudioTrackData.h"

// Public methods
AudioTrackData::AudioTrackData(){
	_name = NULL;
	_album = NULL;
	_artist = NULL;
	_filename = NULL;
/*
	allocateChar("\0", &_name);
	allocateChar("\0", &_album);
	allocateChar("\0", &_artist);
	allocateChar("\0", &_filename);
*/
}

AudioTrackData::~AudioTrackData(){
	if(_name) free(_name);
	if(_album) free(_album);
	if(_artist) free(_artist);
	if(_filename) free(_filename);

	_name = NULL;
	_album = NULL;
	_artist = NULL;
	_filename = NULL;
}

void AudioTrackData::setName(const char *name){
	allocateChar(name, &_name);
}

const char* AudioTrackData::getName(){
	return _name;
}

void AudioTrackData::setArtist(const char *artist){
	allocateChar(artist, &_artist);
}

const char* AudioTrackData::getArtist(){
	return _artist;
}

void AudioTrackData::setAlbum(const char *album){
	allocateChar(album, &_album);
}

const char* AudioTrackData::getAlbum(){
	return _album;
}

void AudioTrackData::setFilename(const char *filename){
	allocateChar(filename, &_filename);
}

const char* AudioTrackData::getFilename(){
	return _filename;
}

void AudioTrackData::setYear(uint16_t year){
	_year = year;
}

uint16_t AudioTrackData::getYear(){
	return _year;
}

void AudioTrackData::setTrack(uint8_t track){
	_track = track;
}

uint8_t AudioTrackData::getTrack(){
	return _track;
}

void AudioTrackData::setSet(uint8_t set){
	_set = set;
}

uint8_t AudioTrackData::getSet(){
	return _set;
}

void AudioTrackData::setCompilation(bool comp){
	_compilation = comp;
}

bool AudioTrackData::getCompilation(){
	return _compilation;
}

void AudioTrackData::setPop(uint8_t rank){
	_rank = rank;
}

uint8_t AudioTrackData::getPop(){
	return _rank;
}


//protected methods
// Note : realloc should behave like malloc when used on a NULL-pointer, but esp32 doesn't seem to like it.
void AudioTrackData::allocateChar(const char *text, char **memberVar){
	if(text == NULL) return;
	char *tmp = (char*)malloc(strlen(text) + 1);
	if(!tmp){
		return;
	}
//	if(*memberVar) free(*memberVar);
	*memberVar = tmp;
	strcpy(*memberVar, text);
	return;
}
