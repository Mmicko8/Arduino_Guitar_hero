#include "display.h"
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>

/* Segment byte maps for numbers 0 to 9 */
const uint8_t NUMBERS_MAP[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99,
                               0x92, 0x82, 0xF8, 0X80, 0X90
};

const uint8_t ALPHABET_MAP[] = {0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E, 0xC2,
                                0x89, 0xCF, 0xE1, 0x8A, 0xC7, 0xEA, 0xC8,
                                0xC0, 0x8C, 0x4A, 0xCC, 0x92, 0x87, 0xC1,
                                0xC1, 0xD5, 0x89, 0x91, 0xA4
};
								
const uint8_t STRIPE_MAP[] = {	0xFF, // no stripe
								0xEF, // LOW stripe
								0xFD, // MID stripe 
								0xED, // MID LOW stripe
								0x7F, // HIGH stripe
								0x6F, // HIGH LOW stripe
								0x7D, // HIGH MID stripe
								0x6D // all stripe
};

/* Byte maps to select digit 1 to 4 */
const uint8_t SEGMENT_SELECT[] = {0xF1, 0xF2, 0xF4, 0xF8};

void initDisplay() {
    sbi(DDRD, LATCH_DIO);
    sbi(DDRD, CLK_DIO);
    sbi(DDRB, DATA_DIO);
}

// loop through seven segments of LED display and shift the correct bits in the
// data register
void shift(uint8_t val, uint8_t bitorder) {
    uint8_t bit;

    for (uint8_t i = 0; i < NUMBER_OF_SEGMENTS; i++) {
        if (bitorder == LSBFIRST) {
            bit = !!(val & (1 << i));
        } else {
            bit = !!(val & (1 << (7 - i)));
        }
        // write bit to register
        if (bit == HIGH)
            sbi(PORTB, DATA_DIO);
        else
            cbi(PORTB, DATA_DIO);

        // Trigger the clock pin so the display updates
        sbi(PORTD, CLK_DIO);
        cbi(PORTD, CLK_DIO);
    }
}

//Schrijft cijfer naar bepaald segment. Segment 0 is meest linkse.
void writeNumberToSegment(uint8_t segment, uint8_t value) {
    cbi(PORTD, LATCH_DIO);
    shift(NUMBERS_MAP[value], MSBFIRST);
    shift(SEGMENT_SELECT[segment], MSBFIRST);
    sbi(PORTD, LATCH_DIO);
}

void displaySmiley(){
	cbi(PORTD, LATCH_DIO);
    shift(0b11101101, LSBFIRST);
    shift(SEGMENT_SELECT[1], MSBFIRST);
    sbi(PORTD, LATCH_DIO);
	_delay_ms(5);
	
	cbi(PORTD, LATCH_DIO);
    shift(0b00001111, LSBFIRST);
    shift(SEGMENT_SELECT[2], MSBFIRST);
    sbi(PORTD, LATCH_DIO);
	_delay_ms(5);
	
}

void displaySadSmiley(){
	cbi(PORTD, LATCH_DIO);
    shift(0b11101101, LSBFIRST);
    shift(SEGMENT_SELECT[1], MSBFIRST);
    sbi(PORTD, LATCH_DIO);
	_delay_ms(5);
	
	cbi(PORTD, LATCH_DIO);
    shift(0b01100011, LSBFIRST);
    shift(SEGMENT_SELECT[2], MSBFIRST);
    sbi(PORTD, LATCH_DIO);
	_delay_ms(5);
}

//Schrijft getal tussen 0 en 9999 naar de display. Te gebruiken in een lus...
void writeNumber(int number) {
    if (number < 0 || number > 9999)
        return;
    writeNumberToSegment(0, number / 1000);
    writeNumberToSegment(1, (number / 100) % 10);
    writeNumberToSegment(2, (number / 10) % 10);
    writeNumberToSegment(3, number % 10);
}

//Schrijft getal tussen 0 en 9999 naar de display en zorgt dat het er een bepaald aantal milliseconden blijft staan.
//Opgelet: de timing is "ongeveer", er wordt geen rekening gehouden met de tijd writeNumberToSegment in beslag neemt...
void writeNumberAndWait(int number, int delay) {
    if (number < 0 || number > 9999)
        return;
    for (int i = 0; i < delay / 20; i++) {
        writeNumberToSegment(0, number / 1000);
        _delay_ms(5);
        writeNumberToSegment(1, (number / 100) % 10);
        _delay_ms(5);
        writeNumberToSegment(2, (number / 10) % 10);
        _delay_ms(5);
        writeNumberToSegment(3, number % 10);
        _delay_ms(5);
    }
}

