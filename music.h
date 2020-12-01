#ifndef ESPOD_MUSIC_H
#define ESPOD_MUSIC_H

// #include <Arduino.h>
#include "esPod.h"

struct album_t{
	String name;
	String artist;
	uint16_t year;
};

struct title_t{
	String filename;
	album_t album;
	String title;
	uint8_t track;
	uint8_t ranking;
};

#endif