#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usart.h>
#include <util/delay.h>

#include "buttons.h"
#include "buzzer.h"
#include "display.h"
#include "leds.h"
#include "potentio.h"
#include "rgb.h"
#include "timer.h "

#define NO_STRIPE 0b000
#define LOW_STRIPE 0b001
#define MID_STRIPE 0b010
#define MID_LOW_STRIPE 0b011
#define HIGH_STRIPE 0b100
#define HIGH_LOW_STRIPE 0b101
#define HIGH_MID_STRIPE 0b110
#define ALL_STRIPE 0b111

#define SEGMENT_0 0
#define SEGMENT_1 1
#define SEGMENT_2 2
#define SEGMENT_3 3
#define NO_SEGMENT -1
#define BUTTON_0 0
#define BUTTON_1 1
#define BUTTON_2 2
#define TRUE 1
#define NUMBER_OF_COLUMNS 3

#define DO 261
#define RE 293
#define MI 329
#define FA 349
#define SO 391
#define LA 440
#define SI 493

#define SONG0_TONE0 DO
#define SONG0_TONE1 RE
#define SONG0_TONE2 LA
#define SONG1_TONE0 MI
#define SONG1_TONE1 FA
#define SONG1_TONE2 SI
#define SONG2_TONE0 SO
#define SONG2_TONE1 RE
#define SONG2_TONE2 SI

#define MAX_POTENTIO 1023
#define LEVELS 10
#define TONE_DURATION 100
#define HIT_STREAK_MAX 9  // TODO CHANGE BACK TO 9, 3 is for testing
#define NUMBER_OF_SUBLEVELS 4
#define SONG_LENGTH 4 * HIT_STREAK_MAX

#define GREEN 'g'
#define RED 'r'
#define BLUE 'b'
#define PURPLE 'p'
#define CYAN 'c'

#define POINTS_INCORRECT 9
#define POINTS_LATE 6
#define POINTS_CORRECT 3

#define NUMBER_OF_HIGHSCORES 3
#define UNINITIALIZED -1
#define MAX_USERNAME_LEN 15

#define TIMER_OVF_TIME 16384  // 256 * 64 (every 64 microseconds the timer increases its counter)

uint8_t game_started;
int8_t current_segment;
uint8_t pushed_correct_button = 0;
uint8_t pushed_incorrect_button = 0;
uint16_t completion_time = 0;  // how long the current player took to finish the game (in seconds)
uint16_t timer_counter = 0;

typedef struct {
    // struct to store player data
    char username[MAX_USERNAME_LEN];
    int16_t score; // it's possible to have a negative score if the player presses too late or presses the wrong button too often
} PLAYER;

ISR(TIMER0_OVF_vect)
{
    if (game_started) {
        ++timer_counter;
        if (timer_counter >= 1000000 / TIMER_OVF_TIME) {  // triggers every second
            ++completion_time;
            timer_counter = 0;
        }
    }
    else {
        timer_counter = 0;
        completion_time = 0;
    }
}

ISR(PCINT1_vect)
{
    if (!game_started) {  // interrupt to start game
        if (buttonPushedAndDebounced(BUTTON_0)) {
            game_started = TRUE;
            writeCharToSegment(0, ' ');  // clears display so that while button 0 pressed nothing is shown
            waitUntilButtonReleased(BUTTON_0);
        }
    }

    else {  // interrupt to check if correct button pressed
        if (buttonPushedAndDebounced(BUTTON_0)) {
            if (current_segment == BUTTON_0) {
                pushed_correct_button = TRUE;
            }
            else {
                pushed_incorrect_button = TRUE;
            }
        }
        else if (buttonPushedAndDebounced(BUTTON_1)) {
            if (current_segment == BUTTON_1) {
                pushed_correct_button = TRUE;
            }
            else {
                pushed_incorrect_button = TRUE;
            }
        }
        else if (buttonPushedAndDebounced(BUTTON_2)) {
            if (current_segment == BUTTON_2) {
                pushed_correct_button = TRUE;
            }
            else {
                pushed_incorrect_button = TRUE;
            }
        }
    }
}

