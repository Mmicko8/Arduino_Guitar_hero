#include <avr/io.h>

// definitie van logische levels
#define LOW 0
#define HIGH 1

// definitie van de pin modi
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2

// defintie van de poortnamen
#define PB 0
#define PC 1
#define PD 2

// declaraties van de arrays
// keyword extern is nodig en kan vergeleken worden met public uit Java
extern volatile uint8_t* DDRX[];
extern volatile uint8_t* PORTX[];
extern volatile uint8_t* PINX[];
extern uint8_t digitalpin_to_portname[];
extern uint8_t digitalpin_to_bitmask[];
extern uint8_t digitalpin_to_timer[];
