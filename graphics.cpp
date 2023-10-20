#include "graphics.h"

#include "globals.h"




#define YELLOW 0xFFFF00
#define BROWN  0xD2691E
#define DIRT   BROWN
void draw_img(int u, int v, int w, int h, const char* img)
{
    int colors[w*h];
    for (int i = 0; i < w*h; i++)
    {
        if (img[i] == 'R') colors[i] = RED;
        else if (img[i] == 'Y') colors[i] = YELLOW;
        else if (img[i] == 'G') colors[i] = GREEN;
        else if (img[i] == 'D') colors[i] = DIRT;
        else if (img[i] == '5') colors[i] = LGREY;
        else if (img[i] == '3') colors[i] = DGREY;
        else if (img[i] == '1') colors[i] = 0xCECC78; //Star Dark Yellow
        else if (img[i] == '2') colors[i] = 0xFFF700; //Star Light Yellow
        else if (img[i] == 'I') colors[i] = 0x012BC3; //Dragon Light Blue
        else if (img[i] == 'A') colors[i] = 0x041C74; //Dragon Dark Blue
        else if (img[i] == 'W') colors[i] = 0xFFFFFF; //white
        else if (img[i] == 'S') colors[i] = 0x001B34; //SpaceX Dark Blue
        else if (img[i] == 'X') colors[i] = 0x004F8F; //SpaceX Light Blue
        else if (img[i] == 'M') colors[i] = 0x582c09; //Mars Dark
        else if (img[i] == 'm') colors[i] = 0xb45700; //Mars light
        else if (img[i] == 'e') colors[i] = 0x0035b4; //earth blue
        else if (img[i] == 'E') colors[i] = 0x0f7e02; //earth green
        else if (img[i] == 'O') colors[i] = 0xff7c00; //Orange
        else colors[i] = BLACK;
    }
    uLCD.BLIT(u, v, w, h, colors);
    wait_us(250); // Recovery time!
}

void draw_player(int u, int v, int state)
{
    const char* img;
    //Giga
    if (state == 1){
        img = "333333333333WWWWWWWWW33WWWX5XWWW33WWW555WWW33WWWX5XWWW33WWXXXXXWW33WSWXXXWSW33WWWXXXWWW33WWWXWXWWW33WWWSWSWWW333333333333";
    }
    //Dragon with Battery
    else if (state == 2){
        img = "BAABBBBBBBBAIIABBWWW3BBAIIA3WWWWBBBAAWW3WWWBBBBWWWW3WWBB3WWWWWW3BBBWBWWWWWABBB33BBWWAIAB3RR3BWBAIIA3RR3W3BBAIAB33BBBBBBAB";
    }
    //Roadster
    else if (state == 3){
        img = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBB3WBBBBBB5XBWWBBBB3BXXBBW3BB3BBBXBWR3RBRRSXRWRRR3RRRRRRRRRRRBB33BBBB33BBBBBBBBBBBBBBBBBBBBBBB";
    }
    //Standard Dragon
    else img = "BAABBBBBBBBAIIABBWWW3BBAIIA3WWWWBBBAAWW3WWWBBBBWWWW3WWBBBWWWWWW3BBBBBWWWWWABBBBBBWWWAIABBBBBBWBAIIABBBBBBBBAIABBBBBBBBBAB";
        
    draw_img(u, v, 11, 11, img);
}

void draw_emptydragon(int u, int v)
{
    const char* img = "BAABBBBBBBBAIIABBWWW3BBAIIA3WWWWBBBAAWW3WWWBBBBWWWW3WWBBBWWWWWW3BBBBBWWWWWABBBBBBWWWAIABBBBBBWBAIIABBBBBBBBAIABBBBBBBBBAB";
        
    draw_img(u, v, 11, 11, img);
}

void draw_nothing(int u, int v)
{
    // Fill a tile with blackness
    uLCD.filled_rectangle(u, v, u+10, v+10, BLACK);
}

void draw_gigafloor(int u, int v)
{
    // Fill a tile with blackness
    const char* img = "333333333333WWWWWWWWW33WWWWWWWWW33WWWWWWWWW33WWWWWWWWW33WWWWWWWWW33WWWWWWWWW33WWWWWWWWW33WWWWWWWWW33WWWWWWWWW333333333333";
    draw_img(u, v, 11, 11, img);
}

void draw_wall(int u, int v)
{
    const char* img = "BBBBBBBBBB5BB3BBBBBB3BB353BB3BBBBBB333BB33BBBB53BBBBB35BBBBBBBBBB3BBBBB33BB333BBBB353B35533BB353BB3B35BBBBBBBBB3BBBBB5BBB";
        
    draw_img(u, v, 11, 11, img);
}

void draw_explosion(int u, int v)
{
    const char* img = "BBBBBBBBYBRYRBRBRBBBBBBBBOBOBRBBBBBROYOROORBBBBROYOYOBBBRBBOYYBBRBBBROYOYRBBBBROORBYORBBBBBRBOOOBBBBRBBBRBRBBBBBBYBBBBBRB";
        
    draw_img(u, v, 11, 11, img);
}

