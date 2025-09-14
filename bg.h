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
    GHOST_STATE_IDLE = 0,      // wander near spawn
    GHOST_STATE_FLY_LIFT,      // take off to cruise AGL
    GHOST_STATE_FLY_CRUISE,    // forward flight to target
    GHOST_STATE_FLY_LAND,      // decelerate + descend
    GHOST_STATE_BARREL_ROLL    // temporary overlay: roll around forward axis while cruising, actually yaw spin, not a real barrel roll
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
    float spawnRadius;
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
    float   accel, decel; // rate to reach targetSpeed

    float   targetYaw;    // where we’re steering to (deg)
    float   steerTimer;   // seconds left before picking a new heading
    float   yawMaxRate;   // max yaw change (deg/s)

    float   desiredAGL;   // nominal altitude above ground (meters)
    float   minAGL, maxAGL; // don’t stay lower/higher than these for long

    float   barrelTimer;  // remaining time in barrel roll (sec)
    unsigned int rng;     // tiny instance RNG
    // --- travel target ---
    Vector3 targetPos;      // where we're flying to
    float   arriveRadius;   // how close is "arrived" (m)

    // --- flight profile ---
    float   cruiseAGL;      // nominal AGL while cruising (m)
    float   liftAGL;        // AGL to reach during lift (m)
    float   landAGL;        // AGL when "landed" (m)
    float   landSpeed;      // forward speed during landing (m/s)

    // --- fancy ---
    float   barrelRollSpeed; // deg/s around forward axis during barrel roll
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

//helpers
static inline float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
static inline float approachf(float cur, float tgt, float rate, float dt) {
    float d = tgt - cur, s = rate * dt;
    return (fabsf(d) <= s) ? tgt : (cur + (d > 0 ? s : -s));
}
static inline float wrapDeg(float a) {
    while (a > 180.0f) a -= 360.0f; while (a < -180.0f) a += 360.0f; return a;
}
static inline Vector3 fwdFromYaw(float yawDeg) {
    float r = DEG2RAD * yawDeg; return (Vector3) { sinf(r), 0.0f, cosf(r) };
}
static inline float rand01u(unsigned int* s) { *s = (*s) * 1664525u + 1013904223u; return ((*s >> 9) & 0x3FFFFFu) / 4194303.0f; }

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

static inline void BG_GhostSetTarget(BadGuy* b, Vector3 target, float arriveRadius,
    float cruiseSpeed, float cruiseAGL)
{
    b->targetPos = target;
    b->arriveRadius = arriveRadius;
    b->targetSpeed = clampf(cruiseSpeed, b->minSpeed, b->maxSpeed);
    b->cruiseAGL = cruiseAGL;
    b->desiredAGL = b->liftAGL;        // start low, LIFT will bring us up
    b->state = GHOST_STATE_FLY_LIFT;
    b->steerTimer = 0.0f;              // immediate steering toward target
}

static inline void BG_GhostReturnToSpawn(BadGuy* b) {
    BG_GhostSetTarget(b, b->spawnPoint, 5.6f, 2.2f, 3.0f);
}

BadGuy CreateGhost(Vector3 pos)
{
    BadGuy b = { 0 };
    b.type = BG_GHOST;
    b.spawnPoint = pos;
    b.spawnRadius = 10; //80
    b.gbm_index = -1;
    b.active = false;
    b.pos = pos;
    b.scale = 4;
    return b;
}

void InitBadGuys(Shader ghostShader)
{
    InitBadGuyModels(ghostShader);
    bg_count = 1; //increment this, every time, you add, a bg...
    bg = (BadGuy*)malloc(sizeof(BadGuy) * bg_count);
    bg[0] = CreateGhost((Vector3) { 3022.00f, 322.00f, 4042.42f }); //y was 319
}

