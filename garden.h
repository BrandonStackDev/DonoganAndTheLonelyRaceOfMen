#ifndef GARDEN_H
#define GARDEN_H

#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>

#include "texture.h"
#include "donogan.h"
#include "bg.h"
#include "frustum.h"

#define MAX_GARDENS 64
#define GARDEN_INTERACT_DIST 16.0f
#define GARDEN_DRAW_DIST 650

typedef enum {
    GARDEN_WILD = 0,
    GARDEN_HOME
} GardenType;

typedef struct {
    bool active;
    GardenType type;

    Vector3 pos;
    float yaw;
    float scale;

    bool grown;

    // for commandeered hopper
    int commandeeredBg;
    Vector3 oldSpawnPoint;
    float oldSpawnRadius;
    float oldAwareRadius;
    float oldTetherRadius;

    Color flowerColor;
    Color leafColor;
} Garden;

static Garden gGardens[MAX_GARDENS];
static int gGardenCount = 0;

static Model gGardenHomeModel = { 0 };
static Texture2D gGardenHomeTex = { 0 };

static Model gGardenWildModel = { 0 };
static Texture2D gGardenWildTex = { 0 };

static bool gGardenPrevSquare = false;

static inline Color Garden_RandFlowerColor(void)
{
    Color c[] = {
        PINK, RED, PURPLE, BLUE, YELLOW,
        (Color) {
255, 80, 180, 255
},
(Color) {
180, 80, 255, 255
},
(Color) {
80, 140, 255, 255
}
    };
    return c[GetRandomValue(0, 7)];
}

static inline Color Garden_RandLeafColor(void)
{
    Color c[] = {
        GREEN, LIME, DARKGREEN,
        (Color) {
60, 160, 70, 255
},
(Color) {
30, 120, 50, 255
}
    };
    return c[GetRandomValue(0, 4)];
}

static inline Vector3 Garden_GroundSnap(Vector3 p)
{
    float gy = GetTerrainHeightFromMeshXZ(p.x, p.z);
    if (gy > -9000) { p.y = gy+0.7; } //lil' offset (my rap name)
    return p;
}

static inline void Garden_Add(GardenType type, Vector3 pos, float yaw, float scale)
{
    if (gGardenCount >= MAX_GARDENS) return;

    Garden* g = &gGardens[gGardenCount++];
    *g = (Garden){ 0 };

    g->active = true;
    g->type = type;
    g->pos = Garden_GroundSnap(pos);
    if (g->type == GARDEN_HOME)
    {
        g->pos.y += 0.4; //these guys up just a bit more
    }
    g->yaw = yaw;
    g->scale = scale;
    g->grown = false;
    g->commandeeredBg = -1;

    g->flowerColor = Garden_RandFlowerColor();
    g->leafColor = Garden_RandLeafColor();
}

static inline void Garden_Init(void)
{
    gGardenCount = 0;

    gGardenHomeModel = LoadModel("models/garden.obj");
    gGardenHomeTex = LoadMyTexture("textures/garden.png");
    if (gGardenHomeModel.materialCount > 0) {
        gGardenHomeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = gGardenHomeTex;
    }

    gGardenWildModel = LoadModel("models/wild.obj");
    gGardenWildTex = LoadMyTexture("textures/wild.png");
    if (gGardenWildModel.materialCount > 0) {
        gGardenWildModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = gGardenWildTex;
    }
    // HOME gardens: outside houses / farms
    Garden_Add(GARDEN_HOME, (Vector3) { -2149.87f, 439.09f, -2363.05f }, 0, 4.0f);
    Garden_Add(GARDEN_HOME, (Vector3) { -2122.53f, 445.52f, -2349.19f }, 0, 4.0f);
    Garden_Add(GARDEN_HOME, (Vector3) { -2097.77f, 444.89f, -2379.44f }, 0, 4.0f);
    Garden_Add(GARDEN_HOME, (Vector3) { -2104.81f, 442.16f, -2400.18f }, 0, 4.0f);
    Garden_Add(GARDEN_HOME, (Vector3) { -2126.55f, 440.34f, -2402.00f }, 0, 4.0f);
    Garden_Add(GARDEN_HOME, (Vector3) { -2069.41f, 466.82f, -2234.02f }, 0, 4.0f);
    Garden_Add(GARDEN_HOME, (Vector3) { -2069.88f, 470.59f, -2193.36f }, 0, 4.0f);

    // WILD gardens: near foliage
    Garden_Add(GARDEN_WILD, (Vector3) { 1431.01f, 350.12f, -3297.20f }, 0, 3.0f);
    Garden_Add(GARDEN_WILD, (Vector3) { 1463.42f, 355.63f, -3289.28f }, 0, 3.0f);
    Garden_Add(GARDEN_WILD, (Vector3) { 1481.17f, 361.41f, -3266.42f }, 0, 3.0f);
    Garden_Add(GARDEN_WILD, (Vector3) { 1524.64f, 365.18f, -3295.97f }, 0, 3.0f);
    Garden_Add(GARDEN_WILD, (Vector3) { 1564.51f, 368.94f, -3301.56f }, 0, 3.0f);
    Garden_Add(GARDEN_WILD, (Vector3) { 1595.97f, 372.71f, -3301.52f }, 0, 3.0f);
    Garden_Add(GARDEN_WILD, (Vector3) { 1593.21f, 374.34f, -3289.06f }, 0, 3.0f);
    Garden_Add(GARDEN_WILD, (Vector3) { 1587.74f, 376.47f, -3272.20f }, 0, 3.0f);
    Garden_Add(GARDEN_WILD, (Vector3) { 1595.70f, 384.00f, -3229.90f }, 0, 3.0f);
    Garden_Add(GARDEN_WILD, (Vector3) { 1586.67f, 391.53f, -3172.58f }, 0, 3.0f);
    Garden_Add(GARDEN_WILD, (Vector3) { 1591.08f, 395.29f, -3151.57f }, 0, 3.0f);
    Garden_Add(GARDEN_WILD, (Vector3) { 1655.96f, 402.82f, -3146.91f }, 0, 3.0f);
}