void draw_gigawall(int u, int v)
{
    const char* img = "BBBBBBBBBBBBBWWWWWWWBBBWBWWWWWBWBBWWBWWWBWWBBWWWBWBWWWBBWWWWBWWWWBBWWWBWBWWWBBWWBWWWBWWBBWBWWWWWBWBBBWWWWWWWBBBBBBBBBBBBB";
        
    draw_img(u, v, 11, 11, img);
}

void draw_star(int u, int v)
{
    const char* img = "1BBBBBBBBBBBBB1BBBBBBBBBBBB2BBB1BBBBBB2BBBBBBB2B121B2BBBBB22222BBBBBBB121BBBBBBBB2B2BB1BB1B2BBB2BBBBBBBBBBBBBBBBBBBB1BBBB";
        
    draw_img(u, v, 11, 11, img);
}

void draw_NPC(int u, int v)
{
    const char* img = "BBBBBBBBBBBBBBBBBBBBBBBBBBBB3WBBBBBBBBBWWBBBBBB3BBBW3BBBB3BBBWR3RRBRRRRWRRRR3RRRRRRRRRRRBB33BBBB33BBBBBBBBBBBBBBBBBBBBBBB";
        
    draw_img(u, v, 11, 11, img);
}

void draw_batt(int u, int v)
{
    const char* img = "333333333333WWWWWWWWW33WW33333WW33W3RRRRR3W33W3RRRRR3W33W3RRRRR3W33W3RRRRR3W33W3RRRRR3W33WW33333WW33WWWWWWWWW333333333333";
        
    draw_img(u, v, 11, 11, img);
}

void draw_mars(int u, int v)
{
    const char* img = "mmmmmmmmmmMmMmmmMmmmmMmMMmmMMmmmmmmmmmMMMmmmmmmmmmmmmmMmmmMMmMmmmmmmmMMmMmMmmMmmMmmmmMMmMMmMMmmmmmmMMmmmmmmMmmMmmmmMmmmmm";
        
    draw_img(u, v, 11, 11, img);
}

void draw_earth(int u, int v)
{
    const char* img = "eeeeeeeeeeEeEeeeEeeeeEeEEeeEEeeeeeeeeeEEEeeeeeeeeeeeeeEeeeEEeEeeeeeeeEEeEeEeeEeeEeeeeEEeEEeEEeeeeeeEEeeeeeeEeeEeeeeEeeeee";
        
    draw_img(u, v, 11, 11, img);
}

void draw_button_off(int u, int v)
{
    const char* img = "333333333333WWWWWWWWW33WWWWBWWWW33WWWBRBWWW33WWBBRBBWW33WBRRBRRBW33WWBBRBBWW33WWWBRBWWW33WWWWBWWWW33WWWWWWWWW333333333333";
        
    draw_img(u, v, 11, 11, img);
}

void draw_button_on(int u, int v)
{
    const char* img = "333333333333WWWWWWWWW33WWWWBWWWW33WWWBGBWWW33WWBBGBBWW33WBGGBGGBW33WWBBGBBWW33WWWBGBWWW33WWWWBWWWW33WWWWWWWWW333333333333";
        
    draw_img(u, v, 11, 11, img);
}

void draw_door(int u, int v)
{
    const char* img = "BBBBBBBBBBBBRRRRRRRRRBBBRRRRRRRBBBBBBBRBBBBBBBBBBRBBBBBBBBBBRBBBBBBBBBBRBBBBBBBBBBRBBBBBBBBBBRBBBBBBBBBBRBBBBBBBBBBBBBBBB";
        
    draw_img(u, v, 11, 11, img);
}


void draw_upper_status(int x, int y)
{
    // Draw bottom border of status bar
    uLCD.line(0, 9, 127, 9, GREEN);
    uLCD.filled_rectangle(0, 0, 127, 8, BLACK);
    char xcoords[5];
    char ycoords[5];
    sprintf(xcoords,"X:%d", x);
    sprintf(ycoords,"Y:%d", y);
    uLCD.text_string(xcoords, 1, 0, FONT_5X7, GREEN);
    uLCD.text_string(ycoords, 7, 0, FONT_5X7, GREEN);
    
    // Add other status info drawing code here
}

void draw_lower_status(char* status)
{
    // Draw top border of status bar
    uLCD.line(0, 118, 127, 118, GREEN);
    uLCD.filled_rectangle(0, 119, 127, 127, BLACK);
    uLCD.text_string(status, 7, 15, FONT_5X7, GREEN);
    
    // Add other status info drawing code here
}

void draw_border()
{
    uLCD.filled_rectangle(0,     9, 127,  14, WHITE); // Top
    uLCD.filled_rectangle(0,    13,   2, 114, WHITE); // Left
    uLCD.filled_rectangle(0,   114, 127, 117, WHITE); // Bottom
    uLCD.filled_rectangle(124,  14, 127, 117, WHITE); // Right
}


