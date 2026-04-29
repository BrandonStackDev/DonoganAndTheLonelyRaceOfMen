#ifndef MAPS_H
#define MAPS_H

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdbool.h>
#include <string.h>

#include "texture.h"
#include "core.h"
#include "donogan.h"
#include "game.h"
#include "jc.h"
#include "items.h"
#include "interact.h"
#include "npc.h"

//holders for important people
Vector3* map_tol;
Vector3* map_atreyu;
//Vector3* map_alister;
//Vector3* map_galadriel;
//Vector3* map_wolf;

typedef enum {
    MAP_HOMES = 0,
    MAP_WHALES,
    MAP_BOOKS,
    MAP_WINDMILL,
    MAP_STORES,
    MAP_FIRES,
    MAP_IMPORTANT,
    MAP_TOTAL_COUNT
} MapTypes;

typedef struct {
    bool collected;
    bool display;
    MapTypes type;
    Vector3 pos;
    BoundingBox box;
    float scale;
    char name[64];
} GameMap;

GameMap maps[MAP_TOTAL_COUNT];

Model mapModel = { 0 };
Texture2D mapTex = { 0 };

static inline GameMap CreateGameMap(const char* name, Vector3 pos, MapTypes type)
{
    GameMap m = { 0 };
    m.collected = false;
    m.display = false;
    m.type = type;
    m.pos = pos;
    m.scale = 1.0f;

    snprintf(m.name, sizeof(m.name), "%s", name ? name : "Map");

    BoundingBox base = GetModelBoundingBox(mapModel);
    base = ScaleBoundingBox(base, m.scale);
    m.box = UpdateBoundingBox(base, pos);

    return m;
}

static inline void Maps_SetOnlyDisplay(int idx)
{
    for (int i = 0; i < MAP_TOTAL_COUNT; i++) {
        maps[i].display = false;
    }

    if (idx >= 0 && idx < MAP_TOTAL_COUNT && maps[idx].collected) {
        maps[idx].display = true;
    }
}

static inline int Maps_GetDisplayed(void)
{
    for (int i = 0; i < MAP_TOTAL_COUNT; i++) {
        if (maps[i].display) return i;
    }
    return -1;
}

static inline void InitMaps(void)
{
    mapModel = LoadModel("models/map.obj");
    mapTex = LoadMyTexture("textures/map.png");

    if (mapModel.materialCount > 0) {
        mapModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = mapTex;
    }

    maps[MAP_HOMES] = CreateGameMap("Homes Map", (Vector3) { 2965.78, 324, 4036.69 }, MAP_HOMES);
    maps[MAP_WHALES] = CreateGameMap("Whales Map", (Vector3) { 2740.00f, 441.00f, 4110.71f }, MAP_WHALES);
    maps[MAP_BOOKS] = CreateGameMap("Books Map", (Vector3) { 2720.00f, 441.00f, 4100.71f }, MAP_BOOKS);
    maps[MAP_WINDMILL] = CreateGameMap("Windmills Map", (Vector3) { -603.88, 577.5, 3695.54 }, MAP_WINDMILL);
    maps[MAP_STORES] = CreateGameMap("Stores Map", (Vector3) { 2493.04, 341.52, 2422.65 }, MAP_STORES);
    maps[MAP_FIRES] = CreateGameMap("Fireplace Map", (Vector3) { -1636.03, 382.50, -1480.01 }, MAP_FIRES);
    maps[MAP_IMPORTANT] = CreateGameMap("Important People", (Vector3) { -3721.90, 329.53, 1108.00 }, MAP_IMPORTANT);
}

static inline void ConsumeMaps(Donogan* d)
{
    for (int i = 0; i < MAP_TOTAL_COUNT; i++) {
        if (maps[i].collected) continue;

        if (CheckCollisionBoxes(maps[i].box, d->outerBox)) {
            maps[i].collected = true;
            maps[i].display = false;

            toast = "Found a Map!";
            StartTimer(&toastTimer);
            PlaySoundVol(menuSaveOrLoad);
        }
    }
}

