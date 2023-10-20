#include "map.h"

#include "globals.h"
#include "graphics.h"

/**
 * The Map structure. This holds a HashTable for all the MapItems, along with
 * values for the width and height of the Map.
 */
struct Map {
    HashTable* items;
    int w, h;
};

/**
 * Storage area for the maps.
 * This is a global variable, but can only be access from this file because it
 * is static.
 */
static Map mainmap, gigamap;
int active_map;

/**
 * The first step in HashTable access for the map is turning the two-dimensional
 * key information (x, y) into a one-dimensional unsigned integer.
 * This function should uniquely map (x,y) onto the space of unsigned integers.
 */
static unsigned XY_KEY(int X, int Y) {
    unsigned int XYKey;
    //Set Upper two bytes to X
    XYKey = X & 255;
    XYKey = XYKey << 8;
    //Set Lower two bytes to Y
    XYKey += Y & 255;
    return XYKey;
}

/**
 * This is the hash function actually passed into createHashTable. It takes an
 * unsigned key (the output of XY_KEY) and turns it into a hash value (some
 * small non-negative integer).
 */
unsigned map_hash(unsigned key)
{
    //key % number of buckets
    return key%127;
}

void maps_init()
{
    // Initialize hash tables
    HashTable* mainht = createHashTable(map_hash, 127);
    mainmap.items = mainht;
    HashTable* gigaht = createHashTable(map_hash, 127);
    gigamap.items = gigaht;
    // Set width & height
    mainmap.w = 50;
    mainmap.h = 50;
    gigamap.w = 21;
    gigamap.h = 21;
}

void maps_deinit()
{
    // Destroy hash tables
    destroyHashTable(mainmap.items);
    destroyHashTable(gigamap.items);
}

Map* get_active_map()
{
    if (active_map == 1) return &gigamap;
    else return &mainmap;
}

int get_active_map_num(){
    return active_map;
}

Map* set_active_map(int m)
{
    active_map = m;
    if (active_map == 1) return &gigamap;
    else return &mainmap;
}

void print_map()
{
    // As you add more types, you'll need to add more items to this array.
    char lookup[] = {'W', 'S', 'N', 'M', 'E', 'B'};
    for(int y = 0; y < map_height(); y++)
    {
        for (int x = 0; x < map_width(); x++)
        {
            MapItem* item = get_here(x,y);
            if (item) pc.printf("%c", lookup[item->type]);
            else pc.printf(" ");
        }
        pc.printf("\r\n");
    }
}

int map_width()
{
    Map* map = get_active_map();
    return map->w;
}

int map_height()
{
    Map* map = get_active_map();
    return map->h;
}

int map_area()
{
    return map_height()*map_width();
}

MapItem* get_north(int x, int y)
{
    y-=1;
    Map* map = get_active_map();
    return (MapItem*)(getItem(map->items, XY_KEY(x,y)));
}

MapItem* get_south(int x, int y)
{
    y+=1;
    Map* map = get_active_map();
    return (MapItem*)(getItem(map->items, XY_KEY(x,y)));
}

MapItem* get_east(int x, int y)
{
    x+=1;
    Map* map = get_active_map();
    return (MapItem*)(getItem(map->items, XY_KEY(x,y)));
}

MapItem* get_west(int x, int y)
{
    x-=1;
    Map* map = get_active_map();
    return (MapItem*)(getItem(map->items, XY_KEY(x,y)));
}

MapItem* get_here(int x, int y)
{
    Map* map = get_active_map();
    return (MapItem*)(getItem(map->items, XY_KEY(x,y)));
}


void map_erase(int x, int y)
{
    unsigned key0 = XY_KEY(x, y);
    deleteItem(get_active_map()->items, key0);
}

void add_wall(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
    {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = WALL;
        if(active_map == 1) w1->draw = draw_gigawall;
        else w1->draw = draw_wall;
        w1->walkable = false;
        w1->data = NULL;
        unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
        void* val = insertItem(get_active_map()->items, key, w1);
        if (val) free(val); // If something is already there, free it
    }
}

void add_planet(int x, int y, int planet)
{ 
    if(planet == MARS){
        for (int j=-2; j<3; j++){
            for (int i=-2; i<3; i++){  
                MapItem* w9 = (MapItem*) malloc(sizeof(MapItem));
                w9->type = AST;
                w9->draw = draw_wall;
                w9->walkable = false;
                w9->data = NULL;
                unsigned key9 = XY_KEY(x+j, y+i);
                void* val9 = insertItem(get_active_map()->items, key9, w9);
                if (val9) free(val9); // If something is already there, free it
            }
        }
    }
    for(int i=-1; i<2; i++){
        for(int j =-1; j<2; j++){
            MapItem* w0 = (MapItem*) malloc(sizeof(MapItem));
            w0->type = planet;
            if(planet == MARS) w0->draw = draw_mars;
            else if(planet == EARTH) w0->draw = draw_earth;
            w0->walkable = false;
            w0->data = NULL;
            unsigned key0 = XY_KEY(x+i, y+j);
            void* val0 = insertItem(get_active_map()->items, key0, w0);
            if (val0) free(val0); // If something is already there, free it
        }
    }   
}

void add_star(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = STAR;
    w1->draw = draw_star;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_button(int x, int y, int num)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    if (num == 1) w1->type = BUTTON1;
    else if (num == 2) w1->type = BUTTON2;
    w1->draw = draw_button_off;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_door(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = DOOR;
    w1->draw = draw_door;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_NPC(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = NPC;
    w1->draw = draw_NPC;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_batt(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = BATT;
    w1->draw = draw_batt;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
