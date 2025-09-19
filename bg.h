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

#define MAX_BG_PER_TYPE_AT_ONCE 12

typedef enum {
    BG_NONE = -1,//probably do not use
    BG_GHOST,
    BG_YETI,
    BG_ROBO,
    BG_TYPE_COUNT
} BadGuyType;

//kill counters (sum for total)
int ghostKillCount;
int yetiKillCount;
int roboKillCount;

typedef enum {
    ATTACK_PUNCH,
    ATTACK_BALL,
    ATTACK_FREEZE,
    ATTACK_THROW,
    ATTACK_ARROW,
} DonAttackType;

//todo: badguy specific (because ghosts are one hit and I only have the yeti it doesnt make sense yet)
int GetDamageDone(GameState *gs, Donogan *d, DonAttackType attack, BadGuyType bg_type)
{
    if (attack == ATTACK_PUNCH)
    {
        if (gs->diff == DIFF_EASY)
        {
            return 20;
        }
        else if (gs->diff == DIFF_NORMAL)
        {
            return 10;
        }
        else//hard
        {
            return 5;
        }
    }
    else if (attack == ATTACK_BALL)
    {
        if (gs->diff == DIFF_EASY)
        {
            return 100;
        }
        else if (gs->diff == DIFF_NORMAL)
        {
            return 50;
        }
        else//hard
        {
            return 20;
        }
    }
    else if (attack == ATTACK_FREEZE)
    {
        if (gs->diff == DIFF_EASY)
        {
            return 10;
        }
        else if (gs->diff == DIFF_NORMAL)
        {
            return 5;
        }
        else//hard
        {
            return 1;
        }
    }
    else if (attack == ATTACK_THROW)
    {
        if (gs->diff == DIFF_EASY)
        {
            return 20;
        }
        else if (gs->diff == DIFF_NORMAL)
        {
            return 10;
        }
        else//hard
        {
            return 5;
        }
    }
    else if (attack == ATTACK_ARROW)
    {
        if (gs->diff == DIFF_EASY)
        {
            return 8 + d->level;
        }
        else if (gs->diff == DIFF_NORMAL)
        {
            return d->level;
        }
        else//hard
        {
            return (d->level/4)+1;
        }
    }
    TraceLog(LOG_WARNING, "we should never reach this, GetDamageDone routine!");
    return 1; //default case
}

typedef enum {
    GHOST_STATE_SPAWN, //raise out of the ground to the spawn point
    GHOST_STATE_PLAN, //AI state, picks something randomly based on rules
    GHOST_STATE_FLY, //fly horizontally to the target position, when we get near enough the target, go to FLY_DEC
    GHOST_STATE_FLY_DEC, //once we get close to the target position, decelerate, target pitch should be so he leans backwards while right, when we get near enough to the target, go back to plan
    GHOST_STATE_WANDER, //wander on the ground aimlessly
    GHOST_STATE_HIT, //nothing yet, for when damage is taken
    GHOST_STATE_DEATH, //nothing yet, for death animation
} GhostState;

typedef enum {
    //for the yeti
    ANIM_YETI_JUMP = 0,
    ANIM_YETI_ROAR = 1,
    ANIM_YETI_WALK = 2,
} YetiAnimation;

typedef enum {
    YETI_STATE_SPAWN,
    YETI_STATE_PLANNING,
    YETI_STATE_DEAD,
    YETI_STATE_DYING,
    YETI_STATE_HIT,
    YETI_STATE_ATTACK,
    YETI_STATE_WALKING,
    //for the yeti
} YetiState;

typedef enum {
    ROBO_STATE_SPAWN,
    ROBO_STATE_PLAN,
    ROBO_STATE_SPIN,
    ROBO_STATE_ZIP,
    ROBO_STATE_SHOOT,
    ROBO_STATE_DYING,
    ROBO_STATE_DEAD,
    //for the robot orb
} RoboState;