static inline void DrawUncollectedMaps(bool drawBoxes)
{
    for (int i = 0; i < MAP_TOTAL_COUNT; i++) {
        if (maps[i].collected) continue;

        DrawModel(mapModel, maps[i].pos, maps[i].scale, WHITE);
        if (drawBoxes) DrawBoundingBox(maps[i].box, SKYBLUE);
    }
}
static inline Vector2 MapWorldToScreen(Vector3 p, Rectangle dest)
{
    float nx = (p.x + (MAX_WORLD_SIZE / 2.0f)) / WORLD_WIDTH;
    float ny = (p.z + (MAX_WORLD_SIZE / 2.0f)) / WORLD_HEIGHT;

    return (Vector2) {
        dest.x + nx * dest.width,
            dest.y + ny * dest.height
    };
}

static inline void MapDrawWorldCircle(Vector3 p, Rectangle dest, Color c)
{
    DrawCircleV(MapWorldToScreen(p, dest), 3.0f, c);
}
static inline void DrawDisplayMaps(
    Rectangle dest,
    Donogan* d,
    Vector3 truckPos,
    Whale* whales,
    int numWhales
)
{
    int active = Maps_GetDisplayed();
    if (active < 0) return;

    // always draw Don + truck as base markers
    MapDrawWorldCircle(d->pos, dest, RED);
    MapDrawWorldCircle(truckPos, dest, BLUE);

    if (active == MAP_HOMES)
    {
        for (int i = 0; i < SCENE_TOTAL_COUNT; i++)
        {
            // skip windmills if you want homes only
            if (Scenes[i].modelType == MODEL_HOME_WINDMILL) continue;
            MapDrawWorldCircle(Scenes[i].pos, dest, GREEN);
        }
    }
    else if (active == MAP_WHALES)
    {
        for (int i = 0; i < numWhales; i++)
        {
            MapDrawWorldCircle(whales[i].pos, dest, PURPLE);
        }
    }
    else if (active == MAP_BOOKS)
    {
        for (int i = 0; i < NUM_TRACKED_ITEMS; i++)
        {
            if (map_tracked_items[i].collected) continue;

            if (map_tracked_items[i].type == INV_BOOK)
            {
                MapDrawWorldCircle(map_tracked_items[i].pos, dest, WHITE);
            }
            else if (map_tracked_items[i].type == INV_EVIL_BOOK)
            {
                MapDrawWorldCircle(map_tracked_items[i].pos, dest, (Color) {0,0,0,245});
            }
        }
    }
    else if (active == MAP_WINDMILL)
    {
        for (int i = 0; i < SCENE_TOTAL_COUNT; i++)
        {
            // skip windmills if you want homes only
            if (Scenes[i].modelType != MODEL_HOME_WINDMILL) { continue; }
            MapDrawWorldCircle(Scenes[i].pos, dest, Scenes[i].active ? MAGENTA: DARKBLUE);
        }
    }
    else if (active == MAP_STORES)
    {
        for (int i = 0; i < NPC_TOTAL; i++)
        {
            if (npcs[i].modelType != NPC_MODEL_TYPE_CLERK) { continue; }
            MapDrawWorldCircle(npcs[i].pos, dest, DARKPURPLE);
        }
    }
    else if (active == MAP_FIRES)
    {
        for (int i = 0; i < FIREPIT_TOTAL_COUNT; i++)
        {
            MapDrawWorldCircle(fires[i].pos, dest, fires[i].lit ? ORANGE : DARKGRAY);
        }
    }
    else if (active == MAP_IMPORTANT)
    {
        //Vector3* map_tol;
        //Vector3* map_atreyu;
        ////Vector3* map_alister;
        ////Vector3* map_galadriel;
        ////Vector3* map_wolf;
        /*
        *       Color tree_of_life = (Color){ 120, 255, 160, 255 };   // Tree of Life (bright living green, magical)
                Color atreyu = (Color){ 80, 180, 255, 255 };          // Atreyu (hero blue, calm + brave)
                Color wolf = (Color){ 200, 200, 220, 255 };           // Wolf (pale silver/ice tone)
                Color alistair = (Color){ 60, 60, 60, 255 };       // Alistair (bweare he is evil, run away!)
                Color galadriel = (Color){ 255, 240, 200, 255 };      // Galadriel (soft radiant light, ethereal)
        */
        MapDrawWorldCircle(*map_tol, dest, (Color) { 120, 255, 160, 255 });
        MapDrawWorldCircle(*map_atreyu, dest, (Color) { 80, 180, 255, 255 });
    }
}

#endif // MAPS_H