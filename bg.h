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
    Timer respawnTimer, interactionTimer; //respawn controls after death how long until respawn, interaction controls how long without interaction to let the bg live
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
    bool frozen;
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
                bgModelBorrower[index].origBox = GetModelBoundingBox(bgModelBorrower[index].model);
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

//helper
Vector3 GetGhostTargetPoint(Vector3 pos, float radius, float low, float high, bool aware)
{
    float ground = GetTerrainHeightFromMeshXZ(pos.x,pos.z);
    float r = (float)GetRandomValue((int)(radius * low), (int)(radius * high));
    float a = (float)GetRandomValue(0, 359) * DEG2RAD;
    float y = (float)GetRandomValue(ground + 4, ground + 12);
    y = aware ? pos.y + 3.8f : y;
    return (Vector3){ pos.x + sinf(a) * r, y, pos.z + cosf(a) * r };
}

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
    b->targetYaw = yawToTarget;
    //first check if he is outside of the activation radius, for ghosts they die if this is true
    if (Vector3Distance(b->pos, b->spawnPoint) > b->spawnRadius)
    {
        b->targetPitch = 0;
        b->speed = 0.4f;
        b->targetPos = b->pos;
        b->targetPos.y = groundY - 20;
        b->state = GHOST_STATE_DEATH;
    }
    if (Vector3Distance(d->pos, b->spawnPoint) > b->spawnRadius && !b->interactionTimer.running)//is donogan outside of our radius
    {
        StartTimer(&b->interactionTimer);
    }
    else if (HasTimerElapsed(&b->interactionTimer))
    {
        b->targetPitch = 0;
        b->speed = 0.4f;
        b->targetPos = b->pos;
        b->targetPos.y = groundY - 20;
        b->state = GHOST_STATE_DEATH;
    }
    //next, switch and update on states
    switch (b->state)
    {
    case GHOST_STATE_SPAWN: {
        b->pos.y += dt;//little boost to make sure we get there
        if (b->pos.y >= b->spawnPoint.y)
        {
            b->state = GHOST_STATE_PLAN;
        }
    }break;
    case GHOST_STATE_PLAN: {
        if (Vector3Distance(d->pos, b->pos) < b->awareRadius || b->aware) //if aware of donogan
        {
            b->speed = 1;
            b->aware = true;
            int decision = RandomRange(0, 10);
            if (decision == 0) 
            {
                b->targetPitch = 60;
                b->targetPos = d->pos;
            }
            else 
            { 
                b->targetPitch = 90;
                b->targetPos = GetGhostTargetPoint(d->pos, b->tetherRadius, 0.01f, 0.998f, b->aware);
            }
            b->state = GHOST_STATE_FLY;
        }
        else
        {
            b->speed = 0.34f;
            b->targetPitch = 15;
            b->targetPos = GetGhostTargetPoint(b->spawnPoint, 45.0f, 0.2f, 0.98f, b->aware);
            b->state = GHOST_STATE_WANDER;
        }
    }break;
    case GHOST_STATE_FLY: {
        if (Vector3Distance(b->targetPos, b->pos) < 8)
        {
            b->targetPitch = -10;
            b->state = GHOST_STATE_FLY_DEC;
        }
    }break;
    case GHOST_STATE_FLY_DEC: {
        if (Vector3Distance(b->targetPos, b->pos) < 3)
        {
            b->targetPitch = 0;
            b->state = GHOST_STATE_PLAN;
        }
    }break;
    case GHOST_STATE_WANDER: {
        if (Vector3Distance(b->targetPos, b->pos) < 4)
        {
            b->targetPitch = 0;
            b->state = GHOST_STATE_PLAN;
        }
    } break;
    case GHOST_STATE_HIT: { //ghosts are one hit always
        b->targetPitch = 0;
        b->speed = 0.4f;
        b->targetPos = b->pos;
        b->targetPos.y = groundY - 20;
        b->state = GHOST_STATE_DEATH;
    }break;
    case GHOST_STATE_DEATH: {
        if (Vector3Distance(b->targetPos, b->pos) < 3)
        {
            b->targetPitch = 0;
            b->active = false;
            b->dead = true;
            b->aware = false;
            bgModelBorrower[b->gbm_index].isInUse = false;
            b->gbm_index = -1;
            StartTimer(&b->respawnTimer);
            ResetTimer(&b->interactionTimer);
        }
    }break;
    default: {}
    }
    //then set everything
    b->pos = Vector3Lerp(b->pos, b->targetPos, dt * b->speed);
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
    b.spawnRadius = 200;
    b.awareRadius = 50;
    b.tetherRadius = 30;
    b.gbm_index = -1;
    b.active = false;
    b.dead = false;
    b.aware = false;
    b.pos = pos;
    b.scale = 4;
    b.speed = 1;
    b.respawnTimer = CreateTimer(360);//6 minutes
    b.interactionTimer = CreateTimer(120);//2 minutes
    return b;
}