static inline void BG_GhostInitFlight(BadGuy* b) {
    b->state = GHOST_STATE_IDLE;

    b->speed = 0.8f;          b->targetSpeed = 1.2f;
    b->minSpeed = 0.25f;      b->maxSpeed = 3.5f;
    b->accel = 1.8f;          b->decel = 2.2f;

    b->targetYaw = b->yaw;    b->yawMaxRate = 60.0f;
    b->steerTimer = 0.0f;

    b->desiredAGL = 3.0f;     b->minAGL = 2.0f; b->maxAGL = 6.0f;
    b->cruiseAGL = 3.2f;     b->liftAGL = 13.0f; b->landAGL = 0.35f;
    b->landSpeed = 0.6f;

    b->arriveRadius = 1.2f;
    b->barrelTimer = 0.0f;
    b->barrelRollSpeed = 420.0f; // deg/s around forward axis

    b->rng = (unsigned)(fabsf(b->pos.x * 73856093.0f) + fabsf(b->pos.z * 19349663.0f)) + 1u;
}


static inline void BG_GhostPickNewWander(BadGuy* b) {
    // +/-45° jitter around current heading; tweak speed a bit
    float jitter = (rand01u(&b->rng) * 2.0f - 1.0f) * 45.0f;
    b->targetYaw = b->yaw + jitter;
    float speedScale = 0.85f + 0.3f * rand01u(&b->rng);
    b->targetSpeed = clampf(b->targetSpeed * speedScale, b->minSpeed, b->maxSpeed);
    b->steerTimer = 1.5f + 2.0f * rand01u(&b->rng); // 1.5..3.5 sec
}

static inline bool BG_GhostMaybeStartBarrel(BadGuy* b, float dt) {
    if (b->state != GHOST_STATE_FLY_CRUISE) return false;
    if (b->barrelTimer > 0.0f) return false;
    float p = (b->speed > 1.0f ? 0.05f : 0.02f);   // ~2–5%/sec chance while cruising
    if (rand01u(&b->rng) < p * dt) {
        b->state = GHOST_STATE_BARREL_ROLL;
        b->barrelTimer = 0.85f; // ~1 spin
        return true;
    }
    return false;
}

static inline void BG_GhostAltitudeControl(BadGuy* b, float dt)
{
    float groundY = GetTerrainHeightFromMeshXZ(b->pos.x, b->pos.z);
    if (groundY < -9000.0f) groundY = b->pos.y - b->desiredAGL; // fallback
    float targetY = groundY + clampf(b->desiredAGL, b->minAGL, b->maxAGL);
    b->pos.y = approachf(b->pos.y, targetY, 2.0f, dt);
}

static inline void BG_GhostSteerAndLean(BadGuy* b, float dt, float yawToTargetDeg)
{
    // steer toward target yaw
    float dYaw = wrapDeg(yawToTargetDeg - b->yaw);
    float step = clampf(dYaw, -b->yawMaxRate * dt, b->yawMaxRate * dt);
    float prevYaw = b->yaw;
    b->yaw += step;
    float yawRate = wrapDeg(b->yaw - prevYaw) / fmaxf(dt, 1e-5f);

    // speed toward target with accel/decel
    float prevSpeed = b->speed;
    float rate = (b->targetSpeed >= b->speed) ? b->accel : b->decel;
    b->speed = approachf(b->speed, b->targetSpeed, rate, dt);
    if (b->speed < b->minSpeed) b->speed = b->minSpeed;

    // lean: pitch from accel, roll from yaw rate (bank)
    float accelF = (b->speed - prevSpeed) / fmaxf(dt, 1e-5f);
    float pitchTarget = clampf(-0.7f * accelF, -18.0f, 14.0f);
    float rollTarget = clampf(-0.35f * yawRate, -25.0f, 25.0f);

    b->pitch = approachf(b->pitch, pitchTarget, 120.0f, dt);

    if (b->state == GHOST_STATE_BARREL_ROLL) {
        b->barrelTimer -= dt;
        b->yaw += b->barrelRollSpeed * dt;           // spin around forward axis
        b->pitch = Lerp(PI,b->pitch,dt);
        if (b->barrelTimer <= 0.0f) {
            b->state = GHOST_STATE_FLY_CRUISE;
            b->yaw = fmodf(b->roll, 360.0f);
        }
    }
    else {
        b->roll = approachf(b->roll, rollTarget, 180.0f, dt);
    }

    // move forward (never backwards)
    Vector3 fwd = fwdFromYaw(b->yaw);
    b->pos.x += fwd.x * b->speed * dt;
    b->pos.z += fwd.z * b->speed * dt;
}

