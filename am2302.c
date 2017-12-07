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

const float celc_to_f(const float temp);

int main(int argc, char **argv){


	if(argc == 1){
		printf("AM2302 Sensor Reader\n");
		printf("USAGE: [switches] [-d data pin] [-m maximum attempts]\n");
		printf("SWITCHES: [-c  print temperature in celcius] [-r  round results] [-j  output in JSON format]\n");
		printf("Returns 0 on success and 1 for error.\n");
		return 1;
	}

	unsigned int maxTries = 5;
	unsigned int retryInterval = 3;
	unsigned int dataPin = 22;
	uint8_t roundResults = 0;
	uint8_t printJson = 0;
	uint8_t printCelc = 0;
	uint8_t argumentError = 0;
	uint8_t result = 0;
	float temp = 0.0f;
	float humid = 0.0f;
	unsigned int failedChecksums = 0;
	unsigned int timeouts = 0;
	unsigned int iterations = 0;
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
					argumentError = 1;
				}
			}else{
				fprintf(stderr, "%s\n", "Unable to parse data pin");
				argumentError = 1;
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
				argumentError = 1;
			}
		}
	}

	if(argumentError){
		fprintf(stderr, "%s\n", "There was a problem parsing the command line argument(s).");
		exit(1);
	}

	for(iterations = 1; iterations <= maxTries; iterations++){
		result = am2302_read(dataPin, &humid, &temp);
		if(result == AM2302_SUCCESS || result == AM2302_ERROR){
			break;
		}

		if(result == AM2302_FAILED_CHECKSUM){
			failedChecksums++;
		}else if(result == AM2302_TIMEOUT){
			timeouts++;
		}

		sleep(retryInterval);
	}

	uninit_gpio();

	if(result == AM2302_SUCCESS){
		if(!printCelc){
			temp = roundResults ? roundf(celc_to_f(temp)) : celc_to_f(temp);
			humid = roundResults ? roundf(humid) : humid;
		}else{
			temp = roundResults ? roundf(temp) : temp;
			humid = roundResults ? roundf(humid) : humid;
		}
	}

	const char *jsonFormat = "{\"exit_code\":\"%u\",\"temperature\":\"%.3f\",\"humidity\":\"%.3f\",\"iterations\":\"%u\",\"failed_checksums\":\"%u\",\"timeouts\":\"%u\"}";
	const char *regularFormat = "Exitcode = %u Temperature = %.3f Humidity = %.3f Failed Checksums = %u Timeouts = %u Iterations = %u";

	if(printJson){
		printf(jsonFormat, result, temp, humid, iterations, failedChecksums, timeouts);
	}else{
		printf(regularFormat, result, temp, humid, iterations, failedChecksums, timeouts);
	}

	return result == AM2302_SUCCESS ? 0 : 1;
}

const float celc_to_f(const float temp){
	return (temp * 9.0 / 5.0 + 32.0);
}
