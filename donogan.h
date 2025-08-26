#ifndef DONOGAN_H
#define DONOGAN_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>   // strcmp
#include <stdlib.h>   // malloc/free if MemAlloc missing
//me
#include "control.h"
#include "util.h"
#include "timer.h"

//bubbles
#define DON_MAX_BUBBLES 128

// ---------- Character states ----------
typedef enum {
    DONOGAN_STATE_IDLE,
    DONOGAN_STATE_WALK,
    DONOGAN_STATE_RUN,
    DONOGAN_STATE_JUMP_START,
    DONOGAN_STATE_JUMPING,
    DONOGAN_STATE_JUMP_LAND,
    DONOGAN_STATE_ROLL,
    DONOGAN_STATE_AIR_ROLL,
    DONOGAN_STATE_SWIM_IDLE,
    DONOGAN_STATE_SWIM_MOVE
} DonoganState;

// ---------- Anim IDs present in your GLB ----------
typedef enum {
    //animation : Crouch_Fwd_Loop(118 frames, 2.000000s)
    DONOGAN_ANIM_Crouch_Fwd_Loop = 0,
    //animation : Crouch_Idle_Loop(172 frames, 2.916667s)
    DONOGAN_ANIM_Crouch_Idle_Loop,
    //animation : Dance_Loop(59 frames, 1.000000s)
    DONOGAN_ANIM_Dance_Loop,
    //animation : Death01(140 frames, 2.375000s)
    DONOGAN_ANIM_Death01,
    //animation : Hit_Chest(20 frames, 0.333333s)
    DONOGAN_ANIM_Hit_Chest = 6,
    //animation : Hit_Head(25 frames, 0.416667s)
    DONOGAN_ANIM_Hit_Head,
    //animation : Idle_Loop(148 frames, 2.500000s)
    DONOGAN_ANIM_Idle_Loop,
    //animation : Interact(118 frames, 2.000000s)
    DONOGAN_ANIM_Interact = 11,
    //animation : Jog_Fwd_Loop(54 frames, 0.916667s)
    DONOGAN_ANIM_Jog_Fwd_Loop,
    //animation : Jump_Land(74 frames, 1.250000s)
    DONOGAN_ANIM_Jump_Land,
    //animation : Jump_Loop(148 frames, 2.500000s)
    DONOGAN_ANIM_Jump_Loop,
    //animation : Jump_Start(79 frames, 1.333333s)
    DONOGAN_ANIM_Jump_Start,
    //animation : Punch_Cross(59 frames, 1.000000s)
    DONOGAN_ANIM_Punch_Cross = 17,
    //animation : Punch_Enter(50 frames, 0.833333s)
    DONOGAN_ANIM_Punch_Enter,
    //animation : Punch_Jab(50 frames, 0.833333s)
    DONOGAN_ANIM_Punch_Jab,
    //animation : Push_Loop(157 frames, 2.666667s)
    DONOGAN_ANIM_Push_Loop,
    //animation : Roll(86 frames, 1.458333s)
    DONOGAN_ANIM_ROLL,
    //animation : Spell_Simple_Enter(30 frames, 0.500000s)
    DONOGAN_ANIM_Spell_Simple_Enter = 26,
    //animation : Spell_Simple_Exit(25 frames, 0.416667s)
    DONOGAN_ANIM_Spell_Simple_Exit,
    //animation : Spell_Simple_Idle_Loop(123 frames, 2.083333s)
    DONOGAN_ANIM_Spell_Simple_Idle_Loop,
    //animation : Spell_Simple_Shoot(30 frames, 0.500000s)
    DONOGAN_ANIM_Spell_Simple_Shoot,
    //animation : Sprint_Loop(40 frames, 0.666667s)
    DONOGAN_ANIM_Sprint_Loop,
    //animation : Swim_Fwd_Loop(79 frames, 1.333333s)
    DONOGAN_ANIM_Swim_Fwd_Loop,
    //animation : Swim_Idle_Loop(197 frames, 3.333333s)
    DONOGAN_ANIM_Swim_Idle_Loop,
    //animation : Walk_Loop(79 frames, 1.333333s)
    DONOGAN_ANIM_Walk_Loop = 37,
    //animation : YMCA(74 frames, 1.250000s)
    DONOGAN_ANIM_YMCA,
} DonoganAnim;