static inline void BG_UpdateGhostTravel(BadGuy* b, float dt)
{
    // how far / where to turn
    Vector3 toT = Vector3Subtract(b->targetPos, b->pos);
    float   d2 = toT.x * toT.x + toT.z * toT.z;
    float   dist = sqrtf(d2);
    float   yawTo = RAD2DEG * atan2f(toT.x, toT.z);

    switch (b->state)
    {
    case GHOST_STATE_FLY_LIFT:
        b->desiredAGL = b->liftAGL;          // climb first
        b->targetSpeed = fmaxf(b->targetSpeed, 1.4f);
        BG_GhostAltitudeControl(b, dt);
        BG_GhostSteerAndLean(b, dt, yawTo);
        if ((b->pos.y - GetTerrainHeightFromMeshXZ(b->pos.x, b->pos.z)) >= (b->liftAGL - 0.1f)) {
            b->desiredAGL = b->cruiseAGL;
            b->state = GHOST_STATE_FLY_CRUISE;
        }
        break;

    case GHOST_STATE_FLY_CRUISE:
        b->desiredAGL = b->cruiseAGL;
        BG_GhostAltitudeControl(b, dt);
        BG_GhostSteerAndLean(b, dt, yawTo);
        BG_GhostMaybeStartBarrel(b, dt);
        if (dist <= fmaxf(b->arriveRadius, b->speed * 0.4f)) {
            b->state = GHOST_STATE_FLY_LAND;
            b->targetSpeed = b->landSpeed;
            b->desiredAGL = b->landAGL;
        }
        break;

    case GHOST_STATE_FLY_LAND:
        BG_GhostAltitudeControl(b, dt);
        BG_GhostSteerAndLean(b, dt, yawTo);
        // close enough + near ground => idle at spawn
        if (dist <= b->arriveRadius * 0.75f) {
            float agl = b->pos.y - GetTerrainHeightFromMeshXZ(b->pos.x, b->pos.z);
            if (agl <= (b->landAGL + 0.05f)) {
                b->state = GHOST_STATE_IDLE;
                b->targetSpeed = 1.0f;
                b->pitch = approachf(b->pitch, 0.0f, 180.0f, dt);
                b->roll = approachf(b->roll, 0.0f, 180.0f, dt);
            }
        }
        break;

    case GHOST_STATE_BARREL_ROLL:
        // Treated as an overlay of cruise: keep cruising behaviors
        b->desiredAGL = b->cruiseAGL;
        BG_GhostAltitudeControl(b, dt);
        BG_GhostSteerAndLean(b, dt, yawTo);
        break;

    default: break;
    }
}


