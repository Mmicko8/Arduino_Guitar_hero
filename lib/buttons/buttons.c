#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define BUTTON_0_PORT PC1
#define ALLBUTTONS 0b00000111

void enableOneButton(int button){
	// button kan 0 t.e.m. 2 zijn
	DDRC &= ~_BV(BUTTON_0_PORT + button); // _BV(BUTTON_0_PORT + button) == (1<<BUTTON_0_PORT + button)
	PORTC |= _BV(BUTTON_0_PORT + button); // Pull up aanzetten
}

void enableMultipleButtons(uint8_t buttons){
	DDRC &= ~(buttons << BUTTON_0_PORT);
	PORTC |= buttons << BUTTON_0_PORT; // Pull up aanzetten
}

void enableAllButtons (){
	enableMultipleButtons(ALLBUTTONS);
}

void disableOneButton(int button){
	DDRC |= _BV(BUTTON_0_PORT + button);
}

void disableAllButtons (){
	DDRC |= ALLBUTTONS << BUTTON_0_PORT;
	//PORTC &= ~(ALLBUTTONS << BUTTON_0_PORT); // Pull up uitzetten
}

int buttonPushed(int button){
	if (bit_is_clear(PINC, BUTTON_0_PORT + button)) { // hetzelfde als (PINC & (1 << BUTTON_0_PORT + button)) == 0)
		return 1;
    } else {
		return 0;
    } 
}

int multipleButtonsPushed (uint8_t buttons){
	if ((PINC & (buttons << BUTTON_0_PORT)) == 0) {
		return 1;
	} else {
		return 0;
    }
}

int allButtonsPushed (){
	if (multipleButtonsPushed(ALLBUTTONS)){
		return 1;
	} else {
		return 0;
	}
}

int buttonReleased(int button){
	return !buttonPushed(button);
}

int multipleButtonsReleased (uint8_t buttons){
	if ((PINC | ~(buttons << BUTTON_0_PORT)) == -1) {  // true als = 11111111, maar het wordt -1 want 2-complement
		return 1;
	} else {
		return 0;
    }
}

int allButtonsReleased (){
	return multipleButtonsReleased(ALLBUTTONS);
}

void enableButtonInterrupt(int button){
	PCICR |= _BV(PCIE1);  // in Pin Change Interrupt Control Register: geef aan
                         // welke interrupt(s) je wil activeren (PCIE0: poort B,
                         // PCIE1: poort C, PCIE2: poort D)
    PCMSK1 |= _BV(BUTTON_0_PORT + button);
    sei();
}

int buttonPushedAndDebounced(int button){
	if (buttonPushed(button)) {
		 _delay_us(800);
		 if (buttonPushed(button)) {
			 return 1;
		 }
	}
	return 0;
}

int buttonReleasedAndDebounced(int button){
	if (buttonReleased(button)) {
		_delay_us(800);
		if (buttonReleased(button)) {
			return 1;
		}
	}
	return 0;
}

void waitUntilButtonReleased(int button){
	while(1){
		if(buttonReleasedAndDebounced(button)){
			return;
		}
	}
}