//bubbles
typedef struct Bubble {
    Vector3 pos;
    Vector3 vel;
    float   radius;
    float   life, maxLife;
    unsigned char alive;
} Bubble;
// ---------- Donogan runtime ----------
typedef struct {
    // Animation & model
    Model model;
    Texture2D tex;

    // Raw animations from GLB and a remapped copy that matches model->bones order
    unsigned int animCount;
    ModelAnimation* animsRaw;
    ModelAnimation* anims; // remapped to model bone order once at load

    // Playback
    DonoganAnim curAnimId;
    bool animLoop;
    bool animFinished;
    float animTime;     // seconds
    float animFps;      // nominal fps (24 default)
    int   curFrame;     // current applied frame

    // State
    DonoganState state;
    bool runningHeld;   // L3 held
    
    // Placement / movement
    Vector3 pos;
    float   yawY;       // face direction (radians, Y axis)
    float   scale;
    float   modelYawX;  // axis fix if needed, baked into model.transform

    // Bounds/info
    BoundingBox firstBB;
    Vector3 bbCenter;

    // Tunables
    float walkSpeed;    // meters/sec
    float runSpeed;     // meters/sec
    float turnSpeed;    // radians/sec (face direction)

    // Jump timing
    bool prevCross;     // for edge detection (X/Jump)
    bool prevCircle;     //
    float jumpTimer;    // simple air timer for JUMPING
    float minAirTime;   // seconds to stay in JUMPING before landing allowed

    // === physics fields ===
        // Simple vertical physics
    float velY;        // vertical velocity (m/s)
    float gravity;     // gravity (m/s^2), negative
    float jumpSpeed;   // initial jump speed (m/s)
    float runJumpSpeed; //when running and jumping
    float groundY;     // world Y of ground plane (meters)
    bool  onGround;    // grounded flag
    float startToLoopTime; // seconds to stay in Jump_Start before switching to Jump_Loop
    Vector3 velXZ;     // <-- NEW: horizontal velocity carried through the air
    // ===============================
    //swimming
    bool  inWater;          // are we in water volume?
    float swimSpeed;        // meters/sec in water
    float swimTurnSpeed;    // slower turn in water
    //float swimFloatOffset;  // how high to ride above water surface
    // Swim thresholds (hysteresis)
    float swimMoveEnter;  // need this stick magnitude to switch to SWIM_MOVE
    float swimMoveExit;   // drop below this to fall back to SWIM_IDLE
    Timer swimEnterToExitLock;
    // Grounding thresholds
    float groundEps;          // tiny landing epsilon (meters), e.g. 0.02
    float stepDownTolerance;  // max step-down we auto-snap to (meters), e.g. 0.35
    float liftoffBump;        // how far to raise on jump start (>= stepDownTolerance + groundEps)
    // Ground stickiness
    float fallGapThreshold;   // max drop we auto-stick to ground (meters)
    float stepUpMax;          // max upward “step” we accept instantly (meters)
    float slopeFollowRate;    // 0 = snap; >0 = smooth follow (units: 1/sec)
    float stepUpRate;        // max climb speed (m/s)
    float stepUpMaxInstant;  // small instant “pop” allowed (m)

    bool runLock;   // true = run is locked on
    bool prevL3;    // previous frame’s L3, for edge detection

    // Camera (set from preview each frame)
    float camPitch;

    // Water & seabed levels
    float waterY;      // water surface Y
    float seabedY;     // terrain/seabed Y at current XZ

    // Dive control
    Vector3 swimDiveVel;     // carries burst + continued motion
    float   swimDiveBurst;   // initial impulse (m/s)
    float   swimDiveDrag;    // velocity damping (1/sec)
    Bubble bubbles[DON_MAX_BUBBLES];
    int    bubbleHead;

    // Roll impulse (ground): seeded on Circle press and decays during roll
    Vector3 rollVel;     // carried forward velocity (XZ)
    float   rollBurst;   // initial impulse magnitude (m/s)
    float   rollDrag;    // damping (1/sec), higher = stops sooner
} Donogan;

// Assets (adjust if needed)
static const char* GLB = "models/donogan_anim.glb";
static const char* PNG = "textures/donogan.png";

// Feet world Y using model-space BB (only Y-rotation on model transform, so Y extent is stable)
static inline float DonFeetWorldY(const Donogan* d) {
    return d->pos.y + d->firstBB.min.y * d->scale;
}
static inline void DonSnapToGround(Donogan* d) {
    d->pos.y = d->groundY - d->firstBB.min.y * d->scale; // place feet exactly on ground
    d->velY = 0.0f;
    d->onGround = true;
}
/// <summary>
/// Run logic to lock and unlock
/// </summary>
/// <param name="d"></param>
/// <param name="L3"></param>
static inline void DonProcessRunToggle(Donogan* d, bool L3)
{
    bool pressed = L3 && !d->prevL3;   // rising edge
    d->prevL3 = L3;
    if (pressed) d->runLock = !d->runLock;
    // runningHeld is what the rest of your code uses to choose run/walk & jump speed
    d->runningHeld = d->runLock;       // (if you also want hold-to-run, use: d->runLock || L3)
}

