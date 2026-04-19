#ifndef PLATFORM_H
#define PLATFORM_H

// Header-only drop‑in for Donogan platforms.
// Types + helpers keep your original spirit but fix timers and collisions.

#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "timer.h"      // CreateTimer/StartTimer/HasTimerElapsed
#include "donogan.h"    // DonFeetWorldY/DonSnapToGround + Donogan fields
#include "collision.h"  // UpdateBoundingBox, CheckCollisionBoxes
#include "texture.h"

#define NUM_PLATS 56

// ------------------------------------------------------------
// Platform types/states
// ------------------------------------------------------------
typedef enum {
    PLATFORM_STILL = 0,
    PLATFORM_MOVER,
    PLATFORM_FALLER
} PlatformType;

typedef enum {
    MOVER_FWD = 0,
    MOVER_BACK,
    MOVER_WAIT_A,
    MOVER_WAIT_B
} MoverState;

// One mover per platform (optional when type!=MOVER)
typedef struct {
    MoverState state;
    Vector3 posA;           // endpoints
    Vector3 posB;
    Vector3 dir;            // current travel dir (unit)
    Vector3 oldPos;         // position last frame (for passenger delta)
    float   speed;          // meters / second
    bool    justSnapped;    // true only on the frame we clamp to endpoint
    Timer   t_wait;         // pause between trips
    bool clover; // do we need to stay exactly in sync
    int boundTo [3];
} Mover;

// Main platform object
typedef struct {
    PlatformType type;
    Vector3 pos, origPos;            // center position
    Vector3 dim;            // dimensions (x,y,z)
    BoundingBox origBox;    // unit box at origin for UpdateBoundingBox
    BoundingBox box;        // world box

    // Optional visuals (safe if you don't use/don't load)
    Model cubeModel;        // simple cube with texture already set
    Texture2D tex;          // keep a handle if you want to unload later
    Color color;

    // Mover payload (used if type==MOVER)
    Mover   mover;

    // Faller payload (used if type==FALLER)
    Timer   t_fallDelay;    // delay before falling once stepped on
    Timer   t_fellDelay;    // delay before falling once stepped on
    bool    falling;        // currently falling?
    float   vy;             // vertical velocity when falling
    bool disabled;

} Platform;

Platform plats[NUM_PLATS];
// ------------------------------------------------------------
// Tunables
// ------------------------------------------------------------
#ifndef PLATFORM_FALL_GRAVITY
#define PLATFORM_FALL_GRAVITY   (-40.0f)   // m/s^2
#endif
#ifndef PLATFORM_FALL_MAX_SPEED
#define PLATFORM_FALL_MAX_SPEED (-70.0f)
#endif
#ifndef PLATFORM_FALL_DELAY_SEC
#define PLATFORM_FALL_DELAY_SEC (0.9999f)     // seconds standing before it drops
#endif
#ifndef PLATFORM_WAIT_SEC
#define PLATFORM_WAIT_SEC       (1.2f)     // pause at A/B ends
#endif

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------
static inline BoundingBox BoxFromPlatformProps(Vector3 pos, Vector3 dim)
{
    BoundingBox b = {
        (Vector3) {
 pos.x - dim.x * 0.5f, pos.y - dim.y * 0.5f, pos.z - dim.z * 0.5f
},
(Vector3) {
pos.x + dim.x * 0.5f, pos.y + dim.y * 0.5f, pos.z + dim.z * 0.5f
}
    };
    return b;
}

static inline Model ModelFromPlatformDims(Vector3 dim, Texture2D texture)
{
    Mesh m = GenMeshCube(dim.x, dim.y, dim.z);
    Model mdl = LoadModelFromMesh(m);
    mdl.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    return mdl;
}

// ------------------------------------------------------------
// Constructors
// ------------------------------------------------------------
static inline Platform Platform_MakeStill(Vector3 pos, Vector3 dim, Texture2D tex, Color color)
{
    Platform p = { 0 };
    p.type = PLATFORM_STILL;
    p.pos = pos; p.dim = dim; p.color = color; p.tex = tex;
    p.origBox = BoxFromPlatformProps((Vector3) { 0, 0, 0 }, dim);
    p.box = UpdateBoundingBox(p.origBox, pos);
    p.cubeModel = ModelFromPlatformDims(dim, tex);
    return p;
}

