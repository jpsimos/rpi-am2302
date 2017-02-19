/*
	Raspberry Pi model B+ simple GPIO library
	~~This library is based off of the pi_mmio library by Tony DiCola at Adafruit Industries~~
	
	Author Jacob Psimos
*/

#ifndef _PI_GPIO_LIB_
#define _PI_GPIO_LIB_

#include <stdint.h>

#ifndef _TYPICAL_DEFS_
#define _TYPICAL_DEFS_
	typedef uint8_t byte;
	typedef uint8_t boolean;
	#define TRUE 1
	#define FALSE 0
#endif

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
#endif

volatile unsigned int *mem_map;

byte init_gpio(void);
void uninit_gpio(void);

static inline boolean gpio_mode(const unsigned int pin, const byte mode){
	if(mem_map == (volatile unsigned int*)NULL){
		#ifdef GPIO_DEBUG
			fprintf(stderr, "%s\n", "gpio_mode() - break");
		#endif
		return FALSE;
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
	
	return TRUE;
}

static inline boolean gpio_set(const unsigned int pin, const byte logic){
	if(mem_map == (volatile unsigned int*)NULL){
		#ifdef GPIO_DEBUG
			fprintf(stderr, "%s\n", "gpio_set() - break");
		#endif
		return FALSE;
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
			return FALSE;
		break;
	}
	
	return TRUE;
}

static inline byte gpio_read(const unsigned int pin){
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

#endif