// --------- Anim track name→index and remap (fixes “warpy skin”) ----------
static int AnimBoneIndexByName(const ModelAnimation* anim, const char* name) {
    for (int i = 0; i < anim->boneCount; i++) {
        if (anim->bones && anim->bones[i].name && name && (strcmp(anim->bones[i].name, name) == 0)) return i;
    }
    return -1;
}

static ModelAnimation BuildRemapped(const Model* model, const ModelAnimation* src) {
    ModelAnimation out = { 0 };
    out.boneCount = model->boneCount;
    out.frameCount = src->frameCount;
    out.bones = model->bones; // target order = model
    out.framePoses = (Transform**)MemAlloc(sizeof(Transform*) * out.frameCount);
    for (int f = 0; f < (int)out.frameCount; f++) {
        out.framePoses[f] = (Transform*)MemAlloc(sizeof(Transform) * out.boneCount);
        for (int mb = 0; mb < model->boneCount; mb++) {
            const char* mname = model->bones[mb].name;
            int ab = AnimBoneIndexByName(src, mname);
            out.framePoses[f][mb] = (ab >= 0) ? src->framePoses[f][ab] : model->bindPose[mb];
        }
    }
    return out;
}

static void FreeRemapped(ModelAnimation* a) {
    if (!a) return;
    for (int f = 0; f < (int)a->frameCount; ++f) if (a->framePoses[f]) MemFree(a->framePoses[f]);
    if (a->framePoses) MemFree(a->framePoses);
}

static inline float frand01(void) {
    return (float)GetRandomValue(0, 1000) * (1.0f / 1000.0f);
}

// Approx butt world-space anchor: ~55% up from feet, nudged backward along facing
static inline Vector3 DonButtWorld(const Donogan* d) {
    float height = (d->firstBB.max.y - d->firstBB.min.y) * d->scale;
    float feetY = DonFeetWorldY(d);                           // you already have this helper
    float buttY = feetY + 0.55f * height;                       // “hips”
    Vector3 fwd = (Vector3){ sinf(d->yawY), 0.0f, cosf(d->yawY) }; // your yaw-only forward
    Vector3 butt = d->pos;
    butt.y = buttY;
    butt = Vector3Add(butt, Vector3Scale(fwd, -0.12f * height)); // small back offset
    return butt;
}

static inline void DonSpawnBubbles(Donogan* d, int count, float strength) {
    for (int i = 0; i < count; i++) {
        Bubble* b = &d->bubbles[d->bubbleHead++ % DON_MAX_BUBBLES];
        Vector3 base = DonButtWorld(d);
        // jitter spawn around butt
        base.x += (frand01() - 0.5f) * 0.05f;
        base.y += (frand01() - 0.5f) * 0.03f;
        base.z += (frand01() - 0.5f) * 0.05f;
        // Upward + some backwash + sideways randomness
        Vector3 up = (Vector3){ 0,1,0 };
        Vector3 fwd = (Vector3){ sinf(d->yawY), 0.0f, cosf(d->yawY) }; // yaw forward
        Vector3 side = (Vector3){ cosf(d->yawY), 0.0f,-sinf(d->yawY) };
        // New (spreads faster)
        Vector3 vel = Vector3Add(Vector3Scale(up, 1.4f + 1.2f * frand01()),
            Vector3Add(Vector3Scale(fwd, -0.7f * strength),
                Vector3Scale(side, (frand01() - 0.5f) * 1.6f)));
        b->pos = base;
        b->vel = vel;
        b->radius = 0.03f + 0.04f * frand01();
        b->life = 0.0f;
        b->maxLife = 0.9f + 0.5f * frand01();
        b->alive = 1;
    }
}

static inline void DonUpdateBubbles(Donogan* d, float dt) {
    for (int i = 0; i < DON_MAX_BUBBLES; i++) {
        Bubble* b = &d->bubbles[i];
        if (!b->alive) continue;
        // simple buoyancy + gentle drag
        b->vel.y += 0.8f * dt;
        b->vel.x *= (1.0f - 0.9f * dt);
        b->vel.z *= (1.0f - 0.9f * dt);
        b->pos = Vector3Add(b->pos, Vector3Scale(b->vel, dt));
        // tiny size drift
        b->radius *= (1.0f + 0.4f * dt);
        b->life += dt;

        // kill if above surface or life over
        if (b->life >= b->maxLife || b->pos.y > d->waterY + 0.05f) b->alive = 0;
    }
}

