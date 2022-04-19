#include <avr/io.h>

#define LOW 0
#define HIGH 1
/* Define shift register pins used for seven segment display */
#define LATCH_DIO PD4
#define CLK_DIO PD7
#define DATA_DIO PB0

#define LSBFIRST 0
#define MSBFIRST 1
#define NUMBER_OF_SEGMENTS 8
#define SPACE 0xFF

#define sbi(register, bit) (register |= _BV(bit))
#define cbi(register, bit) (register &= ~_BV(bit))

#define NO_STRIPE 0b000
#define LOW_STRIPE 0b001
#define MID_STRIPE 0b010
#define MID_LOW_STRIPE 0b011
#define HIGH_STRIPE 0b100
#define HIGH_LOW_STRIPE 0b101
#define HIGH_MID_STRIPE 0b110
#define ALL_STRIPE 0b111

void initDisplay();
void writeNumberToSegment(uint8_t segment, uint8_t value);
void writeNumber(int number);
void writeNumberAndWait(int number, int delay);
void writeLetterToSegment(uint8_t segment, char letter);
void writeCharToSegment(uint8_t segment, char character);
void writeString(char* str);
void writeStringAndWait(char* str, int delay);
void writeEnd();
void writeEndAndWait(unsigned int delay);
void writeStringScroll(char* str, uint8_t repetitions);
void displaySmiley();
void displaySadSmiley();
void writeStripe(uint8_t segment, uint8_t stripe);
void displayGuitarHeroAndWait(uint8_t stripe0, uint8_t stripe1, uint8_t stripe2, uint8_t counter, uint16_t duration);