// Project includes
#include "globals.h"
#include "hardware.h"
#include "map.h"
#include "graphics.h"
#include "speech.h"
#include "songplayer.h"

// Functions in this file
int get_action (GameInputs inputs);
int update_game (int action);
int near_item (int targetType);
void draw_game (int init);
void draw_start();
void init_main_map ();
int main ();
int omni = 0;
int enterx, entery;
int NPCx, NPCy;
Timer tmain;
float note[6]= {391.995,349.228,329.628,174.614,164.814, 0.0};
float duration[6]= {0.96,0.96,0.96,0.48,0.48, 0.0};
SongPlayer mySpeaker(p26);
int AccInverted;

/**
 * The main game state. Must include Player locations and previous locations for
 * drawing to work properly. Other items can be added as needed.
 */
struct {
    int x,y;    // Current locations
    int px, py; // Previous locations
    int state; //Dragon = 0, Giga = 1, Dragon w/ batt = 2, Roadster = 3
    int quest;
    char* status;
} Player;

/**
 * Given the game inputs, determine what kind of update needs to happen.
 * Possbile return values are defined below.
 */
#define NO_ACTION 0
#define ACTION_BUTTON 1
#define MENU_BUTTON 2
#define OMNI_BUTTON 3
#define GO_LEFT 4
#define GO_RIGHT 5
#define GO_UP 6
#define GO_DOWN 7
int get_action(GameInputs inputs)
{
    int but1 = inputs.b1;
    int but2 = inputs.b2;
    int but3 = inputs.b3;
    int accelx = inputs.ax;
    int accely = inputs.ay;

    MapItem* north = get_north(Player.x, Player.y);
    MapItem* south = get_south(Player.x, Player.y);
    MapItem* east = get_east(Player.x, Player.y);
    MapItem* west = get_west(Player.x, Player.y);
        
    //Buttons
    if(!but1){
        return ACTION_BUTTON;
    }
    if(!but2){
        return MENU_BUTTON;
    }
    if(!but3){
        return OMNI_BUTTON;
    }
        
        
    // Movement
    if (AccInverted){
        if((accely >= 175) && ((north->walkable) || omni==true)){ 
            return GO_UP;
        }
        else if((accely <= -175) && ((south->walkable)|| omni==true)){ 
            return GO_DOWN;
        }
        if((accelx <= -175) && ((west->walkable)|| omni==true)){
            return GO_LEFT;
        }
        else if((accelx >= 175) && ((east->walkable)|| omni==true)){
            return GO_RIGHT;
        }
    }
    else if (!AccInverted){
        if((accelx >= 175) && ((north->walkable) || omni==true)){ 
            return GO_UP;
        }
        else if((accelx <= -175) && ((south->walkable)|| omni==true)){ 
            return GO_DOWN;
        }
        if((accely >= 175) && ((west->walkable)|| omni==true)){
            return GO_LEFT;
        }
        else if((accely <= -175) && ((east->walkable)|| omni==true)){ 
            return GO_RIGHT;
        }
    }
    return NO_ACTION;
}

/**
 * Update the game state based on the user action. For example, if the user
 * requests GO_UP, then this function should determine if that is possible by
 * consulting the map, and update the Player position accordingly.
 * 
 * Return values are defined below. FULL_DRAW indicates that for this frame,
 * draw_game should not optimize drawing and should draw every tile, even if
 * the player has not moved.
 */
#define NO_RESULT 0
#define GAME_OVER 1
#define FULL_DRAW 2
#define EXP       3
#define MAZE      4

