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
    INV_EVIL_BOOK, //the ones that are hidden inside buildings, the shadow books
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
    int id;
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

#define NUM_ITEMS 14
#define NUM_TRACKED_ITEMS 4
#define MAX_CLOSE_ITEMS 32
Item map_items[NUM_ITEMS];
TrackedItem map_tracked_items[NUM_TRACKED_ITEMS];

int num_close_map_items = 0; //todo: if I need this...?
Item* close_map_items[MAX_CLOSE_ITEMS];

//fill close_map_items and set num_close_map_items, reset the spawn timers
void DocumentCloseItems(Donogan* d)
{
    num_close_map_items = 0;

    for (int i = 0; i < NUM_ITEMS; i++)
    {
        Item* it = &map_items[i];

        // Handle respawn timing on the SOURCE item
        if (it->collected)
        {
            if (it->respawnTimer.running && HasTimerElapsed(&it->respawnTimer)) {
                it->collected = false;
                ResetTimer(&it->respawnTimer);
            }
            else {
                continue; // still collected and not ready → skip
            }
        }

        // Proximity cull → add pointer to the close list
        if (Vector3Distance(d->pos, it->pos) < 500.0f)   // tune radius
        {
            if (num_close_map_items < MAX_CLOSE_ITEMS) {
                close_map_items[num_close_map_items++] = it;  // <-- store pointer
            }
            else {
                break;
            }
        }
    }
}


//for the ones that respawn
void ConsumeSimpleItems(Donogan* d)
{
    for (int k = 0; k < num_close_map_items; k++)
    {
        Item* it = close_map_items[k];            // pointer to SOURCE
        if (it->collected) continue;

        if (CheckCollisionBoxes(it->box, d->outerBox))
        {
            it->collected = true;             // mutate SOURCE
            inventory[it->type].count++;      // credit the right slot
            StartTimer(&it->respawnTimer);    // start SOURCE timer
            // TODO: Play pickup SFX, spawn VFX
        }
    }
}


//for the ones that do not respawn
void ConsumeTrackedItems(Donogan* d)
{
    for (int i = 0; i < NUM_TRACKED_ITEMS; i++)
    {
        if (map_tracked_items[i].collected) { continue; }
        if (CheckCollisionBoxes(map_tracked_items[i].box, d->outerBox))
        {
            map_tracked_items[i].collected = true;
            inventory[map_tracked_items[i].type].count++;
            //todo: play sound here
        }
    }
}

//Draw Items
void DrawItems(bool drawBoxes)
{
    for (int k = 0; k < num_close_map_items; k++)
    {
        Item* it = close_map_items[k];
        if (it->collected) continue;

        DrawModel(it->model, it->pos, it->scale, WHITE);
        if (drawBoxes) DrawBoundingBox(it->box, PURPLE);
    }
    for (int i = 0; i < NUM_TRACKED_ITEMS; i++)
    {
        if (map_tracked_items[i].collected) { continue; }
        DrawModel(map_tracked_items[i].model, map_tracked_items[i].pos, map_tracked_items[i].scale, WHITE);
        if (drawBoxes) { DrawBoundingBox(map_tracked_items[i].box, PINK); }
    }
}

//create
Item CreateRegularItem(Model model, Vector3 pos, InventoryType type, float scale)
{
    Item i = { 0 };
    //i.id = id;
    i.type = type;
    i.model = model;
    i.box = UpdateBoundingBox(GetModelBoundingBox(model),pos);
    i.pos = pos;
    i.scale = scale;
    i.collected = false;
    i.respawnTimer = CreateTimer(360);
    return i;
}
TrackedItem CreateTrackedItem(Model model, Vector3 pos, InventoryType type, float scale)
{
    TrackedItem i = { 0 };
    i.type = type;
    i.model = model;
    i.box = UpdateBoundingBox(GetModelBoundingBox(model), pos);
    i.pos = pos;
    i.scale = scale;
    i.collected = false;
    return i;
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
    inventory[INV_HEALTH_FULL] = (InventoryItem){ INV_HEALTH_FULL, "Health Pack (Full)", "gives full health", 0 };
    inventory[INV_POTION] = (InventoryItem){ INV_POTION, "Potion", "gives full mana", 0 };
    inventory[INV_BOOK] = (InventoryItem){ INV_BOOK, "Book", "collect these, they might be useful.", 0 };
    inventory[INV_EVIL_BOOK] = (InventoryItem){ INV_EVIL_BOOK, "Book of Shadows", "hmmm, one of the many book of shadows?", 0 };
    //setup map items
    ////for testing: 3022.00f, 322.00f, 4042.42f (use below as examples if needed)
    // /////NOTE: when you record a position with select, it records the floor, add 3 to all y positions, also label the tracked items as you go
    //full health pack
    int mi_init = 0; //this is a good idea, need to do this more often
    map_items[mi_init++] = CreateRegularItem(health_full_model, (Vector3) { 2968.16, 324.85, 4048.35 }, INV_HEALTH_FULL, 1);
    map_items[mi_init++] = CreateRegularItem(health_full_model, (Vector3) { 2968.16, 324.85, 4048.35 }, INV_HEALTH_FULL, 1);
    //health pack
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { 2914.42, 329.96, 4445.58 }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { 2643.51, 336.73, 4501.64 }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { 2643.43, 334.46, 4511.29 }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { 2639.51, 334.29, 4526.07 }, INV_HEALTH, 1);
    //mana
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { 2796.42, 332.17, 4537.03 }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { 2736.30, 334.34, 4511.80 }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { 2737.35, 337.66, 4501.95 }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { 2699.88, 334.29, 4502.28 }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { 2584.71, 587.29, 592.68 }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { 2583.28, 587.28, 602.32 }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { 2592.96, 587.28, 603.09 }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { 2591.27, 587.29, 594.96 }, INV_POTION, 1);//14 (with this method as it gets large I will need counters here anyway, but still I think less work to set up, I just need markers every now and again)
    ////setup tracked map items
    ////for testing: 3022.00f, 322.00f, 4042.42f
    //good book
    int mti_init = 0;
    map_tracked_items[mti_init++] = CreateTrackedItem(book_model, (Vector3) { 2170, 702, 950 }, INV_BOOK, 1); //castle
    map_tracked_items[mti_init++] = CreateTrackedItem(book_model, (Vector3) { 2688.86, 330.53, 4529.79 }, INV_BOOK, 1);//temple
    //book of shadows
    map_tracked_items[mti_init++] = CreateTrackedItem(evil_book_model, (Vector3) { 2126.62, 545.88, 834.43 }, INV_EVIL_BOOK, 1); //castle
    map_tracked_items[mti_init++] = CreateTrackedItem(evil_book_model, (Vector3) { 2711.45, 362.51, 4524.31 }, INV_EVIL_BOOK, 1); //temple
}

#endif // ITEMS_H
