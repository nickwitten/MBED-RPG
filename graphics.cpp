#include "graphics.h"

#include "globals.h"
#include "hardware.h"



void draw_player(int u, int v, int sonar)
{
    if (sonar) {
        const char img[122] = "BB11B1BBBBB"
                              "B1111B111BB"
                              "11111B11B1B"
                              "12211B11111"
                              "12B2BB12BBY"
                              "12BBB111WYB"
                              "12BB1112WWB"
                              "11B11112BBB"
                              "B111122BBBB"
                              "BB1122BBBBB"
                              "BBB1111BBBB"
                              ;
        draw_img(u, v, img);          
    } else {
        // Player without key
        const char img[122] = "BB11B1BBBBB"
                              "B1111B111BB"
                              "11111B11B1B"
                              "12211B11111"
                              "12B2BB12BBB"
                              "12BBB1111BB"
                              "12BB1112BBB"
                              "11B11112BBB"
                              "B111122BBBB"
                              "BB1122BBBBB"
                              "BBB1111BBBB"
                              ;      
        draw_img(u, v, img);
    }              
}

#define YELLOW 0xFFFF00
#define BROWN  0xD2691E
#define DIRT   BROWN
#define GREY   0xA9A9A9
#define DFUR   0xA27832
#define LFUR   0xE5BE74
#define CREAM  0xDCC59A
#define DGREEN 0x086801
#define LBROWN 0x632B1F
#define DBROWN 0x451E16
#define PINK   0xF078CD
void draw_img(int u, int v, const char* img)
{
    int colors[11*11];
    for (int i = 0; i < 11*11; i++)
    {
        if (img[i] == 'R') colors[i] = RED;
        else if (img[i] == 'Y') colors[i] = YELLOW;
        else if (img[i] == 'G') colors[i] = GREEN;
        else if (img[i] == 'D') colors[i] = DIRT;
        else if (img[i] == '5') colors[i] = LGREY;
        else if (img[i] == '3') colors[i] = GREY;
        else if (img[i] == '1') colors[i] = DFUR;
        else if (img[i] == '2') colors[i] = LFUR;
        else if (img[i] == 'C') colors[i] = CREAM;
        else if (img[i] == 'W') colors[i] = WHITE;
        else if (img[i] == '4') colors[i] = DGREEN;
        else if (img[i] == '6') colors[i] = DBROWN;
        else if (img[i] == '7') colors[i] = LBROWN;
        else if (img[i] == 'P') colors[i] = PINK;
        else colors[i] = BLACK;
    }
    uLCD.BLIT(u, v, 11, 11, colors);
    wait_us(250); // Recovery time!
}

void draw_nothing(int u, int v)
{
    // Fill a tile with blackness
    uLCD.filled_rectangle(u, v, u+10, v+10, BLACK);
}

void draw_wall(int u, int v)
{
    const char img[122] = "B5B55B55B5B"
                          "B355355355B"
                          "B535535535B"
                          "B553553553B"
                          "3553553553B"
                          "55355355355"
                          "55535535535"
                          "B355355355B"
                          "B535535535B"
                          "B553553553B"
                          "B555355355B";
                                         
    draw_img(u, v, img);
}

void draw_plant(int u, int v)
{
    const char img[122] = "BBG4BBB4GBB"
                          "BBBG4B4GBBB"
                          "B4BBGBG4B4G"
                          "GG4GGG444GB"
                          "BBG4G4G4GBB"
                          "4B44444GG44"
                          "G4G44444G4G"
                          "B4GG444GGGB"
                          "BB4G44GG4BB"
                          "BB4444G44BB"
                          "BBBB444BBBB";               
    draw_img(u, v, img);
}

