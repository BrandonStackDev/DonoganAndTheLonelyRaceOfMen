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
#include "texture.h"

// Type Definitions
typedef enum {
    //normal
    INV_HEALTH,  //small amount of health, like 20, common
    INV_HEALTH_FULL, //full health, all the way to max health, somewhat rare
    INV_POTION, //full mana, common
    INV_RX, // medicine
    INV_BERRY, //give 5 health, will be spawned from berries that grow on tree 2, if you press square, every pump, will spawn a berry on the closest tree 2 in the active list
    INV_APPLE, //gives +50 health, must be shot down with an arrow from a tree
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

#define NUM_ITEMS 98
#define NUM_TRACKED_ITEMS 20 //all books
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
        if (Vector3Distance(d->pos, it->pos) < 500)   // tune radius
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
            PlaySoundVol(menuSaveOrLoad);//todo: original sound for this
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
            PlaySoundVol(menuSaveOrLoad);//todo: original sound for this
            if (map_tracked_items[i].type == INV_BOOK) { toast = "Found an Ashroot Book!"; }
            else if (map_tracked_items[i].type == INV_EVIL_BOOK) { toast = "Found a Book of Shadows!"; }
            StartTimer(&toastTimer);
            d->xp += 200;
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
    Texture health_tex = LoadMyTexture("textures/health.png");
    health_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = health_tex;
    Model health_full_model = LoadModel("models/health_full.obj");
    Texture health_full_tex = LoadMyTexture("textures/health_full.png");
    health_full_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = health_full_tex;
    Model mana_model = LoadModel("models/mana.obj");
    Texture mana_tex = LoadMyTexture("textures/mana.png");
    mana_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = mana_tex;
    Model book_model = LoadModel("models/book.obj");
    Texture book_tex = LoadMyTexture("textures/book.png");
    book_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = book_tex;
    Model evil_book_model = LoadModel("models/shadow_book.obj");
    Texture evil_book_tex = LoadMyTexture("textures/shadow_book.png");
    evil_book_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = evil_book_tex;
    //setup inventory system
    inventory[INV_HEALTH] = (InventoryItem){ INV_HEALTH, "Health Pack", "gives +20 health", 0};
    inventory[INV_HEALTH_FULL] = (InventoryItem){ INV_HEALTH_FULL, "Health Pack (Full)", "gives full health", 0 };
    inventory[INV_POTION] = (InventoryItem){ INV_POTION, "Potion", "gives full mana", 0 };
    inventory[INV_RX] = (InventoryItem){ INV_RX, "Medicine", "gives full mana and full health", 0 };
    inventory[INV_BERRY] = (InventoryItem){ INV_BERRY, "Berry", "gives +5 mana", 0 };
    inventory[INV_APPLE] = (InventoryItem){ INV_APPLE, "Apple", "gives +30 health and +40 xp", 0 };
    inventory[INV_BOOK] = (InventoryItem){ INV_BOOK, "Ashroot Book", "collect these, they might be useful.", 0 };
    inventory[INV_EVIL_BOOK] = (InventoryItem){ INV_EVIL_BOOK, "Book of Shadows", "hmmm, one of the many book of shadows?", 0 };
    //setup map items
    ////for testing: 3022.00f, 322.00f, 4042.42f (use below as examples if needed)
    // /////NOTE: when you record a position with select, it records the floor, add 3 to all y positions, also label the tracked items as you go
    //full health pack
    int mi_init = 0; //this is a good idea, need to do this more often
    map_items[mi_init++] = CreateRegularItem(health_full_model, (Vector3) { 2968.16, 324.85, 4048.35 }, INV_HEALTH_FULL, 1); //home
    map_items[mi_init++] = CreateRegularItem(health_full_model, (Vector3) { 2587.84, 587.29, 595.96 }, INV_HEALTH_FULL, 1); //castle
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
    //cottage1
    map_items[mi_init++] = CreateRegularItem(health_full_model, (Vector3) { -1226.03, 389.0, 599.22 }, INV_HEALTH_FULL, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -1207.41, 389.0, 599.82 }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -1203.97, 389.0, 611.20 }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -1222.47, 389.0, 605.15 }, INV_POTION, 1); //18
    //tents
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -3750.05f, 333.29f, 1346.28f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -3754.93f, 333.29f, 1361.80f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -3877.11f, 329.53f, 1211.63f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -3887.78f, 329.53f, 1207.71f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -3883.85f, 329.53f, 1199.91f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -3893.04f, 329.53f, 1202.68f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -3853.93f, 329.53f, 952.32f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -3846.85f, 329.53f, 957.33f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -3842.94f, 329.53f, 962.97f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -3849.97f, 329.53f, 966.29f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_full_model, (Vector3) { -3855.96f, 329.53f, 960.88f }, INV_HEALTH_FULL, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -3848.36f, 329.53f, 956.02f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -3856.60f, 329.53f, 956.69f }, INV_HEALTH, 1); //31
    // new regular items
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { 2641.69f, 363.95f, 4565.79f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { 2643.12f, 364.09f, 4535.16f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { 2680.25f, 364.29f, 4534.53f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_full_model, (Vector3) { 2715.01f, 364.23f, 4590.86f }, INV_HEALTH_FULL, 1);

    // ruins / fields
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { 1905.53f, 376.12f, 4169.48f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { 1831.57f, 370.82f, 4166.53f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { 1843.82f, 376.40f, 4172.91f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { 1867.16f, 376.12f, 4175.48f }, INV_HEALTH, 1);

    // castle road
    map_items[mi_init++] = CreateRegularItem(health_full_model, (Vector3) { 2680.58f, 428.06f, 1845.82f }, INV_HEALTH_FULL, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { 2760.57f, 428.08f, 1885.17f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { 2784.73f, 420.05f, 1943.41f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { 2743.59f, 408.30f, 1985.24f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { 2692.07f, 378.49f, 2096.32f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { 2723.60f, 359.06f, 2226.51f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_full_model, (Vector3) { 2773.58f, 349.67f, 2371.67f }, INV_HEALTH_FULL, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { 2735.85f, 344.55f, 2399.11f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { 2597.55f, 337.06f, 2363.37f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { 2600.59f, 337.06f, 2336.21f }, INV_HEALTH, 1);

    // far west cluster
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -893.75f, 314.47f, -4749.62f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -881.72f, 314.47f, -4746.95f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -864.59f, 314.47f, -4742.98f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -865.60f, 314.47f, -4727.99f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -863.15f, 314.47f, -4714.33f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -875.21f, 314.47f, -4707.20f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_full_model, (Vector3) { -857.42f, 314.47f, -4697.05f }, INV_HEALTH_FULL, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -840.34f, 310.71f, -4685.58f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -834.58f, 310.71f, -4672.84f }, INV_POTION, 1);

    // cottage / cinder village
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -2652.91f, 397.29f, -2486.85f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -2679.09f, 395.70f, -2502.92f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_full_model, (Vector3) { -2298.85f, 434.94f, -2247.39f }, INV_HEALTH_FULL, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -2255.31f, 444.42f, -2215.83f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -2200.99f, 450.00f, -2230.66f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -2122.32f, 452.88f, -2362.40f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -2131.79f, 452.86f, -2363.35f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -2106.73f, 466.30f, -2198.76f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -2100.06f, 467.87f, -2206.46f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -2100.06f, 467.87f, -2206.46f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_full_model, (Vector3) { -2100.28f, 467.82f, -2195.87f }, INV_HEALTH_FULL, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -2100.28f, 467.82f, -2195.87f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -2102.99f, 467.18f, -2200.80f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -2102.99f, 467.18f, -2200.80f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_full_model, (Vector3) { -2264.63f, 485.66f, -1871.35f }, INV_HEALTH_FULL, 1);

    // windmill fields
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -2183.20f, 498.94f, -1777.71f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -2268.09f, 495.18f, -1745.97f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -2466.39f, 472.59f, -1796.39f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -2467.79f, 472.59f, -1803.16f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_full_model, (Vector3) { -2467.79f, 472.59f, -1803.16f }, INV_HEALTH_FULL, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -2464.22f, 472.59f, -1810.78f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -2464.22f, 472.59f, -1810.78f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -2466.58f, 468.82f, -1841.95f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_full_model, (Vector3) { -2373.90f, 456.22f, -2117.57f }, INV_HEALTH_FULL, 1);

    // cinder side / deep woods
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -2544.85f, 397.29f, -2469.84f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -2557.34f, 395.98f, -2485.58f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -2576.75f, 395.90f, -2485.93f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_full_model, (Vector3) { -2585.28f, 400.23f, -2498.08f }, INV_HEALTH_FULL, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -2573.31f, 400.23f, -2497.86f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -2560.23f, 400.23f, -2498.23f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -2499.26f, 332.59f, -2992.00f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -2502.32f, 328.09f, -3140.83f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_full_model, (Vector3) { -2495.87f, 325.82f, -3286.31f }, INV_HEALTH_FULL, 1);

    // yeti mountain
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -645.00f, 801.43f, 2842.42f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -642.80f, 804.17f, 2814.38f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -615.66f, 810.01f, 2789.96f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -602.24f, 811.41f, 2791.98f }, INV_HEALTH, 1);
    map_items[mi_init++] = CreateRegularItem(health_full_model, (Vector3) { -565.05f, 806.46f, 2828.97f }, INV_HEALTH_FULL, 1);
    map_items[mi_init++] = CreateRegularItem(mana_model, (Vector3) { -565.17f, 803.55f, 2849.40f }, INV_POTION, 1);
    map_items[mi_init++] = CreateRegularItem(health_model, (Vector3) { -576.66f, 796.82f, 2878.02f }, INV_HEALTH, 1); //98
    ////setup tracked map items
    ////for testing: 3022.00f, 322.00f, 4042.42f
    //good book
    int mti_init = 0;
    map_tracked_items[mti_init++] = CreateTrackedItem(book_model, (Vector3) { 2170, 702, 950 }, INV_BOOK, 1); //castle
    map_tracked_items[mti_init++] = CreateTrackedItem(book_model, (Vector3) { 2688.86, 330.53, 4529.79 }, INV_BOOK, 1);//temple on start island
    map_tracked_items[mti_init++] = CreateTrackedItem(book_model, (Vector3) { -2238.10, 336.00, 3441.12 }, INV_BOOK, 1);//greek white temple
    map_tracked_items[mti_init++] = CreateTrackedItem(book_model, (Vector3) { 585.12, 422, 1874.69 }, INV_BOOK, 1); //plats near barn/windmill
    map_tracked_items[mti_init++] = CreateTrackedItem(book_model, (Vector3) { -2400.32, 421.0, -2589.49 }, INV_BOOK, 1); //tree house by windmill
    map_tracked_items[mti_init++] = CreateTrackedItem(book_model, (Vector3) { 1899.63, 806, -455.70 }, INV_BOOK, 1); //pair
    map_tracked_items[mti_init++] = CreateTrackedItem(book_model, (Vector3) { 1144.80, 787, -1785.58 }, INV_BOOK, 1); //bridge
    map_tracked_items[mti_init++] = CreateTrackedItem(book_model, (Vector3) { -613.01, 801.58, 2833.29 }, INV_BOOK, 1); //yetimt
    map_tracked_items[mti_init++] = CreateTrackedItem(book_model, (Vector3) { -445.29, 570, -1161.96 }, INV_BOOK, 1); //cinderspire
    map_tracked_items[mti_init++] = CreateTrackedItem(book_model, (Vector3) { -2902.58, 1096, -542.52 }, INV_BOOK, 1); //yetimt2
    //book of shadows
    map_tracked_items[mti_init++] = CreateTrackedItem(evil_book_model, (Vector3) { 2126.62, 545.88, 834.43 }, INV_EVIL_BOOK, 1); //castle
    map_tracked_items[mti_init++] = CreateTrackedItem(evil_book_model, (Vector3) { 2711.45, 362.51, 4524.31 }, INV_EVIL_BOOK, 1); //temple on start island
    map_tracked_items[mti_init++] = CreateTrackedItem(evil_book_model, (Vector3) { -2301.45, 336.09, 3530.99 }, INV_EVIL_BOOK, 1); //greek white temple //, -2243.00, 336.00, 3468.78
    map_tracked_items[mti_init++] = CreateTrackedItem(evil_book_model, (Vector3) { 41.14, 906, 1715.81 }, INV_EVIL_BOOK, 1); //plats near barn/windmill
    map_tracked_items[mti_init++] = CreateTrackedItem(evil_book_model, (Vector3) { -2399.84, 421.0, -2579.47 }, INV_EVIL_BOOK, 1); //tree house by windmill
    map_tracked_items[mti_init++] = CreateTrackedItem(evil_book_model, (Vector3) { 1735.15, 806, -1186.69 }, INV_EVIL_BOOK, 1); //pair
    map_tracked_items[mti_init++] = CreateTrackedItem(evil_book_model, (Vector3) { 805.95, 882, -1838.85 }, INV_EVIL_BOOK, 1); //bridge
    map_tracked_items[mti_init++] = CreateTrackedItem(evil_book_model, (Vector3) { -590.53, 802.32, 2830.12 }, INV_EVIL_BOOK, 1); //yetimt //31
    map_tracked_items[mti_init++] = CreateTrackedItem(evil_book_model, (Vector3) { -443.00, 570, -1186.60 }, INV_EVIL_BOOK, 1); //cinderspire
    map_tracked_items[mti_init++] = CreateTrackedItem(evil_book_model, (Vector3) { -2909.07, 1096, -541.44 }, INV_EVIL_BOOK, 1); //yetimt2
}

#endif // ITEMS_H