static inline int Garden_FindFreeBorrower(BadGuyType type)
{
    for (int i = 0; i < total_bg_models_all_types; i++)
    {
        if (bgModelBorrower[i].type != type) continue;
        if (bgModelBorrower[i].isInUse) continue;
        return i;
    }
    return -1;
}

static inline void Garden_AddActiveBgIndex(int bgIndex)
{
    for (int i = 0; i < act_bg_count; i++) {
        if (act_bg[i] == bgIndex) return;
    }

    int cap = MAX_BG_PER_TYPE_AT_ONCE * BG_TYPE_COUNT;
    if (act_bg_count < cap) {
        act_bg[act_bg_count++] = bgIndex;
    }
}

static inline bool Garden_CommandeerHopper(Garden* g, Donogan* d)
{
    if (!g || !d || !bg || !bgModelBorrower) return false;

    for (int i = 0; i < bg_count; i++)
    {
        BadGuy* b = &bg[i];

        if (b->type != BG_PUMPKIN_HOPPER) continue;

        // Steal any hopper that is not currently active.
        // Do NOT require dead == true; sleeping/inactive hoppers may be dead=false.
        if (b->active) continue;

        int borrow = Garden_FindFreeBorrower(BG_PUMPKIN_HOPPER);
        if (borrow < 0) {
            TraceLog(LOG_WARNING, "Garden_CommandeerHopper: no free hopper borrower");
            return false;
        }

        Vector3 spawn = g->pos;

        // save old spawn/tether rules
        g->commandeeredBg = i;
        g->oldSpawnPoint = b->spawnPoint;
        g->oldSpawnRadius = b->spawnRadius;
        g->oldAwareRadius = b->awareRadius;
        g->oldTetherRadius = b->tetherRadius;

        bgModelBorrower[borrow].isInUse = true;

        b->gbm_index = borrow;
        BG_AttachBorrowed(b);

        b->active = true;
        b->dead = false;
        b->aware = true;
        b->frozen = false;
        b->throwing = false;
        b->bounced = false;
        b->onPlatform = false;

        b->spawnPoint = spawn;
        b->pos = spawn;
        b->targetPos = spawn;
        b->groundY = spawn.y;

        b->spawnRadius = 220;
        b->awareRadius = 999.0f;
        b->tetherRadius = 140;

        b->scale = 0.001;

        b->health = b->startHealth;
        if (b->health <= 0) b->health = 20;

        b->speed = 18.0f;
        b->vel = (Vector3){ 0 };
        b->drawColor = WHITE;

        b->state = HOPPER_STATE_WAIT;
        ResetTimer(&b->interactionTimer);
        StartTimer(&b->interactionTimer);

        Garden_AddActiveBgIndex(i);

        d->xp += 5;

        TraceLog(LOG_INFO, "Garden spawned hopper bg[%d] at %.2f %.2f %.2f", i, spawn.x, spawn.y, spawn.z);
        return true;
    }

    TraceLog(LOG_WARNING, "Garden_CommandeerHopper: no inactive hopper found in bg[]");
    return false;
}

static inline void Garden_RestoreCommandeeredHopper(Garden* g)
{
    if (!g || g->commandeeredBg < 0) return;

    BadGuy* b = &bg[g->commandeeredBg];

    // Wait until the hopper is gone again.
    if (b->active && !b->dead) return;

    b->spawnPoint = g->oldSpawnPoint;
    b->spawnRadius = g->oldSpawnRadius;
    b->awareRadius = g->oldAwareRadius;
    b->tetherRadius = g->oldTetherRadius;

    g->commandeeredBg = -1;
}