void draw_start(){
    uLCD.filled_rectangle(1, 1 , 126, 126, BLACK);
    
    uLCD.filled_circle(-5, 61 , 30, 0xfff71e); //Sun
    uLCD.filled_circle(-5, 61 , 27, 0xff6100); //Sun
    uLCD.filled_circle(-5, 61 , 20, 0xfff71e); //Sun
    uLCD.filled_circle(-5, 61 , 17, 0xff6100); //Sun
    uLCD.filled_circle(-5, 61 , 10, 0xfff71e); //Sun
    uLCD.filled_circle(-5, 61 , 7, 0xff6100); //Sun
    uLCD.filled_circle(32, 61 , 2, DGREY); //Mercury
    uLCD.filled_circle(43, 61 , 4, 0xf4bf11); //Venus
    uLCD.filled_circle(56, 61 , 4, 0x1a6df2); //Earth
    uLCD.filled_circle(68, 61 , 3, 0xc6590b); //Mars
    uLCD.filled_circle(81, 61 , 5, 0xc6a695); //Jupiter
    uLCD.filled_circle(96, 61 , 5, 0xf2edc6); //Saturn
    uLCD.line(89, 68 , 104, 53, 0xd1c992);//Saturn
    uLCD.filled_circle(110, 61 , 4, 0xbcbbb3); //Uranus
    uLCD.filled_circle(123, 61 , 4, 0x1ad9f2); //Neptune
    
    uLCD.rectangle(0, 0 , 127, 127, GREEN);
    
    uLCD.text_string("SAVING STARMAN", 2, 1, FONT_8X8, GREEN);
    uLCD.text_string("Press Action", 3, 13, FONT_5X7, GREEN);
    uLCD.text_string("To Start", 5, 14, FONT_5X7, GREEN);
}

void draw_over(){
    uLCD.filled_rectangle(1, 1 , 126, 126, BLACK);
    
    
    uLCD.rectangle(0, 0 , 127, 127, GREEN);
    
    uLCD.text_string("SAVING STARMAN", 2, 1, FONT_8X8, GREEN);
    uLCD.text_string("Thank you, for", 1, 6, FONT_8X8, WHITE);
    uLCD.text_string("saving Starman!", 2, 7, FONT_8X8, WHITE);
    uLCD.text_string("Press Action", 3, 13, FONT_5X7, GREEN);
    uLCD.text_string("To Start Again", 2, 14, FONT_5X7, GREEN);
}

void draw_warning(){
    uLCD.filled_rectangle(0, 0 , 127, 127, BLACK);
    uLCD.text_string("Rotate Device:", 2, 1, FONT_8X8, RED);
    if(AccInverted)uLCD.text_string("90 Degrees CW!", 2, 2, FONT_8X8, RED);
    else if(!AccInverted)uLCD.text_string("90 Degrees CCW", 2, 2, FONT_8X8, RED);
    uLCD.text_string("Press Action", 3, 13, FONT_5X7, RED);
    uLCD.text_string("To Acknowledge", 2, 14, FONT_5X7, RED);
}
void draw_menu(int music, int invert){
    //Base Menu
    uLCD.filled_rectangle(1, 1 , 126, 126, BLACK);
    uLCD.rectangle(0, 0 , 127, 127, GREEN);
    uLCD.text_string("MENU", 7, 1, FONT_8X8, GREEN);
    uLCD.text_string("Press Menu", 4, 13, FONT_5X7, GREEN);
    uLCD.text_string("To Exit!", 5, 14, FONT_5X7, GREEN);
    uLCD.text_string("ACTION to Toggle", 1, 3, FONT_8X8, WHITE);
    uLCD.text_string("Music:", 6, 4, FONT_8X8, WHITE);
    uLCD.text_string("OMNI to Toggle", 2, 7, FONT_8X8, WHITE);
    uLCD.text_string("Accelerometer:", 2, 8, FONT_8X8, WHITE);
  
    // Options
    if (music) uLCD.text_string("ON", 8, 5, FONT_8X8, GREEN);
    if (!music) uLCD.text_string("OFF", 7, 5, FONT_8X8, RED);
    if (!invert) uLCD.text_string("Not Inverted", 3, 9, FONT_8X8, RED);
    if (invert) uLCD.text_string("Inverted", 5, 9, FONT_8X8, GREEN);
}