void draw_upper_status(int x, int y, int player_lives)
{    
    // Draw bottom border of status bar
    uLCD.line(0, 9, 127, 9, GREEN);
    
    // Player position
    char string[20];
    sprintf(string, "X:%d Y:%d ",x,y); 
    uLCD.text_string(string, 0, 0, FONT_7X8, WHITE);
    
    // Player lives
    // For drawing an 8x8 sprite
    int colors[8*8];
    const char img[64] = {"BRRBBRRB"
                          "RRRRRRRR"
                          "RRRRRRRR"
                          "RRRRRRRR"
                          "BRRRRRRB"
                          "BBRRRRBB"
                          "BBBRRBBB"};
    for (int i = 0; i < 8*8; i++)
    {
        if (img[i] == 'R') colors[i] = RED;
        else if (img[i] == 'Y') colors[i] = YELLOW;
        else if (img[i] == 'G') colors[i] = GREEN;
        else if (img[i] == 'D') colors[i] = DIRT;
        else if (img[i] == '5') colors[i] = LGREY;
        else if (img[i] == '3') colors[i] = GREY;
        else if (img[i] == '1') colors[i] = DFUR;
        else if (img[i] == '2') colors[i] = LFUR;
        else if (img[i] == 'C') colors[i] = CREAM;
        else colors[i] = BLACK;
    }
    wait_us(250); // Recovery time!
    int u, v;
    // for each heart
    for (int i=0; i<player_lives; ++i) {
        // calculate where to draw the heart
        u = 10 * (2-i) + 100;
        v = 0; 
        // draw the heart     
        uLCD.BLIT(u, v, 8, 8, colors);  
        wait_us(250); // Recovery time!
    }
    // for each heart lost
    int inactive_hearts = 3 - player_lives;
    for (int i=0; i<inactive_hearts; ++i) {
        // calculate where to erase heart
        u = 100 + 10 * i;
        v = 0;
        // erase heart
        uLCD.filled_rectangle(u,v+8,u+8,v, BLACK);
    }
}

void draw_lower_status()
{
    // Draw top border of status bar
    uLCD.line(0, 118, 127, 118, GREEN);
    
    // Add other status info drawing code here
}

void draw_border()
{
    uLCD.filled_rectangle(0,     9, 127,  14, WHITE); // Top
    uLCD.filled_rectangle(0,    13,   2, 114, WHITE); // Left
    uLCD.filled_rectangle(0,   114, 127, 117, WHITE); // Bottom
    uLCD.filled_rectangle(124,  14, 127, 117, WHITE); // Right
}

// Screen displayed before game
void draw_startscreen() {
    uLCD.filled_rectangle(0, 0, 127, 127, BLACK);
    char *line1 = "The Lost Acorn";
    char *line2 = "press any button";
    char *line3 = "to start...";
    uLCD.text_string(line1,  2, 7, FONT_12X16, GREEN);
    uLCD.text_string(line2,  1, 12, FONT_7X8, GREEN);
    uLCD.text_string(line3,  1, 13, FONT_7X8, GREEN);
    // read the inputs and break out of loop when a button is pressed
    GameInputs inputs = read_inputs();
    do {
        inputs = read_inputs();
        wait_ms(100);
    } while (inputs.b1 && inputs.b2 && inputs.b3);
}

void draw_endscreen(int won) {
    uLCD.filled_rectangle(0, 0, 127, 127, BLACK);
    char *line1, *line2;
    if (won) {
        line1 = "You Found the";
        line2 = "Golden Acorn!";
        uLCD.text_string(line1,  2, 7, FONT_7X8, GREEN);
        uLCD.text_string(line2,  2, 8, FONT_7X8, GREEN);
    } else {
        line1 = "You Lost";
        uLCD.text_string(line1,  5, 8, FONT_7X8, RED);
    }
}

void draw_ladder(int u, int v)
{       
    const char img[122] = "BBYBBBBBYBB"
                          "BBYYYYYYYBB"
                          "BBYBBBBBYBB"
                          "BBYBBBBBYBB"
                          "BBYYYYYYYBB"
                          "BBYBBBBBYBB"
                          "BBYBBBBBYBB"
                          "BBYYYYYYYBB"
                          "B3Y33333Y3B"
                          "B3Y33333Y3B"
                          "BB3333333BB";                    
    draw_img(u, v, img);
}

void draw_wise_bear(int u, int v)
{
    const char img[122] = "BB1B222B1BB"
                          "BBB22222BBB"
                          "BBB2B2B2BBB"
                          "BBB222222BB"
                          "BB22BCC22BB"
                          "BB22CCC122B"
                          "BB21111122B"
                          "BB2211122BB"
                          "BB2211122BB"
                          "B112212211B"
                          "BB122221B2B";
                          
    draw_img(u, v, img);
}

void draw_acorn(int u, int v)
{
    const char img[122] = "BBBBBBBBBBB"
                          "BBBBB1BBBBB"
                          "BBBB111BBBB"
                          "BBB11111BBB"
                          "BB1111111BB"
                          "BB7666666BB"
                          "BB7666666BB"
                          "BB7666666BB"
                          "BBB76666BBB"
                          "BBBB766BBBB"
                          "BBBBB7BBBBB";
                          
    draw_img(u, v, img);
}

