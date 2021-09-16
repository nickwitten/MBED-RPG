#include "speech.h"

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
static void draw_speech_line(const char* line, int which);

/**
 * Delay until it is time to scroll.
 */
static void speech_bubble_wait();

void draw_speech_bubble()
{
    uLCD.filled_rectangle(0,127,127,98,BLACK); // background
    uLCD.filled_rectangle(0,127,2,98, GREEN);  // left border
    uLCD.filled_rectangle(125,127,127,98, GREEN);  // right border
    uLCD.filled_rectangle(0,98,127,96, GREEN);  // top border
    uLCD.filled_rectangle(0,127,127,125, GREEN);    // bottom border
}

void erase_speech_bubble()
{
    pc.printf("Erasing bubble.\r\n");
    uLCD.filled_rectangle(0, 127, 127, 96, BLACK);
}

void draw_speech_line(const char* line, int which)
{
    pc.printf("print to screen3\r\n");
    if (which == TOP) {
        uLCD.text_string((char*)line, 1, 13, FONT_7X8, WHITE);
    } else {
        uLCD.text_string((char*)line, 1, 14, FONT_7X8, WHITE);
    }
    
}

void speech_bubble_wait()
{
    wait_ms(100);
    GameInputs in;    
    do {
        wait_ms(100);
        in = read_inputs();
    } while(in.b1);
    wait_ms(100);  
}

void speech(const char* line1, const char* line2)
{
    draw_speech_bubble();
    draw_speech_line(line1, TOP);
    draw_speech_line(line2, BOTTOM);
    speech_bubble_wait();
    erase_speech_bubble();
}

void long_speech(const char* lines[], int n)
{
    draw_speech_bubble();
    // loop through the lines by twos.
    int i;
    for (i=0; (2*i) < n; ++i) {
        // clear speech bubble
        uLCD.filled_rectangle(2,125,125,98, BLACK);
        // draw the first line
        draw_speech_line(lines[2*i], TOP);
        // check if a second line should be drawn
        if ((2*i + 1) < n) {
            draw_speech_line(lines[2*i+1],BOTTOM);
        }
        speech_bubble_wait();    
    }
    erase_speech_bubble();
}
