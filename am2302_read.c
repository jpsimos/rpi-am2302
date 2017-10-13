/*
	Raspberry Pi model B+ AM2302 Reader
	~~This program contains heavily modified code from Tony DiCola at Adafruit Industries~~
	
	Author Jacob Psimos
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "pi_gpio_lib.h"
#include "am2302_read.h"

#ifndef _AM2302_DEFS_
#define _AM2302_DEFS_
	#define AM2302_MAXCOUNT 30000
	#define AM2302_PULSES 41
	#define AM2302_PULSES_BUFFER (AM2302_PULSES * 2)
	#define AM2302_FAILED_CHECKSUM 104
	#define AM2302_TIMEOUT 100
	#define AM2302_ERROR 101
	#define AM2302_ABORT 102
	#define AM2302_SUCCESS 103
#endif

void accurate_sleep_milliseconds(unsigned int millis){
  struct timeval deltatime;
  deltatime.tv_sec = millis / 1000;
  deltatime.tv_usec = (millis % 1000) * 1000;
  struct timeval walltime;
  gettimeofday(&walltime, (struct timezone*)NULL);
  struct timeval endtime;
  timeradd(&walltime, &deltatime, &endtime);
  while (timercmp(&walltime, &endtime, <)) {
	gettimeofday(&walltime, (struct timezone*)NULL);
  }
}

void sleep_milliseconds(unsigned int millis){
  struct timespec sleep;
  sleep.tv_sec = millis / 1000;
  sleep.tv_nsec = (millis % 1000) * 1000000L;
  while (clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep, &sleep) && errno == EINTR);
}

unsigned char set_max_priority(void){
  struct sched_param sched;
  memset(&sched, 0, sizeof(sched));
  sched.sched_priority = sched_get_priority_max(SCHED_FIFO);
  return (sched_setscheduler(0, SCHED_FIFO, &sched) == 0);
}

unsigned char set_default_priority(void){
  struct sched_param sched;
  memset(&sched, 0, sizeof(sched));
  sched.sched_priority = 0;
  return (sched_setscheduler(0, SCHED_OTHER, &sched) == 0);
}

uint8_t am2302_read(const unsigned int pin, float* humidity, float* temperature){
	if (humidity == (float*)NULL || temperature == (float*)NULL) {
		return AM2302_ERROR;
	}
	
	if(init_gpio() == INIT_ERROR){
		return AM2302_ABORT;
	}

	*temperature = 0.0f;
	*humidity = 0.0f;

	int pulseCounts[AM2302_PULSES_BUFFER];
	memset(pulseCounts, 0, AM2302_PULSES_BUFFER * sizeof(int));

	gpio_mode(pin, MODE_OUTPUT);
	if(!set_max_priority()){
		return AM2302_ERROR;
	}
	
	gpio_set(pin, LOGIC_HIGH);
	sleep_milliseconds(500);

	gpio_set(pin, LOGIC_LOW);
	accurate_sleep_milliseconds(20);

	gpio_mode(pin, MODE_INPUT);
	
	for(volatile int i = 0; i < 50; ++i){ /* extremely short delay */ }
	
	unsigned int count = 0;
	while(gpio_read(pin)){
		if(++count >= AM2302_MAXCOUNT){
			set_default_priority();
			return AM2302_TIMEOUT;
		}
	}
	
	for(int i = 0; i < AM2302_PULSES_BUFFER; i += 2){
		while(!gpio_read(pin)){
			if(++pulseCounts[i] >= AM2302_MAXCOUNT){
				set_default_priority();
				return AM2302_TIMEOUT;
			}
		}
		
		while(gpio_read(pin)){
			if(++pulseCounts[i + 1] >= AM2302_MAXCOUNT){
				set_default_priority();
				return AM2302_TIMEOUT;
			}
		}
	}
	
	set_default_priority();
	
	unsigned int threshold = 0;
	for(int i = 2; i < AM2302_PULSES_BUFFER; i += 2){
		threshold += pulseCounts[i];
	}
	threshold /= (AM2302_PULSES - 1);
	
	uint8_t data[5];
	memset(data, 0, sizeof(data));

	for(int i = 3; i < AM2302_PULSES_BUFFER; i += 2){
		int index = (i - 3) / 16;
		data[index] <<= 1;
		
		if(pulseCounts[i] >= threshold){
			data[index] |= 1;
		}
	}

	
	if(data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)){
		*humidity = (float)(data[0] * 256 + data[1]) / 10.0f;
		*temperature = (float)((data[2] & 0x7F) * 256 + data[3]) / 10.0f;
		if(data[2] & 0x80){
			*temperature *= -1.0f;
		}
		
		return AM2302_SUCCESS;
	}else{
		return AM2302_FAILED_CHECKSUM;
	}
	return AM2302_ERROR;
}
