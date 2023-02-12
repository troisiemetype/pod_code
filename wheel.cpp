// #include "wheel.h"
#include "esPod.h"

TouchWheel::TouchWheel(uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5){
	_pin[0] = pin0;
	_pin[1] = pin1;
	_pin[2] = pin2;
	_pin[3] = pin3;
	_pin[4] = pin4;
	_pin[5] = pin5;
}

TouchWheel::~TouchWheel(){

}

// TODO : see if we can ponderate each value instead of computing them around the strongest.
void TouchWheel::init(){

	for(uint8_t i = 0; i < _channels; ++i){
		_current[i] = 0;
		_previous[i] = 0;
		_delta[i] = 0;
		_baseline[i] = 0;
	}

	_timeRising = 500;
	_timeFalling = 100;
	_timeTouching = 10000;

	_filterWeight = 20;

	_steps = 255;

	tuneBaseline(32);
	tuneThreshold(32);
}

void TouchWheel::setSteps(uint8_t steps){
	if(steps < 12) steps = 12;
	_steps = steps;
}

void TouchWheel::reverse(){
	_invert ^= 1;
}

void TouchWheel::tuneBaseline(uint8_t cycles){
	_baseline[0] = 0;
	_baseline[1] = 0;
	_baseline[2] = 0;
	_baseline[3] = 0;
	_baseline[4] = 0;
	_baseline[5] = 0;

//	Serial.printf("calibrating baseline\n");
	for(uint8_t i = 0; i < cycles; ++i){
//		Serial.printf("channels :\t");
		for(uint8_t j = 0; j < _channels; ++j){
			uint8_t touch = touchRead(_pin[j]);
			_baseline[j] += touch;
//			Serial.printf("%i\t", touch);
//			_baseline[_channels] += touchRead(_pin[j]);
		}
//		Serial.println();
	}
//	_baseline[_channels] /= _channels;

//	Serial.printf("baseline :\t");
	for(uint8_t i = 0; i < _channels; ++i){
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

//	Serial.printf("calibrating threshold\n");
	for(uint8_t i = 0; i < cycles; ++i){
//		Serial.printf("cycle : %i , read / min / max : \t", i);
		for(uint8_t j = 0; j < _channels; ++j){
			_current[j] = touchRead(_pin[j]);
			if(_current[j] > max[j]) max[j] = _current[j];
			if(_current[j] < min[j]) min[j] = _current[j];
//			Serial.printf("%i\t%i\t%i\t\t", _current[j], min[j], max[j]);
		}
//		Serial.println();
	}

	int16_t delta = 0;
	for(uint8_t i = 0; i < _channels; ++i){
		delta += max[i] - min[i];
	}

	delta /= _channels;
/*
	_thresholdRising = (float)delta * 0.4;
	_thresholdFalling = (float)delta * 0.4;
*/
	_thresholdRising = 18;
	_thresholdFalling = 22;

/*
	Serial.printf("max delta :\t%i\n", delta);
	Serial.printf("threshold rising:\t%i\n", _thresholdRising);
	Serial.printf("threshold falling:\t%i\n", _thresholdFalling);
*/
}

bool TouchWheel::touched(){
	return (bool)_touch;
}

int8_t TouchWheel::getPosition(){
	return _pos;
}

int8_t TouchWheel::getStep(){
	return _step;
}

bool TouchWheel::update(){
//	Serial.printf("values :\t%i\t%i\t%i\t%i\n", _delta[0], _delta[1], _delta[2], _delta[3]);
	updateRead();
//	Serial.printf("channels :\t");
/*
//	This part needs tweaking. testing positive channels and updating them almost works
// 	but still lacks precision.
	for(uint8_t i = 0; i < _channels; ++i){
//		Serial.printf("%i\t", (bool)(_touch & (1 << i)));
	}
//	Serial.println();

	_prevPos = _pos;

	switch(_touch){
		case 0b000001:
			_pos = 1;
			break;
		case 0b000011:
			_pos = 2;
			break;
		case 0b000010:
			_pos = 3;
//			_pos = 2;
			break;
		case 0b000110:
			_pos = 4;
			break;
		case 0b000100:
			_pos = 5;
//			_pos = 3;
			break;
		case 0b001100:
			_pos = 6;
			break;
		case 0b001000:
			_pos = 7;
//			_pos = 4;
			break;
		case 0b011000:
			_pos = 8;
			break;
		case 0b010000:
			_pos = 9;
//			_pos = 5;
			break;
		case 0b110000:
			_pos = 10;
			break;
		case 0b100000:
			_pos = 11;
//			_pos = 6;
			break;
		case 0b100001:
			_pos = 12;
			break;
		default:
			break;
	}
	
	_step = _pos - _prevPos;
	if(_step > 6) _step -= 12;
	if(_step < -6) _step += 12;
//	if(_step > 3) _step -= 6;
//	if(_step < -3) _step += 6;
*/
	uint8_t maxDelta = 0;
	uint8_t maxID = 0;
	// We start by finding the max touch intensity on every pads.
//	Serial.printf("deltas :");
	for(uint8_t i = 0; i < _channels; ++i){
//		Serial.printf("\t%i", _delta[i]);
		if(_delta[i] > maxDelta){
			maxDelta = _delta[i];
			maxID = i;
		}
	}

	if(maxDelta < _thresholdRising) return false;

	int8_t prev = maxID - 1;
	if(prev < 0) prev += _channels;

	int16_t delta1 =  _delta[prev];
	int16_t delta2 = _delta[maxID];
	int16_t delta3 = _delta[(maxID + 1) % _channels];

	float totalDelta = delta1 + delta2 + delta3;
	float prorata1 = delta1 / totalDelta;
//	float prorata2 = delta2 / totalDelta;			// not used !!
	float prorata3 = delta3 / totalDelta;

	float offset = 0;
	if(delta1 > delta3){
		offset = -prorata1;
	} else {
		offset = prorata3;
	}

	float position = (float)maxID + offset;
	if(position < 0) position += _channels;

	_prevPos = _pos;
//	_pos = (position / _channels) * 256;
	_pos = ((position / _channels) * 256) / (float)(256 / _steps);

	int8_t steps = _pos - _prevPos;

	_step = steps;
	if(_invert) _step = 0 - _step;

//	Serial.printf("\tposition : %i\n", _pos);
//	Serial.printf("\tstep : %i\n", _step);

	if(!_prevTouch) return false;
	return true;
}

bool TouchWheel::updateRead(){

	_prevTouch = _touch;
	_touch = 0;

	// for each channel
//	Serial.printf("channels :\t");
//	Serial.printf("deltas :\t");
	for(uint8_t i = 0; i < _channels; ++i){
		// update reading
		_previous[i] = _current[i];
		_current[i] = touchRead(_pin[i]);
//		Serial.printf("%i\t", _current[i]);
		// See if we need filtering with esp32 touch !
		// We do need. :)
		uint32_t filtered = (uint32_t)_current[i] * _filterWeight + (uint32_t)_previous[i] * (255 - _filterWeight);
		filtered /= 0xff;
		_current[i] = filtered;
//		Serial.printf("%i\t", _current[i]);
		// compute delta / baseline
		_delta[i] = _baseline[i] - _current[i];
		_prevState[i] = _state[i];
//		Serial.printf("%i\t", _delta[i]);

		// Change state according to delta
		if(_delta[i] > _thresholdRising){
			_state[i] = state_touch;
			_touch |= (1 << i);
		} else if((_prevState[i] == state_touch) && (_delta[i] < _thresholdFalling)){
			_state[i] = state_idle;
		} else if(_delta[i] > 0){
			_state[i] = state_rising;
		} else if(_delta[i] < 0){
			_state[i] = state_falling;
		} else {
			_state[i] = state_idle;
		}

//		Serial.printf("%i, state : %i/%i", _delta[i], _state[i], _prevState[i]);

		// We need to know if the touch channels dont drift with time, so we 
		if(_state[i] != _prevState[i]){
			_counter[i] = millis();
		}
	
		if(_state[i] != state_idle) updateCal(i);
//		Serial.print('\t');
	}
//	Serial.println();
//	return (_state[_channels] == state_touch);
	return _touch;
}

// When we have several successive raising or falling state, we must check if the touch channels are not drifting.
// And adjust the baseline if need be.
void TouchWheel::updateCal(uint8_t channel){
	uint32_t limit = 0;
	int8_t change = 0;
	switch(_state[channel]){
		case state_idle:
			return;
		case state_rising:
			limit = _timeRising;
			change = -1;
			break;
		case state_falling:
			limit = _timeFalling;
			change = 1;
			break;
		case state_touch:
			limit = _timeTouching;
			change = -10;
		default :
			return;
	}

	if(millis() > (_counter[channel] + limit)){
//		Serial.print('c');
		_baseline[channel] += change;
		_state[channel] = state_idle;
	}
}	

