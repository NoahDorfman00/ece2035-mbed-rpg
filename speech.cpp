#include "speech.h"
#include "graphics.h"
#include "globals.h"
#include "hardware.h"

/**
 * Draw the speech bubble background.
 */
static void draw_speech_bubble();

/**
 * Erase the speech bubble.
 */
static void erase_speech_bubble();

/**
 * Draw a single line of the speech bubble.
 * @param line The text to display
 * @param which If TOP, the first line; if BOTTOM, the second line.
 */
#define TOP    0
#define BOTTOM 1
static void draw_speech_line(char* line, int which);

/**
 * Delay until it is time to scroll.
 */
static void speech_bubble_wait();

void draw_speech_bubble()
{
    uLCD.filled_rectangle(1, 94 , 126, 111, BLACK);
    uLCD.rectangle(0, 93 , 127, 112, GREEN); 
}

void erase_speech_bubble()
{
    draw_speech_bubble();
}

void draw_speech_line(char* line, int which)
{
    switch(which){
        case TOP:
            uLCD.text_string(line, 0, 12, FONT_5X7, WHITE);
            break;
        case BOTTOM:
            uLCD.text_string(line, 0, 13, FONT_5X7, WHITE);
            break;
    }
}

void speech_bubble_wait()
{
    wait(0.375);
    while(button1){
        uLCD.text_string("Press Action", 6, 15, FONT_5X7, GREEN);
        if (!button1) break;
        wait(0.25);
        uLCD.filled_rectangle(0, 119, 127, 127, BLACK);
        if (!button1) break;
        wait(0.25);    
    }
    
}

void speech(char* line1, char* line2)
{
    draw_speech_bubble();
    draw_speech_line(line1, TOP);
    draw_speech_line(line2, BOTTOM);
    speech_bubble_wait();
    erase_speech_bubble();
}

void long_speech(char* lines[], int n)
{
    if(n%2){
        for(int i=0; i<n-1; i+=2){
            speech(lines[i], lines[i+1]);
        }
        speech(lines[n-1], "");
    }
    
    else{
        for(int i=0; i<n; i+=2){
            speech(lines[i], lines[i+1]);
        }
        erase_speech_bubble();     
    }
}
