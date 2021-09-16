// Project includes
#include "globals.h"
#include "hardware.h"
#include "map.h"
#include "graphics.h"
#include "speech.h"

// Functions in this file
int get_action (GameInputs inputs);
int update_game (int action);
void draw_game (int init);
void init_main_map ();
int main ();
void change_maze ();

/**
 * The main game state. Must include Player locations and previous locations for
 * drawing to work properly. Other items can be added as needed.
 */
struct {
    int lives;  // How many lives the player has left
    int x,y;    // Current locations
    int px, py; // Previous locations
    int has_acorns, acornx, acorny, acornpx, acornpy, acorndirection; // Projectile location and direction
    int has_sonar;
    int found_treasure_before;
    int current_map; // What map the player is in
    int omnipotent; // 1 when in omnipotent mode
    // You can add other properties for the player here
} Player;

/**
 * Structure for the acorn projectiles
 */
typedef struct {
    int x, y, px, py, direction;
} Acorn;

Acorn acorns[3];
/**
 * Store current treasure location
 */
struct {
    int x,y;    // Current location
} Treasure;


/**
 * Given the game inputs, determine what kind of update needs to happen.
 * Possibile return values are defined below.
 */
 
#define NO_ACTION 0
#define ACTION_BUTTON 1
#define MENU_BUTTON 2
#define GO_LEFT 3
#define GO_RIGHT 4
#define GO_UP 5
#define GO_DOWN 6
#define THROW 7
#define OMNIPOTENT 8
int get_action(GameInputs inputs)
{
    if (!inputs.b3) return OMNIPOTENT;
    
    if (!inputs.b2) return THROW;
    
    if (!inputs.b1) return ACTION_BUTTON;
    
    float deadZone = 0.3;
    
    // tilted right
    if (inputs.ax >= deadZone) return GO_RIGHT;
    
    // tilted left
    if (inputs.ax <= -deadZone) return GO_LEFT;
    
    // tilted forward
    if (inputs.ay >= deadZone) return GO_UP;
    
    // tilted backward
    if (inputs.ay <= -deadZone) return GO_DOWN;
    
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
#define FULL_DRAW 1
#define GAME_OVER 2
int action_button() {
     // Create list of items around player
    MapItem* adjacent_items[4] = {get_north(Player.x, Player.y), 
                                  get_south(Player.x, Player.y), 
                                  get_east(Player.x, Player.y), 
                                  get_west(Player.x, Player.y)};
    
    // Loop through items around player
    int i;
    for (i=0; i<4; ++i) {
        if (adjacent_items[i]) {   
                 
            MapItem* item = adjacent_items[i];
            
            // When Player is next to treasure
            if (item->type == TREASURE) {
                if (Player.found_treasure_before) {
                    return GAME_OVER;
                } else {
                    pc.printf("Found treasure for the first time!\r\n");
                    // End game next time it's found
                    Player.found_treasure_before = 1;
                    // Delete the current treasure
                    map_erase(Treasure.x, Treasure.y);
                    // Create a random location as long as there is no item
                    // currently here
                    do {
                        // Random location in West half of map
                        Treasure.x = (rand() % 24) + 1; //
                        Treasure.y = (rand() % 24) + 1;
                    } while(!get_here(Treasure.x, Treasure.y));
                    add_treasure(Treasure.x, Treasure.y);
                    const char *line1 = "The treasure has";
                    const char *line2 = "moved!";
                    speech(line1, line2);
                }
                return FULL_DRAW;
            }
            
            // When player is next to fake treasure
            if (item->type == FAKE_TREASURE) {
                if (i == 0) { // treasure is north
                    map_erase(Player.x,Player.y-1);
                    add_snake(Player.x,Player.y-1);
                }
                if (i == 1) { // treasure is south
                    map_erase(Player.x,Player.y+1); 
                    add_snake(Player.x,Player.y+1);   
                }
                if (i == 2) { // treasure is east
                    map_erase(Player.x+1,Player.y);  
                    add_snake(Player.x+1,Player.y);  
                }
                if (i == 3) { // treasure is west
                    map_erase(Player.x-1,Player.y); 
                    add_snake(Player.x-1,Player.y);   
                }
                const char *line1 = "Oh no!";
                const char *line2 = "It's a snake!";
                speech(line1,line2);
                return FULL_DRAW;    
            }
            
            
            // When player is next to ladder
            if (item->type == LADDER) {
                int m = *(int*)item->data; // Get map index
                int x = *((int*)item->data+1); // Get desired x
                int y = *((int*)item->data+2); // Get desired y
                // Set values
                Player.current_map = m; // set Player in this map
                set_active_map(m);
                Player.x = x;
                Player.y = y;
                pc.printf("Go Down Ladder!");
                return FULL_DRAW;
            }
            
            // When player is next to NPC
            if (item->type == NPC) {
                if (*(int*)adjacent_items[i]->data == WISE_BEAR) { // For Wise Bear
                    const char* lines[20] = {"Wise Bear:",
                                            "Hello squirrel.",
                                            "For the sake",
                                            "of your brothers",
                                            "and sisters,",
                                            "you must find",
                                            "the Golden Acorn.",
                                            "Go down the",
                                            "ladder,",
                                            "find the sonar,",
                                            "come back, and",
                                            "locate the",
                                            "missing treasure!",
                                            "I advise you to",
                                            "talk to the",
                                            "master squirrel",
                                            "before you leave.",
                                            "Here, take these",
                                            "acorns for",
                                            "protection."};
                    long_speech(lines, 20);
                    pc.printf("moving on\r\n");
                    if (Player.has_acorns != 2) { // Don't unlearn burst throw
                        Player.has_acorns = 1; // Give the player acorns
                    }
                    pc.printf("Player has acorns now\r\n");
                    add_ladder(20,5,1, 2, 24); // Add ladder to access second map
                    pc.printf("Ladder added\r\n");
                    return FULL_DRAW;
                } else if (*(int*)adjacent_items[i]->data == MASTER_SQUIRREL) { // For master squirrel
                    const char* lines[12] = {"Master Squirrel:",
                                            "Hello Young One.",
                                            "I will teach",
                                            "you how to throw",
                                            "three acorns at",
                                            "once.",
                                            "Just follow my",
                                            "lead.",
                                            "Great!",
                                            "Be careful out",
                                            "there and good",
                                            "luck!"};
                    long_speech(lines,12);
                    Player.has_acorns = 2; // Give player burst throw
                    pc.printf("moving on\r\n");
                    return FULL_DRAW;
                } else if (*(int*)adjacent_items[i]->data == NPC_SQUIRREL1) { // For npc squirrel1
                    const char* lines[4] = {"See those walls?",
                                            "They look weak.",
                                            "Maybe something",
                                            "could break them."};
                    long_speech(lines, 4);
                    pc.printf("moving on\r\n");
                    return FULL_DRAW;
                } else if (*(int*)adjacent_items[i]->data == NPC_SQUIRREL2) { // For npc squirrel2
                    const char* lines[7] = {"I'm too scared",
                                            "to go further",
                                            "I've seen so",
                                            "many snakes!",
                                            "They'll attack",
                                            "you as you walk",
                                            "by!"};
                    long_speech(lines, 7);
                    pc.printf("moving on\r\n");
                    return FULL_DRAW;
                } else if (*(int*)adjacent_items[i]->data == NPC_SQUIRREL3) { // For npc squirrel3
                    const char* lines[4] = {"The ground is",
                                            "rumbling!",
                                            "I think the walls",
                                            "are moving!"};
                    long_speech(lines, 4);
                    change_maze();
                    pc.printf("moving on\r\n");
                    return FULL_DRAW;
                }
            }
            
            // When player is next to Sonar
            if (item->type == SONAR) {
                Player.has_sonar = 1;
                if (i == 0) { // Sonar is north
                    map_erase(Player.x,Player.y-1);
                }
                if (i == 1) { // Sonar is south
                    map_erase(Player.x,Player.y+1);    
                }
                if (i == 2) { // Sonar is east
                    map_erase(Player.x+1,Player.y);    
                }
                if (i == 3) { // Sonar is west
                    map_erase(Player.x-1,Player.y);    
                }
                set_active_map(0);
                // Create a random location as long as there is no item
                // currently here
                do {
                    // Random location in East half of map
                    Treasure.x = (rand() % 24) + 25;
                    Treasure.y = (rand() % 24) + 25;
                } while(!get_here(Treasure.x, Treasure.y));
                add_treasure(Treasure.x, Treasure.y);
                set_active_map(1);
                add_npc(10,21,NPC_SQUIRREL3);
                return FULL_DRAW;
            }
        }
    }
    
    if (Player.has_sonar) { // Scan for treasure
        // Only works if in map 0
        if (!Player.current_map) {
            int dx = Treasure.x - Player.x; // Get distance from player
            int dy = Treasure.y - Player.y;
            const char *xreading;
            char yreading[20];
            pc.printf("1\r\n");
            // Get x and y differences in Cardinal Directions
            if (dx > 5) {
                xreading = "East";
            } else if (dx < -5) {
                xreading = "West";
            } else {
                xreading = "";
            }
            if (dy > 4) {
                strcpy(yreading, "South ");
            } else if (dy < -4) {
                strcpy(yreading, "North ");
            } else {
                strcpy(yreading, "");
            }
            // Place the directions in one string
            strcat(yreading, xreading);
            pc.printf("%s\r\n", yreading);
            if (!strcmp(yreading, "")) { //If string is empty player is close
                const char *line1 = "You are close!";
                const char *line2 = "";
                speech(line1, line2);
            } else {
            // Print out direction in speech bubble
                const char *line1 = "The treasure is";
                speech(line1, yreading);
            }
            pc.printf("done scanning\r\n");
        }
        return FULL_DRAW;
    }
    
    
    return NO_RESULT;
 } 
 
void change_maze() {
    // Erased walls
    map_erase(16,20);
    for (int i=0;i<4;++i) {
        map_erase(1+i,20);  
        map_erase(15,21+i); 
    }
    
    
    // Added Walls
    add_wall(17,18,VERTICAL,2);
    add_wall(16,0,VERTICAL,19);
    add_breakable_wall(14,18);
    add_breakable_wall(15,18);
    add_wall(13,13,VERTICAL,6);
    add_wall(11,15,VERTICAL,5);
    add_wall(5,13,HORIZONTAL,8);
    add_wall(8,14,VERTICAL,4);
    add_wall(8,18,HORIZONTAL,2);
    add_wall(5,15,VERTICAL,5);
    add_wall(11,21,VERTICAL,1);
    add_wall(12,24,HORIZONTAL,6);
    add_wall(17,21,VERTICAL,3);
    
    // Added Snakes
    add_snake(12,19);
    add_snake(12,14);
    add_snake(10,16);
    add_snake(10,19);
    add_snake(7,19);
    add_snake(7,15);
    add_snake(7,14);
    add_snake(6,15);
    add_snake(6,14);
}

int move(int dx, int dy) {
    MapItem* item = get_here(Player.x + dx, Player.y + dy);
    int update_type = NO_RESULT;
    // If current position is on an item set full draw
    if (get_here(Player.x, Player.y)) update_type = FULL_DRAW;
    
    // Check if there is an item at desired location
    if (item) {
        // If it's walkable move there
        if (item->walkable || Player.omnipotent) {
            Player.y += dy;
            Player.x += dx;
            // Check if the item occludes the player. If not draw player.
            if (*(int*)item->data == OCCLUDE) {
                update_type = NO_RESULT;
            } else {
                update_type = FULL_DRAW;
            }
        }
    } else {
        // If no item move there
        Player.y += dy;
        Player.x += dx;
    }
    return update_type;
}

// Projectile direction definitions
#define PROJECTILE_RIGHT 1
#define PROJECTILE_DOWN 2
#define PROJECTILE_LEFT 3
#define PROJECTILE_UP 4
void add_projectile(int x, int y, int direction) {
    // Check if player already has thrown acorn before adding one and if there
    // is already an object here
    if (!acorns[0].direction && !get_here(x,y) && Player.has_acorns) {       
        acorns[0].x = x;
        acorns[0].y = y;
        acorns[0].direction = direction;
        pc.printf("has_acorns = %d\r\n",Player.has_acorns);
        // If the player has cluster throw, throw two more
        if (Player.has_acorns == 2) {
            //When throwing up or down add them to the right and left
            if (direction == PROJECTILE_UP || direction == PROJECTILE_DOWN) {
                if (!get_east(x,y)) {
                    acorns[1].x = x + 1;
                    acorns[1].y = y;
                    acorns[1].direction = direction;
                }
                if (!get_west(x,y)) {
                    acorns[2].x = x - 1;
                    acorns[2].y = y;
                    acorns[2].direction = direction;                    
                }
            // When throwing left or right add them to the top and bottom
            } else {
                if (!get_south(x,y)) {
                    acorns[1].x = x;
                    acorns[1].y = y + 1;
                    acorns[1].direction = direction;
                }
                if (!get_north(x,y)) {
                    acorns[2].x = x;
                    acorns[2].y = y - 1;
                    acorns[2].direction = direction;
                } 
            }
        }   
    }  
}

int throw_acorn() {
    GameInputs inputs = read_inputs();
    double deadZone = 0.3;
    // tilted left
    if (inputs.ax <= -deadZone) add_projectile(Player.x - 1, Player.y, PROJECTILE_LEFT);
     // tilted forward
    else if (inputs.ay >= deadZone) add_projectile(Player.x, Player.y - 1, PROJECTILE_UP);
     // tilted backward
    else if (inputs.ay <= -deadZone) add_projectile(Player.x, Player.y + 1, PROJECTILE_DOWN);
    // default is throw right
    else add_projectile(Player.x + 1, Player.y, PROJECTILE_RIGHT);
    
    return NO_RESULT;   
}

int move_acorn() {
    int update_type = NO_RESULT; // What will be returned by the function
    for (int i=0; i<3; ++i) {
        Acorn acorn = acorns[i];
        if (acorn.direction) { // Check for projectile first
            // Save previous position
            acorns[i].py = acorn.y;
            acorns[i].px = acorn.x;
            
            if (acorn.direction == PROJECTILE_RIGHT) {
                // If item to the right or out of range remove acorn
                MapItem* east = get_east(acorn.x,acorn.y);
                if(east || ((acorn.x - Player.x) >= 5)) {
                    // Destroy item if it hit a breakable item
                    if (*((int*)east->data)==BREAKABLE) map_erase(acorn.x + 1, acorn.y);
                    // Destroy acorn
                    acorns[i].x = acorns[i].y = acorns[i].direction = acorns[i].px = acorns[i].py = NULL;
                    update_type = FULL_DRAW; // Full draw to delete acorn
                } else { // If not move acorn to the right
                    acorns[i].x += 1;       
                }
            } else if (acorn.direction == PROJECTILE_LEFT) {
                // If item to the left or out of range remove acorn
                MapItem* west = get_west(acorn.x,acorn.y);
                if(west || ((Player.x - acorn.x) >= 5)) {
                    // Destroy item if it hit a breakable item
                    if (*((int*)west->data)==BREAKABLE) map_erase(acorn.x - 1, acorn.y);
                    // Destroy acorn
                    acorns[i].x = acorns[i].y = acorns[i].direction = acorns[i].px = acorns[i].py = NULL;
                    update_type = FULL_DRAW; // Full draw to delete acorn
                } else { // If not move acorn to the left
                    acorns[i].x -= 1;       
                }
            } else if (acorn.direction == PROJECTILE_UP) {
                // If item above or out of range remove acorn
                MapItem* north = get_north(acorn.x,acorn.y);
                if(north || ((Player.y - acorn.y) >= 4)) {
                    // Destroy item if it hit a breakable item
                    if (*(int*)(north->data)==BREAKABLE) map_erase(acorn.x, acorn.y - 1);
                    acorns[i].x = acorns[i].y = acorns[i].direction = acorns[i].px = acorns[i].py = NULL;
                    update_type =  FULL_DRAW; // Full draw to delete acorn
                } else { // If not move acorn up
                    acorns[i].y -= 1;       
                }    
            } else if (acorn.direction == PROJECTILE_DOWN) {
                // If item below or out of range remove acorn
                MapItem* south = get_south(acorn.x,acorn.y);
                if(south || ((acorn.y - Player.y) >= 4)) {
                    // Destroy item if it hit a breakable item
                    if (*((int*)south->data)==BREAKABLE) map_erase(acorn.x, acorn.y + 1);
                    // Destroy acorn
                    acorns[i].x = acorns[i].y = acorns[i].direction = acorns[i].px = acorns[i].py = NULL;
                    update_type = FULL_DRAW; // Full draw to delete acorn
                } else { // If not move acorn down
                    acorns[i].y += 1;       
                }     
            }
        } 
    }
    return update_type;
}

int check_harm() {
    // Check if the items around are harmful
    MapItem* adjacent_items[4] = {get_north(Player.x, Player.y),
                                  get_east(Player.x, Player.y),
                                  get_south(Player.x, Player.y),
                                  get_west(Player.x, Player.y)};
    for (int i=0; i<4; ++i) {
        if (adjacent_items[i]->type == SNAKE) {
            // Take a life away
            Player.lives--;
            // Reposition player
            // When the player is making it back to map 0
            if (Player.current_map && Player.has_sonar) {
                Player.x = 14;
                Player.y = 22;
            // When the player is making it to the sonar
            } else if (Player.current_map && !Player.has_sonar) {
                Player.x = 2;
                Player.y = 24;
            // When the player is in map 0    
            } else {
                Player.x = 5;
                Player.y = 5;
            }
            // So the player knows what happened
            wait_ms(500);
            // Check if this killed the player
            if (Player.lives < 1) {
                return GAME_OVER;
            } 
            pc.printf("Player Lives: %d\r\n", Player.lives);  
            return FULL_DRAW;
        }
    }
    return NO_RESULT;   
}

int update_game(int action)
{
    int update_type = NO_ACTION;
    
    // Save player previous location before updating
    Player.px = Player.x;
    Player.py = Player.y;
    
    // Check if the player died
    if (!Player.omnipotent) {
        update_type = check_harm();   
    }
    if (update_type == GAME_OVER) return GAME_OVER;
    
    // Used for the action button update
    int action_type;                     
    
    // Do different things based on the each action.
    // You can define functions like "go_up()" that get called for each case.
    switch(action)
    {
        // Return update_type at end.  If update_type was changed before
        // these cases that will take precedence over a case's update_type.
        case OMNIPOTENT:
            Player.omnipotent = !Player.omnipotent;
            wait_ms(200); // To prevent turning on and off in one push
            if (Player.omnipotent) {
                pc.printf("OMNIPOTENT ENABLED\r\n");
            } else pc.printf("OMNIPOTENT DISABLED\r\n");
            break;
        case THROW:
            update_type = throw_acorn() || update_type;
            return update_type;
        case GO_UP:
            update_type = move(0,-1) || update_type;
            break;
        case GO_LEFT:
            update_type = move(-1,0) || update_type;
            break;          
        case GO_DOWN:
            update_type = move(0,1) || update_type;
            break;
        case GO_RIGHT:
            update_type = move(1,0) || update_type;
            break;
        case ACTION_BUTTON:
            action_type = action_button();
            if (action_type == GAME_OVER) return action_type;
            update_type = update_type || action_type;
            break;
        case MENU_BUTTON:       
            break;
        default:       
            break;
    }
    // Move projectile
    update_type = move_acorn() || update_type;
    return update_type;
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
        
    // Draw projectiles
    for (int i=0; i<3; ++i) {
        Acorn acorn = acorns[i];
        if (acorn.direction) { // Check if there is projectile
            int w, z, pw, pz, u, v;
            // Compute distance from player
            w = acorn.x - Player.x;
            z = acorn.y - Player.y;
            // Compute u, v values
            u = (w+5)*11 + 3;
            v = (z+4)*11 + 15;
            // Draw the acorn if its on the screen
            if (w <= 5 && z <= 4) {
                draw_acorn(u, v);
            }
            
            // Erase last acorn from screen
            if (acorn.px) {
                // Compute distance from player
                pw = acorn.px - Player.x;
                pw += Player.x - Player.px; // Account for difference in player movement
                pz = acorn.py - Player.y;
                pz += Player.y - Player.py;
                // Don't erase if player and don't erase if it didnt move on screen
                if ((pw || pz) && !(w == pw && z == pz)) {
                    // Compute u, v values
                    u = (pw+5)*11 + 3;
                    v = (pz+4)*11 + 15;
                    // Erase last acorn
                    draw_nothing(u, v);
                }
            }
        }
    }
    
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
            if (init && i == 0 && j == 0) // Only draw the player on init
            {
                draw_player(u, v, Player.has_sonar);
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
                        draw = draw_nothing;
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
    draw_upper_status(Player.x, Player.y, Player.lives);
    draw_lower_status();
}


/**
 * Initialize the main world map. Add walls around the edges, interior chambers,
 * and plants in the background so you can see motion. Note: using the similar
 * procedure you can init the secondary map(s).
 */

void init_main_map()
{
    set_active_map(0);

    
    // Add NPCs
    
    add_npc(16,5,WISE_BEAR);
    add_npc(5,43,MASTER_SQUIRREL);
    
    
    add_tree(6,6);
    add_tree(8,6);
    add_tree(12,6);
    add_tree(14,6);
    add_tree(7,3);
    add_tree(9,3);
    add_tree(13,3);

    // Add fake treasure
    add_fake_treasure(25,25);
    add_fake_treasure(30,40);
    add_fake_treasure(20,10);
    print_map();
}

void init_second_map()
{
    set_active_map(1);    
    // Entrance walls
    add_wall(0,             20,              HORIZONTAL, map_width());
    add_wall(0,             28,              HORIZONTAL, 11);
    // 
    add_wall(11,            22,              VERTICAL,   19);
    add_wall(15,            21,              VERTICAL,   14);
    add_wall(12,            40,              HORIZONTAL, 14);
    add_wall(26,            28,              VERTICAL,   13);
    // Breakable wall
    for (int i = 0; i<10; ++i) {
        add_breakable_wall(16 + i, 28);
    }
  
    // Add ladder
    add_ladder(1, 24, 0, 19, 5);
    // Add sonar
    add_sonar(34,24);
    // Add snakes
    add_snake(34,23);
    add_snake(34,25);
    add_snake(33,24);
    add_snake(35,24);
    // Add NPCs
    add_npc(16,29,NPC_SQUIRREL1);
    add_npc(26,27,NPC_SQUIRREL2);
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
    pc.printf("starting\r\n");
    // First things first: initialize hardware
    ASSERT_P(hardware_init() == ERROR_NONE, "Hardware init failed!");

    // Initialize the maps
    maps_init();
    
    pc.printf("Map created\r\n");
    
    init_main_map();
    
    init_second_map();
    
    // Initialize game state
    set_active_map(0);
    Player.x = Player.y = 5;
    Player.lives = 3;
    
    // Draw the startscreen
    draw_startscreen();

    // Initial drawing
    draw_game(true);


    // Main game loop
    while(1)
    {
        // Timer to measure game update speed
        Timer t; t.start();
        
        // Actually do the game update:
        // 1. Read inputs  

        GameInputs inputs = read_inputs();
              
        // 2. Determine action (get_action)

        int action = get_action(inputs);
                
        // 3. Update game (update_game)

        int update_type = update_game(action);
        
        // 3b. Check for game over
        
        if (update_type == GAME_OVER) {
            break;
        }
        
        // 4. Draw frame (draw_game)

        draw_game(update_type);
        
        // 5. Frame delay
        t.stop();
        int dt = t.read_ms();
        if (dt < 100) wait_ms(100 - dt);
        //pc.printf("NewFrame\r\n");
    }
    draw_endscreen(Player.lives);
}