typedef struct {
    bool isInUse;
    BadGuyType type;
    Model model;
    Texture tex;
    Shader shader;
    BoundingBox origBox, origBodyBox, origHeadBox;
    ModelAnimation* anims;
    int animCount;
} BadGuyBorrowModel; //for borrowing models for bad guy instances

BadGuyBorrowModel * bgModelBorrower;

typedef struct {
    bool active;
    bool dead;
    bool aware;
    BadGuyType type;
    Vector3 spawnPoint;
    float spawnRadius, awareRadius;
    Timer respawnTimer, interactionTimer;
    int gbm_index;
    int state;

    // === NEW: simple, general per-BG animation control
    int    curAnim;        // which animation index is playing
    int    animFrame;      // current frame within that animation
    float  animFPS;        // playback speed (frames/sec)
    ModelAnimation* anims; // shared pointer to per-type animations
    int    animCount;      // number of animations for this BG

    Vector3 pos;
    float yaw, pitch, roll;
    float scale;
    BoundingBox box, bodyBox, headBox;
    int health, startHealth;

    // flight runtime (ghost) ...
    Vector3 vel;
    float   speed, targetSpeed, minSpeed, maxSpeed, accel;
    float   targetYaw, targetPitch, targetRoll;
    float   steerTimer;
    float   yawMaxRate;
    Vector3 targetPos;
    float   arriveRadius, tetherRadius;
    bool frozen;
    Color drawColor;
    bool     throwing;
    Vector3  throwVel;

    bool bounced;
} BadGuy;
//instance of a bad guy, will borrow its model

int act_bg_count = 0;
int act_bg[MAX_BG_PER_TYPE_AT_ONCE * BG_TYPE_COUNT]; //store indexes of active bg's so we dont loop alot ever (except for spawning check...)
BadGuy * bg;
int total_bg_models_all_types, bg_count;

void InitBadGuyModels(Shader ghostShader)
{
    total_bg_models_all_types = MAX_BG_PER_TYPE_AT_ONCE * BG_TYPE_COUNT;
    bgModelBorrower = (BadGuyBorrowModel*)malloc(sizeof(BadGuyBorrowModel) * total_bg_models_all_types);
    //ghost model can be shared
    Model ghost_model = LoadModel("models/ghost.obj");
    Texture ghost_tex = LoadTexture("textures/ghost.png");
    int yeti_animCount = 0;
    ModelAnimation* yeti_anims = LoadModelAnimations("models/yeti_anim_2.glb", &yeti_animCount);
    Model robo_model = LoadModel("models/robo.obj");
    Texture robo_tex = LoadTexture("textures/robo.png");
    for (int bg_t = 0; bg_t < BG_TYPE_COUNT; bg_t++)
    {
        for (int i = 0; i < MAX_BG_PER_TYPE_AT_ONCE; i++)
        {
            int index = i+(bg_t * MAX_BG_PER_TYPE_AT_ONCE);
            bgModelBorrower[index].type = (BadGuyType)bg_t;
            bgModelBorrower[index].isInUse = false;
            if (bg_t == BG_GHOST)
            {
                bgModelBorrower[index].model = ghost_model;
                bgModelBorrower[index].tex = ghost_tex;
                bgModelBorrower[index].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = bgModelBorrower[index].tex;
                bgModelBorrower[index].shader = ghostShader;
                bgModelBorrower[index].model.materials[0].shader = ghostShader;
                bgModelBorrower[index].origBox = GetModelBoundingBox(bgModelBorrower[index].model);
            }
            else if (bg_t == BG_YETI)
            {
                bgModelBorrower[index].model = LoadModel("models/yeti_anim_2.glb"); //models with animations have to have a unique model instance in raylib, otherwise they all display the same animation at the same time
                bgModelBorrower[index].tex = LoadTexture("textures/yeti_skin_2.png");
                bgModelBorrower[index].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = bgModelBorrower[index].tex;
                bgModelBorrower[index].origBox = ScaleBoundingBox(GetModelBoundingBox(bgModelBorrower[index].model),1.6);
                bgModelBorrower[index].anims = yeti_anims;
                bgModelBorrower[index].animCount = yeti_animCount;
            }
            else if (bg_t == BG_ROBO)
            {
                bgModelBorrower[index].model = robo_model;
                bgModelBorrower[index].tex = robo_tex;
                bgModelBorrower[index].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = bgModelBorrower[index].tex;
                bgModelBorrower[index].origBox = GetModelBoundingBox(bgModelBorrower[index].model);
            }
        }
    }
}