static inline void Garden_Update(Donogan* d, bool squareDown)
{
    if (!d) return;

    bool squarePressed = squareDown && !gGardenPrevSquare;
    gGardenPrevSquare = squareDown;

    for (int i = 0; i < gGardenCount; i++)
    {
        Garden* g = &gGardens[i];
        if (!g->active) continue;

        Garden_RestoreCommandeeredHopper(g);

        if (!squarePressed) continue;
        if (g->grown) continue;

        if (Vector3DistanceSqr(d->pos, g->pos) > GARDEN_INTERACT_DIST * GARDEN_INTERACT_DIST) continue;

        if (g->type == GARDEN_HOME)
        {
            if (Garden_CommandeerHopper(g, d))
            {
                g->grown = true;
                toast = "A pumpkin hopper grew!";
                StartTimer(&toastTimer);
            }
        }
        else if (g->type == GARDEN_WILD)
        {
            g->grown = true;
            d->xp += 5;
            toast = "A wild flower bloomed!";
            StartTimer(&toastTimer);
        }
    }
}

static inline void Garden_DrawFlower(const Garden* g)
{
    Vector3 base = g->pos;
    base.y -= 2;
    float h = 2.0f;
    float topY = base.y + (h+1.2);

    DrawCylinder(
        (Vector3) {
        base.x, base.y + h * 0.5f, base.z
    },
        0.035f, 0.055f, h,
        6,
        g->leafColor
    );

    // leaves: simple green triangles
    DrawTriangle3D(
        (Vector3) {
        base.x, base.y + 0.65f, base.z
    },
        (Vector3) {
        base.x + 0.75f, base.y + 0.95f, base.z + 0.10f
    },
        (Vector3) {
        base.x + 0.12f, base.y + 0.45f, base.z + 0.10f
    },
        g->leafColor
    );

    DrawTriangle3D(
        (Vector3) {
        base.x, base.y + 0.95f, base.z
    },
        (Vector3) {
        base.x - 0.70f, base.y + 1.20f, base.z - 0.10f
    },
        (Vector3) {
        base.x - 0.10f, base.y + 0.75f, base.z - 0.10f
    },
        g->leafColor
    );

    Vector3 center = (Vector3){ base.x, topY, base.z };

    DrawSphere(center, 0.12f, YELLOW);

    // polygon petals around the center
    rlDisableBackfaceCulling();
    const int petals = 8;
    for (int p = 0; p < petals; p++)
    {
        float a0 = ((float)p / petals) * PI * 2.0f;
        float a1 = ((float)(p + 1) / petals) * PI * 2.0f;
        float amid = (a0 + a1) * 0.5f;

        float inner = 0.14f;
        float outer = 0.48f;

        Vector3 v0 = {
            center.x + cosf(a0) * inner,
            center.y,
            center.z + sinf(a0) * inner
        };

        Vector3 v1 = {
            center.x + cosf(a1) * inner,
            center.y,
            center.z + sinf(a1) * inner
        };

        Vector3 tip = {
            center.x + cosf(amid) * outer,
            center.y + 0.06f,
            center.z + sinf(amid) * outer
        };

        Color pc = g->flowerColor;
        pc.r = (unsigned char)Clamp(pc.r + GetRandomValue(-10, 10), 0, 255);
        pc.g = (unsigned char)Clamp(pc.g + GetRandomValue(-10, 10), 0, 255);
        pc.b = (unsigned char)Clamp(pc.b + GetRandomValue(-10, 10), 0, 255);
        DrawTriangle3D(v0, tip, v1, pc);
        DrawTriangle3D(v1, tip, v0, pc); // reverse side
        
    }
    rlEnableBackfaceCulling();
}

static inline void Garden_Draw(Donogan* d, Frustum frustum)
{
    for (int i = 0; i < gGardenCount; i++)
    {
        Garden* g = &gGardens[i];
        if (!g->active) continue;

        if(IsPointInFrustum(g->pos, frustum))
        if (d && Vector3DistanceSqr(d->pos, g->pos) > GARDEN_DRAW_DIST * GARDEN_DRAW_DIST) continue;

        Model* m = (g->type == GARDEN_HOME) ? &gGardenHomeModel : &gGardenWildModel;

        DrawModelEx(
            *m,
            g->pos,
            (Vector3) {0, 1, 0},
            g->yaw* RAD2DEG,
            (Vector3) {g->scale, g->scale, g->scale},
            WHITE
        );

        if (g->type == GARDEN_WILD && g->grown)
        {
            Garden_DrawFlower(g);
        }
    }
}

#endif