//end of the file stuff, important!
void InitBadGuys(Shader ghostShader)
{
    InitBadGuyModels(ghostShader);
    bg_count = 38; //increment this, every time, you add, a bg...
    bg = (BadGuy*)malloc(sizeof(BadGuy) * bg_count);
    bg[0] = CreateGhost((Vector3) { 237, 394, 1039 }); //for testing: 3022.00f, 322.00f, 4042.42f
    bg[1] = CreateGhost((Vector3) { -652, 404, 1005 });
    bg[2] = CreateGhost((Vector3) { -738, 750, 3117 });
    bg[3] = CreateGhost((Vector3) { -461, 755, 3168 });
    bg[4] = CreateGhost((Vector3) { -681, 723, 3285 });
    bg[5] = CreateGhost((Vector3) { -612, 755, 3133 });
    bg[6] = CreateGhost((Vector3) { 2284, 528, 670 });
    bg[7] = CreateGhost((Vector3) { 2467, 510, 922 });
    bg[8] = CreateGhost((Vector3) { 2520, 452, 627 });
    bg[9] = CreateGhost((Vector3) { 176, 728, 517 });
    bg[10] = CreateGhost((Vector3) { -109, 721, 628 });
    bg[11] = CreateGhost((Vector3) { -703, 715, -154 });
    bg[12] = CreateGhost((Vector3) { 83, 720, -683 });
    bg[13] = CreateGhost((Vector3) { 196, 735, 450 });
    bg[14] = CreateGhost((Vector3) { 259, 372, 1206 });
    bg[15] = CreateGhost((Vector3) { 2689, 337, 4540 });
    bg[16] = CreateGhost((Vector3) { 2798, 365, 4377 });
    bg[17] = CreateGhost((Vector3) { 2287, 420, 4350 });
    bg[18] = CreateGhost((Vector3) { -2129, 328, 3594 });
    bg[19] = CreateGhost((Vector3) { -2246, 343, 3487 });
    bg[20] = CreateGhost((Vector3) { -2299, 343, 3435 });
    bg[21] = CreateGhost((Vector3) { -2308, 343, 3534 });
    bg[22] = CreateGhost((Vector3) { -362, 962, 1957 });
    bg[23] = CreateGhost((Vector3) { -525, 758, 3140 });
    bg[24] = CreateGhost((Vector3) { -528, 718, 3355 });
    bg[25] = CreateGhost((Vector3) { -785, 767, 2207 });
    bg[26] = CreateGhost((Vector3) { -1349, 375, 972 });
    bg[27] = CreateGhost((Vector3) { -506, 412, 964 });
    bg[28] = CreateGhost((Vector3) { -358, 415, 1029 });
    bg[29] = CreateGhost((Vector3) { -169, 417, 1054 });
    bg[30] = CreateGhost((Vector3) { 49, 386, 1000 });
    bg[31] = CreateGhost((Vector3) { 232, 394, 1147 });
    bg[32] = CreateGhost((Vector3) { -375, 469, 234 });
    bg[33] = CreateGhost((Vector3) { -344, 474, 106 });
    bg[34] = CreateGhost((Vector3) { -341, 610, -728 });
    bg[35] = CreateGhost((Vector3) { 2200, 550, 818 });
    bg[36] = CreateGhost((Vector3) { 2430, 498, 809 });
    bg[37] = CreateGhost((Vector3) { 2462, 460, 680 });
}