// ---------- Init / Free ----------
static Donogan InitDonogan(void)
{
    Donogan d = (Donogan){ 0 };

    d.model = LoadModel(GLB);
    if (d.model.meshCount == 0) {
        TraceLog(LOG_ERROR, "Failed to load model: %s", GLB);
        return d;
    }

    d.tex = LoadTexture(PNG);
    if (d.tex.id != 0) {
        for (int i = 0; i < d.model.materialCount; i++)
            SetMaterialTexture(&d.model.materials[i], MATERIAL_MAP_ALBEDO, d.tex);
    }

    // Load animations and build remapped copies by bone name
    d.animsRaw = LoadModelAnimations(GLB, &d.animCount);
    if (d.animCount > 0) {
        d.anims = (ModelAnimation*)MemAlloc(sizeof(ModelAnimation) * d.animCount);
        for (unsigned i = 0; i < d.animCount; ++i) d.anims[i] = BuildRemapped(&d.model, &d.animsRaw[i]);
    }

    // Bounds + autoscale to ~2m tall
    d.firstBB = GetMeshBoundingBox(d.model.meshes[0]);
    d.bbCenter = Vector3Scale(Vector3Add(d.firstBB.min, d.firstBB.max), 0.5f);
    //float height = d.firstBB.max.y - d.firstBB.min.y;
    //d.scale = (height > 0.0001f) ? Clampf(2.0f / height, 0.01f, 100.0f) : 1.0f;
    d.scale = 1.8;
    // Pose/orient
    d.modelYawX = 0.0f; // set -90 if needed; we’ll bake it into model.transform below
    d.model.transform = MatrixMultiply(d.model.transform, MatrixRotateX(DEG2RAD * d.modelYawX));
    d.pos = (Vector3){ 0 };
    d.yawY = 0.0f;

    // State/anim defaults
    d.state = DONOGAN_STATE_IDLE;
    d.curAnimId = DONOGAN_ANIM_Idle_Loop;
    d.animLoop = true;
    d.animFinished = false;
    d.animTime = 0.0f;
    d.curFrame = 0;
    d.animFps = 24.0f; // nominal

    // Movement tunables
    d.walkSpeed = 6.2f;
    d.runSpeed = 12.8f;
    d.turnSpeed = DEG2RAD * 540.0f; // turn quickly to face motion
    d.runningHeld = false;

    // Jump timing
    d.prevCross = false;
    d.jumpTimer = 0.0f;
    d.minAirTime = 0.28f;

    // --- Physics defaults ---
    d.groundY = 0.0f;
    d.gravity = -40.0f;  // gamey gravity; tweak  (-9.81 feels floaty with 24fps anims)
    d.jumpSpeed = 12.0f;    // ~1.5m jump apex with gravity=-20
    d.runJumpSpeed = 20.0f;    //
    d.velY = 0.0f;
    d.onGround = false;
    d.startToLoopTime = 0.18f; // how long Jump_Start should play before switching to Jump_Loop
    d.velXZ = (Vector3){ 0,0,0 };   // <-- start with no horizontal velocity

    //water swimming
    d.inWater = false;
    d.swimSpeed = 13.666f;
    d.swimTurnSpeed = DEG2RAD * 240.0f;
    //d.swimFloatOffset = 0.90f;   // ~chest at surface

    d.groundEps = 0.81f;
    d.stepDownTolerance = 0.35f;  // roughly ankle height – tweak to taste
    d.liftoffBump = d.stepDownTolerance + (d.groundEps/4.0f) + 0.01f;

    d.fallGapThreshold = 1.20f;   // your “only fall if > 1.2f”
    d.stepUpMax = 0.60f;   // how high he can “step up” instantly
    d.slopeFollowRate = 0.0f;    // 0 = snap; try 12.0f for smoothing

    d.swimMoveEnter = 0.14f;  // enter when stick > 14%
    d.swimMoveExit = 0.08f;  // stay moving until < 8%

    d.stepUpRate = 6.0f;   // climbs up to 6 m/s
    d.stepUpMaxInstant = 0.25f;  // allows a small pop for jaggy ground

    d.swimEnterToExitLock = CreateTimer(0.42f);//very short

    d.runLock = false;
    d.prevL3 = false;

    d.camPitch = 0.0f;
    d.waterY = PLAYER_FLOAT_Y_POSITION;      // start same; preview will set both properly
    d.seabedY = d.groundY;

    d.swimDiveVel = (Vector3){ 0 };
    d.swimDiveBurst = 58.88f;   // try 8–14
    d.swimDiveDrag = 3.732f;    // higher = stops sooner
    //d.swimMinClear = 0.25f;   // ~ankle clearance
    
    // roll burst
    d.rollVel = (Vector3){ 0 };
    d.rollBurst = 10.0f;   // ~1.25x your run speed; tweak 12–20
    d.rollDrag = 6.5f;    // 1/sec; 6–10 gives a snappy decel

    DonSnapToGround(&d);
    return d;
}