void initGuitarHero()
{
    initUSART();
    initDisplay();
    initPotentio();
    enableButtonInterrupt(BUTTON_0);
    enableButtonInterrupt(BUTTON_1);
    enableButtonInterrupt(BUTTON_2);
    enableAllButtons();
    enableBuzzer();
    enableAllLeds();
    lightDownAllLeds();
    enableLedsRGB();
    initTimer0_FPWM_1024PS();
}

void shiftColumns(uint8_t *column0_ptr, uint8_t *column1_ptr, uint8_t *column2_ptr)
{  // shifs the columns
    *column0_ptr = (*column0_ptr) >> 1;
    *column1_ptr = (*column1_ptr) >> 1;
    *column2_ptr = (*column2_ptr) >> 1;
}

void addStripeToColumn(uint8_t *col0, uint8_t *col1, uint8_t *col2, uint8_t latest_segment)
{  // adds the new stripe to the column corresponding to the given segment
    if (latest_segment == SEGMENT_0) {
        *col0 |= 0b100;
    }
    else if (latest_segment == SEGMENT_1) {
        *col1 |= 0b100;
    }
    else {
        *col2 |= 0b100;
    }
}

void updateStripeData(int8_t stripe_data[], uint8_t latest_segment)
{
    stripe_data[0] = stripe_data[1];
    current_segment = stripe_data[0];
    stripe_data[1] = stripe_data[2];
    stripe_data[2] = latest_segment;
}

void nextTurn(uint8_t *column0, uint8_t *column1, uint8_t *column2, int8_t stripe_data[], uint8_t latest_segment)
{  // shifts the columns and adds the new stripe to the correct column
    shiftColumns(column0, column1, column2);
    pushed_correct_button = 0;
    pushed_incorrect_button = 0;
    addStripeToColumn(column0, column1, column2, latest_segment);
    updateStripeData(stripe_data, latest_segment);
}

void playCorrectPressSound(int8_t latest_segment, uint16_t tone0, uint16_t tone1, uint16_t tone2)
{  // plays a sound and lights up the RGB LED when a button is pressed correctly,
    //      the tone and color of the LED depends on the latest_segment of the stripe
    uint16_t tone;
    char RGBcolor;
    if (latest_segment == SEGMENT_0) {
        tone = tone0;
        RGBcolor = RED;
    }
    else if (latest_segment == SEGMENT_1) {
        tone = tone1;
        RGBcolor = GREEN;
    }
    else {
        tone = tone2;
        RGBcolor = BLUE;
    }
    lightUpLedRGB(RGBcolor);
    playTone(tone, TONE_DURATION);
    lightDownLedRGB(RGBcolor);
}

void checkSubLevelUp(uint8_t *sublevel, uint8_t *hit_streak)
{  // checks if the hit streak passed the maximum, if so it increases the sublevel
    if (*hit_streak > HIT_STREAK_MAX) {
        lightUpLed(*sublevel);
        (*sublevel)++;
        *hit_streak = 0;
    }
}

uint8_t *selectRandomSong(uint8_t *song0, uint8_t *song1, uint8_t *song2, uint16_t *tone0, uint16_t *tone1, uint16_t *tone2)
{  // selects a random song and sets the correct tone frequenties
    uint8_t rnd = rand() % 3;
    if (rnd == 0) {
        *tone0 = SONG0_TONE0;
        *tone1 = SONG0_TONE1;
        *tone2 = SONG0_TONE2;
        return song0;
    }
    else if (rnd == 1) {
        *tone0 = SONG1_TONE0;
        *tone1 = SONG1_TONE1;
        *tone2 = SONG1_TONE2;
        return song1;
    }
    else {
        *tone0 = SONG2_TONE0;
        *tone1 = SONG2_TONE1;
        *tone2 = SONG2_TONE2;
        return song2;
    }
}

uint16_t calculateTickDuration(uint8_t level)
{  // returns the duration in miliseconds of how much time the player has to press the button for each stripe
    return 1200 - level * 100;
}

void reset(uint8_t *col0, uint8_t *col1, uint8_t *col2, uint8_t *hit_streak, uint8_t *sublevel, int8_t *stripe_data, int16_t *score)
{  // resets the game
    *col0 = 0b000;
    *col1 = 0b000;
    *col2 = 0b000;
    *hit_streak = 0;
    *sublevel = 0;
    *score = 0;
    lightDownAllLeds();

    for (int i = 0; i < NUMBER_OF_COLUMNS; ++i) {
        *(stripe_data + i) = NO_SEGMENT;
    }
}

