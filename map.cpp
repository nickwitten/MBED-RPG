#include "map.h"
#include <math.h>
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
static Map maps[2];
static int active_map;

/**
 * The first step in HashTable access for the map is turning the two-dimensional
 * key information (x, y) into a one-dimensional unsigned integer.
 * This function should uniquely map (x,y) onto the space of unsigned integers.
 */
static unsigned XY_KEY(int X, int Y) {
    // gives the object a key counting from left to right based on its X Y
    // coordinates.
    if (X < 0 || Y < 0){  // Catch invalid inputs
        return NULL;
    }
    unsigned int XY = Y * map_width() + X;
    return XY;
}

/**
 * This is the hash function actually passed into createHashTable. It takes an
 * unsigned key (the output of XY_KEY) and turns it into a hash value (some
 * small non-negative integer).
 */
unsigned map_hash(unsigned key)
{
    return key % 10;
}

void maps_init()
{  
    // Initialize hash table
    // Set width & height

    // Initialize first map
    maps[0].w = 50;
    maps[0].h = 50;
    HashTable* HT = createHashTable(map_hash,10);
    maps[0].items = HT;
    
    
    // Initialize second map
    maps[1].w = 50;
    maps[1].h = 50;
    HashTable* HT2 = createHashTable(map_hash,10);
    maps[1].items = HT2;
}

Map* get_active_map()
{
    return &maps[active_map];
}

Map* set_active_map(int m)
{
    // Delete other map plants/walls and add plants/walls to new map to save space
    
    // delete plants
    for(int i = map_width() + 3; i < (map_area()); i += 39)
    {
        int x = i % map_width();
        int y = i / map_width();
        if (get_here(x, y)->type == PLANT) {
            map_erase(x,y);
        }
    }
    // delete walls
    for (int i=0; i<map_width(); ++i) {
        map_erase(i,0);
        map_erase(i,map_height());
    }
    for (int i=0; i<map_height(); ++i) {
        map_erase(0,i);
        map_erase(map_width(),i);
    }
    
    // Set new map
    active_map = m;
    
    // add plants
    for(int i = map_width() + 3; i < (map_area()); i += 39)
    {
        int x = i % map_width();
        int y = i / map_width();
        if (!get_here(x,y)) {
            add_plant(i % map_width(), i / map_width());
        }
    }
    // add walls
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    return get_active_map();
}

void print_map()
{
    // As you add more types, you'll need to add more items to this array.
    char lookup[] = {'W', 'P', 'L', 'N', 'B', 'S','T','E','O'};
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
    Map* mapPtr = get_active_map();
    return mapPtr->w;
}

int map_height()
{
    Map* mapPtr = get_active_map();
    return mapPtr->h;
}

int map_area()
{
    Map* mapPtr = get_active_map();
    int area = mapPtr->h * mapPtr->w;
    return area;
}

MapItem* get_north(int x, int y)
{
    Map* mapPtr = get_active_map();
    HashTable* items = mapPtr->items;
    unsigned int key = XY_KEY(x,y);
    key -= map_width();
    MapItem* item = (MapItem*)getItem(items, key);
    return item;
}

MapItem* get_south(int x, int y)
{
    Map* mapPtr = get_active_map();
    HashTable* items = mapPtr->items;
    unsigned int key = XY_KEY(x,y);
    key += map_width();
    MapItem* item = (MapItem*)getItem(items, key);
    return item;
}

MapItem* get_east(int x, int y)
{
    Map* mapPtr = get_active_map();
    HashTable* items = mapPtr->items;
    unsigned int key = XY_KEY(x,y);
    key += 1;
    MapItem* item = (MapItem*)getItem(items, key);
    return item;
}

MapItem* get_west(int x, int y)
{
    Map* mapPtr = get_active_map();
    HashTable* items = mapPtr->items;
    unsigned int key = XY_KEY(x,y);
    key -= 1;
    MapItem* item = (MapItem*)getItem(items, key);
    return item;
}

MapItem* get_here(int x, int y)
{
    Map* mapPtr = get_active_map();
    HashTable* items = mapPtr->items;
    unsigned int key = XY_KEY(x,y);
    MapItem* item = (MapItem*)getItem(items, key);
    return item;
}


void map_erase(int x, int y)
{
    Map* mapPtr = get_active_map();
    HashTable* items = mapPtr->items;
    unsigned int key = XY_KEY(x,y);
    deleteItem(items,key); 
}

void add_wall(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
    {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = WALL;
        w1->draw = draw_wall;
        w1->walkable = false;
        w1->data = NULL;
        unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
        void* val = insertItem(get_active_map()->items, key, w1);
        if (val) free(val); // If something is already there, free it
    }
}

void add_plant(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = PLANT;
    w1->draw = draw_plant;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_ladder(int x, int y, int target_map, int target_x, int target_y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = LADDER;
    w1->draw = draw_ladder;
    w1->walkable = false;
    w1->data = (int*) malloc(3 * sizeof(int*));
    *(int*)(w1->data) = target_map;
    *((int*)(w1->data)+1) = target_x;
    *((int*)(w1->data)+2) = target_y;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val);
}

void add_npc(int x, int y, int character){
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = NPC;
    pc.printf("Character %d\r\n",character);
    if (character == WISE_BEAR) {
        w1->draw = draw_wise_bear;
    } else if (character == MASTER_SQUIRREL) {
        w1->draw = draw_master_squirrel;    
    } else {
        w1->draw = draw_npc_squirrel;
    }
    w1->walkable = false;
    w1->data = (int*) malloc(sizeof(int*));
    *(int*)(w1->data) = character;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val);
}

void add_snake(int x, int y) {
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = SNAKE;
    w1->draw = draw_snake;
    w1->walkable = false;
    w1->data = (int*) malloc(sizeof(int*));
    *(int*)(w1->data) = BREAKABLE;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); 
}

void add_breakable_wall(int x, int y) {
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = BREAKABLEWALL;
    w1->draw = draw_breakable_wall;
    w1->walkable = false;
    w1->data = (int*) malloc(sizeof(int*));
    *(int*)(w1->data) = BREAKABLE;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); 
}

void add_sonar(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = SONAR;
    w1->draw = draw_sonar;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val);
}

void add_treasure(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = TREASURE;
    w1->draw = draw_treasure;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val);        
}

void add_tree(int x, int y)
{
    for (int i=0; i<3; ++i) {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = TREE;
        if (i == BOTTOM) w1->draw = draw_tree_bottom;
        if (i == MIDDLE) w1->draw = draw_tree_middle;
        if (i == TOP)    w1->draw = draw_tree_top;
        w1->walkable = true;
        w1->data = (int*) malloc(sizeof(int*));
        *(int*)(w1->data) = OCCLUDE;
        void* val = insertItem(get_active_map()->items, XY_KEY(x, y-i), w1);
        if (val) free(val);  
    }
}

void add_fake_treasure(int x, int y) {
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = FAKE_TREASURE;
    w1->draw = draw_treasure;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val);        
}