int near_item(int targetType){
    MapItem* north = get_north(Player.x, Player.y);
    MapItem* south = get_south(Player.x, Player.y);
    MapItem* east = get_east(Player.x, Player.y);
    MapItem* west = get_west(Player.x, Player.y);
    
    if(north->type == targetType)return 1;
    else if(east->type == targetType) return 2;
    else if(south->type == targetType) return 3;
    else if(west->type == targetType) return 4;
    else return 0;
}

int update_game(int action)
{
    // Save player previous location before updating
    Player.px = Player.x;
    Player.py = Player.y;
    
    // Do different things based on the each action.
    // You can define functions like "go_up()" that get called for each case.
    switch(action)
    {
        case GO_UP:
            Player.y--;
            break;
        case GO_LEFT:   
            Player.x--;
            break;            
        case GO_DOWN:   
            Player.y++;
            break;
        case GO_RIGHT:  
            Player.x++;
            break;
        case ACTION_BUTTON:
            if(Player.x == 20 && Player.y == 35){
                char* NPCSpeech[] = {"When you get", "back to Earth,", "tell Schimmel", "I said hi."};
                long_speech(NPCSpeech, 4);
                return FULL_DRAW;
            } 
            else if(near_item(NPC)){
                if(Player.quest == 0){
                    Player.quest = 1;
                    char* NPCSpeech[] = {"Will you help me", "get to Mars?", "I'm out of power,", 
                                            "I need a battery.", "Travel to the", "Tesla Gigafactory", "on Earth to fetch", "me one."};
                    long_speech(NPCSpeech, 8);
                    return FULL_DRAW;
                }
                else if(Player.quest == 1){
                    speech("I need that extra","battery!");
                    return FULL_DRAW; 
                }
                else if(Player.quest == 2){
                    Player.quest = 3;
                    char* NPCSpeech[] = {"Fantastic!", "Just what I need.",
                                        "Hop in my Tesla", "Roadster and we", 
                                        "can head to", "the motherland.",
                                        "To Mars!"};
                    long_speech(NPCSpeech, 7);
                    get_here(NPCx, NPCy)->draw = draw_emptydragon;
                    Player.state = 3;
                    return FULL_DRAW;
                }
            }
            
            else if (near_item(MARS)){
                if(Player.quest == 3){
                    char* MarsSpeech[] = {"We made it to","Mars!",
                                        "Thank you so much", "for your help!"};
                    long_speech(MarsSpeech, 4);
                    return GAME_OVER;
                }
            }
            
            else if(near_item(AST)){
                if(Player.quest == 0){
                    char* AstSpeech[] = {"Mars is trapped", "in the asteroids!", "The Dragon capsule", "can't maneuver", "through that."};
                    long_speech(AstSpeech, 5);
                    return FULL_DRAW;
                }
                else if(Player.quest == 1){
                    char* AstSpeech[] = {"I should head to","Earth to get that","battery."};
                    long_speech(AstSpeech, 3);
                    return FULL_DRAW; 
                }
                else if(Player.quest == 2){
                    char* AstSpeech[] = {"Let's bring this","battery back to","Starman!", "", "Maybe he knows", "how to maneuver", "the asteroids."};
                    long_speech(AstSpeech, 7);
                    return FULL_DRAW;
                }
                else if(Player.quest == 3){
                    char* AstSpeech[] = {"The Roadster has","a secret power.","..."};
                    long_speech(AstSpeech, 3);
                    return EXP;
                }
                    
            }
            
            else if(near_item(EARTH)){
                if(Player.quest == 0){
                    char* EarthSpeech[] = {"The Earth!", "", "Let's not go home", "quite yet."};
                    long_speech(EarthSpeech, 4);
                    return FULL_DRAW;
                }
                else if(Player.quest == 1){
                    enterx = Player.x;
                    entery = Player.y;
                    set_active_map(1);
                    Player.x = Player.y = 2;
                    Player.state = 1;
                    return FULL_DRAW; 
                }
                else if(Player.quest == 2){
                    speech("We just got to space!","Head to Starman!");
                    return FULL_DRAW;
                }
                else if(Player.quest == 3){
                    char* EarthSpeech[] = {"Wrong planet!","This is Earth.",
                                        "Starman said to", "head for Mars."};
                    long_speech(EarthSpeech, 4);
                }
            }
            
            else if(near_item(BATT)){
                Player.quest = 2;
                char* EarthSpeech[] = {"Alas!","A Tesla Battery!",
                                        "Let's head back", "to see Starman."};
                long_speech(EarthSpeech, 4);
                set_active_map(0);
                Player.x = enterx;
                Player.y = entery;
                Player.state = 2;
                return FULL_DRAW;
            }
            
            else if(near_item(BUTTON1) || near_item(BUTTON2)){
                return MAZE;
            }
            break;
        case MENU_BUTTON:
            draw_menu(mySpeaker.on, AccInverted); //(music, inverted)
            wait(0.25);
            while(button2){
                if (!button1){
                    wait(0.125);
                    if (mySpeaker.on == false){
                        mySpeaker.on = true;
                        mySpeaker.PlaySong(note,duration, 0.25);
                    }
                    else if(mySpeaker.on == true) mySpeaker.on = false;
                    draw_menu(mySpeaker.on, AccInverted); //(music, inverted)
                }
                if (!button3){
                    wait(0.125);
                    if (AccInverted == false){
                        AccInverted = true;
                        draw_warning();
                        while(button1);
                    }
                    else if(AccInverted == true){
                        AccInverted = false;
                        draw_warning();
                        while(button1);
                    }
                    draw_menu(mySpeaker.on, AccInverted); //(music, inverted)
                }
            }
            wait(0.25);
            draw_game(true); 
            break;
        
        case OMNI_BUTTON:
            wait(0.25);
            if (omni == true){
                omni = false;
                Player.status = "";
            }
            else if (omni == false){
                omni = true;
                Player.status = "OMNI";
            }
            break;
            
        default:        break;
    }
    return NO_RESULT;
}