void selectLevel(uint8_t *level)
{  // lets the player select the level by turning the potentiometer
    printf("\nGeef een draai aan de potentiometer en druk op knop 0 om te beginnen");
    uint16_t pv;
    while (!game_started) {
        pv = potentioValue();                                  // waits for player confirmation to start game
        *level = (float)pv / (MAX_POTENTIO + 1) * LEVELS + 1;  // zorgt ervoor dat: eerste level -> level 1 EN laatste level -> level 10
        writeNumberAndWait(*level, 100);
    }
    srand(pv);
}

void printHighScores(PLAYER *highScores[NUMBER_OF_HIGHSCORES])
{  // prints the TOP 3 scoring players
    printf("\nLEADERBORD");
    printf("\n___________________________________________");
    for (uint8_t i = 0; i < NUMBER_OF_HIGHSCORES; ++i) {
        if (highScores[i]->score != UNINITIALIZED) {
            _delay_ms(10);
            printf("\n%s : %d", highScores[i]->username, highScores[i]->score);
        }
    }
}

void updateHighScores(PLAYER *highScores[NUMBER_OF_HIGHSCORES], int16_t score)
{  // updates the TOP 3 scoring players
    int8_t lowestScoreSoFarIndex = UNINITIALIZED;
    for (uint8_t i = 0; i < NUMBER_OF_HIGHSCORES; ++i) {
        if (highScores[i]->score == UNINITIALIZED) {  // als de username NULL is - > dus minder dan 3 players het spel gespeeld hebben -> huidige speler wordt toegevoegd aan de top 3
            printf("\nYou came in the top 3, type in your username:");
            readString(highScores[i]->username, MAX_USERNAME_LEN);
            highScores[i]->score = score;
            printHighScores(highScores);
            return;
        }
        else if (lowestScoreSoFarIndex == UNINITIALIZED || highScores[i]->score < highScores[lowestScoreSoFarIndex]->score) {
            lowestScoreSoFarIndex = i;
        }
    }
    if (score > highScores[lowestScoreSoFarIndex]->score) {  // the lowest scoring of the top 3 gets replaced by the current player if the current player scored higher
        printf("\nYou came in the top 3, type in your username:");
        readString(highScores[lowestScoreSoFarIndex]->username, MAX_USERNAME_LEN);
        highScores[lowestScoreSoFarIndex]->score = score;
        printHighScores(highScores);
    }
}

void playLevelUpSound()
{  // plays a sound to indicate a level up
    playTone(LA, 200);
    _delay_ms(50);
    playTone(SO, 200);
    _delay_ms(50);
    playTone(SI, 100);
    _delay_ms(50);
}

void levelUp(uint8_t level)
{
    if (level != LEVELS) {
        lightUpLedRGB(PURPLE);
        playLevelUpSound();
        writeStringScroll("level up", 1);
        lightDownLedRGB(PURPLE);
    }
    else {
        lightUpLedRGB(CYAN);
        writeStringScroll("finish", 1);
        lightDownLedRGB(CYAN);
    }
}

void playSong(uint16_t tone0, uint16_t tone1, uint16_t tone2, uint8_t *song)
{  // plays the given song
    uint8_t current_segment;
    for (uint8_t i = 0; i < SONG_LENGTH; ++i) {
        _delay_ms(20);
        current_segment = song[i];

        if (current_segment == SEGMENT_0) {
            playTone(tone0, TONE_DURATION);
        }
        else if (current_segment == SEGMENT_1) {
            playTone(tone1, TONE_DURATION);
        }
        else {
            playTone(tone2, TONE_DURATION);
        }
    }
}

