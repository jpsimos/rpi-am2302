/*
	Raspberry Pi model B+ AM2302 Reader
	~~This program contains heavily modified code from Tony DiCola at Adafruit Industries~~
	
	Author Jacob Psimos
*/

#ifndef _AM2302_READ_
#define _AM2302_READ_

#include <stdint.h>

#ifndef _AM2302_DEFS_
#define _AM2302_DEFS_
	#define AM2302_MAXCOUNT 30000
	#define AM2302_PULSES 41
	#define AM2302_PULSES_BUFFER (AM2302_PULSES * 2)
	#define AM2302_SUCCESS 100
	#define AM2302_TIMEOUT 101
	#define AM2302_ERROR 102
	#define AM2302_ABORT 103
	#define AM2302_FAILED_CHECKSUM 104
#endif

void accurate_sleep_milliseconds(unsigned int millis);
void sleep_milliseconds(unsigned int millis);
unsigned char set_max_priority(void);
unsigned char set_default_priority(void);
uint8_t am2302_read(const unsigned int pin, float* humidity, float* temperature);

#endif
