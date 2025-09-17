#ifndef NPC_H
#define NPC_H

// --- npc.h additions ---
#include "donogan.h"   // so we can accept Donogan* in update/draw
// Includes
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h> 
#include <stdbool.h>

// Type Definitions
typedef enum {
    NPC_MODEL_TYPE_DARREL //darrel is the name of the model + the name of the first chararcter that we are using it with...
} NPC_Model_Type;

typedef enum {
    NPC_DARREL = 0, //a few npc's will use the darrel model, but only one darrel record
    NPC_TOTAL,
} NPC_Type;

typedef enum {
    DARREL_STATE_HELLO = 0,
    DARREL_STATE_RUN,
    DARREL_STATE_CONFUSED,
    DARREL_STATE_TALK,
    DARREL_STATE_WALKING,
} DarrelState; //darrel is simple, so animations indexes match states and we use state for animation index, many npc's will be simple and can do it this way
//we dont care about shared models with shared animations, I will place the similar models far aprt and cull on distance for update and draw

typedef struct {
    NPC_Type type;
    NPC_Model_Type modelType;
    int state;

    // === NEW: simple, general per-BG animation control
    int    curAnim;        // which animation index is playing
    float  animFrame;      // current frame within that animation, this is a float now...!
    float  animFPS;        // playback speed (frames/sec)
    ModelAnimation* anims; // shared pointer to per-type animations
    int    animCount;      // number of animations for this BG
    Model model;
    Texture tex;

    Vector3 pos;
    float yaw, pitch, roll;
    float scale;
    BoundingBox box, origBox;
    //
    Vector3 vel;
    float   speed, targetSpeed, minSpeed, maxSpeed, accel;
    float   targetYaw;
    Vector3 targetPos;
} NPC;

NPC npcs[NPC_TOTAL];

// Optional: tiny helpers
static inline float NPC_GroundY(Vector3 p) {
    float g = GetTerrainHeightFromMeshXZ(p.x, p.z);
    return (g < -9000.0f) ? p.y : g;
}
static inline float WrapAngleNpc(float a) {
    while (a > PI)  a -= 2.0f * PI;
    while (a < -PI) a += 2.0f * PI;
    return a;
}
static inline float TurnToward(float cur, float target, float maxStep) {
    float e = WrapAngleNpc(target - cur);
    if (e > maxStep) e = maxStep;
    if (e < -maxStep) e = -maxStep;
    return cur + e;
}

// --- General animation helpers (reusable) ---
static inline void NPC_AnimSet(NPC* n, int animIndex, bool forceReset, float fps) {
    if (!n || !n->anims || n->animCount <= 0) return;
    animIndex = (animIndex % n->animCount + n->animCount) % n->animCount;
    if (forceReset || n->curAnim != animIndex) {
        n->curAnim = animIndex;
        n->animFPS = (fps > 0.0f) ? fps : n->animFPS;
        n->animFrame = 0;
    }
}