// === NEW: helper for ground
static inline float BG_GroundY(Vector3 p) {
    float g = GetTerrainHeightFromMeshXZ(p.x, p.z);
    return (g < -9000.0f) ? p.y : g;
}
//anim helpers
// === NEW: general animation helpers =========================================
static inline void BG_SetAnim(BadGuy* b, int animIndex, bool forceRestart) {
    if (!b || b->gbm_index < 0) return;
    if (b->animCount <= 0 || !b->anims) return;
    if (animIndex < 0 || animIndex >= b->animCount) return;

    if (forceRestart || b->curAnim != animIndex) {
        b->curAnim = animIndex;
        b->animFrame = 0;
    }
}

static inline void BG_UpdateAnim(BadGuy* b, float dt) {
    if (!b || b->gbm_index < 0) return;
    if (b->animCount <= 0 || !b->anims) return;

    const int a = b->curAnim;
    const ModelAnimation* A = &b->anims[a];
    if (A->frameCount <= 0) return;

    // advance frames
    float framesToAdvance = b->animFPS * dt;
    int oldFrame = b->animFrame;
    int adv = (int)(framesToAdvance + 0.5f);
    if (adv <= 0) { adv = 1; }
    b->animFrame += adv;

    // Loop everything except "one-shot" jumps (we'll clamp on ATTACK state)
    if (b->animFrame >= A->frameCount) {
        b->animFrame %= A->frameCount;
    }

    // Apply pose to the shared model we're borrowing
    Model* M = &bgModelBorrower[b->gbm_index].model;
    UpdateModelAnimation(*M, b->anims[a], b->animFrame);
}
// === NEW: attach shared borrowed resources to instance
static inline void BG_AttachBorrowed(BadGuy* b) {
    if (!b || b->gbm_index < 0) return;
    BadGuyBorrowModel* BM = &bgModelBorrower[b->gbm_index];
    b->anims = BM->anims;
    b->animCount = BM->animCount;
    if (b->animFPS <= 0.0f) b->animFPS = 24.0f; // default
    b->curAnim = 0;
    b->animFrame = 0;
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

static inline void DrawBadGuy(BadGuy * b) {
    if (!b || !b->active || b->gbm_index < 0) return;

    Model* M = &bgModelBorrower[b->gbm_index].model;

    Quaternion q = BG_BuildWorldQuat(b);
    Matrix R = QuaternionToMatrix(q);
    Matrix T = MatrixTranslate(b->pos.x, b->pos.y, b->pos.z);
    float s = (b->scale > 0.0f) ? b->scale : 1.0f;
    Matrix S = MatrixScale(s, s, s);
    Matrix world = MatrixMultiply(S, MatrixMultiply(R, T));

    rlPushMatrix();
    rlMultMatrixf(MatrixToFloatV(world).v);
    DrawModel(*M, (Vector3) { 0, 0, 0 }, 1.0f, b->drawColor);
    rlPopMatrix();
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
    else if (HasTimerElapsed(&b->interactionTimer) && Vector3Distance(d->pos, b->spawnPoint) > b->spawnRadius)
    {
        b->targetPitch = 0;
        b->speed = 0.4f;
        b->targetPos = b->pos;
        b->targetPos.y = groundY - 20;
        b->state = GHOST_STATE_DEATH;
    }
    else
    {
        ResetTimer(&b->interactionTimer);
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
            d->xp += 10;
            ghostKillCount++;
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

// === NEW: Yeti state update ================================================
static inline void BG_Update_Yeti(Donogan* d, BadGuy* b, float dt)
{
    if (Vector3Distance(d->pos, b->spawnPoint) > b->spawnRadius && !b->interactionTimer.running)//is donogan outside of our radius
    {
        StartTimer(&b->interactionTimer);
    }
    else if (HasTimerElapsed(&b->interactionTimer) && Vector3Distance(d->pos, b->spawnPoint) > b->spawnRadius)//if it expires and donny is still outisde the radius, kill him
    {
        b->targetPitch = 0;
        b->targetPos = b->pos;
        b->state = YETI_STATE_DEAD;
    }
    else
    {
        ResetTimer(&b->interactionTimer);
    }
    float groundY = BG_GroundY(b->pos);
    float dxT = b->targetPos.x - b->pos.x;
    float dzT = b->targetPos.z - b->pos.z;
    float distToTarget = sqrtf(dxT * dxT + dzT * dzT);
    float yawToTarget = (distToTarget > 1e-4f) ? (RAD2DEG * atan2f(dxT, dzT)) : b->yaw;
    b->targetYaw = yawToTarget;

    // Awareness check
    float distToDon = Vector3Distance(b->pos, d->pos);
    if (distToDon < b->awareRadius) b->aware = true;
    if (b->health <= 0)
    {
        b->state = YETI_STATE_DYING;
        BG_SetAnim(b, ANIM_YETI_ROAR, true);
    }
    switch (b->state)
    {
    case YETI_STATE_SPAWN:
        // ensure on ground, then plan
        b->pos.y = groundY;
        b->state = YETI_STATE_PLANNING;
        BG_SetAnim(b, ANIM_YETI_ROAR, true);
        break;

    case YETI_STATE_PLANNING:
    {
        // choose target: wander within spawn radius or chase Donogan
        if (b->aware) {
            b->targetPos = d->pos;          // chase
        }
        else {
            float a = (float)GetRandomValue(0, 359) * DEG2RAD;
            float r = (float)GetRandomValue(8, (int)b->spawnRadius);
            Vector3 p = (Vector3){ b->spawnPoint.x + sinf(a) * r, 0, b->spawnPoint.z + cosf(a) * r };
            p.y = BG_GroundY(p);
            b->targetPos = p;
        }
        BG_SetAnim(b, ANIM_YETI_WALK, (b->curAnim != ANIM_YETI_WALK));
        b->state = YETI_STATE_WALKING;
    } break;

    case YETI_STATE_WALKING:
    {
        // face and move toward target on ground
        b->targetPitch = 0;
        //b->pos = Vector3Lerp(b->pos, (Vector3) { b->targetPos.x, BG_GroundY(b->pos), b->targetPos.z }, dt* b->speed);
        Vector3 to = (Vector3){ b->targetPos.x, BG_GroundY(b->pos), b->targetPos.z };
        float dist = Vector3Distance(b->pos, to);
        float t = (dist > 0.0f) ? fminf((b->speed * dt) / dist, 1.0f) : 1.0f;
        b->pos = Vector3Lerp(b->pos, to, t);
        b->pos.y = groundY;//keep him on the ground always
        b->yaw = Lerp(b->yaw, b->targetYaw, dt * 4.0f); // snappier turn for a brawler

        // Arrived / chase logic
        if (b->aware) {
            // keep chasing Don — don't bounce back to PLANNING
            b->targetPos = d->pos;

            // close enough? do the jump attack
            if (distToDon <= 16.0f) {
                Vector3 dir = Vector3Normalize(Vector3Subtract(d->pos, b->pos));
                b->vel.x = dir.x * 9.0f;
                b->vel.z = dir.z * 9.0f;
                b->vel.y = 12.0f;
                BG_SetAnim(b, ANIM_YETI_JUMP, true);
                b->state = YETI_STATE_ATTACK;
            }
        }
        else {
            // only replan if we actually reached the wander target
            if (distToTarget < 1.8f) {
                b->state = YETI_STATE_PLANNING;
                BG_SetAnim(b, ANIM_YETI_ROAR, false);
            }
        }

    } break;

    case YETI_STATE_ATTACK: // jump attack (ballistic)
    {
        // Advance physics
        b->pos.x += b->vel.x * dt * b->speed;
        b->pos.z += b->vel.z * dt * b->speed;
        b->vel.y += -24.0f * dt; // gravity
        b->pos.y += b->vel.y * dt;

        // Don't loop the JUMP anim: clamp last frame until we land
        if (b->anims && b->animCount > 0) {
            int a = b->curAnim;
            const int last = b->anims[a].frameCount > 0 ? b->anims[a].frameCount - 1 : 0;
            if (b->animFrame > last) b->animFrame = last;
        }

        // Land?
        float gy = BG_GroundY(b->pos);
        if (b->pos.y <= gy) {
            b->pos.y = gy;
            // Post-attack behavior: roar then continue hunting/roaming
            BG_SetAnim(b, ANIM_YETI_ROAR, true);
            b->state = YETI_STATE_PLANNING;
            if (Vector3Distance(d->pos, b->pos) < 25)
            {
                d->shook += dt;
                d->health -= 5;
                DonSetState(d,DONOGAN_STATE_HIT);
                StartTimer(&d->hitTimer);
            }
        }
    } break;

    case YETI_STATE_HIT:
        if (b->animFrame >= b->anims[b->curAnim].frameCount - 1) {
            b->state = YETI_STATE_PLANNING;
        }
        break;

    case YETI_STATE_DYING:
        if (b->drawColor.a != 0) { b->drawColor.a--; }
        if (b->drawColor.a == 0)
        {
            b->state = YETI_STATE_DEAD;
            b->drawColor.a = 255;
            b->active = false; b->dead = true;
            bgModelBorrower[b->gbm_index].isInUse = false;
            b->gbm_index = -1;
            StartTimer(&b->respawnTimer);
            ResetTimer(&b->interactionTimer);
            d->xp += 50;
            yetiKillCount++;
        }
        if (b->animFrame >= b->anims[b->curAnim].frameCount - 1) {
            BG_SetAnim(b, ANIM_YETI_ROAR, true);
        }
        break;
    case YETI_STATE_DEAD: //todo: why did we never reach this code?
        b->active = false; b->dead = true;
        bgModelBorrower[b->gbm_index].isInUse = false;
        b->gbm_index = -1;
        StartTimer(&b->respawnTimer);
        ResetTimer(&b->interactionTimer);
        break;

    default: break;
    }

    // keep box in sync
    b->box = UpdateBoundingBox(bgModelBorrower[b->gbm_index].origBox, b->pos);
}

static inline void BG_Update_Robo(Donogan* d, BadGuy* b, float dt)
{
    //pre guard stuff
    if (Vector3Distance(d->pos, b->spawnPoint) > b->spawnRadius && !b->interactionTimer.running)//is donogan outside of our radius
    {
        StartTimer(&b->interactionTimer);
    }
    else if (HasTimerElapsed(&b->interactionTimer) && Vector3Distance(d->pos, b->spawnPoint) > b->spawnRadius)//if it expires and donny is still outisde the radius, kill him
    {
        b->targetPitch = 0;
        b->targetPos = b->pos;
        b->state = ROBO_STATE_DEAD;
    }
    else
    {
        ResetTimer(&b->interactionTimer);
    }
    if (b->health <= 0 && b->state != ROBO_STATE_DEAD)
    {
        b->state = ROBO_STATE_DYING;
    }
    //start
    float groundY = GetTerrainHeightFromMeshXZ(b->pos.x, b->pos.z);
    //switch
    switch (b->state)
    {
    case ROBO_STATE_SPAWN: {
        b->pos.y = groundY + 8; //spawn above the ground and then enter planning phase
        b->state = ROBO_STATE_PLAN;
    } break;
    case ROBO_STATE_PLAN: {
        b->aware = Vector3Distance(d->pos,b->pos)<b->awareRadius;
        if (b->aware)
        {
            if (GetRandomValue(0, 3) == 3)
            {
                float a = (float)GetRandomValue(0, 359) * DEG2RAD;
                float r = (float)GetRandomValue(8, (int)b->tetherRadius);
                Vector3 p = (Vector3){ d->pos.x + sinf(a) * r, 0, d->pos.z + cosf(a) * r };
                p.y = BG_GroundY(p);
                p.y += (float)GetRandomValue(3, 10);
                b->targetPos = p;
                b->state = ROBO_STATE_ZIP;
            }
            else
            {
                b->targetPos = b->pos;
                b->targetYaw = (float)GetRandomValue(0, 1080);
                b->targetPitch = (float)GetRandomValue(0, 1080);
                b->targetRoll = (float)GetRandomValue(0, 1080);
                b->state = ROBO_STATE_SPIN;
            }
        }
        else
        {
            b->targetPos = b->pos;
            b->targetYaw = (float)GetRandomValue(0, 1080);
            b->targetPitch = (float)GetRandomValue(0, 1080);
            b->targetRoll = (float)GetRandomValue(0, 1080);
            b->state = ROBO_STATE_SPIN;
        }
    } break;
    case ROBO_STATE_SPIN: {
        if (fabsf(b->targetYaw - b->yaw) < 10) 
        { 
            if (GetRandomValue(0, 4) == 4)
            {
                b->state = ROBO_STATE_SHOOT;
            }
            else
            {
                b->state = ROBO_STATE_PLAN;
            }
        }
    } break;
    case ROBO_STATE_ZIP: {
        if (Vector3Distance(b->pos, b->targetPos) < 3.4f){b->state = ROBO_STATE_PLAN;}
    } break;
    case ROBO_STATE_SHOOT: {
        // Spawn an instant beam aimed at Don’s torso (no collision tests)
        Vector3 from = (Vector3){ b->pos.x, b->pos.y + 0.4f, b->pos.z };   // slight vertical offset
        Vector3 to = (Vector3){ d->pos.x, d->pos.y + 1.2f, d->pos.z };   // Don chest-ish

        FireLaser(from, to, 0.16f);  // short flash; fades automatically

        // Immediate hit feedback & damage (simple gate via hitTimer)
        if (HasTimerElapsed(&d->hitTimer))
        {
            d->health -= 8;                          // tune damage to taste
            DonSetState(d, DONOGAN_STATE_HIT);
            StartTimer(&d->hitTimer);

            d->shook = fmaxf(d->shook, 0.22f);       // kick the camera a bit
        }

        // Return to planning
        b->state = ROBO_STATE_PLAN;

    } break;
    case ROBO_STATE_DYING: {
        // Simple gravity + one-or-two tiny bounces, then settle → DEAD
        const float gy = BG_GroundY(b->pos);
        //if (b->vel.y == 0.0f && b->pos.y > gy + 0.02f) b->vel.y = -6.0f; // give it a push if stationary

        b->vel.y += -24.0f * dt;                  // gravity
        b->pos.y += b->vel.y * dt;

        if (b->pos.y <= gy)
        {
            b->pos.y = gy;
            if (fabsf(b->vel.y) > 2.0f && !b->bounced)           // bounce threshold
            {
                b->vel.y = -b->vel.y * 0.33f;     // damped bounce
                b->bounced = true;
            }
            else
            {
                b->vel = (Vector3){ 0 };
                b->state = ROBO_STATE_DEAD;
            }
        }
    } break;
    case ROBO_STATE_DEAD: {
        b->active = false;
        b->dead = true;
        b->bounced = false;
        if (b->gbm_index >= 0) { bgModelBorrower[b->gbm_index].isInUse = false; }
        b->gbm_index = -1;
        StartTimer(&b->respawnTimer);
        ResetTimer(&b->interactionTimer);
        d->xp += 25;
    } break;
    default: break;
    }
    if (b->state != ROBO_STATE_DYING && b->state != ROBO_STATE_DEAD)
    {
        //end routine
        b->pos = Vector3Lerp(b->pos, b->targetPos, dt * b->speed);
        b->yaw = Lerp(b->yaw, b->targetYaw, dt * b->speed);
        b->pitch = Lerp(b->pitch, b->targetPitch, dt * b->speed);
        b->roll = Lerp(b->roll, b->targetRoll, dt * b->speed);
    }
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
    b.drawColor = WHITE;
    return b;
}

BadGuy CreateYeti(Vector3 pos)
{
    BadGuy b = { 0 };
    b.type = BG_YETI;
    b.spawnPoint = pos;
    b.spawnRadius = 200;
    b.awareRadius = 80;
    b.tetherRadius = 30;
    b.gbm_index = -1;
    b.active = false;
    b.dead = false;
    b.aware = false;
    b.pos = pos;
    b.scale = 1.6;
    b.speed = 1.826;
    b.startHealth = 100;   // === NEW: give the big guy some HP
    b.health = b.startHealth;
    b.animFPS = 24.0f;   // === NEW: default playback speed
    b.respawnTimer = CreateTimer(360);
    b.interactionTimer = CreateTimer(120);
    b.drawColor = WHITE;
    return b;
}

BadGuy CreateRobo(Vector3 pos)
{
    BadGuy b = { 0 };
    b.type = BG_ROBO;
    b.spawnPoint = pos;
    b.spawnRadius = 110;
    b.awareRadius = 95;
    b.tetherRadius = 25;
    b.gbm_index = -1;
    b.active = false;
    b.dead = false;
    b.aware = false;
    b.startHealth = 20;   // === NEW: give the big guy some HP
    b.health = b.startHealth;
    b.pos = pos;
    b.targetPos = pos;
    b.scale = 1;
    b.speed = 3.2111;
    b.respawnTimer = CreateTimer(360);//6 minutes
    b.interactionTimer = CreateTimer(120);//2 minutes
    b.drawColor = WHITE;
    return b;
}

//end of the file stuff, important!
void InitBadGuys(Shader ghostShader)
{
    InitBadGuyModels(ghostShader);
    bg_count = 44; //increment this, every time, you add, a bg...
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
    bg[38] = CreateYeti((Vector3) { -73, 877, 2145 });
    bg[39] = CreateYeti((Vector3) { -418, 922, 2152 });
    bg[40] = CreateYeti((Vector3) { -249, 921, 2183 });
    bg[41] = CreateYeti((Vector3) { -274, 874, 2470 });
    bg[42] = CreateYeti((Vector3) { -625, 864, 2476 });
    bg[43] = CreateRobo((Vector3) { 3022.00f, 322.00f, 4042.42f });
}

static inline void BG_UpdateAll(Donogan *d, float dt)
{
    for (int b = 0; b < act_bg_count; b++) {
        int i = act_bg[b];
        if (!bg[i].active) { continue; }
        if (Vector3Distance(d->pos, bg[i].pos) > 800) //general guard to help enforce that when don is far away, bad guys get put away
        {
            bg[i].active = false;
            bg[i].dead = true;
            bgModelBorrower[bg[i].gbm_index].isInUse = false;
            bg[i].gbm_index = -1;
            d->xp += 1; //give don a point for it I guess
            continue;
        }
        //handle square spell
        if (d->squareThrowRequest && bg[i].frozen)
        {
            TraceLog(LOG_INFO, "throwing request!");
            Vector3 dir = Vector3Normalize(Vector3Subtract(bg[i].pos, d->pos));
            bg[i].throwing = true;
            bg[i].throwVel = (Vector3){ dir.x * 128.0f, 32.0f, dir.z * 128.0f }; // tweakable
        }
        if (d->state == DONOGAN_STATE_HIT)
        {
            bg[i].frozen = false;
            bg[i].throwing = false;
            bg[i].targetPos = bg[i].pos;
        }
        if (bg[i].throwing) {
            //TraceLog(LOG_INFO,"throwing...");
            // simple ballistic arc + friction horizontal slow-down
            bg[i].throwVel.y += -24.0f * dt;
            bg[i].pos.x += bg[i].throwVel.x * dt;
            bg[i].pos.z += bg[i].throwVel.z * dt;
            bg[i].pos.y += bg[i].throwVel.y * dt;

            // light air drag
            bg[i].throwVel.x *= (1.0f - 1.5f * dt);
            bg[i].throwVel.z *= (1.0f - 1.5f * dt);

            float gy = BG_GroundY(bg[i].pos);
            if (bg[i].pos.y <= gy) {
                TraceLog(LOG_INFO, "thrown landing!");
                bg[i].pos.y = gy;
                bg[i].health -= GetDamageDone(d->gs, d, ATTACK_THROW, bg[i].type);
                bg[i].targetPos = bg[i].pos;
                bg[i].throwing = false;
                bg[i].frozen = false;
            }
        }
        else
        {
            if (Vector3Distance(d->pos, bg[i].pos) < 60)
            {
                if (!d->spellTimer.running) { bg[i].frozen = false; }
                else if (HasTimerElapsed(&d->spellTimer))
                {
                    bg[i].health -= GetDamageDone(d->gs, d, ATTACK_FREEZE, bg[i].type);
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
                    bg[i].pos.y += dt / 100.0f;
                    //make em spin
                    float deltaDeg = d->yawY - d->cached_yawY;       // Donogan's spin since last frame
                    if (deltaDeg > 4 * PI) { deltaDeg = 4 * PI; } //limit spin speed
                    if (deltaDeg < -4 * PI) { deltaDeg = 4 * -PI; }
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
        }
        if (bg[i].frozen) { continue; }
        //end handle square spell, handle types next....
        if (bg[i].type == BG_GHOST)
        {
            BG_Update_Ghost(d, &bg[i], dt);
        }
        else if (bg[i].type == BG_YETI) {
            BG_Update_Yeti(d, &bg[i], dt);
            BG_UpdateAnim(&bg[i], dt);
        }
        else if (bg[i].type == BG_ROBO)
        {
            BG_Update_Robo(d, &bg[i], dt);
        }
        //update general stuff
        bg[i].box = UpdateBoundingBox(bgModelBorrower[bg[i].gbm_index].origBox,bg[i].pos);
        if (bg[i].type == BG_YETI) {//im sick of these mfn snakes on this mfn plane!
            bg[i].box.max.y += 4.5;
            bg[i].box.min.y += 4;
        }
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
    if (d->squareThrowRequest) { d->squareThrowRequest = false; }
}


//only activate one per call
bool CheckSpawnAndActivateNext(Vector3 pos)
{
    for (int b = 0; b < bg_count; b++)
    {
        if (bg[b].active) { continue; }//if its turned on, dont turn it on again
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
                    BG_AttachBorrowed(&bg[b]);   // <-- add this
                    if (bg[b].type == BG_GHOST) 
                    { 
                        bg[b].pos.y = GetTerrainHeightFromMeshXZ(bg[b].pos.x, bg[b].pos.z) - 30;
                        bg[b].state = GHOST_STATE_SPAWN;
                    }
                    else if (bg[b].type == BG_YETI) {
                        // snap to ground and start in SPAWN (will fall into PLANNING next update)
                        bg[b].pos.y = GetTerrainHeightFromMeshXZ(bg[b].pos.x, bg[b].pos.z);
                        bg[b].state = YETI_STATE_SPAWN;
                        BG_SetAnim(&bg[b], ANIM_YETI_ROAR, true); // default roar on spawn
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

#endif // BG_H