static void FreeDonogan(Donogan* d)
{
    if (!d) return;
    if (d->anims) {
        for (unsigned i = 0; i < d->animCount; i++) FreeRemapped(&d->anims[i]);
        MemFree(d->anims);
    }
    if (d->animsRaw && d->animCount > 0) UnloadModelAnimations(d->animsRaw, d->animCount);
    if (d->tex.id) UnloadTexture(d->tex);
    if (d->model.meshCount) UnloadModel(d->model);
}

// ---------- Anim control ----------
static void DonPlay(Donogan* d, DonoganAnim anim, bool loop, bool resetTime)
{
    if (!d) return;
    if (d->curAnimId != anim || resetTime) {
        d->curAnimId = anim;
        d->animLoop = loop;
        d->animFinished = false;
        d->animTime = 0.0f;
        d->curFrame = 0;
    }
}

static void DonApplyFrame(Donogan* d)
{
    if (!d || d->animCount == 0 || !d->anims) return;
    const ModelAnimation* A = &d->anims[d->curAnimId];
    int fc = (int)A->frameCount; if (fc < 1) fc = 1;

    // Frame advance
    if (!d->animLoop) {
        if (d->curFrame >= fc - 1) { d->curFrame = fc - 1; d->animFinished = true; }
    }
    else {
        d->curFrame = d->curFrame % fc;
    }

    UpdateModelAnimation(d->model, *A, d->curFrame);
}

// ---------- State helpers ----------
static DonoganAnim AnimForState(DonoganState s)
{
    switch (s) {
    case DONOGAN_STATE_IDLE:        return DONOGAN_ANIM_Idle_Loop;
    case DONOGAN_STATE_WALK:        return DONOGAN_ANIM_Walk_Loop;
    case DONOGAN_STATE_RUN:         return DONOGAN_ANIM_Sprint_Loop; // or DONOGAN_ANIM_Jog_Fwd_Loop
    case DONOGAN_STATE_JUMP_START:  return DONOGAN_ANIM_Jump_Start;
    case DONOGAN_STATE_JUMPING:     return DONOGAN_ANIM_Jump_Loop;
    case DONOGAN_STATE_JUMP_LAND:   return DONOGAN_ANIM_Jump_Land;
    case DONOGAN_STATE_ROLL:        return DONOGAN_ANIM_ROLL;
    case DONOGAN_STATE_AIR_ROLL:    return DONOGAN_ANIM_ROLL;
    case DONOGAN_STATE_SWIM_IDLE:   return DONOGAN_ANIM_Swim_Idle_Loop;
    case DONOGAN_STATE_SWIM_MOVE:   return DONOGAN_ANIM_Swim_Fwd_Loop;
    default:                        return DONOGAN_ANIM_Idle_Loop;
    }
}

static void DonSetState(Donogan* d, DonoganState s)
{
    if (!d) return;
    if (d->state == s) return;
    d->state = s;

    // Loop only on locomotion/idle; jump phases don’t loop
    bool loop = (s == DONOGAN_STATE_IDLE 
                    || s == DONOGAN_STATE_WALK || s == DONOGAN_STATE_RUN 
                    || s == DONOGAN_STATE_JUMPING
                    || s == DONOGAN_STATE_SWIM_IDLE || s == DONOGAN_STATE_SWIM_MOVE );
    bool locomotion = (s == DONOGAN_STATE_IDLE || s == DONOGAN_STATE_WALK || s == DONOGAN_STATE_RUN
                        || s == DONOGAN_STATE_JUMPING || s == DONOGAN_STATE_JUMP_START || s == DONOGAN_STATE_JUMP_LAND
                        || s == DONOGAN_STATE_ROLL || s == DONOGAN_STATE_AIR_ROLL);
    if (!locomotion) {
        d->runLock = false;      // auto-break on swimming
        d->runningHeld = false;
    }
    DonPlay(d, AnimForState(s), loop, true);

    if (s == DONOGAN_STATE_JUMPING) d->jumpTimer = 0.0f;
}

//water helpers------------------------------------------------------------------------------------
static inline void DonClampToWater(Donogan* d) {
    // Keep the body riding at the surface
    float surfaceY = d->waterY; // treat groundY as water level 
    d->pos.y = surfaceY - d->firstBB.min.y * d->scale; // +d->swimFloatOffset;
    d->velY = 0.0f;
}

static inline void DonEnterWater(Donogan* d, float moveMag) {
    d->runLock = false;
    d->runningHeld = false;
    d->prevL3 = false;   // avoid an immediate retrigger on first frame back
    d->inWater = true;
    DonClampToWater(d);
    DonSetState(d, (moveMag > 0.1f) ? DONOGAN_STATE_SWIM_MOVE : DONOGAN_STATE_SWIM_IDLE);
}