//init the stuff
void InitAllNPC()
{
    Model darrel_model = LoadModel("models/darrel.glb");
    Texture darrel_tex = LoadTexture("textures/darrel.png");
    int darrel_animCount = 0;
    ModelAnimation* darrel_anims = LoadModelAnimations("models/darrel.glb", &darrel_animCount);
    npcs[NPC_DARREL].type = NPC_DARREL;
    npcs[NPC_DARREL].modelType = NPC_MODEL_TYPE_DARREL;
    npcs[NPC_DARREL].model = darrel_model; //models with animations have to have a unique model instance in raylib, otherwise they all display the same animation at the same time
    npcs[NPC_DARREL].tex = darrel_tex;
    npcs[NPC_DARREL].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = darrel_tex;
    npcs[NPC_DARREL].anims = darrel_anims;
    npcs[NPC_DARREL].animCount = darrel_animCount;
    npcs[NPC_DARREL].pos = (Vector3){ -346.16f, 496, -24.73f }; //test pos = (Vector3){ 3022.00f, 322.00f, 4042.42f };
    npcs[NPC_DARREL].targetPos = npcs[NPC_DARREL].pos;
    npcs[NPC_DARREL].scale = 3.8f;
    npcs[NPC_DARREL].yaw = 0.0f;
    npcs[NPC_DARREL].state = DARREL_STATE_CONFUSED;
    npcs[NPC_DARREL].curAnim = npcs[NPC_DARREL].state;
    npcs[NPC_DARREL].animFPS = 24.0f;
    npcs[NPC_DARREL].animFrame = 0.0f;
    NPC_AnimSet(&npcs[NPC_DARREL], npcs[NPC_DARREL].curAnim, true, 24.0f); // start correct clip
}
//static inline void NPC_AnimTick(NPC* n, float dt) {
//    if (!n || !n->anims || n->animCount <= 0) return;
//    ModelAnimation* a = &n->anims[n->curAnim];
//    if (a->frameCount <= 0) return;
//
//    n->animFrame += n->animFPS * dt;
//    if (n->animFrame >= a->frameCount) {
//        n->animFrame = fmodf(n->animFrame, (float)a->frameCount);
//    }
//    // Safe to guard, but generally fine:
//    if (IsModelAnimationValid(n->model, *a)) {
//        UpdateModelAnimation(n->model, *a, (int)n->animFrame);
//    }
//}
// 
bool IsModelAnimationValidMe(Model model, ModelAnimation anim)
{
    int result = true;

    if (model.boneCount != anim.boneCount)
    {
        result = false;
    }
    else
    {
        for (int i = 0; i < model.boneCount; i++)
        {
            if (model.bones[i].parent != anim.bones[i].parent) 
            { 
                result = false; 
                break; 
            }
        }
    }

    return result;
}
// was: static inline void NPC_AnimTick(NPC* n, float dt)
static inline bool NPC_AnimTick(NPC* n, float dt) {
    if (!n || !n->anims || n->animCount <= 0) return false;
    ModelAnimation* a = &n->anims[n->curAnim];
    if (a->frameCount <= 0) return false;

    float prev = n->animFrame;
    n->animFrame += n->animFPS * dt;

    bool looped = false;
    if (n->animFrame >= a->frameCount) {
        n->animFrame = fmodf(n->animFrame, (float)a->frameCount);
        looped = true;
    }
    if (IsModelAnimationValidMe(n->model, *a)) {
        UpdateModelAnimation(n->model, *a, (int)n->animFrame);
    }
    return looped;
}


// --- Case-specific handler for Darrel ---
static inline void NPC_Update_Darrel(NPC* n, const Donogan* d, float dt) {
    n->pos.y -= 0.2f;
    ModelAnimation* a = &n->anims[n->curAnim];
    if (n->animFrame >= a->frameCount)
    {
        n->curAnim = n->state;
        NPC_AnimSet(n, n->curAnim, true, 24.0f);
    }
}

// --- General per-NPC update entry point ---
static inline void NPC_Update(NPC* n, const Donogan* d, float dt) {
    if (!n || !d) return;

    // Distance cull (skip everything if too far)
    float dist = Vector3Distance(n->pos, d->pos);
    float cutoff = 1000.0f;
    if (dist > cutoff) return;

    // Face Donogan
    float targetYaw = atan2f(d->pos.x - n->pos.x, d->pos.z - n->pos.z);
    n->yaw = TurnToward(n->yaw, targetYaw, dt * 6.0f); // gentle turn rate

    //put them on the ground always
    n->pos.y = NPC_GroundY(n->pos);
    // Case dispatch
    switch (n->type) {
    case NPC_DARREL: NPC_Update_Darrel(n, d, dt); break;
    default: break;
    }

    // Tick animation & refresh box
    NPC_AnimTick(n, dt);
    //n->box = UpdateBoundingBox(n->origBox, n->pos);
}

// --- Minimal draw (preview will do the frustum check before calling this) ---
static inline void NPC_Draw(const NPC* n) {
    if (!n) return;
    DrawModelEx(n->model, n->pos, (Vector3) { 0, 1, 0 }, RAD2DEG* n->yaw,
        (Vector3) {
        n->scale, n->scale, n->scale
    }, WHITE);
    // Optionally: DrawBoundingBox(n->box, YELLOW);
}


#endif // NPC_H
