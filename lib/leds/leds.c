
#include <util/delay.h>
#include <avr/io.h>

#define NUMBER_OF_LEDS 4  //#define is een "preprocessor directive". Het zorgt ervoor dat in onderstaande file elke NUMBER_OF_LEDS vervangen wordt door 4.  

void delay_ms(int delay){
    for (int i = 0; i < delay; ++i){
        _delay_ms(1);
    }
}

void delay_us(int delay){
    for (int i = 0; i < delay; ++i){
        _delay_us(1);
    }
}

void enableLed(int lednumber) {
	// lednumber kan 0 t.e.m. 3 zijn
    if (lednumber<0||lednumber>NUMBER_OF_LEDS-1) return;
    DDRB |= (1 << (PB2 + lednumber)); 
}

void enableLeds(uint8_t leds){
    if (leds > 15) return;
    DDRB |= leds << PB2;
}

void enableAllLeds (){
    enableLeds(0b00001111);
}

void disableLed(int lednumber){
	DDRB &= ~(1 << (PB2 + lednumber));
}

void lightUpLed(int lednumber){//opgelet: ge-enabelde leds staan ook ineens aan (want 0 = aan)
    if (lednumber<0||lednumber>NUMBER_OF_LEDS-1) return;
    PORTB &= ~(1 << (PB2 + lednumber));//Check de tutorial "Bit operaties" om te weten wat hier juist gebeurd.
}

void lightUpLeds (uint8_t leds){
    if (leds > 15) return;
    PORTB &= ~(leds << PB2);
}

void lightUpAllLeds (){
    lightUpLeds(0b00001111);
}

void lightDownLed(int lednumber){
    if (lednumber<0||lednumber>3) return;
    PORTB |= (1 << (PB2 + lednumber));//Zorg ook dat je deze code goed begrijpt!
}

void lightDownLeds (uint8_t leds){
    if (leds > 15) return;
    PORTB |= leds << PB2;
};

void lightDownAllLeds (){
    lightDownLeds(0b00001111);
};

void dimLed(int lednumber, int percentage, int duration){
    if (percentage > 100) return;

    long int count = 0;
    while(count < (long) duration * 10){
        lightUpLed(lednumber);
        delay_us(percentage);
        lightDownLed(lednumber);
        delay_us(100 - percentage);
        ++count;
    }
}

void fadeInLed(int lednumber, int duration){
    // minimum duration van dimLed is 1 ms, dus fade van 0 tot 100% duurt minimaal 100 ms
    if (duration < 100) duration = 100; 
    for (int i = 0; i <= 100; ++i){
        dimLed(lednumber, i, duration / 100); // als dimLed exact 1 ms zou duren dan zou dit werken, maar omdat het niet exact is duurt dit langer dan verwacht, delen door 150 geeft meer accurate tijdsduur
    }
}

void fadeOutLed(int lednumber, int duration){
    // minimum duration van dimLed is 1 ms, dus fade van 0 tot 100% duurt minimaal 100 ms
    if (duration < 100) duration = 100; 
    for (int i = 100; i > 0; --i){
        dimLed(lednumber, i, duration / 100);
    }
}