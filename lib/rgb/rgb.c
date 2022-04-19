#include <avr/io.h>
#include <util/delay.h>

#include "buttons.h"
#include "display.h"
#include "leds.h"

#define GREEN 'g'
#define RED 'r'
#define BLUE 'b'
#define PURPLE 'p'
#define CYAN 'c'

#define GREEN_PIN PB1
#define RED_PIN PD5
#define BLUE_PIN PD6

void enableLedsRGB()
{
    DDRB |= _BV(PB1);
    DDRD |= _BV(PD5);
    DDRD |= _BV(PD6);
}

void lightUpLedRGB(char color)
{
    if (color == RED) {
        PORTD |= _BV(RED_PIN);
    }
    else if (color == GREEN) {
        PORTB |= _BV(GREEN_PIN);
    }
    else if (color == PURPLE) {
        PORTD |= _BV(RED_PIN);
        PORTD |= _BV(BLUE_PIN);
    }
	else if (color == CYAN) {
        PORTB |= _BV(GREEN_PIN);
        PORTD |= _BV(BLUE_PIN);
    }
    else {
        PORTD |= _BV(BLUE_PIN);
    }
}

void lightDownLedRGB(char color)
{
    if (color == RED) {
        PORTD &= ~_BV(RED_PIN);
    }
    else if (color == GREEN) {
        PORTB &= ~_BV(GREEN_PIN);
    }
    else if (color == PURPLE) {
        PORTD &= ~_BV(RED_PIN);
        PORTD &= ~_BV(BLUE_PIN);
    }
	else if (color == CYAN) {
        PORTB &= ~_BV(GREEN_PIN);
        PORTD &= ~_BV(BLUE_PIN);
    }
    else {
        PORTD &= ~_BV(BLUE_PIN);
    }
}

void lightDownAllRGB() {
    PORTD &= (~_BV(RED_PIN)) & (~_BV(BLUE_PIN));
	PORTB &= ~_BV(GREEN_PIN);
}