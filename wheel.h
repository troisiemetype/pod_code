#ifndef ESPOD_WHEEL_H
#define ESPOD_WHEEL_H

#include <Arduino.h>

class TouchWheel{
public:
	TouchWheel(uint8_t pin0, uint8_t pin1, uint8_t pin2);
	~TouchWheel();

	void init();

	void setSteps(uint8_t steps);

	void tuneBaseline(uint8_t cycles = 16);
	void tuneThreshold(uint8_t cycles = 16);

	bool touched();

	int8_t getPosition();
	int8_t getStep();

	bool update();

private:
	enum state_t{
		state_idle = 0,
		state_rising,
		state_falling,
		state_touch,
	};

	bool updateRead();
	void updateCal(uint8_t channel);
	const uint8_t _channels = 3;

	uint8_t _pin[3];

	uint16_t _current[4];
	uint16_t _previous[4];
	int16_t _delta[4];
	uint16_t _baseline[4];
	uint16_t _weight[3];

	state_t _prevState[4];
	state_t _state[4];

	uint32_t _counter[4];

	uint8_t _pos;
	uint8_t _prevPos;
	int8_t _step;
	int8_t _stepCounter;

	uint8_t _steps;

	uint16_t _thresholdRising;
	uint16_t _thresholdFalling;

	uint16_t _timeRising;
	uint16_t _timeFalling;

	uint8_t _filterWeight;
};

#endif