void guitarHero()
{
    initGuitarHero();

    uint8_t song0[] = {SEGMENT_0, SEGMENT_1, SEGMENT_2, SEGMENT_1, SEGMENT_0, SEGMENT_0, SEGMENT_2, SEGMENT_0, SEGMENT_2,
                       SEGMENT_1, SEGMENT_0, SEGMENT_2, SEGMENT_0, SEGMENT_0, SEGMENT_2, SEGMENT_1, SEGMENT_1, SEGMENT_0,
                       SEGMENT_0, SEGMENT_1, SEGMENT_2, SEGMENT_2, SEGMENT_1, SEGMENT_0, SEGMENT_0, SEGMENT_2, SEGMENT_2,
                       SEGMENT_1, SEGMENT_0, SEGMENT_0, SEGMENT_1, SEGMENT_1, SEGMENT_2, SEGMENT_2, SEGMENT_0, SEGMENT_1};

    uint8_t song1[] = {SEGMENT_1, SEGMENT_2, SEGMENT_0, SEGMENT_1, SEGMENT_2, SEGMENT_0, SEGMENT_1, SEGMENT_2, SEGMENT_1,
                       SEGMENT_2, SEGMENT_0, SEGMENT_2, SEGMENT_2, SEGMENT_1, SEGMENT_1, SEGMENT_0, SEGMENT_1, SEGMENT_0,
                       SEGMENT_2, SEGMENT_2, SEGMENT_1, SEGMENT_0, SEGMENT_0, SEGMENT_2, SEGMENT_1, SEGMENT_2, SEGMENT_1,
                       SEGMENT_1, SEGMENT_1, SEGMENT_2, SEGMENT_0, SEGMENT_1, SEGMENT_2, SEGMENT_1, SEGMENT_2, SEGMENT_0};

    uint8_t song2[] = {SEGMENT_2, SEGMENT_2, SEGMENT_2, SEGMENT_1, SEGMENT_1, SEGMENT_1, SEGMENT_0, SEGMENT_0, SEGMENT_0,
                       SEGMENT_2, SEGMENT_0, SEGMENT_2, SEGMENT_0, SEGMENT_1, SEGMENT_0, SEGMENT_1, SEGMENT_0, SEGMENT_2,
                       SEGMENT_0, SEGMENT_2, SEGMENT_0, SEGMENT_1, SEGMENT_2, SEGMENT_2, SEGMENT_1, SEGMENT_0, SEGMENT_0,
                       SEGMENT_0, SEGMENT_1, SEGMENT_1, SEGMENT_0, SEGMENT_2, SEGMENT_0, SEGMENT_0, SEGMENT_2, SEGMENT_0};

    uint8_t column0, column1, column2, hit_streak, level, sublevel;
    uint8_t *current_song;
    uint16_t tickDuration, tone0, tone1, tone2;
    int8_t stripe_data[NUMBER_OF_COLUMNS];
    int16_t score;

    PLAYER *highScores[NUMBER_OF_HIGHSCORES];
    for (uint8_t i = 0; i < NUMBER_OF_HIGHSCORES; ++i) {
        highScores[i] = malloc(sizeof(PLAYER));
        highScores[i]->score = UNINITIALIZED;
    }

    while (TRUE) {
        game_started = 0;
        score = 0;
        selectLevel(&level);

        while (level <= 10) {
            reset(&column0, &column1, &column2, &hit_streak, &sublevel, stripe_data, &score);
            current_song = selectRandomSong(song0, song1, song2, &tone0, &tone1, &tone2);
            playSong(tone0, tone1, tone2, current_song);
            tickDuration = calculateTickDuration(level);

            uint8_t i = 0;
            while (sublevel < NUMBER_OF_SUBLEVELS) {
                uint8_t latest_segment = *(current_song + i);
                nextTurn(&column0, &column1, &column2, stripe_data, latest_segment);
                displayGuitarHeroAndWait(column0, column1, column2, hit_streak, tickDuration);

                if (pushed_incorrect_button) {  // pushed wrong button
                    hit_streak = 0;
                    score -= POINTS_INCORRECT;
                }
                else if (pushed_correct_button) {  // pushed correct button on time
                    ++hit_streak;
                    playCorrectPressSound(current_segment, tone0, tone1, tone2);
                    checkSubLevelUp(&sublevel, &hit_streak);
                    score += POINTS_CORRECT;
                }
                else {  //  ran out of time
                    hit_streak = 0;
                    score -= POINTS_LATE;
                }

                ++i;
                if (i == SONG_LENGTH) {
                    i = 0;  // loops current_song
                }
            }

            levelUp(level);
            ++level;
        }
        printf("\nYou played for %d seconds.", completion_time);
        updateHighScores(highScores, score);
        _delay_ms(1000);
    }

    for (uint8_t i = 0; i < NUMBER_OF_HIGHSCORES; ++i) {
        free(highScores[i]);
    }
}