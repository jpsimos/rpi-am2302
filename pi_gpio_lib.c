/*
	Raspberry Pi model B+ simple GPIO library
	~~This library is based off of the pi_mmio library by Tony DiCola at Adafruit Industries~~
	
	Author Jacob Psimos
*/

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include "pi_gpio_lib.h"

#ifndef _PI_GPIO_DEFS_
#define _PI_GPIO_DEFS_
	#define MODE_INPUT 1
	#define MODE_OUTPUT 2
	#define LOGIC_HIGH 1
	#define LOGIC_LOW 0
	#define LOGIC_ERROR 0xFF
	#define INIT_SUCCESS 1
	#define INIT_SKIPPED 2
	#define INIT_ERROR 3
	#define BCM2835_PERIF_BASE 0x20000000
	#define BCM2835_GPIO_BASE (BCM2835_PERIF_BASE + 0x200000)
	#define BCM2835_GPIO_LENGTH 4096
#endif

//#define GPIO_DEBUG

//volatile unsigned int* mem_map = NULL;

uint8_t init_gpio(void){	
	if(mem_map == (volatile unsigned int*)NULL){
		int handle = open("/dev/mem", O_RDWR | O_SYNC);
		if(handle == -1){
			#ifdef GPIO_DEBUG
				fprintf(stderr, "%s\n", "Failed to open /dev/mem");
			#endif
			return INIT_ERROR;
		}
		
		mem_map = (volatile unsigned int*)mmap(NULL, BCM2835_GPIO_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, handle, BCM2835_GPIO_BASE);
		close(handle);
		
		if(mem_map == MAP_FAILED){
			#ifdef GPIO_DEBUG
				fprintf(stderr, "%s\n", "Map Failed");
			#endif
			mem_map = (volatile unsigned int*)NULL;
			return INIT_ERROR;
		}else{
			return INIT_SUCCESS;
		}
	}
	return INIT_SKIPPED;
}

void uninit_gpio(void){
	if(mem_map != (volatile unsigned int*)NULL){
		munmap((void*)mem_map, BCM2835_GPIO_LENGTH);
		mem_map = NULL;
	}
}

uint8_t gpio_mode(const unsigned int pin, const uint8_t mode){
	if(mem_map == (volatile unsigned int*)NULL){
		#ifdef GPIO_DEBUG
			fprintf(stderr, "%s\n", "gpio_mode() - break");
		#endif
		return 0;
	}
	
	switch(mode){
		case MODE_INPUT:
			*(mem_map+((pin)/10)) &= ~(7<<(((pin)%10)*3));
		break;
		case MODE_OUTPUT:
			*(mem_map+((pin)/10)) &= ~(7<<(((pin)%10)*3));
			*(mem_map+((pin)/10)) |=  (1<<(((pin)%10)*3));
		break;
	}
	
	return 1;
}

uint8_t gpio_set(const unsigned int pin, const uint8_t logic){
	if(mem_map == (volatile unsigned int*)NULL){
		#ifdef GPIO_DEBUG
			fprintf(stderr, "%s\n", "gpio_set() - break");
		#endif
		return 0;
	}
	
	
	switch(logic){
		case LOGIC_HIGH:
			*(mem_map+7) = 1 << pin;
		break;
		case LOGIC_LOW:
			*(mem_map+10) = 1 << pin;
		break;
		default:
			#ifdef GPIO_DEBUG
				fprintf(stderr, "%s\n", "gpio_set() - invalid logic input");
			#endif
			return 0;
		break;
	}
	
	return 1;
}

uint8_t gpio_read(const unsigned int pin){
	if(mem_map == (volatile unsigned int*)NULL){
		#ifdef GPIO_DEBUG
			fprintf(stderr, "%s\n", "gpio_read() - break");
		#endif
		return LOGIC_ERROR;
	}
	
	if( (*(mem_map+13) & (1 << pin)) > 0 ){
		return LOGIC_HIGH;
	}
	return LOGIC_LOW;
}

