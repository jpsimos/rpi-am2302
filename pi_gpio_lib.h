/*
	Raspberry Pi model B+ simple GPIO library
	~~This library is based off of the pi_mmio library by Tony DiCola at Adafruit Industries~~
	
	Author Jacob Psimos
*/

#ifndef _PI_GPIO_LIB_
#define _PI_GPIO_LIB_

#include <stdint.h>

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

volatile unsigned int *mem_map;

uint8_t init_gpio(void);
void uninit_gpio(void);
uint8_t gpio_mode(const unsigned int pin, const uint8_t mode);
uint8_t gpio_set(const unsigned int pin, const uint8_t logic);
uint8_t gpio_read(const unsigned int pin);

#endif