#ifndef ESPOD_LOGGER_H
#define ESPOD_LOGGER_H

// #include <Arduino.h>
#include "piPod.h"

class Logger : public Print {
public:
	Logger();
	~Logger();

	size_t write(uint8_t);

	void saveBuffer();

private:
//	cbuf buffer;

};

#endif