static inline Mover Mover_Make(Vector3 posA, Vector3 posB, float speed)
{
    Vector3 d = Vector3Subtract(posB, posA);
    float L = Vector3Length(d); if (L < 1e-6f) L = 1.0f;
    Mover m = {0};
    m.clover = false;              // explicit
    m.boundTo[0] = m.boundTo[1] = m.boundTo[2] = -1;   // INVALID until you wire it up
    m.state = MOVER_FWD; m.posA = posA; m.posB = posB;
    m.dir = Vector3Scale(d, 1.0f / L); m.oldPos = posA; m.speed = fmaxf(speed, 0.0f);
    m.justSnapped = false; m.t_wait = CreateTimer(PLATFORM_WAIT_SEC); StartTimer(&m.t_wait); // start with a tiny settle if you want
    return m;
}

static inline Platform Platform_MakeMover(Vector3 posA, Vector3 posB, Vector3 dim, float speed, Texture2D tex, Color color)
{
    Platform p = { 0 };
    p.type = PLATFORM_MOVER; p.pos = posA; p.dim = dim; p.color = color; p.tex = tex;
    p.origBox = BoxFromPlatformProps((Vector3) { 0, 0, 0 }, dim);
    p.box = UpdateBoundingBox(p.origBox, posA);
    p.cubeModel = ModelFromPlatformDims(dim, tex);
    p.mover = Mover_Make(posA, posB, speed);
    return p;
}

static inline Platform Platform_MakeFaller(Vector3 pos, Vector3 dim, Texture2D tex, Color color)
{
    Platform p = Platform_MakeStill(pos, dim, tex, color);
    p.type = PLATFORM_FALLER;
    p.t_fallDelay = CreateTimer(PLATFORM_FALL_DELAY_SEC);
    p.t_fellDelay = CreateTimer(30);
    p.origPos = pos;
    p.falling = false; p.vy = 0.0f;
    return p;
}

// ------------------------------------------------------------
// Update
// ------------------------------------------------------------
static inline void Platform_UpdateMover( Platform* p, float dt, Platform* all)
{
    if (!p || p->type != PLATFORM_MOVER) return;

    p->mover.justSnapped = false;
    p->mover.oldPos = p->pos; // store world pos for passenger delta

    if (p->mover.state == MOVER_FWD || p->mover.state == MOVER_BACK)
    {
        // current target end
        Vector3 start = (p->mover.state == MOVER_FWD) ? p->mover.posA : p->mover.posB;
        Vector3 end = (p->mover.state == MOVER_FWD) ? p->mover.posB : p->mover.posA;
        Vector3 seg = Vector3Subtract(end, start);
        float segLen2 = Vector3DotProduct(seg, seg);

        // advance
        p->pos = Vector3Add(p->pos, Vector3Scale(p->mover.dir, p->mover.speed * dt));

        // how far past the end did we move?
        Vector3 moved = Vector3Subtract(p->pos, start);
        float movedDot = Vector3DotProduct(moved, seg);
        if (movedDot >= segLen2) {
            p->pos = end;                             // clamp
            p->mover.justSnapped = true;
            p->mover.state = (p->mover.state == MOVER_FWD) ? MOVER_WAIT_A : MOVER_WAIT_B;
            ResetTimer(&p->mover.t_wait); StartTimer(&p->mover.t_wait);
        }
    }
    else if (p->mover.state == MOVER_WAIT_A || p->mover.state == MOVER_WAIT_B)
    {
        bool unlocked = true;
        if (p->mover.clover && p->mover.state == MOVER_WAIT_A)
        {
            for (int i = 0; i < 3; i++) 
            { 
                MoverState s = all[p->mover.boundTo[i]].mover.state;
                if (s== MOVER_FWD)
                { 
                    unlocked = false; 
                } 
            } //lock to stay in sync, just once every now and aagain would be fine, so this wshould work...
        }
        if (unlocked && HasTimerElapsed(&p->mover.t_wait)) {
            // flip direction
            p->mover.state = (p->mover.state == MOVER_WAIT_A) ? MOVER_BACK : MOVER_FWD;
            Vector3 d = (p->mover.state == MOVER_FWD)
                ? Vector3Subtract(p->mover.posB, p->mover.posA)
                : Vector3Subtract(p->mover.posA, p->mover.posB);
            float L = Vector3Length(d); if (L < 1e-6f) L = 1.0f;
            p->mover.dir = Vector3Scale(d, 1.0f / L);
        }
    }

    // refresh world box
    p->box = UpdateBoundingBox(p->origBox, p->pos);
}