void draw_snake(int u, int v)
{
    const char img[122] = "BBBBBBBBBBB"
                          "BBBB44YBBBB"
                          "BBB44G44BBB"
                          "BBB4GBBBPBB"
                          "BBB4GBBBBBB"
                          "BBBB4GBBBBB"
                          "B44G4GG44BB"
                          "B4GG4G4G44B"
                          "B4G4G44G4B4"
                          "B44GGGG4BB4"
                          "BB44444BB4B";
    draw_img(u, v, img);
}

void draw_breakable_wall(int u, int v)
{
    const char img[122] = "33333333333"
                          "33333333333"
                          "33333333333"
                          "33333333333"
                          "33333333333"
                          "33333333333"
                          "33333333333"
                          "33333333333"
                          "33333333333"
                          "33333333333"
                          "33333333333";
    draw_img(u, v, img); 
}

void draw_sonar(int u, int v)
{
    const char img[122] = "BBBBBBBBBBB"
                          "BBBBBBYBBBB"
                          "BBBBWYBBBBB"
                          "BBBBWWBBBBB"
                          "B222WW2222B"
                          "B222222222B"
                          "BB3BBBB3BBB"
                          "BB3BBBB3BBB"
                          "BB3BBBB3BBB"
                          "BB3BBBB3BBB";
    draw_img(u, v, img); 
}

void draw_treasure(int u, int v)
{
    const char img[122] = "BBBBBBBBBBB"
                          "BBBBBBBBBBB"
                          "BBBBBBBBBBB"
                          "BBBBBBBBBBB"
                          "BBBY22YBBBB"
                          "BB2Y22Y2BBB"
                          "B22Y22Y22BB"
                          "BBBBYYBBBBB"
                          "B22Y22Y22BB"
                          "B22Y22Y22BB"
                          "B22Y22Y22BB";                          
    draw_img(u, v, img); 
}

void draw_master_squirrel(int u, int v) {
    const char img[122] = "BWWWW1BBBBB"
                          "B1111WWWWBB"
                          "1WW11W11B1B"
                          "122WWB11111"
                          "12B2BB12BBB"
                          "12BBB1111BB"
                          "12BB1112BBB"
                          "11B11112BBB"
                          "B111122BBBB"
                          "BB1122BBBBB"
                          "BBB1111BBBB"
                          ;      
    draw_img(u, v, img);
}

void draw_tree_bottom(int u, int v) {
    const char img[122] = "BBB21222BBB"
                          "BBB22222BBB"
                          "BBB22222BBB"
                          "BBB22212BBB"
                          "BBB22212BBB"
                          "BBB22212BBB"
                          "BBB21222BBB"
                          "BBB21222B2B"
                          "12222222221"
                          "B222222221B"
                          "221B221B221";
                               
    draw_img(u, v, img);
}

void draw_tree_middle(int u, int v) {
    const char img[122] = "BBB22222BBB"
                          "BBB21222BBB"
                          "BBB22222BBB"
                          "BBB21222BBB"
                          "BBB22212BBB"
                          "BBB22212BBB"
                          "BBB22222BBB"
                          "BBB22222BBB"
                          "BBB22122BBB"
                          "BBB22222BBB"
                          "BBB21222BBB";   
    draw_img(u, v, img);
}

void draw_tree_top(int u, int v) {
    const char img[122] = "BBBBBG4BBBB"
                          "BBGG444GGBB"
                          "B444G4G4G4B"
                          "B4G4GGG44GB"
                          "B44G4G4G4G4"
                          "GGG4G44GG44"
                          "G4G4G44GG4g"    
                          "G2GG4G44444"
                          "442G422242G"
                          "BG42442424G"
                          "BBG224222GB";
    draw_img(u, v, img);
}

void draw_npc_squirrel(int u, int v)
{
    const char img[122] = "BB11B1BBBBB"
                          "B1111B111BB"
                          "11111B11B1B"
                          "12211B11111"
                          "12B2BB12BBB"
                          "12BBB1111BB"
                          "12BB1112BBB"
                          "11B11112BBB"
                          "B111122BBBB"
                          "BB1122BBBBB"
                          "BBB1111BBBB";
    draw_img(u, v, img); 
}