void writeStripe(uint8_t segment, uint8_t stripe) {
	cbi(PORTD, LATCH_DIO);
	shift(STRIPE_MAP[stripe], LSBFIRST);
	shift(SEGMENT_SELECT[segment], MSBFIRST);
	sbi(PORTD, LATCH_DIO);
}

void displayGuitarHeroAndWait(uint8_t stripe0, uint8_t stripe1, uint8_t stripe2, uint8_t counter, uint16_t duration) {
	 for (int i = 0; i < duration / 20; i++) {
		writeStripe(0, stripe0);
		_delay_ms(5);
		writeStripe(1, stripe1);
		_delay_ms(5);
		writeStripe(2, stripe2);
		_delay_ms(5);
		writeNumberToSegment(3, counter);
		_delay_ms(5);
	 }
}

void writeCharToSegment(uint8_t segment, char character) {
    uint8_t code;
    if (character >= 'a' && character <= 'z') {
        character -= 'a' - 'A';
    }
    if (character >= 'A' && character <= 'Z') {
        code = ALPHABET_MAP[character - 'A'];

    } else if (character >= '0' && character <= '9' ){
		writeNumberToSegment(segment, character - '0');
		return;
	} else {
        code = SPACE;
    }
	cbi(PORTD, LATCH_DIO);
    shift(code, MSBFIRST);
    shift(SEGMENT_SELECT[segment], MSBFIRST);
    sbi(PORTD, LATCH_DIO);
	
    cbi(PORTD, LATCH_DIO);
    shift(code, MSBFIRST);
    shift(SEGMENT_SELECT[segment], MSBFIRST);
    sbi(PORTD, LATCH_DIO);
}

void writeString(char* str) {
    writeCharToSegment(0, *str);
    writeCharToSegment(1, *++str);
    writeCharToSegment(2, *++str);
    writeCharToSegment(3, *++str);
}

void writeStringAndWait(char* str, int delay){
    char* ptr;
    for (int i = 0; i < delay / 20; i++) {
        ptr = str;
        writeCharToSegment(0, *ptr);
        _delay_ms(5);
        writeCharToSegment(1, *++ptr);
        _delay_ms(5);
        writeCharToSegment(2, *++ptr);
        _delay_ms(5);
        writeCharToSegment(3, *++ptr);
        _delay_ms(5);
    }
}

void writeStringScroll(char* str, uint8_t repetitions) {
    char* ptr;
	char* temp_ptr;
	char temp_string[5];
    
	// display string once
	ptr = str;
	while (*(ptr + 3) != '\0') {
		writeStringAndWait(ptr, 500);
		++ptr;
	}
	
	for(int i = 0; i < repetitions; i++){
		// make transition
		temp_ptr = ptr;
		for (int j = 0; j < 4; j++){
			ptr = temp_ptr + j;
			for (int k = 0; k < 4; k++){
				if (*ptr == '\0'){
					temp_string[k] = ' ';
					ptr = str;
				} else {
					temp_string[k] = *ptr++;
				}
			}
			writeStringAndWait(temp_string, 500);
		}
		
		// repeat string
		ptr = str;
		while (*(ptr + 3) != '\0') {
			writeStringAndWait(ptr, 500);
			++ptr;
		}
	}
	writeCharToSegment(0, ' '); // clear 
}


/*
void writeStringScroll(char* str, uint8_t repetitions) {
    char* ptr;
	char* temp_ptr;
	char temp_string[5];
    for (int i = 0; i < repetitions; i++) {
        ptr = str;
        while (*(ptr + 3) != '\0') {
			writeStringAndWait(ptr, 500);
			++ptr;
        }
		
		temp_ptr = ptr;
		for (int j = 0; j < 4; j++){
			ptr = temp_ptr + j;
			for (int k = 0; k < 4; k++){
				if (*ptr == '\0'){
					temp_string[k] = ' ';
					ptr = str;
				} else {
					temp_string[k] = *ptr++;
				}
			}
			writeStringAndWait(temp_string, 500);
		}
		
		ptr = str;
        while (*(ptr + 3) != '\0') {
			writeStringAndWait(ptr, 500);
			++ptr;
        }
		
    }
}

*/