/**
 * Entry point for frame drawing. This should be called once per iteration of
 * the game loop. This draws all tiles on the screen, followed by the status 
 * bars. Unless init is nonzero, this function will optimize drawing by only 
 * drawing tiles that have changed from the previous frame.
 */
void draw_game(int init)
{
    // Draw game border first
    if(init) draw_border();
    
    // Iterate over all visible map tiles
    for (int i = -5; i <= 5; i++) // Iterate over columns of tiles
    {
        for (int j = -4; j <= 4; j++) // Iterate over one column of tiles
        {
            // Here, we have a given (i,j)
            
            // Compute the current map (x,y) of this tile
            int x = i + Player.x;
            int y = j + Player.y;
            
            // Compute the previous map (px, py) of this tile
            int px = i + Player.px;
            int py = j + Player.py;
                        
            // Compute u,v coordinates for drawing
            int u = (i+5)*11 + 3;
            int v = (j+4)*11 + 15;
            
            // Figure out what to draw
            DrawFunc draw = NULL;
            if (i == 0 && j == 0) 
            {
                draw_player(u, v, Player.state);
                continue;
            }
            else if (x >= 0 && y >= 0 && x < map_width() && y < map_height()) // Current (i,j) in the map
            {
                MapItem* curr_item = get_here(x, y);
                MapItem* prev_item = get_here(px, py);
                if (init || curr_item != prev_item) // Only draw if they're different
                {
                    if (curr_item) // There's something here! Draw it
                    {
                        draw = curr_item->draw;
                    }
                    else // There used to be something, but now there isn't
                    {
                        if(get_active_map_num() == 1) draw = draw_gigafloor;
                        else draw = draw_nothing;
                    }
                }
            }
            else if (init) // If doing a full draw, but we're out of bounds, draw the walls.
            {
                draw = draw_wall;
            }

            // Actually draw the tile
            if (draw) draw(u, v);
        }
    }

    // Draw status bars    
    draw_upper_status(Player.x, Player.y);
    draw_lower_status(Player.status);
}


