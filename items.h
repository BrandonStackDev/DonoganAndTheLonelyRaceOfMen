#ifndef ITEMS_H
#define ITEMS_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h> 
#include <stdbool.h>
#include "timer.h"
#include "donogan.h"

// Type Definitions
typedef enum {
    //normal
    INV_HEALTH,  //small amount of health, like 20, common
    INV_HEALTH_FULL, //full health, all the way to max health, somewhat rare
    INV_POTION, //full mana, common
    //special - all rare or special in someway
    //INV_CLOAK, //bad guys don't spawn for 120 seconds (rename this one)
    //INV_NO_DAMAGE, //bad guys cannot hurt you when this is used for 60 seconds
    //INV_MEDICINE, //for the Adriana mission
    //INV_FLAME_TONIC, //sets a timer for like 60 seconds, that we have flaming arrows, flaming arrows will explode on impact and do more damage, this will be hard
    //INV_BERRY, //give 5 health, will be spawned from berries that grow on tree 2, if you press square, every pump, will spawn a berry on the closest tree 2 in the active list
    //INV_FART_POWDER, //very rare, when consumed will play a fart sound and immediately kill all bad guys in a 100 foot radius. when in water, should also produce some bubbles
    //INV_KEY, //very rare, will only be like 5 max in the game, used to open locked things, the number of these should match the number of locked things (like buildings with doors and chests), the rule will be you need a key in inventory to open the thing, it consumes one key and then its permanently opened
    //INV_BOOK, most important item in the game, these are what you look for....should be like 10 - 20 on the map (todo: Alistair stuff also)
    //INV_EVIL_BOOK
    INV_TOTAL_TYPES,
} InventoryType;

typedef struct {
    InventoryType type;
    char* name;
    char* desc;
    int count;
} InventoryItem; //for items in inventory

InventoryItem inventory[INV_TOTAL_TYPES];

typedef struct {
    InventoryType type;
    Vector3 pos;
    BoundingBox box;
    float scale;
    Model model;
    bool collected;//this one means collected until the spawn timer expires
    Timer respawnTimer;
} Item; //for items on the map that respawn

typedef struct {
    InventoryType type;
    Vector3 pos;
    BoundingBox box;
    float scale;
    Model model;
    bool collected; //this one means collected forever
} TrackedItem; //for items on the map that do not respawn and are kept in the save file

#define NUM_ITEMS 3
#define NUM_TRACKED_ITEMS 1
Item map_items[NUM_ITEMS];
TrackedItem map_tracked_items[NUM_TRACKED_ITEMS];

int num_close_map_items = 0; //todo: if I need this...?
Item close_map_items[32];

//fill close_map_items and set num_close_map_items, reset the spawn timers
void DocumentCloseItems()
{
    for (int i = 0; i < NUM_ITEMS)
    {
        if (map_items[i].collected)
        {
            if (map_items[i].respawnTimer.running && HasTimerElapsed(&map_items[i].respawnTimer))
            {
                map_items[i].collected = false;
                ResetTimer(&map_items[i].respawnTimer);
            }
            else { continue; }
        }
        if (!map_items[i].collected)//probably redundant but good to have
        {

        }
    }
}

//for the ones that respawn
void ConsumeSimpleItems(Donogan *d)
{

}

//for the ones that do not respawn
void ConsumeTrackedItems(Donogan* d)
{

}

//Draw Items
void DrawItems()
{
    for (int i = 0; i < NUM_ITEMS)
    {
        DrawModel(map_items[i].model, map_items[i].pos, map_items[i].scale, WHITE);
    }
    for (int i = 0; i < NUM_TRACKED_ITEMS)
    {
        DrawModel(map_tracked_items[i].model, map_tracked_items[i].pos, map_tracked_items[i].scale, WHITE);
    }
}

//create
CreateRegularItem(Model model, Vector3 pos, InventoryType type, float scale)
{
    Item i = { 0 };
    i.type = type;
    i.model = model;
    i.box = UpdateBoundingBox(GetModelBoundingBox(model),pos);
    i.pos = pos;
    i.scale = scale;
    i.collected = false;
    i.respawnTimer = CreateTimer(360);
}
CreateTrackedItem(Model model, Vector3 pos, InventoryType type, float scale)
{
    Item i = { 0 };
    i.type = type;
    i.model = model;
    i.box = UpdateBoundingBox(GetModelBoundingBox(model), pos);
    i.pos = pos;
    i.scale = scale;
    i.collected = false;
}
//init all of the stuff
void InitItems()
{
    //for testing: 3022.00f, 322.00f, 4042.42f
}

#endif // ITEMS_H
