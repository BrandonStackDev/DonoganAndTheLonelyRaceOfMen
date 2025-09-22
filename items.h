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
    INV_BOOK, //most important item in the game, these are what you look for....should be like 10 - 20 on the map (todo: Alistair stuff also)
    INV_EVIL_BOOK //the ones that are hidden inside buildings, the shadow books
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
#define NUM_TRACKED_ITEMS 2
#define MAX_CLOSE_ITEMS 32
Item map_items[NUM_ITEMS];
TrackedItem map_tracked_items[NUM_TRACKED_ITEMS];

int num_close_map_items = 0; //todo: if I need this...?
Item close_map_items[MAX_CLOSE_ITEMS];

//fill close_map_items and set num_close_map_items, reset the spawn timers
void DocumentCloseItems(Donogan* d)
{
    num_close_map_items = 0;
    for (int i = 0; i < NUM_ITEMS && num_close_map_items < MAX_CLOSE_ITEMS; i++)
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
        if (!map_items[i].collected && Vector3Distance(d->pos, map_items[i].pos)<500)//probably redundant but good to have
        {
            close_map_items[num_close_map_items] = map_items[i];
            num_close_map_items++;
        }
    }
}

//for the ones that respawn
void ConsumeSimpleItems(Donogan *d)
{
    for (int i = 0; i < num_close_map_items; i++)
    {
    }
}

//for the ones that do not respawn
void ConsumeTrackedItems(Donogan* d)
{
    for (int i = 0; i < NUM_TRACKED_ITEMS; i++)
    {
    }
}

//Draw Items
void DrawItems()
{
    for (int i = 0; i < num_close_map_items; i++)
    {
        if (map_items[i].collected) { continue; }
        DrawModel(close_map_items[i].model, close_map_items[i].pos, close_map_items[i].scale, WHITE);
    }
    for (int i = 0; i < NUM_TRACKED_ITEMS; i++)
    {
        if (map_tracked_items[i].collected) { continue; }
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
    //models
    Model health_model = LoadModel("models/health.obj");
    Texture health_tex = LoadTexture("textures/health.png");
    health_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = health_tex;
    Model health_full_model = LoadModel("models/health_full.obj");
    Texture health_full_tex = LoadTexture("textures/health_full.png");
    health_full_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = health_full_tex;
    Model mana_model = LoadModel("models/mana.obj");
    Texture mana_tex = LoadTexture("textures/mana.png");
    mana_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = mana_tex;
    Model book_model = LoadModel("models/book.obj");
    Texture book_tex = LoadTexture("textures/book.png");
    book_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = book_tex;
    Model evil_book_model = LoadModel("models/shadow_book.obj");
    Texture evil_book_tex = LoadTexture("textures/shadow_book.png");
    evil_book_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = evil_book_tex;
    //setup inventory system
    inventory[INV_HEALTH] = (InventoryItem){ INV_HEALTH, "Health Pack", "gives 20 health", 0};
    inventory[INV_HEALTH_FULL] = (InventoryItem){ INV_HEALTH, "Health Pack (Full)", "gives full health", 0 };
    inventory[INV_POTION] = (InventoryItem){ INV_HEALTH, "Potion", "gives full mana", 0 };
    inventory[INV_BOOK] = (InventoryItem){ INV_HEALTH, "Book", "collect these, they might be useful.", 0 };
    inventory[INV_EVIL_BOOK] = (InventoryItem){ INV_HEALTH, "Book of Shadows", "hmmm, one of the many book of shadows?", 0 };
    //setup map items
    //for testing: 3022.00f, 322.00f, 4042.42f
    CreateRegularItem(health_model, (Vector3) { 3020, 322, 4040 }, INV_HEALTH, 1);
    CreateRegularItem(health_model, (Vector3) { 3025, 322, 4045 }, INV_HEALTH_FULL, 1);
    CreateRegularItem(health_model, (Vector3) { 3030, 322, 4050 }, INV_POTION, 1);
    //setup tracked map items
    //for testing: 3022.00f, 322.00f, 4042.42f
    CreateTrackedItem(health_model, (Vector3) { 3010, 322, 4010 }, INV_BOOK, 1);
    CreateTrackedItem(health_model, (Vector3) { 3015, 322, 4015 }, INV_EVIL_BOOK, 1);
}

#endif // ITEMS_H
