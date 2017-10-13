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
const char *getErrorString(const uint8_t num);

int main(int argc, char **argv){


	
	if(argc == 1){
		printf("AM2302 Sensor Reader\n");
		printf("USAGE: [switches] [-d data pin] [-m maximum attempts]\n");
		printf("SWITCHES: [-c  print temperature in celcius] [-r  round results] [-j  output in JSON format]\n");
		printf("Returns 0 on success and 1 for error.\n");
		return 1;
	}
	
	unsigned int maxTries = 10;
	unsigned int retryInterval = 2;
	unsigned int dataPin = 22;
	uint8_t roundResults = 0;
	uint8_t printJson = 0;
	uint8_t printCelc = 0;
	uint8_t result = 0;
	
	int i;
	for(i = 0; i < argc; i++){
		if(!strncmp(argv[i], "-r", 3)){
			roundResults = 1;
		}
		
		if(!strncmp(argv[i], "-j", 3)){
			printJson = 1;
		}
		
		if(!strncmp(argv[i], "-c", 3)){
			printCelc = 1;
		}
		
		if(!strncmp(argv[i], "-d", 3)){
			if(argc >= i + 1){
				dataPin = (unsigned int)strtoul(argv[i + 1], (char**)NULL, 10);
				if(!(dataPin > 0 && dataPin < 255)){
					fprintf(stderr, "Invalid data pin: %u\n", dataPin);
					result = 1;
				}
			}else{
				fprintf(stderr, "%s\n", "Unable to parse data pin");
				result = 1;
			}
		}
		
		if(!strncmp(argv[i], "-m", 3)){
			if(argc >= i + 1){
				maxTries = (unsigned int)strtoul(argv[i + 1], (char**)NULL, 10);
				if(maxTries == 0){
					maxTries = 1;
				}
			}else{
				fprintf(stderr, "%s\n", "Unable to parse maximum tries");
				result = 1;
			}
		}
	}
	
	float temp = 0.0f;
	float humid = 0.0f;

	unsigned int failedChecksums = 0;
	unsigned int timeouts = 0;
	uint8_t attempt = 0;
	
	if(!result){
		for(attempt = 1; attempt <= maxTries; attempt++){
			result = am2302_read(dataPin, &humid, &temp);
			if(result == AM2302_FAILED_CHECKSUM){
				failedChecksums++;
			}else if(result == AM2302_TIMEOUT){
				timeouts++;
			}else if(result == AM2302_ABORT){
				break;
			}else if(result == AM2302_SUCCESS){
				break;
			}
			sleep(retryInterval);
		}
		
		uninit_gpio();
	}
	
	if(result == AM2302_SUCCESS){
		if(!printCelc){
			temp = roundResults ? roundf(celc_to_f(temp)) : celc_to_f(temp);
			humid = roundResults ? roundf(humid) : humid;
		}else{
			temp = roundResults ? roundf(temp) : temp;
			humid = roundResults ? roundf(humid) : humid;
		}
	}
	
	const char *strException = getErrorString(result);
	if(strException == (const char*)NULL){
		return 0xFF;
	}
	
	if(printJson){
		printf("{\"success\":\"%s\",\"temperature\":\"%f\",\"humidity\":\"%f\",\"unit\":\"%s\",\"exception\":\"%s\",\"failedchecksums\":\"%u\",\"timeouts\":\"%u\",\"iterations\":\"%u\"}", 
			result == AM2302_SUCCESS ? "true" : "false",
			temp,
			humid,
			printCelc ? "c" : "f",
			strException,
			failedChecksums,
			timeouts,
			attempt
		);
	}else{
		printf("Success = %s Temperature = %.3f Humidity = %.3f Failed Checksums = %d Timeouts = %d Iterations = %d Exception = %s\n", 
			result == AM2302_SUCCESS ? "true" : "false",
			temp, 
			humid, 
			failedChecksums, 
			timeouts, 
			attempt,
			strException
		);
	}

	return result == AM2302_SUCCESS ? 0 : result;
}

float celc_to_f(float temp){
	return (temp * 9.0 / 5.0 + 32.0);
}

const char *getErrorString(const uint8_t num){
	static const char *strChecksum = "Checksum mismatch";
	static const char *strTimeout = "Timeout";
	static const char *strAbort = "Aborted read";
	static const char *strInvalidArguments = "Invalid arguments";
	static const char *strNoException = "None";
	static const char *strError = "AM2302 general error";
	
	switch(num){
		case 1:
			return strInvalidArguments;
		break;
		case AM2302_SUCCESS:
			return strNoException;
		break;
		case AM2302_FAILED_CHECKSUM:
			return strChecksum;
		break;
		case AM2302_TIMEOUT:
			return strTimeout;
		break;
		case AM2302_ABORT:
			return strAbort;
		break;
		case AM2302_ERROR:
			return strError;
		break;
		default:
			return strError;
		break;
	}
	return strError;
}

