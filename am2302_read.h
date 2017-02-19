/*
	Raspberry Pi model B+ AM2302 Reader
	~~This program contains heavily modified code from Tony DiCola at Adafruit Industries~~
	
	Author Jacob Psimos
*/

#ifndef _AM2302_READ_
#define _AM2302_READ_

#include <stdint.h>

#ifndef _TYPICAL_DEFS_
#define _TYPICAL_DEFS_
	typedef uint8_t byte;
	typedef uint8_t boolean;
	#define TRUE 1
	#define FALSE 0
#endif

#ifndef _AM2302_DEFS_
#define _AM2302_DEFS_
	#define AM2302_MAXCOUNT 30000
	#define AM2302_PULSES 41
	#define AM2302_PULSES_BUFFER (AM2302_PULSES * 2)
	#define AM2302_FAILED_CHECKSUM 1
	#define AM2302_TIMEOUT 2
	#define AM2302_ERROR 3
	#define AM2302_ABORT 4
	#define AM2302_SUCCESS 5
#endif

void accurate_sleep_milliseconds(unsigned int millis);
void sleep_milliseconds(unsigned int millis);
boolean set_max_priority(void);
boolean set_default_priority(void);
byte am2302_read(const unsigned int pin, float* humidity, float* temperature);

#endif
