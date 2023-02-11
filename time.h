#ifndef ESPOD_TIME_H
#define ESPOD_TIME_H

#include "esPod.h"

/* Ok, this might work, but there is the drawback that altought timers would be quite precise
 * (less the time for the interrupt to loop trough them), they would not "start" when started, but on the interrupt following their start)
 * Thus a better solution would probably use either of this solutions :
 * Hardware timers 0-3 for sensible timing, like audio counter.
 * Threads (tasks) with priority and sleep delay between calls, for less sensible timing, like polling the battery or sleep delays.
 */

/* List of timers needed :
 * 		Timers with precise timing (using hardware timers 0-3) :
 *			Elapsed playing time
 *
 *		Timers less crucial :
 *			Battery polling.
 *			Smooth backlight diming.
 *			Last user interraction, for diming backlight.
 *			Last user interraction, for puting the system to sleep when it's not playing
 *
 */

/*	This is a class definition to handle several timers from one hardware timer and its interrupt
 *
 *	TimerGroup handles the hardware timer definition :
 *		timer index (0-4)
 *		handle
 *		delay (1ms)
 *		interrupt function
 *
 *	It has static variables : millisecond increment (similar to millis() function)
 *	And static method: the function that loops through attached timers to call their own callback,
 *	that is called (/linked ?) by the timer interrupt
 *
 *	Timer is the specific timer needed for one task.
 *	It has user accessible methods :
 *		start()
 *		pause()
 *		stop()
 */
class Timer{
public:

private:

};

#endif