/*
	Raspberry Pi model B+ AM2302 Reader
	~~This program contains heavily modified code from Tony DiCola at Adafruit Industries~~
	
	Author Jacob Psimos
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "pi_gpio_lib.h"
#include "am2302_read.h"

float celc_to_f(float temp);

boolean parseArguments(const int argc, char **argv, 
	byte *maxTries, unsigned int *retryInterval, unsigned int *dataPin);

int main(int argc, char **argv){

	byte maxTries = 10;						//Maximum number of re-read attempts
	unsigned int retryInterval = 2;		//Time to wait before re-reading in millisecs
	unsigned int dataPin = 17;
	
	if(!parseArguments(argc, argv, &maxTries, &retryInterval, &dataPin)){
		printf("AM2302 Sensor Reader by Jacob Psimos\n");
		printf("USAGE: [data pin] [maximum read attempts] [re-read interval secs]\n");
		printf("Argument 2 and 3 are optional, defaulting to 10 and 2\n");
		return 1;
	}
	
	float temp = 0.0f;
	float humid = 0.0f;

	int failedChecksums = 0;
	int timeouts = 0;
	byte result;
	byte attempt;
	
	for(attempt = 1; attempt <= maxTries; attempt++){
	
		result = am2302_read(dataPin, &humid, &temp);
		if(result == AM2302_FAILED_CHECKSUM){
			failedChecksums++;
		}else if(result == AM2302_TIMEOUT){
			timeouts++;
		}else if(result == AM2302_ABORT){
			fprintf(stderr, "%s\n", "am2302_read() - abort");
			break;
		}else if(result == AM2302_SUCCESS){
			break;
		}
		sleep(retryInterval);
	}
	
	uninit_gpio();
	
	if(result == AM2302_SUCCESS){
		temp = celc_to_f(temp);
		printf("Temperature = %d Humidity = %d Failed Checksums = %d Timeouts = %d Iterations = %d\n", (int)temp, (int)humid, failedChecksums, timeouts, attempt);
	}else{
		return 1;
	}

	return 0;
}

float celc_to_f(float temp){
	return roundf(temp * 9.0 / 5.0 + 32.0);
}

boolean parseArguments(const int argc, char **argv, 
	byte *maxTries, unsigned int *retryInterval, unsigned int *dataPin){
	
	if(maxTries == NULL || retryInterval == NULL || dataPin == NULL){
		return FALSE;
	}
	
	if(argc == 2){
		unsigned int _dataPin = (unsigned int)strtoul(argv[1], NULL, 10);
		if(_dataPin >= 0 && _dataPin <= 40){
			*dataPin = _dataPin;
			return TRUE;
		}
	}else if(argc == 3){
		unsigned int _dataPin = (unsigned int)strtoul(argv[1], NULL, 10);
		byte _maxTries = (byte)strtoul(argv[2], NULL, 10);
		*dataPin = _dataPin;
		*maxTries = (_maxTries > 32 ? 32 : _maxTries);
		return TRUE;
	}else if(argc == 4){
		unsigned int _dataPin = (unsigned int)strtoul(argv[1], NULL, 10);
		byte _maxTries = (byte)strtoul(argv[2], NULL, 10);
		unsigned int _retryInterval = (unsigned int)strtoul(argv[3], NULL, 10);
		*dataPin = _dataPin;
		*maxTries = (_maxTries > 32 ? 32 : _maxTries);
		*retryInterval = (_retryInterval > 60 ? 60 : _retryInterval);
		return TRUE;
	}
	
	return FALSE;
}
