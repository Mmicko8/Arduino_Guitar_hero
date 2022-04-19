#include "digital.h"

// init functies
/**
 werk hier pinMode uit.
*/

// zoek de bitmask op die hoort bij een pin, en sla op in een variabele bitmask
// zoek de poortnaam op die hoort bij een pin, en sla op in een variabele portname

// zoek het het adres van data direction register (DDRX) op dat hoort bij die portname, sla op in variabele ddrx (pointer)
// zoek het het adres van port output register (PORTX) op dat hoort bij die portname, sla op in variabele portx (pointer)

// voor de volgende if-statements gebruik je de bitmask met bitoperaties
  // als de mode INPUT is configureer ddrx en portx als input
  // als de mode INPUT_PULLUP is configureer ddrx en portx als input met pullup
  // als de mode OUTPUT is configueer je ddrx als output

void pinMode(uint8_t pin, uint8_t mode){
	uint8_t bitmask = digitalpin_to_bitmask[pin];
	uint8_t portname = digitalpin_to_portname[pin];
	volatile uint8_t *ddrx = DDRX[pin];
	volatile uint8_t *portx = PORTX[pin];
	
	if (mode == INPUT) {
		*ddrx &= ~bitmask;
		*portx &= ~bitmask;
	} else if (mode == INPUT_PULLUP){
		*ddrx &= ~bitmask;
		*portx |= bitmask;
	} else if (mode == OUTPUT){
		*ddrx |= bitmask;
	}
}



// digitale functies
/**
 werk hier digitalWrite uit.
*/

// zoek de bitmask op die hoort bij een pin, en sla op in een variabele bitmask
// zoek de poortnaam op die hoort bij een pin, en sla op in een variabele portname

// zoek het het adres van port output register (PORTX) op dat hoort bij die portname, sla op in variabele portx (pointer)

// voor de volgende if-statements gebruik je de bitmask met bitoperaties
   // als de value LOW is, configureer je portx met het bitmask
   // als de value HIGH is, configureer je portx met het bitmask
   
   
 void digitalWrite(uint8_t pin, uint8_t value) {
	uint8_t bitmask = digitalpin_to_bitmask[pin];
	uint8_t portname = digitalpin_to_portname[pin];
	volatile uint8_t *portx = PORTX[portname];
	
	if (value == LOW) {
		*portx &= ~bitmask;
	} else {
		*portx |= bitmask;
	}
 }
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