/**
 * Initialize the main world map. Add walls around the edges, interior chambers,
 * and plants in the background so you can see motion.
 */
void init_main_map()
{
    // "Random" stars
    Map* map = set_active_map(0);
    for(int i = map_width() + 3; i < map_area(); i += 39)
    {
        add_star(i % map_width(), i / map_width());
    }
        
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    
    srand(tmain.read_ms());
    
    int area2035[9][2] = {{19,34},{20,34},{21,34},{19,35},{20,35},{21,35},{19,36},{20,36},{21,36}};
    
    int collision = true;
    int marsx = 0;
    int marsy = 0;
    int marsarea[25][2];
    while(collision == true){
        marsx = (rand() % 44)+3;
        marsy = (rand() % 44)+3;
    
        int marsarea[25][2] = {
        {marsx,marsy},{marsx+1,marsy},{marsx+2,marsy},{marsx,marsy+1},{marsx,marsy+2},
        {marsx+1,marsy+1},{marsx+1,marsy+2},{marsx+2,marsy+1},{marsx+2,marsy+2},{marsx-1,marsy},
        {marsx-2,marsy},{marsx,marsy-1},{marsx,marsy-2},{marsx-1,marsy-1},{marsx-1,marsy-2},
        {marsx-2,marsy-1},{marsx-2,marsy-2},{marsx+1,marsy-1},{marsx+1,marsy-2},{marsx+2,marsy-1},
        {marsx+2,marsy-2},{marsx-1,marsy+1},{marsx-2,marsy+1},{marsx-1,marsy+2},{marsx-2,marsy+2}};
        
        for(int i=0; i<9; i++){
            for(int j=0; j<25; j++){
               if((area2035[i][0] == marsarea[j][0]) && (area2035[i][1] == marsarea[j][1])){
                    collision=true;
                }
                else collision=false;
             }
        }
    }
    add_planet(marsx,marsy,MARS);
    
    collision = true;
    int earthx = 0;
    int earthy = 0;
    int eartharea[25][2];
    while (collision==true){
        earthx = (rand() % 44)+3;
        earthy = (rand() % 44)+3;
        int eartharea[25][2] = {
        {earthx,earthy},{earthx+1,earthy},{earthx+2,earthy},{earthx,earthy+1},{earthx,earthy+2},
        {earthx+1,earthy+1},{earthx+1,earthy+2},{earthx+2,earthy+1},{earthx+2,earthy+2},{earthx-1,earthy},
        {earthx-2,earthy},{earthx,earthy-1},{earthx,earthy-2},{earthx-1,earthy-1},{earthx-1,earthy-2},
        {earthx-2,earthy-1},{earthx-2,earthy-2},{earthx+1,earthy-1},{earthx+1,earthy-2},{earthx+2,earthy-1},
        {earthx+2,earthy-2},{earthx-1,earthy+1},{earthx-2,earthy+1},{earthx-1,earthy+2},{earthx-2,earthy+2}};
        for(int i=0; i<25; i++){
            for(int j=0; j<25; j++){
               if((eartharea[i][0] == marsarea[j][0]) && (eartharea[i][1] == marsarea[j][1])){
                    collision=true;
                }
                else collision=false;
             }
             for(int j=0; j<9; j++){
               if((area2035[j][0] == eartharea[i][0]) && (area2035[j][1] == eartharea[i][1])){
                    collision=true;
                }
                else collision=false;
             }
        }
    }
    add_planet(earthx,earthy,EARTH);
    
    collision = true;
    while (collision==true){
        NPCx = (rand() % 48)+1;
        NPCy = (rand() % 48)+1;
        for(int i=0; i<25; i++){
            if((marsarea[i][0] == NPCx) && (marsarea[i][1] == NPCy)){
                collision=true;
            }
            else if((eartharea[i][0] == NPCx) && (eartharea[i][1] == NPCy)){
                collision=true;
            }
            for(int j=0; j<9; j++){
                if((area2035[j][0] == NPCx) && (area2035[j][1] == NPCy)){
                    collision=true;
                }
                else collision=false;
            } 
        }
    }
    add_NPC(NPCx, NPCy);
    
    collision = true;
    while (collision==true){
        Player.x = (rand() % 48)+1;
        Player.y = (rand() % 48)+1;
        for(int i=0; i<25; i++){
            if((marsarea[i][0] == Player.x) && (marsarea[i][1] == Player.y)){
                collision=true;
            }
            else if((eartharea[i][0] == Player.x) && (eartharea[i][1] == Player.y)){
                collision=true;
            }
            for(int j=0; j<9; j++){
                if((area2035[j][0] == Player.x) && (area2035[j][1] == Player.y)){
                    collision=true;
                }
                else collision=false;
            }
        }
        if((NPCx == Player.x) && (NPCy==Player.y)){
            collision=true;
        }  
    }
       
    print_map();
}