static inline void BG_UpdateAll(Donogan *d, float dt)
{
    for (int i = 0; i < bg_count; ++i) {
        if (!bg[i].active) { continue; }
        //handle square spell
        if (Vector3Distance(d->pos, bg[i].pos) < 60)
        {
            if (!d->spellTimer.running) { bg[i].frozen = false; }
            else if (HasTimerElapsed(&d->spellTimer))
            {
                bg[i].health -= 5;
                bg[i].frozen = true;
                if (bg[i].type == BG_GHOST)
                {
                    bg[i].state = GHOST_STATE_DEATH;
                    bg[i].targetPos = bg[i].pos;
                }
            }
            if (d->spellTimer.running)
            {
                //make em raise up
                bg[i].pos.y += dt/100.0f;
                //make em spin
                float deltaDeg = d->yawY - d->cached_yawY;       // Donogan's spin since last frame
                // Wrap to [-180, 180] so crossing 359->0 doesn't cause a huge jump.
                //if (deltaDeg > 180.0f)  deltaDeg -= 360.0f;
                //if (deltaDeg < -180.0f) deltaDeg += 360.0f;
                if (deltaDeg > 4*PI) { deltaDeg = PI; } //good thing to do this....?
                if (deltaDeg < -4 * PI) { deltaDeg = -PI; }
                if (fabsf(deltaDeg) > 0.0001f) {
                    float r = DEG2RAD * -deltaDeg;
                    float s = sinf(r), c = cosf(r);

                    float rx = bg[i].pos.x - d->pos.x;  // vector from Don -> BG (XZ only)
                    float rz = bg[i].pos.z - d->pos.z;

                    float nx = rx * c - rz * s;         // rotate around Donogan
                    float nz = rx * s + rz * c;

                    bg[i].pos.x = d->pos.x + nx;
                    bg[i].pos.z = d->pos.z + nz;
                }
            }
        }
        if (bg[i].frozen) { continue; }
        //end handle square spell, handle types next....
        if (bg[i].type == BG_GHOST)
        {
            BG_Update_Ghost(d, &bg[i], dt);
        }
        //update general stuff
        bg[i].box = UpdateBoundingBox(bgModelBorrower[bg[i].gbm_index].origBox,bg[i].pos);
    }
    //handle don and timer for square spell
    if (d->spellTimer.running)
    {
        //d->cached_yawY = d->yawY;//this does not work....?
    }
    if (HasTimerElapsed(&d->spellTimer))
    {
        d->mana -= 1;
        StartTimer(&d->spellTimer);
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
            if (Vector3Distance(pos, bg[b].spawnPoint) < bg[b].spawnRadius && (!bg[b].respawnTimer.running || HasTimerElapsed(&bg[b].respawnTimer)))
            {
                for (int i = 0; i < MAX_BG_PER_TYPE_AT_ONCE; i++)
                {
                    int index = i + (bg[b].type * MAX_BG_PER_TYPE_AT_ONCE);
                    if (bgModelBorrower[index].isInUse) { continue; }
                    bgModelBorrower[index].isInUse = true;
                    bg[b].gbm_index = index;
                    bg[b].active = true;
                    bg[b].dead = false;
                    bg[b].aware = false;
                    bg[b].health = bg[b].startHealth;
                    bg[b].pos = bg[b].spawnPoint;
                    bg[b].targetPos = bg[b].spawnPoint;
                    //todo: respawn timer
                    if (bg[b].type == BG_GHOST) 
                    { 
                        bg[b].pos.y = GetTerrainHeightFromMeshXZ(bg[b].pos.x, bg[b].pos.z) - 30;
                        bg[b].state = GHOST_STATE_SPAWN;
                    }
                    return true;
                }
            }
        }
    }
    return false;
}
#endif // BG_H