static inline void Platform_UpdateFaller(Platform* p, float dt)
{
    if (!p || p->type != PLATFORM_FALLER) return;

    // If falling, just integrate
    if (p->falling) {
        p->vy += PLATFORM_FALL_GRAVITY * dt;
        if (p->vy < PLATFORM_FALL_MAX_SPEED) p->vy = PLATFORM_FALL_MAX_SPEED;
        p->pos.y += p->vy * dt;

        // stop at terrain (simple ground)
        float gy = GetTerrainHeightFromMeshXZ(p->pos.x, p->pos.z);
        float bottom = p->pos.y - p->dim.y * 0.5f;
        if (bottom <= gy) {
            float offset = p->dim.y * 0.5f;
            p->pos.y = gy + offset; // rest on ground
            p->vy = 0.0f; p->falling = false; // done
            StartTimer(&p->t_fellDelay);
        }
        p->box = UpdateBoundingBox(p->origBox, p->pos);
        return;
    }

    // not falling – just keep box fresh
    p->box = UpdateBoundingBox(p->origBox, p->pos);
}

static inline void Platform_Update(Platform* p, float dt, Platform* all)
{
    if (!p) return;
    if (p->type == PLATFORM_MOVER) Platform_UpdateMover(p, dt, all);
    else if (p->type == PLATFORM_FALLER) Platform_UpdateFaller(p, dt);
    else p->box = UpdateBoundingBox(p->origBox, p->pos);
}

// ------------------------------------------------------------
// Collision (character ↔ top surface + ride movers)
// ------------------------------------------------------------
////static inline bool Platform_LandableTopOverlapXZ(const Platform* p, const Donogan* d)
////{
////    // Build a very thin slab on the top face so we ignore side hits.
////    const float slab = 0.2f; // meters of thickness used for the test
////    Vector3 topCenter = p->pos; topCenter.y += (p->dim.y * 0.5f) - (slab * 0.5f);
////    BoundingBox topBB = BoxFromPlatformProps(topCenter, (Vector3) { p->dim.x, slab, p->dim.z });
////    return CheckCollisionBoxes(d->outerBox, topBB);
////}
//static inline bool Overlap1D(float a0, float a1, float b0, float b1) {
//    return (a1 >= b0) && (b1 >= a0);
//}
//
//// Replace the whole function body:
//static inline bool Platform_LandableTopOverlapXZ(const Platform* p, const Donogan* d)
//{
//    const float skin = 0.05f; // small forgiveness for fast movers/jumps
//    return Overlap1D(d->outerBox.min.x, d->outerBox.max.x, p->box.min.x - skin, p->box.max.x + skin) &&
//        Overlap1D(d->outerBox.min.z, d->outerBox.max.z, p->box.min.z - skin, p->box.max.z + skin);
//}