void init_giga_map()
{
    Map* map = set_active_map(1);
    
    //maze
    add_wall(2, 4, VERTICAL, 5);
    add_wall(2, 10, VERTICAL, 3);
    add_wall(2, 14, VERTICAL, 5);
    add_wall(4, 1, VERTICAL, 6);
    add_wall(4, 8, VERTICAL, 3);
    add_wall(4, 12, VERTICAL, 7);
    add_wall(6, 2, VERTICAL, 3);
    add_wall(6, 6, VERTICAL, 3);
    add_wall(6, 10, VERTICAL, 3);
    add_wall(6, 14, VERTICAL, 6);
    add_wall(8, 4, VERTICAL, 3);
    add_wall(8, 10, VERTICAL, 3);
    add_wall(8, 16, VERTICAL, 4);
    add_wall(10, 6, VERTICAL, 3);
    add_wall(10, 12, VERTICAL, 3);
    add_wall(10, 18, VERTICAL, 2);
    add_wall(12, 1, VERTICAL, 2);
    add_wall(12, 4, VERTICAL, 3);
    add_wall(12, 14, VERTICAL, 3);
    add_wall(14, 2, VERTICAL, 3);
    add_wall(14, 6, VERTICAL, 3);
    add_wall(16, 2, VERTICAL, 3);
    add_wall(16, 8, VERTICAL, 5);
    add_wall(16, 16, VERTICAL, 4);
    add_wall(18, 1, VERTICAL, 2);
    add_wall(18, 4, VERTICAL, 7);
    add_wall(18, 14, VERTICAL, 3);
    
    add_wall(7, 2, HORIZONTAL, 7);
    add_wall(17, 2, HORIZONTAL, 1);
    add_wall(7, 4, HORIZONTAL, 5);
    add_wall(5, 6, HORIZONTAL, 1);
    add_wall(9, 6, HORIZONTAL, 1);
    add_wall(16, 6, HORIZONTAL, 2);
    add_wall(3, 8, HORIZONTAL, 1);
    add_wall(7, 8, HORIZONTAL, 2);
    add_wall(11, 8, HORIZONTAL, 2);
    add_wall(15, 8, HORIZONTAL, 3);
    add_wall(3, 10, HORIZONTAL, 3);
    add_wall(9, 10, HORIZONTAL, 7);
    add_wall(5, 12, HORIZONTAL, 1);
    add_wall(11, 12, HORIZONTAL, 4);
    add_wall(18, 12, HORIZONTAL, 2);
    add_wall(3, 14, HORIZONTAL, 1);
    add_wall(7, 14, HORIZONTAL, 3);
    add_wall(13, 14, HORIZONTAL, 4);
    add_wall(9, 16, HORIZONTAL, 3);
    add_wall(14, 16, HORIZONTAL, 4);
    add_wall(5, 18, HORIZONTAL, 1);
    add_wall(11, 18, HORIZONTAL, 4);
    
    //Main Walls    
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    
    //Items
    add_batt(18,18);
    add_button(5, 19, 1);
    add_button(7, 3, 2);
    add_door(6, 9);
    add_door(19, 15);
    print_map();
}