static inline void DonExitWater(Donogan* d, float moveMag, bool runningHeld) {
    d->inWater = false;
    DonSnapToGround(d);
    if (moveMag > 0.1f) DonSetState(d, runningHeld ? DONOGAN_STATE_RUN : DONOGAN_STATE_WALK);
    else                DonSetState(d, DONOGAN_STATE_IDLE);
}
// --------------------------------------------------------------------------------------------------------

// ---------- Per-frame update (controller → state → anim/frame) ----------
static void DonUpdate(Donogan* d, const ControllerData* pad, float dt, bool freeze)
{
    if (!d) return;
    if (!freeze)
    {
        // --- Input ---
        bool padPresent = (pad != NULL);
        float lx = padPresent ? pad->normLX : 0.0f;
        float ly = padPresent ? pad->normLY : 0.0f;
        bool cross = padPresent ? pad->btnCross : IsKeyDown(KEY_SPACE);
        bool circle = padPresent ? pad->btnCircle : IsKeyDown(KEY_O);
        bool L3 = padPresent ? pad->btnL3 : IsKeyDown(KEY_LEFT_SHIFT);

        // Edge for X (jump)
        bool crossPressed = (cross && !d->prevCross);
        d->prevCross = cross;
        bool circlePressed = (circle && !d->prevCircle);
        d->prevCircle = circle;

        // --- Water locomotion (no gravity) ---
        if (d->inWater) {
            //1) Read stick as usual 1
            DonProcessRunToggle(d, L3);
            //2) choose swim idle vs move 2
            float moveMag = sqrtf(lx * lx + ly * ly);
            bool wantMove = (d->state == DONOGAN_STATE_SWIM_MOVE)
                ? (moveMag > d->swimMoveExit)
                : (moveMag > d->swimMoveEnter);
            // 3) DIVE burst on X press (edge)
            // Camera forward from yaw/pitch/roll (right-handed, Y up)
            float cy = cosf(d->yawY), sy = sinf(d->yawY);
            float cp = cosf(d->camPitch), sp = sinf(d->camPitch);
            // roll is optional for now; you can include it if you want banked dives
            Vector3 fwd = (Vector3){ sy * cp, -sp, cy * cp };
            fwd = Vector3Normalize(fwd);
            if (crossPressed) {
                d->swimDiveVel = Vector3Scale(fwd, d->swimDiveBurst);
                DonSpawnBubbles(d, 18 + GetRandomValue(0, 6), 1.0f);
            }
            // do this in a separate `if` (not `else if`) so burst + swim can happen same frame
            if (wantMove) {
                // camera-yaw basis (ignores pitch so speed doesn't collapse when looking up/down)
                Vector3 camFwd = (Vector3){ sinf(d->yawY), 0.0f, cosf(d->yawY) };
                Vector3 camRight = (Vector3){ cosf(d->yawY), 0.0f,-sinf(d->yawY) };

                // stick → world planar direction
                Vector3 moveXZ = Vector3Add(Vector3Scale(camRight, lx), Vector3Scale(camFwd, -ly));
                float m = Vector3Length(moveXZ);
                if (m > 0.001f) {
                    moveXZ = Vector3Scale(moveXZ, 1.0f / m);
                    d->pos = Vector3Add(d->pos, Vector3Scale(moveXZ, d->swimSpeed * dt)); // or dtLoc
                }
            }

            //4) Apply dive velocity
            d->pos = Vector3Add(d->pos, Vector3Scale(d->swimDiveVel, dt));
            // drag
            float drag = fmaxf(0.0f, 1.0f - d->swimDiveDrag * dt);
            d->swimDiveVel = Vector3Scale(d->swimDiveVel, drag);

            // 5) Clamp vertical between seabed (with clearance) and surface (never pop out)
            float feetOff = -d->firstBB.min.y * d->scale;  // feet offset from origin
            float minY = d->seabedY + feetOff; // + d->swimMinClear;
            float maxY = d->waterY - feetOff; // +d->swimFloatOffset; // surface ride height
            if (d->pos.y < minY) d->pos.y = minY;
            if (d->pos.y > maxY) d->pos.y = maxY;
            //6)
            DonSetState(d, wantMove ? DONOGAN_STATE_SWIM_MOVE : DONOGAN_STATE_SWIM_IDLE);
            d->onGround = false;         // prevent land logic from firing while in water
            // keep body at surface
            if (d->state == DONOGAN_STATE_SWIM_IDLE && Vector3LengthSqr(d->swimDiveVel) < 1e-6f && d->pos.y < PLAYER_FLOAT_Y_POSITION) {
                d->pos.y += dt;
            }
            // then fall through to your existing frame-stepper at the end of DonUpdate()
        }
        else
        {
            float feetY = DonFeetWorldY(d);
            d->onGround = (feetY <= d->groundY + d->groundEps);
            // --- State machine with physics ---
            switch (d->state) {
            case DONOGAN_STATE_JUMP_START:
            case DONOGAN_STATE_JUMPING: {
                // Airborne: integrate vertical physics
                d->velY += d->gravity * dt;
                d->pos.y += d->velY * dt;

                // Switch Jump_Start -> Jump_Loop after some time OR after we stop rising
                if (d->state == DONOGAN_STATE_JUMP_START) {
                    d->jumpTimer += dt;
                    if (d->jumpTimer >= d->startToLoopTime || d->velY <= 0.0f) {
                        DonSetState(d, DONOGAN_STATE_JUMPING); // loops
                    }
                }

                // Land if feet cross ground while falling
                if (d->velY <= 0.0f && DonFeetWorldY(d) <= d->groundY) {
                    DonSnapToGround(d);
                    DonSetState(d, DONOGAN_STATE_JUMP_LAND); // one-shot
                }
                d->pos = Vector3Add(d->pos, Vector3Scale(d->velXZ, (dt) * (d->runningHeld ? d->runSpeed : d->walkSpeed)));

                if (circlePressed && !d->onGround)
                {
                    // Use current planar move direction (velXZ set from preview.c each frame)
                    float m = Vector3Length(d->velXZ);
                    if (m > 0.1f) {
                        Vector3 dir = Vector3Scale(d->velXZ, 1.0f / m);
                        d->rollVel = Vector3Scale(dir, d->rollBurst);
                        // optional: face the roll direction instantly
                        d->yawY = atan2f(dir.x, dir.z);
                    }
                    else {
                        d->rollVel = (Vector3){ 0 };
                    }
                    DonSetState(d, DONOGAN_STATE_AIR_ROLL); // one-shot start
                    break;
                }

            } break;

            case DONOGAN_STATE_JUMP_LAND:
                // Non-loop; when finished → locomotion or idle
                if (d->animFinished) {
                    if (fabsf(lx) > 0.1f || fabsf(ly) > 0.1f)
                        DonSetState(d, d->runningHeld ? DONOGAN_STATE_RUN : DONOGAN_STATE_WALK);
                    else
                        DonSetState(d, DONOGAN_STATE_IDLE);
                }
                break;

            case DONOGAN_STATE_ROLL: {
                // (Optional) keep some horizontal impulse during roll:
                //old way of updating the pos when rolling //d->pos = Vector3Add(d->pos, Vector3Scale(d->velXZ, (dt) * (d->runningHeld ? d->runSpeed : d->walkSpeed)));
                // Propel forward using the seeded roll velocity
                d->pos = Vector3Add(d->pos, Vector3Scale(d->rollVel, (dt) * (d->runningHeld ? d->runSpeed : d->walkSpeed)));
                // Exponential-ish damping
                float drag = fmaxf(0.0f, 1.0f - d->rollDrag * dt);
                d->rollVel = Vector3Scale(d->rollVel, drag);

                // Stay in ROLL until the non-looping animation finishes
                if (d->animFinished) {
                    // Return to locomotion based on stick
                    if (fabsf(lx) > 0.1f || fabsf(ly) > 0.1f)
                        DonSetState(d, d->runningHeld ? DONOGAN_STATE_RUN : DONOGAN_STATE_WALK);
                    else
                        DonSetState(d, DONOGAN_STATE_IDLE);
                }
            } break;

            case DONOGAN_STATE_AIR_ROLL: {
                // Air roll = like JUMPING but with roll pose; keep vertical physics
                d->velY += d->gravity * dt;
                d->pos.y += d->velY * dt;

                // (Optional) drift horizontally with current air velocity:
                //d->pos = Vector3Add(d->pos, Vector3Scale(d->velXZ, (dt) * (d->runningHeld ? d->runSpeed : d->walkSpeed)));
                //d->pos = Vector3Add(d->pos, Vector3Scale(d->rollVel, dt));
                d->pos = Vector3Add(d->pos, Vector3Scale(d->rollVel, (dt) * (d->runningHeld ? d->runSpeed : d->walkSpeed)));
                // Exponential-ish damping
                float drag = fmaxf(0.0f, 1.0f - d->rollDrag * dt);
                d->rollVel = Vector3Scale(d->rollVel, drag);

                // If we touch ground during/after air roll, snap & exit
                if (d->velY <= 0.0f && DonFeetWorldY(d) <= d->groundY) {
                    DonSnapToGround(d);
                    DonSetState(d, DONOGAN_STATE_JUMP_LAND);
                    break;
                }

                // Otherwise, wait for the one-shot to end and then go to JUMPING (fall loop)
                if (d->animFinished) DonSetState(d, DONOGAN_STATE_JUMPING);
            } break;

            default: { // IDLE / WALK / RUN (grounded locomotion)
                // Update runningHeld etc. as you already do...
                // Hold-to-run refresh (must happen every frame on ground)
                DonProcessRunToggle(d, L3);
                // If player pressed jump, do your existing liftoff (keep it first)
                if (crossPressed && d->onGround) {
                    d->velY = d->runningHeld ? d->runJumpSpeed : d->jumpSpeed;
                    d->pos.y += d->liftoffBump;   // you already have this bump
                    d->onGround = false;
                    d->jumpTimer = 0.0f;
                    DonSetState(d, DONOGAN_STATE_JUMP_START);
                    break;
                }

                // roll:
                if (circlePressed && d->onGround) {
                    // Use current planar move direction (velXZ set from preview.c each frame)
                    float m = Vector3Length(d->velXZ);
                    if (m > 0.1f) {
                        Vector3 dir = Vector3Scale(d->velXZ, 1.0f / m);
                        d->rollVel = Vector3Scale(dir, d->rollBurst);
                        // optional: face the roll direction instantly
                        d->yawY = atan2f(dir.x, dir.z);
                    }
                    else {
                        d->rollVel = (Vector3){ 0 };
                    }
                    DonSetState(d, DONOGAN_STATE_ROLL);
                    break;
                }

                // --- Ground stick logic ---
                float targetY = d->groundY - d->firstBB.min.y * d->scale;
                float dy = targetY - d->pos.y;

                if (dy >= 0.0f) {
                    float maxUpThisFrame = d->stepUpMaxInstant + d->stepUpRate * dt;
                    float climb = (dy < maxUpThisFrame) ? dy : maxUpThisFrame;
                    d->pos.y += climb;
                    d->onGround = true;
                }
                else {
                    float drop = -dy;
                    if (drop <= d->fallGapThreshold) {
                        // follow downward (snap or smooth if you’ve set slopeFollowRate)
                        if (d->slopeFollowRate <= 0.0f) d->pos.y = targetY;
                        else d->pos.y -= drop * Clampf(d->slopeFollowRate * dt, 0.0f, 1.0f);
                        d->onGround = true;
                    }
                    else {
                        // big cliff: go airborne
                        d->onGround = false;
                        d->velY = 0.0f;
                        DonSetState(d, DONOGAN_STATE_JUMPING);
                        break;
                    }
                }

                // Decide locomotion from stick (your existing code)
                float moveMag = sqrtf(lx * lx + ly * ly);
                DonSetState(d, (moveMag > 0.1f) ? (d->runningHeld ? DONOGAN_STATE_RUN : DONOGAN_STATE_WALK)
                    : DONOGAN_STATE_IDLE);
            } break;
            }
        }
    }
    // --- Animation stepping (smooth): advance one frame every (1/fps) seconds ---
    // --- Advance anim time & frame ---
    // locomotion loops; jump start/land clamp at last frame; jump loop loops
    if (!d->animFinished) d->animTime += dt;

    const ModelAnimation* A = &d->anims[d->curAnimId];
    int frameCount = (A) ? (int)A->frameCount : 1;
    if (frameCount < 1) frameCount = 1;

    if (d->animLoop) {
        d->curFrame = (d->curFrame + 2) % frameCount;
    }
    else {
        d->curFrame+=(d->curAnimId == DONOGAN_ANIM_Jump_Land ? 11 : 2); //d->curFrame++; //do this twice because it feels slow, 10 for landing
        if (d->curFrame >= frameCount) {
            d->curFrame = frameCount - 1;
            d->animFinished = true;
        }
    }

    DonApplyFrame(d);
    DonUpdateBubbles(d, dt);
}

#include "rlgl.h"  // at top of preview.c

// ... in your 3D draw loop, after DrawModel(don.model, ...) ...
void DonDrawBubbles(const Donogan* d) {
    // draw transparent without writing depth to avoid sorting artifacts
    rlDisableDepthMask();
    for (int i = 0; i < DON_MAX_BUBBLES; i++) {
        const Bubble* b = &d->bubbles[i];
        if (!b->alive) continue;
        float t = b->life / b->maxLife;             // 0..1
        unsigned char a = (unsigned char)((1.0f - t) * 160); // fade out
        Color c = (Color){ 180, 220, 255, a };      // light blue with alpha
        DrawSphereEx(b->pos, b->radius, 8, 8, c);   // small, simple sphere
    }
    rlEnableDepthMask();
}
#endif // DONOGAN_H
