#ifndef AUDIO_TRACK_DATA_H
#define ADUIO_TRACK_DATA_H

#include <Arduino.h>

class AudioTrackData{
public:
	AudioTrackData();
	~AudioTrackData();

	void setName(const char *name);
	const char* getName();

	void setArtist(const char *artist);
	const char* getArtist();

	void setAlbum(const char *album);
	const char* getAlbum();

	void setFilename(const char *filename);
	const char* getFilename();

	void setYear(uint16_t year);
	uint16_t getYear();

	void setTrack(uint8_t track);
	uint8_t getTrack();

	void setSet(uint8_t set);
	uint8_t getSet();

	void setCompilation(bool comp);
	bool getCompilation();

	void setPop(uint8_t rank);
	uint8_t getPop();

protected:
	void allocateChar(const char *text, char **memberVar);

private:
	char *_name;
	char *_artist;
	char *_album;
	char *_filename;
	uint16_t _year;
	uint8_t _track;
	uint8_t _set;
	bool _compilation;
	uint8_t _rank;

};

#endif