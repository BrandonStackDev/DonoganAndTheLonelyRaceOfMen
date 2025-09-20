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

#define NUM_PLATS 21
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

} Platform;

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
    Mover m; m.state = MOVER_FWD; m.posA = posA; m.posB = posB;
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
static inline void Platform_UpdateMover(Platform* p, float dt)
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
        if (HasTimerElapsed(&p->mover.t_wait)) {
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

    if (HasTimerElapsed(&p->t_fellDelay))
    {
        ResetTimer(&p->t_fellDelay);
        p->pos = p->origPos;
    }
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

static inline void Platform_Update(Platform* p, float dt)
{
    if (!p) return;
    if (p->type == PLATFORM_MOVER) Platform_UpdateMover(p, dt);
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
static inline void Platform_CollideAndRide(Platform* p, Donogan* d, float dt)
{
    if (!p || !d) return;

    // Update first so p->box and mover deltas are valid
    Platform_Update(p, dt);

    // Fast reject: AABB test against full box
    if (!CheckCollisionBoxes(d->outerBox, p->box)) return;

    // Consider only landings from above (feet cross the top plane)
    const float topY = p->box.max.y;
    /*const float feetOld = d->oldPos.y + d->firstBB.min.y * d->scale;
    const float feetNew = DonFeetWorldY(d);*/

    //bool crossedDown = (feetOld >= topY) && (feetNew <= topY + d->groundEps) && (d->velY <= 0.0f);
    //if (!crossedDown) return; // side hit or rising up: ignore here, your world collider handles walls

    //if (!Platform_LandableTopOverlapXZ(p, d)) return;

    // LAND: set Don’s ground to the platform top and snap.
    d->groundY = topY;       // DonSnapToGround uses groundY as feet plane
    DonSnapToGround(d);

    // If it’s a faller, arm the delay timer once we step on it
    if (p->type == PLATFORM_FALLER && !p->falling) {
        if (!p->t_fallDelay.running) { StartTimer(&p->t_fallDelay); }
        if (HasTimerElapsed(&p->t_fallDelay)) { p->falling = true; p->vy = 0.0f; }
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

#endif // PLATFORM_H
