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
    float swimFloatOffset;  // how high to ride above water surface
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
    d.walkSpeed = 3.2f;
    d.runSpeed = 9.1f;
    d.turnSpeed = DEG2RAD * 540.0f; // turn quickly to face motion
    d.runningHeld = false;

    // Jump timing
    d.prevCross = false;
    d.jumpTimer = 0.0f;
    d.minAirTime = 0.28f;

    // --- Physics defaults ---
    d.groundY = 0.0f;
    d.gravity = -20.0f;  // gamey gravity; tweak  (-9.81 feels floaty with 24fps anims)
    d.jumpSpeed = 6.8f;    // ~1.5m jump apex with gravity=-20
    d.runJumpSpeed = 9.64f;    // ~1.5m jump apex with gravity=-20
    d.velY = 0.0f;
    d.onGround = false;
    d.startToLoopTime = 0.18f; // how long Jump_Start should play before switching to Jump_Loop
    d.velXZ = (Vector3){ 0,0,0 };   // <-- start with no horizontal velocity

    //water swimming
    d.inWater = false;
    d.swimSpeed = 1.6f;
    d.swimTurnSpeed = DEG2RAD * 240.0f;
    d.swimFloatOffset = 0.90f;   // ~chest at surface

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
    DonPlay(d, AnimForState(s), loop, true);

    if (s == DONOGAN_STATE_JUMPING) d->jumpTimer = 0.0f;
}

//water helpers------------------------------------------------------------------------------------
static inline void DonClampToWater(Donogan* d) {
    // Keep the body riding at the surface
    float surfaceY = d->groundY; // treat groundY as water level 
    d->pos.y = surfaceY - d->firstBB.min.y * d->scale + d->swimFloatOffset;
    d->velY = 0.0f;
}

static inline void DonEnterWater(Donogan* d, float moveMag) {
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
static void DonUpdate(Donogan* d, const ControllerData* pad, float dt)
{
    if (!d) return;

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
        // Read stick as usual
        bool padPresent = (pad != NULL);
        float lx = padPresent ? pad->normLX : 0.0f;
        float ly = padPresent ? pad->normLY : 0.0f;
        bool L3 = padPresent ? pad->btnL3 : IsKeyDown(KEY_LEFT_SHIFT);
        d->runningHeld = L3; // not used for speed in water but kept consistent

        // choose swim idle vs move
        float moveMag = sqrtf(lx * lx + ly * ly);
        DonSetState(d, (moveMag > 0.1f) ? DONOGAN_STATE_SWIM_MOVE : DONOGAN_STATE_SWIM_IDLE);

        // keep body at surface
        DonClampToWater(d);
        // (Horizontal XZ integration still happens in main.c using swimSpeed)

        // then fall through to your existing frame-stepper at the end of DonUpdate()
    }
    else
    {
        d->onGround = (DonFeetWorldY(d) <= d->groundY + 0.001f);
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
            d->pos = Vector3Add(d->pos, Vector3Scale(d->velXZ, (dt) * (d->runningHeld ? d->runSpeed : d->walkSpeed)));

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
            d->pos = Vector3Add(d->pos, Vector3Scale(d->velXZ, (dt) * (d->runningHeld ? d->runSpeed : d->walkSpeed)));

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
            // Ground snap guard
            if (!d->onGround) {
                // fell off a ledge: switch to jumping loop immediately
                DonSetState(d, DONOGAN_STATE_JUMPING);
                break;
            }

            d->runningHeld = L3;

            if (crossPressed && d->onGround) {
                // Takeoff: set vertical speed and start the jump
                d->velY = d->runningHeld ? d->runJumpSpeed : d->jumpSpeed;
                d->pos.y = d->pos.y + 0.22f;
                //d->jumpTimer = 0.0f;
                DonSetState(d, DONOGAN_STATE_JUMP_START); // one-shot start
                break;
            }
            if (circlePressed && d->onGround)
            {
                DonSetState(d, DONOGAN_STATE_ROLL); // one-shot start
                break;
            }

            // Locomotion selection from stick
            float moveMag = sqrtf(lx * lx + ly * ly);
            if (moveMag > 0.1f) DonSetState(d, d->runningHeld ? DONOGAN_STATE_RUN : DONOGAN_STATE_WALK);
            else                DonSetState(d, DONOGAN_STATE_IDLE);
        } break;
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
        d->curFrame = (d->curFrame + 1) % frameCount;
    }
    else {
        d->curFrame+=2; //d->curFrame++; //do this twice because it feels slow
        if (d->curFrame >= frameCount) {
            d->curFrame = frameCount - 1;
            d->animFinished = true;
        }
    }

    DonApplyFrame(d);
}

#endif // DONOGAN_H
