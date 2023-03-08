#ifndef POD_LOG_H
#define POD_LOG_H

#include "pod.h"

#define LOG_WIDTH		40
#define LOG_HEIGHT		40
#define LOG_MAX			LOG_WIDTH * LOG_HEIGHT

class Logger : public Print{
public:
	Logger();
	~Logger();
	size_t init(size_t buffersize);
	void attachOutput(Print *out);

	void output();

	void clear();

	virtual size_t write(uint8_t c);
	void progress(uint16_t part, uint16_t total, bool percent = false);

protected:
	bool notInit(){return (_output == NULL || _history == NULL);}
	size_t addChar(uint8_t c);
	void findNextLine();

	Print *_output;

	size_t _maxSize;

	int16_t _wIndex;
	int16_t _rIndex;
	int16_t _sIndex;

	uint16_t _size;
	uint16_t _lines;
	uint8_t _count;

	bool _progress;
	int16_t _wProgress;

	uint8_t *_history;
};

#endif