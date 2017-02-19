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

#define BCM2835_PERIF_BASE 0x20000000
#define BCM2835_GPIO_BASE (BCM2835_PERIF_BASE + 0x200000)
#define BCM2835_GPIO_LENGTH 4096

//#define GPIO_DEBUG

//volatile unsigned int* mem_map = NULL;

byte init_gpio(void){	
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
			mem_map = NULL;
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

