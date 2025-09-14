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
    bool aware;
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
    //stuff
    float groundY = GetTerrainHeightFromMeshXZ(b->pos.x, b->pos.z);
    if (groundY < -9000.0f) groundY = b->pos.y - 3.0f; // safe fallback
    float flyAGL = 3.0f;   // cruise height while flying
    float landAGL = 0.30f;  // near-ground for landing/wander
    float targetY = b->pos.y; // set per-state below
    float dxT = b->targetPos.x - b->pos.x;
    float dzT = b->targetPos.z - b->pos.z;
    float distToTarget = sqrtf(dxT * dxT + dzT * dzT);
    float yawToTarget = (distToTarget > 1e-4f) ? (RAD2DEG * atan2f(dxT, dzT)) : b->yaw;
    //first check if he is outside of the activation radius, for ghosts they die if this is true
    if (Vector3Distance(b->pos, b->spawnPoint) > b->spawnRadius)
    {
        b->state = GHOST_STATE_DEATH;
    }
    //next, switch and update on states
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
        if (Vector3Distance(d->pos, b->pos) < b->awareRadius || b->aware) //if aware of donogan
        {
            b->aware = true;
            int decision = RandomRange(0, 10);
            if (decision == 0) 
            {
                //todo: set target position,
            }
            else 
            { 
                //todo: set target position,
            }
            b->state = GHOST_STATE_FLY;
        }
        else
        {
            //wander
        }
    }
    case GHOST_STATE_FLY: {}
    case GHOST_STATE_FLY_DEC: {}
    case GHOST_STATE_WANDER: {
        //stuff
        
        // Low, slow amble near spawn. If no target, pick one.
        if (distToTarget <= b->arriveRadius * 0.9f) {
            float r = (float)GetRandomValue((int)(b->spawnRadius * 0.2f), (int)(b->spawnRadius * 0.6f));
            float a = (float)GetRandomValue(0, 359) * DEG2RAD;
            b->targetPos = (Vector3){ b->spawnPoint.x + sinf(a) * r, b->spawnPoint.y, b->spawnPoint.z + cosf(a) * r };
        }
        b->targetSpeed = fminf(b->maxSpeed, 0.9f);
        targetY = groundY + landAGL;

        float dyaw = yawToTarget - b->yaw; while (dyaw > 180.0f) dyaw -= 360.0f; while (dyaw < -180.0f) dyaw += 360.0f;
        float yawStepMax = (b->yawMaxRate * 0.6f) * dt;
        if (dyaw > yawStepMax) dyaw = yawStepMax;
        if (dyaw < -yawStepMax) dyaw = -yawStepMax;
        b->yaw += dyaw;

        float prevSpeed = b->speed;
        float sStep = (b->accel * 0.75f) * dt;
        float ds = b->targetSpeed - b->speed;
        if (fabsf(ds) <= sStep) b->speed = b->targetSpeed;
        else b->speed += (ds > 0 ? sStep : -sStep);
        if (b->speed < b->minSpeed) b->speed = b->minSpeed;

        // tiny leans
        float pStep = 90.0f * dt, rStep = 120.0f * dt;
        float pitchTarget = -0.4f * (b->speed - prevSpeed) / fmaxf(dt, 1e-5f);
        if (pitchTarget < -10.0f) pitchTarget = -10.0f;
        if (pitchTarget > 10.0f) pitchTarget = 10.0f;
        float rollTarget = -0.25f * (dyaw / fmaxf(dt, 1e-5f));
        if (rollTarget < -12.0f) rollTarget = -12.0f;
        if (rollTarget > 12.0f) rollTarget = 12.0f;

        float dp = pitchTarget - b->pitch;
        if (fabsf(dp) <= pStep) b->pitch = pitchTarget; else b->pitch += (dp > 0 ? pStep : -pStep);
        float dr = rollTarget - b->roll;
        if (fabsf(dr) <= rStep) b->roll = rollTarget; else b->roll += (dr > 0 ? rStep : -rStep);

        float yawRad = DEG2RAD * b->yaw;
        b->pos.x += sinf(yawRad) * b->speed * dt;
        b->pos.z += cosf(yawRad) * b->speed * dt;

        float yStep = 1.6f * dt;
        float dy = (groundY + landAGL) - b->pos.y;
        if (fabsf(dy) <= yStep) b->pos.y = groundY + landAGL; else b->pos.y += (dy > 0 ? yStep : -yStep);
    } break;
    case GHOST_STATE_HIT: {}
    case GHOST_STATE_DEATH: {
        //todo: this should do something, identify when its complete, then mark him as dead and inactive
        b->active = false;
        b->dead = true;
        bgModelBorrower[b->gbm_index].isInUse = false;
    }
    default: {}
    }
    //then set everything
    b->yaw = Lerp(b->yaw, b->targetYaw, dt);
    b->pitch = Lerp(b->pitch, b->targetPitch, dt);
    b->roll = Lerp(b->roll, b->targetRoll, dt);
}

//create functions
BadGuy CreateGhost(Vector3 pos)
{
    BadGuy b = { 0 };
    b.type = BG_GHOST;
    b.spawnPoint = pos;
    b.spawnRadius = 80;
    b.awareRadius = 30;
    b.tetherRadius = 20;
    b.gbm_index = -1;
    b.active = false;
    b.dead = false;
    b.aware = false;
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
                    //todo: respawn timer
                    if (bg[b].type == BG_GHOST) { bg[b].pos.y = GetTerrainHeightFromMeshXZ(bg[b].pos.x, bg[b].pos.z) - 30; }
                    return true;
                }
            }
        }
    }
    return false;
}
#endif // BG_H
