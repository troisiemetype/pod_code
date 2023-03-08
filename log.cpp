#include "pod.h"

Logger::Logger(){
	_output = NULL;
	_history = NULL;
	clear();
}

Logger::~Logger(){
	free(_history);
}

size_t Logger::init(size_t size){
	if(size > 32766) size = 32766;
	uint8_t *history = (uint8_t*)realloc(_history, size * sizeof(uint8_t));
	if(history == NULL){
		return 0;
	} else {
		_history = history;
		_maxSize = size;
		return size;
	}
}

void Logger::attachOutput(Print *out){
	_output = out;
}

void Logger::output(){
	if(_output == NULL) return;

	if(_wIndex == _sIndex) return;

	int16_t size = _wIndex - _sIndex;

	consoleSprite.fillScreen(TFT_BLACK);
	consoleSprite.setCursor(0, 0);

	if(size < 0){
		uint16_t limit = _maxSize - _wIndex;
		size += _maxSize;
		_output->write((_history + _sIndex), limit);
		size -= limit;
		_output->write(_history, size);
	} else {
		_output->write((_history + _sIndex), size);
	}
	consoleSprite.pushSprite(0, 0);
}

void Logger::clear(){
	_wIndex = 0;
	_rIndex = 0;
	_sIndex = 0;
	_size = 0;
	_lines = 0;
	_count = 0;

	_progress = false;
}

size_t Logger::write(uint8_t c){
	if(_history == NULL) return 0;

	_progress = false;

	return addChar(c);

}

size_t Logger::addChar(uint8_t c){

//	_output->write(c);
	_history[_wIndex] = c;
	if(++_size > _maxSize) _size = _maxSize;
	_count++;

	if(_count >= (LOG_WIDTH + 1) || c == '\n'){
		_count = 0;
		_lines++;
		if(_lines >= LOG_HEIGHT && !_progress){
			findNextLine();
		}
		output();
	}

	if(++_wIndex > _maxSize){
		_wIndex = 0;
	}
	return 1;
}

void Logger::findNextLine(){
	int16_t index = _sIndex;
	uint8_t count = 0;

	for(;;){
		if(_history[index] == '\n'){
			_sIndex += count + 1;
			return;			
		} else if(count >= LOG_WIDTH){
			_sIndex += count;
			return;			
		}
		index++;
		count++;
	}
}

void Logger::progress(uint16_t part, uint16_t total, bool percent){
	if(_history == NULL) return;

	if(!_progress){
		if(_count != 0){
			write('\n');
		}
		_wProgress = _wIndex;
	}
	_wIndex = _wProgress;
	_lines--;
	_progress = true;

	if(part > total) part = total;

	float div = (float)total / (float)(LOG_WIDTH - 2);
	float p = part / div;
	part = floor(p);

	addChar('[');
	for(uint8_t i = 0; i < part; ++i){
		addChar('#');
	}
	part = (LOG_WIDTH - 2) - part;
	for(uint8_t i = 0; i < part; ++i){
		addChar('_');
	}
	addChar(']');
	addChar('\n');
}
