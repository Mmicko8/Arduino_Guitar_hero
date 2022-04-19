#include <avr/io.h>
#include "mappings.h"

#define NUM_DIGITAL_PINS 20

// definitie van logische levels
#define LOW 0x0
#define HIGH 0x1

// definitie van de pin modi
#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

// defintie van de digitale poortnamen
#define PB 0
#define PC 1
#define PD 2

extern volatile uint8_t* DDRX[];
extern volatile uint8_t* PORTX[];
extern volatile uint8_t* PINX[];
extern uint8_t digitalpin_to_portname[];
extern uint8_t digitalpin_to_bitmask[];
extern uint8_t digitalpin_to_timer[];

// init functies
void pinMode(uint8_t pin, uint8_t mode);

// digitale functies
void digitalWrite(uint8_t pin, uint8_t value);

