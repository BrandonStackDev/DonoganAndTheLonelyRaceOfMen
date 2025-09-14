#ifndef BG_H
#define BG_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h> 
#include <stdbool.h>
#include <stdlib.h>
//me
#include "timer.h"
#include "game.h"
#include "util.h"

#define MAX_BG_PER_TYPE_AT_ONCE 16

typedef enum {
    BG_NONE = -1,//probably do not use
    BG_GHOST,
    BG_TYPE_COUNT
} BadGuyType;

typedef enum {
    GHOST_STATE_SPAWN, //raise out of the ground to the spawn point
    GHOST_STATE_PLAN, //AI state, picks something randomly based on rules
    GHOST_STATE_FLY, //fly horizontally to the target position, when we get near enough the target, go to FLY_DEC
    GHOST_STATE_FLY_DEC, //once we get close to the target position, declerate, target pitch should be so he leans backwards while right, when we get near enough to the target, go back to plan
    GHOST_STATE_WANDER, //wander on the ground aimlessly
    GHOST_STATE_SWOOP_ATTACK, //horizontal fly directly through dongans position, 
    GHOST_STATE_HIT, //nothing yet, for when damage is taken
    GHOST_STATE_DEATH, //nothing yet, for death animation
} GhostState;

typedef struct {
    bool isInUse;
    BadGuyType type;
    Model model;
    Texture tex;
    Shader shader;
    BoundingBox origBox, origBodyBox, origHeadBox;
} BadGuyBorrowModel; //for borrowing models for bad guy instances

BadGuyBorrowModel * bgModelBorrower;

typedef struct {
    bool active;
    bool dead;
    BadGuyType type;
    Vector3 spawnPoint;
    float spawnRadius, awareRadius;
    Timer respawnTimer;
    int gbm_index; //global borrowed model index
    int state;
    int curAnim;
    Vector3 pos;
    float yaw, pitch, roll;
    float scale;
    BoundingBox box, bodyBox, headBox;
    int health, startHealth;
    // --- flight runtime (ghost) ---
    Vector3 vel;          // current velocity (we'll mostly use XZ; Y is altitude control)
    float   speed;        // current forward speed
    float   targetSpeed;  // desired forward speed (always >= minSpeed; never backward)
    float   minSpeed, maxSpeed;
    float   accel; // rate to reach targetSpeed
    float   targetYaw, targetPitch, targetRoll;    // where we’re steering to (deg)
    float   steerTimer;   // seconds left before picking a new heading
    float   yawMaxRate;   // max yaw change (deg/s)
    Vector3 targetPos;      // where we're flying to
    float   arriveRadius, tetherRadius;   // how close is "arrived" (m)
} BadGuy; //instance of a bad guy, will borrow its model

BadGuy * bg;
int total_bg_models_all_types, bg_count;

void InitBadGuyModels(Shader ghostShader)
{
    total_bg_models_all_types = MAX_BG_PER_TYPE_AT_ONCE * BG_TYPE_COUNT;
    bgModelBorrower = (BadGuyBorrowModel*)malloc(sizeof(BadGuyBorrowModel) * total_bg_models_all_types);
    for (int bg_t = 0; bg_t < BG_TYPE_COUNT; bg_t++)
    {
        for (int i = 0; i < MAX_BG_PER_TYPE_AT_ONCE; i++)
        {
            int index = i+(bg_t * MAX_BG_PER_TYPE_AT_ONCE);
            bgModelBorrower[index].type = (BadGuyType)bg_t;
            bgModelBorrower[index].isInUse = false;
            if (bg_t == BG_GHOST)
            {
                bgModelBorrower[index].model = LoadModel("models/ghost.obj");
                bgModelBorrower[index].tex = LoadTexture("textures/ghost.png");
                bgModelBorrower[index].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = bgModelBorrower[index].tex;
                bgModelBorrower[index].shader = ghostShader;
                bgModelBorrower[index].model.materials[0].shader = ghostShader;
            }
        }
    }
}

//draw stuff
// --- Quaternion helpers for BadGuy full-body rotation -----------------------
static inline Quaternion BG_ModelFixQuat(const BadGuy* b) {
    // Exporter/model local-axis fix (if needed). Ghost seems fine → 0.
    // If the ghost appears 90° off, try setting xFixDeg = -90.0f.
    float xFixDeg = 0.0f;
    (void)b; // per-type switch if you add more BG types later
    return QuaternionFromAxisAngle((Vector3) { 1, 0, 0 }, DEG2RAD* xFixDeg);
}

