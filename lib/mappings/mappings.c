 
#include "mappings.h"

#include <avr/io.h>

// port_to_mode_PGM
volatile uint8_t* DDRX[] = {
    &DDRB,
    &DDRC,
    &DDRD,
};

// port_to_output_PGM
volatile uint8_t* PORTX[] = {
    &PORTB,
    &PORTC,
    &PORTD,
};

// port_to_input_PGM
volatile uint8_t* PINX[] = {
    &PINB,
    &PINC,
    &PIND,
};

// digital_pin_to_port_PGM
uint8_t digitalpin_to_portname[] = {
    PD, /* pin 0 */
    PD,
    PD,
    PD,
    PD,
    PD,
    PD,
    PD,
    PB, /* pin 8 */
    PB,
    PB,
    PB,
    PB,
    PB,
    PC, /* pin 14 */
    PC,
    PC,
    PC,
    PC,
    PC,
};

// digital_pin_to_bit_mask_PGM
uint8_t digitalpin_to_bitmask[] = {
    _BV(PD0), /* 0, port D */
    _BV(PD1),
    _BV(PD2),
    _BV(PD3),
    _BV(PD4),
    _BV(PD5),
    _BV(PD6),
    _BV(PD7),
    _BV(PB0), /* 8, port B */
    _BV(PB1),
    _BV(PB2),
    _BV(PB3),
    _BV(PB4),
    _BV(PB5),
    _BV(PC0), /* 14, port C */
    _BV(PC1),
    _BV(PC2),
    _BV(PC3),
    _BV(PC4),
    _BV(PC5),
};