/**
 * Program entry point! This is where it all begins.
 * This function orchestrates all the parts of the game. Most of your
 * implementation should be elsewhere - this holds the game loop, and should
 * read like a road map for the rest of the code.
 */
int main()
{
    // First things first: initialize hardware
    tmain.start();
    ASSERT_P(hardware_init() == ERROR_NONE, "Hardware init failed!");
    AccInverted = 0;
    
    //Start Music
    mySpeaker.on = true;
    mySpeaker.PlaySong(note,duration, 0.25);

    // Start Menu
    draw_start();
    while(button1);
    
    // Initialize the maps
    maps_init();
    init_main_map();
    init_giga_map();
    
    // Initialize game state
    set_active_map(0);
    Player.quest = 0;
    Player.state = 0;

    // Initial drawing
    draw_game(true);

    // Main game loop
    while(1)
    {
        // Timer to measure game update speed
        Timer t; t.start();
        
        // Actually do the game update:
        
        // 1. Read inputs
        // 2. Determine action (get_action)
        // 3. Update game (update_game)
        int result = update_game(get_action(read_inputs()));
        if(result == EXP){
            switch(near_item(AST)){
                case 1:
                    get_here(Player.x, Player.y-1)->draw = draw_explosion; //North
                    draw_game(true); 
                    wait(0.25);
                    map_erase(Player.x, Player.y-1);
                    draw_game(true);
                    break;
                case 2:
                    get_here(Player.x+1, Player.y)->draw = draw_explosion; //East
                    draw_game(true);
                    wait(0.25);
                    map_erase(Player.x+1, Player.y);
                    draw_game(true);
                    break;
                case 3:
                    get_here(Player.x, Player.y+1)->draw = draw_explosion; //South
                    draw_game(true);
                    wait(0.25);
                    map_erase(Player.x, Player.y+1);
                    draw_game(true);
                    break;
                case 4:
                    get_here(Player.x-1, Player.y)->draw = draw_explosion; //West
                    draw_game(true);
                    wait(0.25);
                    map_erase(Player.x-1, Player.y);
                    draw_game(true);
                    break; 
                default: break;
            }
        }
        
        if(result == MAZE){
            int type;
            int BUTTON;
            if (near_item(BUTTON1)) BUTTON = BUTTON1;
            else if (near_item(BUTTON2)) BUTTON = BUTTON2;
            switch(near_item(BUTTON)){
                case 1:
                    get_here(Player.x, Player.y-1)->draw = draw_button_on; //North
                    type = get_here(Player.x, Player.y-1)->type;
                    pc.printf("Button is North\r\n");
                    break;
                case 2:
                    get_here(Player.x+1, Player.y)->draw = draw_button_on; //East
                    type = get_here(Player.x+1, Player.y)->type;
                    break;
                case 3:
                    get_here(Player.x, Player.y+1)->draw = draw_button_on; //South
                    type = get_here(Player.x, Player.y+1)->type;
                    break;
                case 4:
                    get_here(Player.x-1, Player.y)->draw = draw_button_on; //West
                    type = get_here(Player.x-1, Player.y)->type;
                    break; 
                default: break;
            }
            char *coords;
            if (type == BUTTON1){
                map_erase(6, 9);
                coords = "(6, 9) opened.";
            }
            else if (type == BUTTON2){
                map_erase(19, 15);
                coords = "(19, 15) opened.";
            }
            speech("The door at", coords);
            draw_game(true);
        }                  
        // 3b. Check for game over
        if (result == GAME_OVER){
            maps_deinit();
            draw_over();
            while(button1);
            wait(0.25);
            main();
        }
        // 4. Draw frame (draw_game)
        if (result == FULL_DRAW) draw_game(true);
        else draw_game(false);
        // 5. Frame delay
        t.stop();
        int dt = t.read_ms();
        if (dt < 100) wait_ms(100 - dt);
    }
}