//cool name, no idea what it does...
static inline void Platform_CollideAndRide(Platform* p, Donogan* d, float dt, Platform* all)
{
    if (!p || !d) return;
    if (p->disabled) { return; }
    if (HasTimerElapsed(&p->t_fellDelay))
    {
        ResetTimer(&p->t_fallDelay);//need this one too
        ResetTimer(&p->t_fellDelay);
        p->pos = p->origPos;
        p->falling = false;
    }

    // Update first so p->box and mover deltas are valid
    Platform_Update(p, dt, all);

    // Fast reject: AABB test against full box
    if (!CheckCollisionBoxes(d->outerBox, p->box)) return;

    // Consider only landings from above (feet cross the top plane)
    const float topY = p->box.max.y;

    // LAND: set Don’s ground to the platform top and snap.
    d->groundY = topY;       // DonSnapToGround uses groundY as feet plane
    DonSnapToGround(d);

    // If it’s a faller, arm the delay timer once we step on it
    if (p->type == PLATFORM_FALLER && !p->falling) {
        if (!p->t_fallDelay.running) { StartTimer(&p->t_fallDelay); }
        else if (HasTimerElapsed(&p->t_fallDelay)) { p->falling = true; p->vy = 0.0f; }
    }

    // If it’s a mover, carry Don by the platform delta this frame
    if (p->type == PLATFORM_MOVER) {
        Vector3 delta = Vector3Subtract(p->pos, p->mover.oldPos);
        d->pos = Vector3Add(d->pos, (Vector3) { delta.x, delta.y, delta.z });
        // keep feet stuck to the top after vertical moves
        d->groundY = topY; DonSnapToGround(d);
    }
}

// ------------------------------------------------------------
// Draw (optional helpers)
// ------------------------------------------------------------
static inline void Platform_Draw(const Platform* p, bool bb)
{
    if (!p) return;
    // If you loaded cubeModel, draw it; otherwise draw a simple cube
    if (p->cubeModel.meshCount > 0) {
        DrawModel(p->cubeModel, p->pos, 1.0f, WHITE);
    }
    else {
        DrawCubeV(p->pos, p->dim, p->color.a ? p->color : GRAY);
    }
    // Bounding box for debugging
    if (bb) { DrawBoundingBox(p->box, (Color) { 200, 255, 180, 180 }); }
}