static inline void BG_UpdateGhostIdle(BadGuy* b, float dt)
{
    // --- Altitude control (XZ mesh “thingy”) ---
    float groundY = GetTerrainHeightFromMeshXZ(b->pos.x, b->pos.z);
    if (groundY < -9000.0f) groundY = b->pos.y - b->desiredAGL;   // safe fallback
    float agl = b->pos.y - groundY;
    float targetY = groundY + clampf(b->desiredAGL, b->minAGL, b->maxAGL);
    if (agl < b->minAGL) targetY = groundY + b->minAGL;
    if (agl > b->maxAGL) targetY = groundY + b->maxAGL;
    b->pos.y = approachf(b->pos.y, targetY, 2.0f, dt);            // gentle vertical

    // if we drifted out of leash, go home with travel system
    float dHome = Vector3Distance(b->pos, b->spawnPoint);
    if (dHome > b->spawnRadius || b->pos.y < PLAYER_FLOAT_Y_POSITION) { BG_GhostReturnToSpawn(b); return; }
    // --- Barrel roll (rare) ---
    if (BG_GhostMaybeStartBarrel(b, dt)) {
        // keep heading/speed; roll handled below
    }

    // --- Steering & speed (always forward, no back moves) ---
    b->steerTimer -= dt;
    if (b->steerTimer <= 0.0f) BG_GhostPickNewWander(b);

    // ease yaw toward targetYaw, bounded by max turn rate
    float dYaw = wrapDeg(b->targetYaw - b->yaw);
    float step = clampf(dYaw, -b->yawMaxRate * dt, b->yawMaxRate * dt);
    float prevYaw = b->yaw;
    b->yaw += step;
    float yawRate = wrapDeg(b->yaw - prevYaw) / fmaxf(dt, 1e-5f);  // deg/s

    // ease speed to target
    float prevSpeed = b->speed;
    float rate = (b->targetSpeed >= b->speed) ? b->accel : b->decel;
    b->speed = approachf(b->speed, b->targetSpeed, rate, dt);
    if (b->speed < b->minSpeed) b->speed = b->minSpeed;

    // --- Leaning (no front/back flips) ---
    // pitch: lean forward when accelerating, back when decelerating
    float accelF = (b->speed - prevSpeed) / fmaxf(dt, 1e-5f);
    float pitchTarget = clampf(-0.7f * accelF, -18.0f, 14.0f);    // degrees
    b->pitch = approachf(b->pitch, pitchTarget, 120.0f, dt);

    // roll: bank into the turn; in barrel-roll state, we override below
    float rollTarget = clampf(-0.35f * yawRate, -15.0f, 15.0f);
    if (b->state == GHOST_STATE_BARREL_ROLL) {
        b->barrelTimer -= dt;
        b->yaw += 360.0f * 1.2f * dt;             // 1.2 spins/sec
        if (b->barrelTimer <= 0.0f) {
            b->state = GHOST_STATE_IDLE;
            b->yaw = fmodf(b->yaw, 360.0f);
        }
    }
    else {
        b->roll = approachf(b->roll, rollTarget, 180.0f, dt);
    }

    // --- Advance forward (never backwards) ---
    Vector3 fwd = fwdFromYaw(b->yaw);
    b->pos.x += fwd.x * b->speed * dt;
    b->pos.z += fwd.z * b->speed * dt;
}

static inline void BG_UpdateAll(float dt)
{
    for (int i = 0; i < bg_count; ++i) {
        if (!bg[i].active) continue;
        if (bg[i].type == BG_GHOST)
        {
            switch (bg[i].state) {
                case GHOST_STATE_IDLE:        BG_UpdateGhostIdle(&bg[i], dt);    break;
                case GHOST_STATE_FLY_LIFT:
                case GHOST_STATE_FLY_CRUISE:
                case GHOST_STATE_FLY_LAND:
                case GHOST_STATE_BARREL_ROLL: BG_UpdateGhostTravel(&bg[i], dt);  break;
                default:                      BG_UpdateGhostIdle(&bg[i], dt);    break;
            }
        }
    }
}


//only activate one per call
bool CheckSpawnAndActivateNext(Vector3 pos)
{
    for (int b = 0; b < bg_count; b++)
    {
        if (bg[b].active) { continue; }//if its turned on, dont run it on again
        if (Vector3Distance(pos,bg[b].spawnPoint)<bg[b].spawnRadius)
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
                if (bg[b].type == BG_GHOST) { BG_GhostInitFlight(&bg[b]); }
                return true;
            }
        }
    }
    return false;
}
#endif // BG_H
