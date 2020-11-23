#include "wheel.h"

TouchWheel::TouchWheel(uint8_t pin0, uint8_t pin1, uint8_t pin2){
	_pin[0] = pin0;
	_pin[1] = pin1;
	_pin[2] = pin2;
}

TouchWheel::~TouchWheel(){

}

// TODO : see if we can ponderate each value instead of computing them around the strongest.
void TouchWheel::init(){
	_weight[0] = 256;
	_weight[1] = 86;
	_weight[2] = 171;

	for(uint8_t i = 0; i <= _channels; ++i){
		_current[i] = 0;
		_previous[i] = 0;
		_delta[i] = 0;
		_baseline[i] = 0;

		_state[i] = state_idle;
		_prevState[i] = state_idle;
	}

	_timeRising = 10000;
	_timeFalling = 2000;

	_filterWeight = 30;

	tuneBaseline(32);
	tuneThreshold(32);
}

void TouchWheel::setSteps(uint8_t steps){
	if(steps < 4){
		_steps = 0;
		return;
	}
	_steps = 255 / steps;
}

void TouchWheel::tuneBaseline(uint8_t cycles){
	_baseline[0] = 0;
	_baseline[1] = 0;
	_baseline[2] = 0;
	_baseline[3] = 0;

	for(uint8_t i = 0; i < cycles; ++i){
		for(uint8_t j = 0; j < _channels; ++j){
			_baseline[j] += touchRead(_pin[j]);
		}
	}
	_baseline[3] = (_baseline[0] + _baseline[1] + _baseline[2]) / 3;

//	Serial.printf("baseline :\t");
	for(uint8_t i = 0; i <= _channels; ++i){
		_baseline[i] /= cycles;
		_current[i] = _previous[i] = _baseline[i];
		_delta[i] = 0;
//		Serial.printf("%i\t",_baseline[i]);
	}
//	Serial.println();
}

void TouchWheel::tuneThreshold(uint8_t cycles){
	uint16_t min[_channels];
	uint16_t max[_channels];

	for(uint8_t i = 0; i < _channels; ++i){
		min[i] = max[i] = _baseline[i];
	}

	for(uint8_t i = 0; i < cycles; ++i){
		for(uint8_t j = 0; j < _channels; ++j){
			_current[j] = touchRead(_pin[j]);
			if(_current[j] > max[j]) max[j] = _current[j];
			if(_current[j] < min[j]) min[j] = _current[j];
		}
	}

	int16_t delta = 0;
	for(uint8_t i = 0; i < _channels; ++i){
		delta += max[i] - min[i];
	}

	delta /= _channels;

	_thresholdRising = (float)delta * 0.6;
	_thresholdFalling = (float)delta * 0.6;
//	Serial.printf("max delta :\t%i\n", delta);
//	Serial.printf("threshold rising:\t%i\n", _thresholdRising);
//	Serial.printf("threshold falling:\t%i\n", _thresholdFalling);
}

bool TouchWheel::touched(){
	return _state[_channels] == state_touch;
}

int8_t TouchWheel::getPosition(){
	return _pos;
}

int8_t TouchWheel::getStep(){
	if(_steps == 0) return _step;

	int8_t steps = 0;
	if(abs(_stepCounter) > _steps){
		steps = _stepCounter / _steps;
		_stepCounter = 0;
	}
	return steps;
}

bool TouchWheel::update(){
//	Serial.printf("values :\t%i\t%i\t%i\t%i\n", _delta[0], _delta[1], _delta[2], _delta[3]);
	if(!updateRead()) return false;
	_prevPos = _pos;

	// To compute position on the wheel, we need to know where it is touched.
	// We use barycenters to check that.
	uint16_t bary0 = 83 * _delta[0];
	uint16_t bary1 = 83 * _delta[1];
	uint16_t bary2 = 83 * _delta[2];

	// Ponderate each center with the global delta.
	bary0 /= _delta[3];
	bary1 /= _delta[3];
	bary2 /= _delta[3];

	int16_t bary = 0;

	// Then, following where the wheel is touched, ponderate neigbours to refine measure.
	if(bary0 > bary1 && bary0 > bary2){
		// 12 hours
		bary = 256 + (bary1 - bary2) / 3;
	} else if( bary1 >= bary0 && bary1 >= bary2){
		// 4 hours
		bary = 86 + (bary2 - bary0) / 3;
	} else {
		// 8 hours
		bary = 171 + (bary0 - bary1) / 3;
	}

	// And we make the position modulo 256.
	_pos = bary % 256;

	// last, we compute the numbers of steps moved since last positon.
	_step = _pos - _prevPos;
	_stepCounter += _step;

	// last thing : wen we touch the wheel, we want to avoid a big gap with the previous touch.
	// So we return true only if the previous state was state_touch too.
	if(_state[_channels] != _prevState[_channels]) return false;
	return true;;
}

bool TouchWheel::updateRead(){
	_previous[_channels] = _current[_channels];
	_current[_channels] = 0;

	// for each channel
//	Serial.printf("channels :\t");
//	Serial.printf("deltas :\t");
	for(uint8_t i = 0; i <= _channels; ++i){
		// update reading
		if(i < _channels){
			_previous[i] = _current[i];
			_current[i] = touchRead(_pin[i]);
			_current[_channels] += _current[i];
//			Serial.printf("%i\t", _current[i]);
		} else {
			_current[_channels] /= _channels;
//				Serial.printf("\t\tglobal :\t%i\n", _current[_channels]);
		}
//		Serial.printf("%i\t", _current[i]);
		// See if we need filtering with esp32 touch !
		// We do need. :)
		uint32_t filtered = (uint32_t)_current[i] * _filterWeight + (uint32_t)_previous[i] * (255 - _filterWeight);
		filtered /= 0xff;
		_current[i] = filtered;
		// compute delta / baseline
		_delta[i] = _baseline[i] - _current[i];
		_prevState[i] = _state[i];

		// Change state according to delta
		if(_delta[i] > _thresholdRising){
			_state[i] = state_touch;
		} else if(_delta[i] > 0){
			_state[i] = state_rising;
		} else if(_delta[i] < 0){
			_state[i] = state_falling;
		} else {
			_state[i] = state_idle;
		}

		// We need to know if the touch channels dont drift with time, so we 
		if(_state[i] != _prevState[i]){
			_counter[i] = millis();
		}

		// If we are in touch state, we need to go back to idle when above release threshold
		// or check calibration if we have several consecutive rising or falling states.
		if(_state[i] == state_touch){
			if(_delta[i] < _thresholdFalling){
				_state[i] = state_falling;
			}
		} else if(i != _channels || _state[i] == state_rising || _state[i] == state_falling){
			updateCal(i);
		}
	}
//	Serial.println();
	return (_state[_channels] == state_touch);
}

// When we have several successive raising or falling state, we must check if the touch channels are not drifting.
// And adjust the baseline if need be.
void TouchWheel::updateCal(uint8_t channel){
	uint16_t limit = 0;
	int8_t change = 1;
	if(_state[channel] == state_rising){
		limit = _timeRising;
		change = -1;
	} else {
		limit = _timeFalling;
		change = 1;
	}

	if(_counter[channel] > (millis() + limit)){
		_baseline[channel] += change;
	}
}	