void InitPlats()
{
    Texture tex_plat = LoadMyTexture("textures/wood2.png");
    Texture tex_fall = LoadMyTexture("textures/wood1.png");
    ////////////////////////////////////////CASTLE SECTION////////////////////////////////////////////////////////////////////////////////
    //testing 3022.00f, 322.00f, 4042.42f
    //plats[0] = Platform_MakeStill((Vector3) { 3022, 319, 4042 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    //plats[1] = Platform_MakeFaller((Vector3) { 3032, 319, 4052 }, (Vector3) { 6, 1, 6 }, tex_fall, WHITE);
    plats[0] = Platform_MakeMover((Vector3) { 2408, 509, 936 }, (Vector3) { 2408, 521, 936 }, (Vector3) { 6, 1, 6 }, 4.0f, tex_plat, WHITE);
    plats[1] = Platform_MakeMover((Vector3) { 2190, 525, 605 }, (Vector3) { 2191, 545, 604 }, (Vector3) { 6, 1, 6 }, 4.0f, tex_plat, WHITE);
    plats[1].disabled = true;
    plats[2] = Platform_MakeMover((Vector3) { 2200, 545, 605 }, (Vector3) { 2275, 545, 604 }, (Vector3) { 6, 1, 6 }, 4.0f, tex_plat, WHITE);
    plats[3] = Platform_MakeFaller((Vector3) { 2250, 545, 615 }, (Vector3) { 6, 1, 6 }, tex_fall, WHITE);
    plats[4] = Platform_MakeStill((Vector3) { 2255, 545, 625 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    plats[5] = Platform_MakeFaller((Vector3) { 2260, 545, 635 }, (Vector3) { 6, 1, 6 }, tex_fall, WHITE);
    //clover of death
    plats[6] = Platform_MakeMover((Vector3) { 2270, 545, 650 }, (Vector3) { 2270, 545, 700 }, (Vector3) { 10, 1, 10 }, 4.0f, tex_plat, WHITE);
    plats[7] = Platform_MakeMover((Vector3) { 2270, 545, 700 }, (Vector3) { 2320, 545, 700 }, (Vector3) { 10, 1, 10 }, 4.0f, tex_plat, WHITE);
    plats[8] = Platform_MakeMover((Vector3) { 2320, 545, 700 }, (Vector3) { 2320, 545, 650 }, (Vector3) { 10, 1, 10 }, 4.0f, tex_plat, WHITE);
    plats[9] = Platform_MakeMover((Vector3) { 2320, 545, 650 }, (Vector3) { 2270, 545, 650 }, (Vector3) { 10, 1, 10 }, 4.0f, tex_plat, WHITE);
    plats[6].mover.clover = true; plats[7].mover.clover = true; plats[8].mover.clover = true; plats[9].mover.clover = true;
    plats[6].mover.boundTo[0] = 7; plats[6].mover.boundTo[1] = 8; plats[6].mover.boundTo[2] = 9;
    plats[7].mover.boundTo[0] = 8; plats[7].mover.boundTo[1] = 9; plats[7].mover.boundTo[2] = 6;
    plats[8].mover.boundTo[0] = 9; plats[8].mover.boundTo[1] = 6; plats[8].mover.boundTo[2] = 7;
    plats[9].mover.boundTo[0] = 6; plats[9].mover.boundTo[1] = 7; plats[9].mover.boundTo[2] = 8;
    //pairs
    plats[10] = Platform_MakeFaller((Vector3) { 2330, 550, 700 }, (Vector3) { 6, 1, 6 }, tex_fall, WHITE);
    plats[11] = Platform_MakeStill((Vector3) { 2340, 555, 700 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    plats[12] = Platform_MakeFaller((Vector3) { 2340, 560, 710 }, (Vector3) { 6, 1, 6 }, tex_fall, WHITE);
    plats[13] = Platform_MakeStill((Vector3) { 2340, 565, 720 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    //end phase 1
    plats[14] = Platform_MakeStill((Vector3) { 2345, 570, 735 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    plats[15] = Platform_MakeStill((Vector3) { 2350, 575, 750 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    plats[16] = Platform_MakeStill((Vector3) { 2365, 575, 750 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    //plats[17] = Platform_MakeMover((Vector3) { 2365, 575, 760 }, (Vector3) { 2365, 510, 760 }, (Vector3) { 6, 1, 6 }, 4.0f, tex_plat, WHITE); //testing
    plats[17] = Platform_MakeMover((Vector3) { 2365, 520, 760 }, (Vector3) { 2365, 500, 760 }, (Vector3) { 6, 1, 6 }, 4.0f, tex_plat, WHITE); //for real
    //faller mover pair
    plats[18] = Platform_MakeFaller((Vector3) { 2400, 575, 925 }, (Vector3) { 6, 1, 6 }, tex_fall, WHITE);
    plats[19] = Platform_MakeMover((Vector3) { 2400, 575, 935 }, (Vector3) { 2400, 620, 935 }, (Vector3) { 6, 1, 6 }, 4.0f, tex_plat, WHITE);
    //phase 3, in which, Doris gets her oats
    plats[20] = Platform_MakeMover((Vector3) { 2170, 590, 950 }, (Vector3) { 2170, 700, 950 }, (Vector3) { 6, 1, 6 }, 8.0f, tex_plat, WHITE);
    //plats[21] = Platform_MakeMover((Vector3) { 2170, 590, 960 }, (Vector3) { 2100, 700, 960 }, (Vector3) { 6, 1, 6 }, 8.0f, tex_plat, WHITE);
    //plats[22] = Platform_MakeMover((Vector3) { 2110, 700, 960 }, (Vector3) { 2170, 800, 950 }, (Vector3) { 6, 1, 6 }, 8.0f, tex_plat, WHITE);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////NEAR WINDMILL BY BARN///////////////////////////////////////////////////////////////
    // area anchors:
    //      742, 323, 2038 -- near windmill by barn
    //      511, 444, 1755 -- up on the first hill
    //      -71, 926, 1669 -- near by mt peak
    // don_pos = 511.75, 444.50, 1756.01 //need to match

    // start lift at the barn
    plats[21] = Platform_MakeMover((Vector3) { 742, 327, 2038 }, (Vector3) { 742, 360, 2038 }, (Vector3) { 8, 1, 8 }, 12.0f, tex_plat, WHITE);
    plats[21].disabled = true;
    // path from first area to second area - tighter jumps
    plats[22] = Platform_MakeStill((Vector3) { 724, 366, 2012 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    plats[23] = Platform_MakeStill((Vector3) { 706, 372, 1988 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    plats[24] = Platform_MakeStill((Vector3) { 688, 378, 1964 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    plats[25] = Platform_MakeMover((Vector3) { 670, 384, 1940 }, (Vector3) { 670, 404, 1940 }, (Vector3) { 6, 1, 6 }, 8.0f, tex_plat, WHITE);
    plats[26] = Platform_MakeStill((Vector3) { 650, 398, 1916 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    plats[27] = Platform_MakeStill((Vector3) { 630, 404, 1892 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    plats[28] = Platform_MakeStill((Vector3) { 610, 412, 1868 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);

    // clover of death 1
    plats[29] = Platform_MakeMover((Vector3) { 585, 420, 1835 }, (Vector3) { 585, 420, 1875 }, (Vector3) { 10, 1, 10 }, 4.0f, tex_plat, WHITE);
    plats[30] = Platform_MakeMover((Vector3) { 585, 420, 1875 }, (Vector3) { 625, 420, 1875 }, (Vector3) { 10, 1, 10 }, 4.0f, tex_plat, WHITE);
    plats[31] = Platform_MakeMover((Vector3) { 625, 420, 1875 }, (Vector3) { 625, 420, 1835 }, (Vector3) { 10, 1, 10 }, 4.0f, tex_plat, WHITE);
    plats[32] = Platform_MakeMover((Vector3) { 625, 420, 1835 }, (Vector3) { 585, 420, 1835 }, (Vector3) { 10, 1, 10 }, 4.0f, tex_plat, WHITE);
    plats[29].mover.clover = true; plats[30].mover.clover = true; plats[31].mover.clover = true; plats[32].mover.clover = true;
    plats[29].mover.boundTo[0] = 30; plats[29].mover.boundTo[1] = 31; plats[29].mover.boundTo[2] = 32;
    plats[30].mover.boundTo[0] = 31; plats[30].mover.boundTo[1] = 32; plats[30].mover.boundTo[2] = 29;
    plats[31].mover.boundTo[0] = 32; plats[31].mover.boundTo[1] = 29; plats[31].mover.boundTo[2] = 30;
    plats[32].mover.boundTo[0] = 29; plats[32].mover.boundTo[1] = 30; plats[32].mover.boundTo[2] = 31;

    // approach first hill
    plats[33] = Platform_MakeStill((Vector3) { 565, 428, 1810 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    plats[34] = Platform_MakeStill((Vector3) { 545, 436, 1786 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    plats[35] = Platform_MakeMover((Vector3) { 525, 442, 1768 }, (Vector3) { 525, 455, 1768 }, (Vector3) { 6, 1, 6 }, 6.0f, tex_plat, WHITE);
    plats[36] = Platform_MakeMover((Vector3) { 511, 450, 1755 }, (Vector3) { 485, 486, 1751 }, (Vector3) { 8, 1, 8 }, 8.0f, tex_plat, WHITE);

    // path from second area to third area - tighter jumps
    plats[37] = Platform_MakeStill((Vector3) { 474, 486, 1748 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    plats[38] = Platform_MakeMover((Vector3) { 436, 530, 1742 }, (Vector3) { 436, 560, 1742 }, (Vector3) { 6, 1, 6 }, 8.0f, tex_plat, WHITE);
    plats[39] = Platform_MakeStill((Vector3) { 396, 576, 1736 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    plats[40] = Platform_MakeMover((Vector3) { 356, 612, 1730 }, (Vector3) { 320, 648, 1728 }, (Vector3) { 6, 1, 6 }, 6.0f, tex_plat, WHITE);

    // clover of death 2
    plats[41] = Platform_MakeMover((Vector3) { 314, 648, 1722 }, (Vector3) { 314, 648, 1762 }, (Vector3) { 10, 1, 10 }, 4.0f, tex_plat, WHITE);
    plats[42] = Platform_MakeMover((Vector3) { 314, 648, 1762 }, (Vector3) { 354, 648, 1762 }, (Vector3) { 10, 1, 10 }, 4.0f, tex_plat, WHITE);
    plats[43] = Platform_MakeMover((Vector3) { 354, 648, 1762 }, (Vector3) { 354, 648, 1722 }, (Vector3) { 10, 1, 10 }, 4.0f, tex_plat, WHITE);
    plats[44] = Platform_MakeMover((Vector3) { 354, 648, 1722 }, (Vector3) { 314, 648, 1722 }, (Vector3) { 10, 1, 10 }, 4.0f, tex_plat, WHITE);
    plats[41].mover.clover = true; plats[42].mover.clover = true; plats[43].mover.clover = true; plats[44].mover.clover = true;
    plats[41].mover.boundTo[0] = 42; plats[41].mover.boundTo[1] = 43; plats[41].mover.boundTo[2] = 44;
    plats[42].mover.boundTo[0] = 43; plats[42].mover.boundTo[1] = 44; plats[42].mover.boundTo[2] = 41;
    plats[43].mover.boundTo[0] = 44; plats[43].mover.boundTo[1] = 41; plats[43].mover.boundTo[2] = 42;
    plats[44].mover.boundTo[0] = 41; plats[44].mover.boundTo[1] = 42; plats[44].mover.boundTo[2] = 43;

    // approach peak
    plats[45] = Platform_MakeStill((Vector3) { 274, 684, 1716 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    plats[46] = Platform_MakeStill((Vector3) { 232, 724, 1710 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    plats[47] = Platform_MakeMover((Vector3) { 188, 764, 1704 }, (Vector3) { 188, 804, 1704 }, (Vector3) { 6, 1, 6 }, 8.0f, tex_plat, WHITE);
    plats[48] = Platform_MakeStill((Vector3) { 142, 816, 1698 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    plats[49] = Platform_MakeStill((Vector3) { 96, 868, 1692 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);

    // clover of death 3
    plats[50] = Platform_MakeMover((Vector3) { 42, 904, 1676 }, (Vector3) { 42, 904, 1716 }, (Vector3) { 10, 1, 10 }, 4.0f, tex_plat, WHITE);
    plats[51] = Platform_MakeMover((Vector3) { 42, 904, 1716 }, (Vector3) { 82, 904, 1716 }, (Vector3) { 10, 1, 10 }, 4.0f, tex_plat, WHITE);
    plats[52] = Platform_MakeMover((Vector3) { 82, 904, 1716 }, (Vector3) { 82, 904, 1676 }, (Vector3) { 10, 1, 10 }, 4.0f, tex_plat, WHITE);
    plats[53] = Platform_MakeMover((Vector3) { 82, 904, 1676 }, (Vector3) { 42, 904, 1676 }, (Vector3) { 10, 1, 10 }, 4.0f, tex_plat, WHITE);
    plats[50].mover.clover = true; plats[51].mover.clover = true; plats[52].mover.clover = true; plats[53].mover.clover = true;
    plats[50].mover.boundTo[0] = 51; plats[50].mover.boundTo[1] = 52; plats[50].mover.boundTo[2] = 53;
    plats[51].mover.boundTo[0] = 52; plats[51].mover.boundTo[1] = 53; plats[51].mover.boundTo[2] = 50;
    plats[52].mover.boundTo[0] = 53; plats[52].mover.boundTo[1] = 50; plats[52].mover.boundTo[2] = 51;
    plats[53].mover.boundTo[0] = 50; plats[53].mover.boundTo[1] = 51; plats[53].mover.boundTo[2] = 52;

    plats[54] = Platform_MakeStill((Vector3) { -8, 918, 1672 }, (Vector3) { 6, 1, 6 }, tex_plat, WHITE);
    plats[55] = Platform_MakeStill((Vector3) { -71, 926, 1669 }, (Vector3) { 8, 1, 8 }, tex_plat, WHITE);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif // PLATFORM_H