static inline Quaternion BG_BuildWorldQuat(const BadGuy* b) {
    // World yaw (Y), local pitch (X), local roll (Z) – same axis choices as whales.
    Quaternion qYaw = QuaternionFromAxisAngle((Vector3) { 0, 1, 0 }, DEG2RAD* b->yaw);
    Quaternion qPitch = QuaternionFromAxisAngle((Vector3) { 1, 0, 0 }, DEG2RAD* b->pitch);
    Quaternion qRoll = QuaternionFromAxisAngle((Vector3) { 0, 0, 1 }, DEG2RAD* b->roll);

    Quaternion qWorld = QuaternionMultiply(QuaternionMultiply(qYaw, qPitch), qRoll);
    return QuaternionMultiply(qWorld, BG_ModelFixQuat(b));  // apply local fix last
}

// Draw the model with S * (R * T), matching the whale path.
static inline void DrawBadGuy(BadGuy* b) {
    if (!b || !b->active || b->gbm_index < 0) return;

    Model* M = &bgModelBorrower[b->gbm_index].model;

    Quaternion q = BG_BuildWorldQuat(b);
    Matrix R = QuaternionToMatrix(q);
    Matrix T = MatrixTranslate(b->pos.x, b->pos.y, b->pos.z);
    float s = (b->scale > 0.0f) ? b->scale : 1.0f;
    Matrix S = MatrixScale(s, s, s);

    Matrix world = MatrixMultiply(S, MatrixMultiply(R, T));
    DrawMesh(M->meshes[0], M->materials[0], world);
}
//end draw stuff

//update functions
static inline void BG_Update_Ghost(Donogan* d, BadGuy* b, float dt)
{
    //todo: if outside of spawn radius, target pos = spawn point and then fly straight there
    switch (b->state)
    {
    case GHOST_STATE_SPAWN: {
        b->pos.y += dt;
        if (b->pos.y >= b->spawnPoint.y)
        {
            b->state = GHOST_STATE_PLAN;
        }
    }
    case GHOST_STATE_PLAN: {
        if (Vecor3Distance(d->pos, b->pos) < b->awareRadius) //if aware of donogan
        {
            int decision = RandomRange(0, 10);
            if (decision == 0) { b->state = GHOST_STATE_SWOOP_ATTACK; }
            else 
            { 
                //todo: set target position, and pitch to like PI or -PI so he flys horizontally
                b->state = GHOST_STATE_FLY; 
            }
        }
        else
        {
            //wander
        }
    }
    case GHOST_STATE_FLY: {}
    case GHOST_STATE_FLY_DEC: {}
    case GHOST_STATE_WANDER: {}
    case GHOST_STATE_SWOOP_ATTACK: {}
    case GHOST_STATE_HIT: {}
    case GHOST_STATE_DEATH: {}
    default: {}
    }
    //before exit, lerp/advance everything like yaw/pitch.roll to targets
}

//create functions
BadGuy CreateGhost(Vector3 pos)
{
    BadGuy b = { 0 };
    b.type = BG_GHOST;
    b.spawnPoint = pos;
    b.spawnRadius = 80;
    b.awareRadius = 20;
    b.gbm_index = -1;
    b.active = false;
    b.pos = pos;
    b.scale = 4;
    return b;
}


//end of the file stuff, important!
void InitBadGuys(Shader ghostShader)
{
    InitBadGuyModels(ghostShader);
    bg_count = 1; //increment this, every time, you add, a bg...
    bg = (BadGuy*)malloc(sizeof(BadGuy) * bg_count);
    bg[0] = CreateGhost((Vector3) { 3022.00f, 322.00f, 4042.42f }); //y was 319
}

static inline void BG_UpdateAll(Donogan *d, float dt)
{
    for (int i = 0; i < bg_count; ++i) {
        if (!bg[i].active) continue;
        if (bg[i].type == BG_GHOST)
        {
            BG_Update_Ghost(d, &bg[i], dt);
        }
    }
}


//only activate one per call
bool CheckSpawnAndActivateNext(Vector3 pos)
{
    for (int b = 0; b < bg_count; b++)
    {
        if (bg[b].active) { continue; }//if its turned on, dont run it on again
        else 
        {
            if (Vector3Distance(pos, bg[b].spawnPoint) < bg[b].spawnRadius)
            {
                for (int i = 0; i < MAX_BG_PER_TYPE_AT_ONCE; i++)
                {
                    int index = i + (bg[b].type * MAX_BG_PER_TYPE_AT_ONCE);
                    if (bgModelBorrower[index].isInUse) { continue; }
                    bgModelBorrower[index].isInUse = true;
                    bg[b].gbm_index = index;
                    bg[b].active = true;
                    bg[b].dead = false;
                    bg[b].health = bg[b].startHealth;
                    bg[b].pos = bg[b].spawnPoint;
                    bg[b].pos.y = GetTerrainHeightFromMeshXZ(bg[b].pos.z, bg[b].pos.z) - 30;//put him under the ground because spawn action is to rais out of the ground
                    //todo: respawn timer
                    if (bg[b].type == BG_GHOST) { BG_GhostInitFlight(&bg[b]); }
                    return true;
                }
            }
        }
    }
    return false;
}
#endif // BG_H
