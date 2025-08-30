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
#include "core.h"
#include "control.h"
#include "util.h"
#include "timer.h"
#include "collision.h"

//bubbles
#define DON_MAX_BUBBLES 128

// ---------- Character states ----------
// ===== Donny (67 bones) ======================================================
typedef enum DonBone {
    DON_BONE_ROOT = 0,
    DON_BONE_DEF_HIPS = 1,
    DON_BONE_DEF_SPINE001 = 2,
    DON_BONE_DEF_SPINE002 = 3,
    DON_BONE_DEF_SPINE003 = 4,
    DON_BONE_DEF_NECK = 5,
    DON_BONE_DEF_NECK01 = 6,
    DON_BONE_DEF_HEAD = 7,
    DON_BONE_DEF_HEADTIP = 8,
    DON_BONE_DEF_SHOULDER_L = 9,
    DON_BONE_DEF_UPPER_ARM_L = 10,
    DON_BONE_DEF_FOREARM_L = 11,
    DON_BONE_DEF_HAND_L = 12,
    DON_BONE_DEF_F_INDEX01_L = 13,
    DON_BONE_DEF_F_INDEX02_L = 14,
    DON_BONE_DEF_F_INDEX03_L = 15,
    DON_BONE_DEF_F_INDEX04TIP_L = 16,
    DON_BONE_DEF_F_MIDDLE01_L = 17,
    DON_BONE_DEF_F_MIDDLE02_L = 18,
    DON_BONE_DEF_F_MIDDLE03_L = 19,
    DON_BONE_DEF_F_MIDDLE04TIP_L = 20,
    DON_BONE_DEF_F_PINKY01_L = 21,
    DON_BONE_DEF_F_PINKY02_L = 22,
    DON_BONE_DEF_F_PINKY03_L = 23,
    DON_BONE_DEF_F_PINKY04TIP_L = 24,
    DON_BONE_DEF_F_RING01_L = 25,
    DON_BONE_DEF_F_RING02_L = 26,
    DON_BONE_DEF_F_RING03_L = 27,
    DON_BONE_DEF_F_RING04TIP_L = 28,
    DON_BONE_DEF_THUMB01_L = 29,
    DON_BONE_DEF_THUMB02_L = 30,
    DON_BONE_DEF_THUMB03_L = 31,
    DON_BONE_DEF_THUMB04TIP_L = 32,
    DON_BONE_DEF_SHOULDER_R = 33,
    DON_BONE_DEF_UPPER_ARM_R = 34,
    DON_BONE_DEF_FOREARM_R = 35,
    DON_BONE_DEF_HAND_R = 36,
    DON_BONE_DEF_F_INDEX01_R = 37,
    DON_BONE_DEF_F_INDEX02_R = 38,
    DON_BONE_DEF_F_INDEX03_R = 39,
    DON_BONE_DEF_F_INDEX04TIP_R = 40,
    DON_BONE_DEF_F_MIDDLE01_R = 41,
    DON_BONE_DEF_F_MIDDLE02_R = 42,
    DON_BONE_DEF_F_MIDDLE03_R = 43,
    DON_BONE_DEF_F_MIDDLE04TIP_R = 44,
    DON_BONE_DEF_F_PINKY01_R = 45,
    DON_BONE_DEF_F_PINKY02_R = 46,
    DON_BONE_DEF_F_PINKY03_R = 47,
    DON_BONE_DEF_F_PINKY04TIP_R = 48,
    DON_BONE_DEF_F_RING01_R = 49,
    DON_BONE_DEF_F_RING02_R = 50,
    DON_BONE_DEF_F_RING03_R = 51,
    DON_BONE_DEF_F_RING04TIP_R = 52,
    DON_BONE_DEF_THUMB01_R = 53,
    DON_BONE_DEF_THUMB02_R = 54,
    DON_BONE_DEF_THUMB03_R = 55,
    DON_BONE_DEF_THUMB04TIP_R = 56,
    DON_BONE_DEF_THIGH_L = 57,
    DON_BONE_DEF_SHIN_L = 58,
    DON_BONE_DEF_FOOT_L = 59,
    DON_BONE_DEF_TOE_L = 60,
    DON_BONE_DEF_TOETIP_L = 61,
    DON_BONE_DEF_THIGH_R = 62,
    DON_BONE_DEF_SHIN_R = 63,
    DON_BONE_DEF_FOOT_R = 64,
    DON_BONE_DEF_TOE_R = 65,
    DON_BONE_DEF_TOETIP_R = 66,
    DON_BONE_COUNT = 67
} DonBone;

static const char* kDonBoneName[DON_BONE_COUNT] = {
    "root",
    "DEF-hips",
    "DEF-spine001",
    "DEF-spine002",
    "DEF-spine003",
    "DEF-neck",
    "DEF-neck01",
    "DEF-head",
    "DEF-headtip",
    "DEF-shoulderL",
    "DEF-upper_armL",
    "DEF-forearmL",
    "DEF-handL",
    "DEF-f_index01L",
    "DEF-f_index02L",
    "DEF-f_index03L",
    "DEF-f_index04tipL",
    "DEF-f_middle01L",
    "DEF-f_middle02L",
    "DEF-f_middle03L",
    "DEF-f_middle04tipL",
    "DEF-f_pinky01L",
    "DEF-f_pinky02L",
    "DEF-f_pinky03L",
    "DEF-f_pinky04tipL",
    "DEF-f_ring01L",
    "DEF-f_ring02L",
    "DEF-f_ring03L",
    "DEF-f_ring04tipL",
    "DEF-thumb01L",
    "DEF-thumb02L",
    "DEF-thumb03L",
    "DEF-thumb04tipL",
    "DEF-shoulderR",
    "DEF-upper_armR",
    "DEF-forearmR",
    "DEF-handR",
    "DEF-f_index01R",
    "DEF-f_index02R",
    "DEF-f_index03R",
    "DEF-f_index04tipR",
    "DEF-f_middle01R",
    "DEF-f_middle02R",
    "DEF-f_middle03R",
    "DEF-f_middle04tipR",
    "DEF-f_pinky01R",
    "DEF-f_pinky02R",
    "DEF-f_pinky03R",
    "DEF-f_pinky04tipR",
    "DEF-f_ring01R",
    "DEF-f_ring02R",
    "DEF-f_ring03R",
    "DEF-f_ring04tipR",
    "DEF-thumb01R",
    "DEF-thumb02R",
    "DEF-thumb03R",
    "DEF-thumb04tipR",
    "DEF-thighL",
    "DEF-shinL",
    "DEF-footL",
    "DEF-toeL",
    "DEF-toetipL",
    "DEF-thighR",
    "DEF-shinR",
    "DEF-footR",
    "DEF-toeR",
    "DEF-toetipR"
};

// ===== Bow (7 bones) ==========================================================
// (Two roots here: "Bone" and "Bone.004". Names kept literal, dots -> underscores)

typedef enum BowBone {
    BOW_BONE = 0,
    BOW_BONE_001 = 1,
    BOW_BONE_002 = 2,
    BOW_BONE_003 = 3,
    BOW_BONE_004 = 4,
    BOW_BONE_005 = 5,
    BOW_BONE_006 = 6,
    BOW_BONE_COUNT = 7
} BowBone;

static const char* kBowBoneName[BOW_BONE_COUNT] = {
    "Bone",
    "Bone.001",
    "Bone.002",
    "Bone.003",
    "Bone.004",
    "Bone.005",
    "Bone.006"
};

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
    DONOGAN_STATE_SWIM_MOVE,
    DONOGAN_STATE_BOW_ENTER,
    DONOGAN_STATE_BOW_AIM,
    DONOGAN_STATE_BOW_PULL,
    DONOGAN_STATE_BOW_REL,
    DONOGAN_STATE_BOW_EXIT,
    DONOGAN_STATE_SLIDE,
} DonoganState;

// ---------- Anim IDs present in your GLB ----------
typedef enum {
    DONOGAN_ANIM_PROC_BOW_ENTER = -5,
    DONOGAN_ANIM_PROC_BOW_AIM = -4,
    DONOGAN_ANIM_PROC_BOW_PULL = -3,
    DONOGAN_ANIM_PROC_BOW_REL = -2,
    DONOGAN_ANIM_PROC_BOW_EXIT = -1,
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
//proc anim
#define MAX_KEY_FRAME_BONES 16
#define MAX_KEY_FRAMES 4 //we should be able to lerp eveything between 4 key frames to get where we are going
#define MAX_KEY_FRAME_GROUPS BOW_KFG_COUNT
// Indices into Donogan.kfGroups[]
typedef enum {
    BOW_KFG_ENTER,
    BOW_KFG_AIM,
    BOW_KFG_PULL,
    BOW_KFG_REL,
    BOW_KFG_EXIT,
    BOW_KFG_COUNT
} BowKfgIndex; // ensure MAX_KEY_FRAMES_GROUPS >= BOW_KFG_COUNT
typedef float (*InterpolateFunc)(float*, float*, float*); //to from dt
typedef struct {
    DonBone boneId;
    float     rate;        // ? to mutliply by dt in the interpol functions?
    Vector3   pos;
    Quaternion rot;
    InterpolateFunc interpol;
} KeyFrameBone;
typedef struct {
    float     time;        // seconds
    int maxBones;
    KeyFrameBone kfBones[MAX_KEY_FRAME_BONES];
} KeyFrame;
typedef struct {
    DonoganState state;
    DonoganAnim anim;
    int maxKey, curKey;
    KeyFrame keyFrames[MAX_KEY_FRAMES];
} KeyFrameGroup;
//proc anim inerpol funcs
// typedef float (*InterpolateFunc)(float*, float*, float*); // to, from, dt
static float LerpFloat(float* to, float* from, float* dt) {
    float a = *from, b = *to, t = *dt;
    return a + (b - a) * t;
}
// ---------- Donogan runtime ----------
typedef struct {
    // Animation & model
    Model model;
    Texture2D tex;

    Model bowModel;
    Texture2D bowTex;
    // (optional, for later attachment tuning)
    Vector3 bowOffset;       // local offset from Donogan origin
    Vector3 bowEulerDeg;     // local rotation (degrees)
    float   bowScale;
    int bowBoneIndex;
    bool     bowMode;
    bool     prevL2Held;   // edge-detect L2
    float    bowBlend;     // 0..1 simple raise/settle timer if you want later
    bool prevL2;
    bool prevR2;

    // Raw animations from GLB and a remapped copy that matches model->bones order
    unsigned int animCount;
    ModelAnimation* animsRaw;
    ModelAnimation* anims; // remapped to model bone order once at load
    unsigned int bowAnimCount;
    ModelAnimation* bowAnimsRaw;

    // bow playback
    int   bowCur;         // -1 = none, otherwise [0..bowAnimCount-1]
    int   bowFrame;
    bool  bowLoop;
    bool  bowFinished;
    float bowTime;        // seconds
    float bowFps;         // default 24


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

    //proc anim
    KeyFrameGroup kfGroups[MAX_KEY_FRAME_GROUPS];

    // Ground contact info
    Vector3 groundNormal;    // terrain triangle normal under feet

    // Steep-slope handling
    float   slopeMinUpDot;   // cos(maxSlopeDeg). Example: cos(60°)=0.5 -> too steep if n·up < 0.5
    float   steepSlideAccel; // how quickly we accelerate along the steep face (1/sec)
    float   steepSlideMax;   // target planar speed while sliding, in "walk/run speed" units
    float   steepSlideFriction; // damping when sliding (1/sec)
    Timer slideDwell;
} Donogan;

// Assets (adjust if needed)
static const char* GLB = "models/donogan_anim.glb";
static const char* PNG = "textures/donogan.png";
static const char* BOW_GLB = "models/bow.glb";
static const char* BOW_PNG = "textures/bow.png";

// Feet world Y using model-space BB (only Y-rotation on model transform, so Y extent is stable)
static inline float DonFeetWorldY(const Donogan* d) {
    return d->pos.y + d->firstBB.min.y * d->scale;
}
static inline void DonSnapToGround(Donogan* d) {
    d->pos.y = d->groundY - d->firstBB.min.y * d->scale; // place feet exactly on ground
    d->velY = 0.0f;
    d->onGround = true;
}
//proc anim
// Helper to fill one KeyFrame with zeros + identity rotation
// Fill one KeyFrameBone with zeros + identity rotation + linear interpolator
static inline void KfBoneZero(KeyFrameBone* kb, DonBone bone) {
    kb->boneId = bone;
    kb->rate = 0.0f;                      // not used yet, but kept for future
    kb->pos = (Vector3){ 0.0f,0.0f,0.0f }; // delta translation (local)
    kb->rot = QuaternionIdentity();      // delta rotation (local)
    kb->interpol = LerpFloat;                 // linear
}

// Write a KeyFrame with N bones, all zeroed, at time t
static inline void KfMakeZeroKey(KeyFrame* kf, float t,
    const DonBone* bones, int boneCount) {
    kf->time = t;
    kf->maxBones = (boneCount > MAX_KEY_FRAME_BONES) ? MAX_KEY_FRAME_BONES : boneCount;
    for (int i = 0; i < kf->maxBones; ++i) { KfBoneZero(&kf->kfBones[i], bones[i]); }
}

//bow anim stuff
// Remove per-bone scales (set to 1) and zero any root translations so our hand attach controls placement.
static void BowStripScaleAndRootOffset(Donogan* d)
{
    if (!d || !d->bowAnimsRaw || d->bowAnimCount == 0) return;

    for (unsigned k = 0; k < d->bowAnimCount; ++k) {
        ModelAnimation* A = &d->bowAnimsRaw[k];
        if (!A || !A->framePoses) continue;

        for (int f = 0; f < (int)A->frameCount; ++f) {
            Transform* F = A->framePoses[f];

            bool firstRootDone = false;
            for (int b = 0; b < (int)A->boneCount; ++b) {
                // 1) kill per-bone scale
                F[b].scale = (Vector3){ 1,1,1 };

                // 2) zero translation on the *first* root only
                if (d->bowModel.bones[b].parent == -1) {
                    if (!firstRootDone) { F[b].translation = (Vector3){ 0,0,0 }; firstRootDone = true; }
                }
            }
        }
    }
}

static void BowPlay(Donogan* d, int clip, bool loop, bool reset)
{
    if (!d || !d->bowAnimsRaw || d->bowAnimCount == 0) return;
    if (clip < 0 || clip >= (int)d->bowAnimCount) return;

    if (reset || d->bowCur != clip) {
        d->bowCur = clip;
        d->bowLoop = loop;
        d->bowFinished = false;
        d->bowTime = 0.0f;
        d->bowFrame = 0;
    }
}

static void BowApplyFrame(Donogan* d)
{
    if (!d || d->bowCur < 0 || !d->bowAnimsRaw) return;
    const ModelAnimation* A = &d->bowAnimsRaw[d->bowCur];
    int fc = (int)A->frameCount; if (fc < 1) fc = 1;

    // Clamp/finish behavior like body
    if (!d->bowLoop) {
        if (d->bowFrame >= fc - 1) { d->bowFrame = fc - 1; d->bowFinished = true; }
    }
    else {
        d->bowFrame = d->bowFrame % fc;
    }

    UpdateModelAnimation(d->bowModel, *A, d->bowFrame);
}
// Build a quaternion that applies local X then Y then Z rotations (XYZ order)
static inline Quaternion QuatXYZ(float rx, float ry, float rz) {
    Quaternion qx = QuaternionFromAxisAngle((Vector3) { 1, 0, 0 }, rx);
    Quaternion qy = QuaternionFromAxisAngle((Vector3) { 0, 1, 0 }, ry);
    Quaternion qz = QuaternionFromAxisAngle((Vector3) { 0, 0, 1 }, rz);
    // Compose so X happens first, then Y, then Z.
    // With raymath's QuaternionMultiply(a,b) meaning "a followed by b",
    // the total for XYZ is: q = qx ∘ qy ∘ qz  →  QuaternionMultiply(QuaternionMultiply(qx, qy), qz)
    return QuaternionMultiply(QuaternionMultiply(qx, qy), qz);
}

// Convenience: degrees version
static inline Quaternion QuatXYZDeg(float dx, float dy, float dz) {
    return QuatXYZ(dx * DEG2RAD, dy * DEG2RAD, dz * DEG2RAD);
}

//
static void DonInitBowKeyframeGroups(Donogan* d)
{
    // Bones we’ll drive for the bow pose (you can add fingers later):
    const DonBone BOW_BONES[] = {
        DON_BONE_DEF_UPPER_ARM_L,
        DON_BONE_DEF_UPPER_ARM_R,
        DON_BONE_DEF_FOREARM_R,
        DON_BONE_DEF_HEAD,
    };
    const int NUM_BOW_BONES = (int)(sizeof(BOW_BONES) / sizeof(BOW_BONES[0]));

    // --- ENTER ---
    KeyFrameGroup* g0 = &d->kfGroups[BOW_KFG_ENTER];
    g0->state = DONOGAN_STATE_BOW_ENTER;
    g0->anim = DONOGAN_ANIM_PROC_BOW_ENTER;
    g0->maxKey = 1;
    g0->curKey = 0;
    KfMakeZeroKey(&g0->keyFrames[0], 0.0f, BOW_BONES, NUM_BOW_BONES);
    g0->keyFrames[0].kfBones[0].rot = QuatXYZDeg(0, 0, -85.0f);
    g0->keyFrames[0].kfBones[1].rot = QuatXYZDeg(0, 0,  85.0f);

    // --- AIM ---
    KeyFrameGroup* g1 = &d->kfGroups[BOW_KFG_AIM];
    g1->state = DONOGAN_STATE_BOW_AIM;
    g1->anim = DONOGAN_ANIM_PROC_BOW_AIM;
    g1->maxKey = 1;
    g1->curKey = 0;
    KfMakeZeroKey(&g1->keyFrames[0], 0.0f, BOW_BONES, NUM_BOW_BONES);
    g1->keyFrames[0].kfBones[0].rot = QuatXYZDeg(-2.0f, -88.0f, -12.0f);
    g1->keyFrames[0].kfBones[1].rot = QuatXYZDeg(60.0f, -60.0f, 60.0f);
    g1->keyFrames[0].kfBones[2].rot = QuatXYZDeg(55.0f,0, 0);
    g1->keyFrames[0].kfBones[3].rot = QuatXYZDeg(0, 76.0f, 0);

    // PULL
    KeyFrameGroup* g2 = &d->kfGroups[BOW_KFG_PULL];
    g2->state = DONOGAN_STATE_BOW_PULL;
    g2->anim = DONOGAN_ANIM_PROC_BOW_PULL;
    g2->maxKey = 1;
    g2->curKey = 0;
    KfMakeZeroKey(&g2->keyFrames[0], 0.0f, BOW_BONES, NUM_BOW_BONES);
    g2->keyFrames[0].kfBones[0].rot = QuatXYZDeg(-2.0f, -88.0f, -11.0f);
    g2->keyFrames[0].kfBones[1].rot = QuatXYZDeg(60.0f, -30.0f, 30.0f);
    g2->keyFrames[0].kfBones[2].rot = QuatXYZDeg(55.0f, 0, 0);
    g2->keyFrames[0].kfBones[3].rot = QuatXYZDeg(0, 76.0f, 0);

    // RELEASE
    KeyFrameGroup* g3 = &d->kfGroups[BOW_KFG_REL];
    g3->state = DONOGAN_STATE_BOW_REL;
    g3->anim = DONOGAN_ANIM_PROC_BOW_REL;
    g3->maxKey = 1;
    g3->curKey = 0;
    KfMakeZeroKey(&g3->keyFrames[0], 0.0f, BOW_BONES, NUM_BOW_BONES);
    g3->keyFrames[0].kfBones[0].rot = QuatXYZDeg(-2.0f, -88.0f, -10.0f);
    g3->keyFrames[0].kfBones[1].rot = QuatXYZDeg(60.0f, -45.0f, 45.0f);
    g3->keyFrames[0].kfBones[2].rot = QuatXYZDeg(55.0f, 0, 0);
    g3->keyFrames[0].kfBones[3].rot = QuatXYZDeg(0, 76.0f, 0);

    // --- EXIT ---
    KeyFrameGroup* g4 = &d->kfGroups[BOW_KFG_EXIT];
    g4->state = DONOGAN_STATE_BOW_EXIT;
    g4->anim = DONOGAN_ANIM_PROC_BOW_EXIT;
    g4->maxKey = 1;
    g4->curKey = 0;
    KfMakeZeroKey(&g4->keyFrames[0], 0.0f, BOW_BONES, NUM_BOW_BONES);
    g4->keyFrames[0].kfBones[0].rot = QuatXYZDeg(0, 0, -85.0f);
    g4->keyFrames[0].kfBones[1].rot = QuatXYZDeg(0, 0, 85.0f);
}

// Choose the active keyframe group based on current proc anim
static inline KeyFrameGroup* DonActiveKfGroup(Donogan* d) {
    switch (d->curAnimId) {
    case DONOGAN_ANIM_PROC_BOW_ENTER: return &d->kfGroups[BOW_KFG_ENTER];
    case DONOGAN_ANIM_PROC_BOW_AIM:   return &d->kfGroups[BOW_KFG_AIM];
    case DONOGAN_ANIM_PROC_BOW_PULL:  return &d->kfGroups[BOW_KFG_PULL];  // NEW
    case DONOGAN_ANIM_PROC_BOW_REL:   return &d->kfGroups[BOW_KFG_REL];   // NEW
    case DONOGAN_ANIM_PROC_BOW_EXIT:  return &d->kfGroups[BOW_KFG_EXIT];
    default:                          return NULL;
    }
}
// Build a matrix from a local Transform (T * R * S)
//static inline Matrix TRS(Transform t) {
//    Matrix T = MatrixTranslate(t.translation.x, t.translation.y, t.translation.z);
//    Matrix R = QuaternionToMatrix(t.rotation);
//    Matrix S = MatrixScale(t.scale.x, t.scale.y, t.scale.z);
//    return MatrixMultiply(MatrixMultiply(T, R), S);
//}
///actually SRT
static inline Matrix SRT(Transform t) {
    Matrix S = MatrixScale(t.scale.x, t.scale.y, t.scale.z);
    Matrix R = QuaternionToMatrix(t.rotation);
    Matrix T = MatrixTranslate(t.translation.x, t.translation.y, t.translation.z);
    return MatrixMultiply(S, MatrixMultiply(R, T)); // S*R*T
}
static Matrix BoneWorldFromPose(const Donogan* d, const Transform* pose, int bone) {
    Matrix M = MatrixIdentity();
    for (int b = bone; b != -1; b = d->model.bones[b].parent) {
        M = MatrixMultiply(SRT(pose[b]), M);
    }
    //if your character has its own world transform, left-multiply it here:
    M = MatrixMultiply(d->model.transform, M);
    return M;
}
// --- Local -> World for a single bone ---------------------------------------
// Computes world(boneId) = world(parent) * TRS(local(boneId))
static inline Matrix LocalToWorldMatrix(const Model* model,
    const Transform* locals, // pose locals per bone
    int boneId)
{
    Matrix M = SRT(locals[boneId]);
    int p = model->bones[boneId].parent;
    while (p >= 0) {
        Matrix Mp = SRT(locals[p]);
        M = MatrixMultiply(Mp, M); // world = parentWorld * local
        p = model->bones[p].parent;
    }
    return M;
}

// --- World -> Local for a single bone ---------------------------------------
// Given a desired world matrix for `boneId`, compute the *local* TRS needed,
// i.e. local = inverse(world(parent)) * world(boneId).
static inline Transform WorldToLocalTransform(const Model* model,
    const Transform* locals, // current pose
    int boneId,
    Matrix desiredWorld)
{
    // 1) Parent world
    Matrix parentWorld = MatrixIdentity();
    int parent = model->bones[boneId].parent;
    if (parent >= 0) parentWorld = LocalToWorldMatrix(model, locals, parent);

    // 2) Local matrix from parent space
    Matrix parentInv = MatrixInvert(parentWorld);
    Matrix localM = MatrixMultiply(parentInv, desiredWorld);

    // 3) Decompose localM -> TRS (keep it simple; scale optional)
    Transform out;
    out.translation = (Vector3){ localM.m12, localM.m13, localM.m14 };
    out.rotation = QuaternionFromMatrix(localM);

    // If you don’t animate scales, lock to 1 to avoid “arm stretching”.
    out.scale = (Vector3){ 1.0f, 1.0f, 1.0f };
    return out;
}

// World rotation of a bone from a pose (bind or current), no matrices/scales.
static Quaternion DonWorldRotFromPose(const Donogan* d, const Transform* pose, int bone)
{
    Quaternion q = pose[bone].rotation;                  // local
    int p = d->model.bones[bone].parent;
    while (p >= 0) {                                     // climb to root
        q = QuaternionNormalize(QuaternionMultiply(pose[p].rotation, q));
        p = d->model.bones[p].parent;
    }
    return q;                                            // world-space rotation
}
// Decompose a raylib Matrix into T (m12/m13/m14), R (unit quaternion), S (lengths of basis columns).
// Handles non-uniform and negative scale. Numerically stable for typical animation use.
static inline Transform MatrixToTransform(Matrix m)
{
    Transform t;

    // 1) Translation (raylib stores it in m12, m13, m14)
    t.translation = (Vector3){ m.m12, m.m13, m.m14 };

    // 2) Extract column vectors of the upper-left 3x3 (basis * scale)
    Vector3 c0 = (Vector3){ m.m0,  m.m1,  m.m2 };
    Vector3 c1 = (Vector3){ m.m4,  m.m5,  m.m6 };
    Vector3 c2 = (Vector3){ m.m8,  m.m9,  m.m10 };

    // 3) Scales are lengths of those columns
    float sx = sqrtf(c0.x * c0.x + c0.y * c0.y + c0.z * c0.z);
    float sy = sqrtf(c1.x * c1.x + c1.y * c1.y + c1.z * c1.z);
    float sz = sqrtf(c2.x * c2.x + c2.y * c2.y + c2.z * c2.z);

    // Avoid division by zero
    const float EPS = 1e-8f;
    if (sx < EPS) sx = EPS;
    if (sy < EPS) sy = EPS;
    if (sz < EPS) sz = EPS;

    // 4) Preserve negative scale sign (orientation of the basis)
    //    If det < 0, flip Z scale (common convention), and flip c2 accordingly.
    Vector3 cx = (Vector3){ c0.x / sx, c0.y / sx, c0.z / sx };
    Vector3 cy = (Vector3){ c1.x / sy, c1.y / sy, c1.z / sy };
    Vector3 cz = (Vector3){ c2.x / sz, c2.y / sz, c2.z / sz };

    float det = (cx.x * (cy.y * cz.z - cy.z * cz.y)
        - cx.y * (cy.x * cz.z - cy.z * cz.x)
        + cx.z * (cy.x * cz.y - cy.y * cz.x));

    if (det < 0.0f) {
        sz = -sz;
        cz.x = -cz.x; cz.y = -cz.y; cz.z = -cz.z;
    }

    t.scale = (Vector3){ sx, sy, sz };

    // 5) Build a pure rotation 3x3 from the normalized columns (row-major values for the formula)
    //    r[row][col] = [cx cy cz] with rows being x/y/z components
    float r00 = cx.x, r01 = cy.x, r02 = cz.x;
    float r10 = cx.y, r11 = cy.y, r12 = cz.y;
    float r20 = cx.z, r21 = cy.z, r22 = cz.z;

    // 6) Convert 3x3 rotation to quaternion (stable branch selection)
    float trace = r00 + r11 + r22;
    Quaternion q;
    if (trace > 0.0f) {
        float s = sqrtf(trace + 1.0f) * 2.0f; // s = 4*qw
        q.w = 0.25f * s;
        q.x = (r21 - r12) / s;
        q.y = (r02 - r20) / s;
        q.z = (r10 - r01) / s;
    }
    else if (r00 > r11 && r00 > r22) {
        float s = sqrtf(1.0f + r00 - r11 - r22) * 2.0f; // s = 4*qx
        q.w = (r21 - r12) / s;
        q.x = 0.25f * s;
        q.y = (r01 + r10) / s;
        q.z = (r02 + r20) / s;
    }
    else if (r11 > r22) {
        float s = sqrtf(1.0f + r11 - r00 - r22) * 2.0f; // s = 4*qy
        q.w = (r02 - r20) / s;
        q.x = (r01 + r10) / s;
        q.y = 0.25f * s;
        q.z = (r12 + r21) / s;
    }
    else {
        float s = sqrtf(1.0f + r22 - r00 - r11) * 2.0f; // s = 4*qz
        q.w = (r10 - r01) / s;
        q.x = (r02 + r20) / s;
        q.y = (r12 + r21) / s;
        q.z = 0.25f * s;
    }

    // 7) Normalize quaternion to be safe
    float qlen = sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    if (qlen > EPS) {
        q.x /= qlen; q.y /= qlen; q.z /= qlen; q.w /= qlen;
    }
    else {
        q = (Quaternion){ 0, 0, 0, 1 };
    }

    t.rotation = q;
    return t;
}


//worst thing ever, do not touch unless you need to...
static void DonApplyPoseFk(int rootBoneId, int boneId, Donogan* d, const KeyFrameBone* KB, Transform* out)
{
    if (boneId < 0 || boneId >= d->model.boneCount) return;

    int parent = d->model.bones[boneId].parent;

    if (boneId == rootBoneId)
    {
        // Compose delta ON TOP of the pose already in out[root]
        // Order: current ∘ delta  (apply delta after current local)
        Transform prev = out[rootBoneId];         // whatever the base is (bind or current clip)
        Quaternion prevRot = prev.rotation;

        // 1) Accumulate rotation in local space
        //    Using the same order you’ve been using: prev * delta
        Quaternion newRot = QuaternionNormalize(QuaternionMultiply(prevRot, KB->rot));

        // 2) Accumulate translation in local space (rotate delta by *previous* rotation)
        //    This makes KB->pos relative to the root’s local axes.
        Vector3 deltaLocal = Vector3RotateByQuaternion(KB->pos, prevRot);
        Vector3 newPos = Vector3Add(prev.translation, deltaLocal);

        out[rootBoneId].rotation = newRot;
        out[rootBoneId].translation = newPos;
    }
    else
    {
        // Parent delta: how parent moved vs bind (bind->current in parent space)
        const Quaternion qBindP = d->model.bindPose[parent].rotation;
        const Quaternion qCurP = out[parent].rotation;
        const Quaternion qDeltaP = QuaternionNormalize(QuaternionMultiply(qCurP, QuaternionInvert(qBindP)));
        // Rebuild CHILD from bind locals using the parent's delta (no stretch):
        const Quaternion childBindRot = d->model.bindPose[boneId].rotation;
        // World position: parent world + rotated local bind offset
        Vector3 childBindWorld = d->model.bindPose[boneId].translation;
        Vector3 parentBindWorld = d->model.bindPose[parent].translation;
        Vector3 childRelBindWorld = Vector3Subtract(childBindWorld, parentBindWorld);
        Vector3 childRel = Vector3RotateByQuaternion(childRelBindWorld, qDeltaP);
        out[boneId].translation = Vector3Add(out[parent].translation, childRel);
        // World rotation: parent delta * child's bind local rotation
        out[boneId].rotation = QuaternionNormalize(QuaternionMultiply(qDeltaP, childBindRot));
    }

    // Recurse
    for (int i = 0; i < d->model.boneCount; ++i)
    {
        if (d->model.bones[i].parent == boneId)
            DonApplyPoseFk(rootBoneId, i, d, KB, out);
    }
}


// Apply current group's current key (single key for now) as deltas on top of bind pose
static void DonApplyProcPoseFromKF(Donogan* d)
{
    if (!d || d->model.boneCount <= 0 || !d->model.bindPose) { return; }

    const int bc = d->model.boneCount;
    // Temp frame (1 frame) – simple and clear
    Transform* out = (Transform*)MemAlloc(sizeof(Transform) * bc);
    if (!out) return;

    // Base = bind pose (later, you can switch this to a cached GLB pose to avoid "snap")
    for (int i = 0; i < bc; ++i) { out[i] = d->model.bindPose[i]; }

    KeyFrameGroup* G = DonActiveKfGroup(d);
    if (G && G->maxKey > 0) {
        const KeyFrame* K = &G->keyFrames[G->curKey]; // one key for now
        for (int i = 0; i < K->maxBones; ++i) {
            KeyFrameBone* KB = &K->kfBones[i];
            int b = (int)KB->boneId;
            if (b >= 0 && b < bc) {
                // Delta add translation, delta multiply rotation //simple version commented out here, no recursive on children
                //out[b].translation = Vector3Add(out[b].translation, KB->pos);
                //out[b].rotation = QuaternionNormalize(QuaternionMultiply(out[b].rotation, KB->rot));
                DonApplyPoseFk(b, b,d, KB, out);
            }
        }
    }

    // Build 1-frame "animation" and push it
    Transform* framesArr[1] = { out };
    ModelAnimation A1;
    A1.boneCount = bc;
    A1.frameCount = 1;
    A1.bones = d->model.bones;
    A1.framePoses = framesArr;

    UpdateModelAnimation(d->model, A1, 0);
    MemFree(out);
}
// Tunable durations for the one-shot proc anims
#ifndef BOW_ENTER_T
#define BOW_ENTER_T 0.25f
#endif
#ifndef BOW_EXIT_T
#define BOW_EXIT_T  0.20f
#endif

// Minimal “procedural anim stepper”:
// - ENTER/EXIT finish after fixed time
// - AIM never finishes (loops/holds)
// No bone posing here; this is only to unblock the state machine.
static void DonApplyProcFrame(Donogan* d)
{
    if (!d) return;

    // 1) Apply current procedural pose from keyframes
    DonApplyProcPoseFromKF(d);

    // 2) Timing gates for enter/aim/exit
    switch (d->curAnimId) {
    case DONOGAN_ANIM_PROC_BOW_ENTER:
        if (d->animTime >= BOW_ENTER_T) d->animFinished = true;
        break;
    case DONOGAN_ANIM_PROC_BOW_AIM:
        d->animFinished = false; // holds indefinitely
        break;
    case DONOGAN_ANIM_PROC_BOW_EXIT:
        if (d->animTime >= BOW_EXIT_T) d->animFinished = true;
        break;
    default:
        d->animFinished = true;  // unknown proc id → finish immediately
        break;
    }
}


//bone print
// Print the full bone list as a tree with bind-pose data
// Forward decl
static void PrintBoneRecursive(const Model* m, int boneIndex, int depth);

// Print the full bone list as a tree with bind-pose data
void PrintModelBones(const Model* m) {
    if (!m) { printf("PrintModelBones: model == NULL\n"); return; }
    if (m->boneCount <= 0 || !m->bones) {
        printf("PrintModelBones: no bones\n");
        return;
    }
    printf("=== Bones (%d) ===\n", m->boneCount);

    // Print all roots (parent == -1), then recurse into children
    for (int i = 0; i < m->boneCount; ++i) {
        if (m->bones[i].parent == -1) {
            PrintBoneRecursive(m, i, 0);
        }
    }

    // Also list any unparented-but-non-root anomalies (just in case)
    for (int i = 0; i < m->boneCount; ++i) {
        if (m->bones[i].parent >= m->boneCount) {
            printf("[WARN] Bone %d ('%s') has invalid parent index %d\n",
                i, m->bones[i].name, m->bones[i].parent);
        }
    }
}

static void PrintBoneRecursive(const Model* m, int boneIndex, int depth) {
    const BoneInfo* bi = &m->bones[boneIndex];
    // Indent
    for (int d = 0; d < depth; ++d) printf("  ");

    // Header line with index/name/parent
    printf("└─[%3d] '%s'  parent=%d", boneIndex, bi->name, bi->parent);

    // Bind pose (if available)
    if (m->bindPose) {
        const Transform tp = m->bindPose[boneIndex];
        printf("\n");
        for (int d = 0; d < depth; ++d) printf("  ");
        printf("    T=(%.3f, %.3f, %.3f)  "
            "Rquat=(%.3f, %.3f, %.3f, %.3f)  "
            "S=(%.3f, %.3f, %.3f)\n",
            tp.translation.x, tp.translation.y, tp.translation.z,
            tp.rotation.x, tp.rotation.y, tp.rotation.z, tp.rotation.w,
            tp.scale.x, tp.scale.y, tp.scale.z);
    }
    else {
        printf("  (no bindPose)\n");
    }

    // Recurse: find children (linear scan; simple & safe)
    for (int j = 0; j < m->boneCount; ++j) {
        if (m->bones[j].parent == boneIndex) {
            PrintBoneRecursive(m, j, depth + 1);
        }
    }
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
//helper for drawing
static Matrix DonBoneGlobalMatrix(const Donogan* d, int boneIndex) {
    Matrix M = MatrixIdentity();
    const Transform* frame = d->anims[d->curAnimId].framePoses[d->curFrame];
    // Walk up the hierarchy, multiplying local (scale*rot*trans) at each parent
    for (int b = boneIndex; b != -1; b = d->model.bones[b].parent) {
        Transform t = frame[b];
        Matrix L = MatrixMultiply(
            MatrixScale(t.scale.x, t.scale.y, t.scale.z),
            MatrixMultiply(QuaternionToMatrix(t.rotation), MatrixTranslate(t.translation.x, t.translation.y, t.translation.z))
        );
        M = MatrixMultiply(L, M);
    }
    return M;
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

    //bow
    d.bowModel = LoadModel(BOW_GLB);
    d.bowAnimsRaw = LoadModelAnimations(BOW_GLB, &d.bowAnimCount);
    d.bowTex = LoadTexture(BOW_PNG);
    SetMaterialTexture(&d.bowModel.materials[0], MATERIAL_MAP_ALBEDO, d.bowTex);
    d.bowOffset = (Vector3){ 1.8f, 2.92f, 0.1f };  // start at exact Donogan origin
    d.bowEulerDeg = (Vector3){ 0, 180, 0 };
    d.bowScale = 0.76f;
    //d.bowBoneIndex = DON_BONE_DEF_SPINE002; //DON_BONE_DEF_HAND_L;
    // attach to LEFT HAND now
    d.bowBoneIndex = DON_BONE_DEF_HAND_L;

    // bow mode state
    d.bowMode = false;
    d.prevL2Held = false;
    d.bowBlend = 0.0f;

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
    d.scale = 2.8;
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

    //bow stuff for animation
    d.bowCur      = -1;
    d.bowFrame = 0;
    d.bowLoop = false;
    d.bowFinished = true;
    d.bowTime = 0.0f;
    d.bowFps = 24.0f;

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

    d.groundNormal = (Vector3){ 0,1,0 };   // safe default

    d.slopeMinUpDot = 0.65f;     // ~75°+ becomes “too steep”
    d.steepSlideAccel = 8.0f;      // ramp into the slide quickly
    d.steepSlideMax = d.runSpeed * 1.1134f;
    d.steepSlideFriction = 1.6f;    // decay a bit each frame
    d.slideDwell = CreateTimer(0.25f);


    PrintModelBones(&d.model);
    PrintModelBones(&d.bowModel);
    //proc anim setup
    BowStripScaleAndRootOffset(&d);
    DonInitBowKeyframeGroups(&d);

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
    if (d->bowAnimsRaw && d->bowAnimCount > 0) UnloadModelAnimations(d->bowAnimsRaw, d->bowAnimCount);
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
    case DONOGAN_STATE_BOW_ENTER:   return DONOGAN_ANIM_PROC_BOW_ENTER;
    case DONOGAN_STATE_BOW_AIM:     return DONOGAN_ANIM_PROC_BOW_AIM;
    case DONOGAN_STATE_BOW_PULL:    return DONOGAN_ANIM_PROC_BOW_PULL;
    case DONOGAN_STATE_BOW_REL:     return DONOGAN_ANIM_PROC_BOW_REL;
    case DONOGAN_STATE_BOW_EXIT:    return DONOGAN_ANIM_PROC_BOW_EXIT;
    case DONOGAN_STATE_SLIDE:       return DONOGAN_ANIM_Jump_Loop; // sliding
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
                    || s == DONOGAN_STATE_SWIM_IDLE || s == DONOGAN_STATE_SWIM_MOVE 
                    || s == DONOGAN_STATE_BOW_AIM || s == DONOGAN_ANIM_PROC_BOW_PULL
                    || s == DONOGAN_STATE_SLIDE);
    bool locomotion = (s == DONOGAN_STATE_IDLE || s == DONOGAN_STATE_WALK || s == DONOGAN_STATE_RUN
                        || s == DONOGAN_STATE_JUMPING || s == DONOGAN_STATE_JUMP_START || s == DONOGAN_STATE_JUMP_LAND
                        || s == DONOGAN_STATE_ROLL || s == DONOGAN_STATE_AIR_ROLL
                        || s == DONOGAN_STATE_BOW_ENTER || s == DONOGAN_STATE_BOW_AIM || s == DONOGAN_STATE_BOW_EXIT);
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
        bool cross = padPresent ? pad->btnCross : false;
        bool circle = padPresent ? pad->btnCircle : false;
        bool L3 = padPresent ? pad->btnL3 : false;
        bool L2 = padPresent ? pad->btnL2 : false;
        bool L2Pressed = L2 && !d->prevL2;
        bool L2Released = !L2 && d->prevL2;
        bool R2 = padPresent ? pad->btnR2 : false;
        bool R2Pressed = (R2 && !d->prevR2);
        bool R2Released = (!R2 && d->prevR2);
        d->prevR2 = R2;
        d->prevL2 = L2;
        //get bow ready
        if (R2Pressed && d->bowAnimCount >= 1) {
            BowPlay(d, 0, false, true);   // 0 = PULL, one-shot
        }
        if (R2Released && d->bowAnimCount >= 2) {
            BowPlay(d, 1, false, true);   // 1 = RELEASE, one-shot
        }
        // Edge for X (jump)
        bool crossPressed = (cross && !d->prevCross);
        d->prevCross = cross;
        bool circlePressed = (circle && !d->prevCircle);
        d->prevCircle = circle;

        if (onLoad)
        {
            //for sliding when too steep
            d->groundNormal = GetTerrainNormalFromMeshXZ(d->pos.x, d->pos.z);
        }

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
                //stick to ground
                // // --- Ground stick logic ---
                float targetY = d->groundY - d->firstBB.min.y * d->scale;
                float dy = targetY - d->pos.y;

                //if (dy >= 0.0f)
                d->pos.y += dy;// climb;
                d->onGround = true;
                
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

            case DONOGAN_STATE_BOW_ENTER: {
                // wait for enter (non-looping) to finish
                if (d->animFinished) {
                    DonSetState(d, DONOGAN_STATE_BOW_AIM);
                }
            } break;

            case DONOGAN_STATE_BOW_AIM: {
                // hold to stay; release to exit
                if (L2Released) {
                    DonSetState(d, DONOGAN_STATE_BOW_EXIT);
                }
                if (R2Pressed) { DonSetState(d, DONOGAN_STATE_BOW_PULL); break; } // NEW
                // (optional) you can also damp movement/turn here if you want tighter aim feel
            } break;

            case DONOGAN_STATE_BOW_PULL:
                if (L2Released) { DonSetState(d, DONOGAN_STATE_BOW_EXIT); break; } // optional: or go REL then EXIT
                if (R2Released) { DonSetState(d, DONOGAN_STATE_BOW_REL);  break; } // NEW
                // keep holding PULL while R2 held
                break;

            case DONOGAN_STATE_BOW_REL:
                // optionally allow L2 release here to chain to EXIT after REL finishes
                if (d->animFinished) {
                    if (!L2) DonSetState(d, DONOGAN_STATE_BOW_EXIT);
                    else     DonSetState(d, DONOGAN_STATE_BOW_AIM); // back to aiming
                }
                break;

            case DONOGAN_STATE_BOW_EXIT: {
                // when exit finishes, return to locomotion based on stick
                if (d->animFinished) {
                    d->bowMode = false;
                    if (fabsf(lx) > 0.1f || fabsf(ly) > 0.1f)
                        DonSetState(d, d->runningHeld ? DONOGAN_STATE_RUN : DONOGAN_STATE_WALK);
                    else
                        DonSetState(d, DONOGAN_STATE_IDLE);
                }
            } break;

            case DONOGAN_STATE_SLIDE: //sliding....slide...
            {
                // Treat like ...
                d->onGround = false;

                // 5a) Push downhill along the plane (project gravity onto plane)
                Vector3 g = (Vector3){ 0.0f, d->gravity, 0.0f }; // gravity is negative
                float gdotn = Vector3DotProduct(g, d->groundNormal);
                Vector3 aSlide = Vector3Subtract(g, Vector3Scale(d->groundNormal, gdotn)); // parallel to plane
                Vector3 aXZ = (Vector3){ aSlide.x, 0.0f, aSlide.z };

                // accelerate + friction + clamp
                d->velXZ = Vector3Add(d->velXZ, Vector3Scale(aXZ, d->steepSlideAccel * dt));
                float sp = Vector3Length(d->velXZ);
                if (sp > d->steepSlideMax && sp > 1e-5f) {
                    d->velXZ = Vector3Scale(d->velXZ, d->steepSlideMax / sp);
                }
                d->velXZ = Vector3Scale(d->velXZ, fmaxf(0.0f, 1.0f - d->steepSlideFriction * dt));

                d->pos.x += d->velXZ.x * dt;
                d->pos.z += d->velXZ.z * dt;

                // 5b) Maintain a tiny gap above the ground so we never "land"
                // 3) RE-SAMPLE surface at the NEW XZ and HARD-STICK Y to it
                float newGroundY = GetTerrainHeightFromMeshXZ(d->pos.x, d->pos.z);
                Vector3 newN = GetTerrainNormalFromMeshXZ(d->pos.x, d->pos.z);

                // If there's no ground under the new XZ, we truly left the wall: go to air.
                if (newGroundY <= -9000.0f || Vector3Length(newN) < 1e-6f) {
                    DonSetState(d, DONOGAN_STATE_JUMPING);
                    break;
                }

                d->groundY = newGroundY;
                d->groundNormal = newN;

                // feet offset and tiny hover so we never trigger "land"
                float feetOff = -d->firstBB.min.y * d->scale;
                float hover = fmaxf(0.02f, 0.5f * d->groundEps);

                // HARD set Y to follow the face (this is the key change)
                d->pos.y = d->groundY + feetOff + hover;
                d->velY = 0.0f;

                // (optional) face slide direction if moving
                if (sp > 0.05f) d->yawY = atan2f(d->velXZ.x, d->velXZ.z);

                // 5c) Exits:
                // leave if the face becomes walkable
                float upDot = d->groundNormal.y;
                if (upDot >= d->slopeMinUpDot - 0.01f && HasTimerElapsed(&d->slideDwell)) {
                    DonSnapToGround(d);
                    DonSetState(d, (fabsf(lx) > 0.1f || fabsf(ly) > 0.1f)
                        ? (d->runningHeld ? DONOGAN_STATE_RUN : DONOGAN_STATE_WALK)
                        : DONOGAN_STATE_IDLE);
                    break;
                }
                // or if we lose ground under us, go to air
                if (d->groundY < -9000.0f && HasTimerElapsed(&d->slideDwell)) {
                    d->velY = 0.0f;
                    DonSetState(d, DONOGAN_STATE_JUMPING);
                    break;
                }
                // allow a jump off the wall
                if (crossPressed) {
                    d->velY = d->runningHeld ? d->runJumpSpeed : d->jumpSpeed;
                    d->pos.y += d->liftoffBump;
                    DonSetState(d, DONOGAN_STATE_JUMP_START);
                    break;
                }
                // roll:
                if (circlePressed) {
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
            } break;

            default: { // IDLE / WALK / RUN (grounded locomotion)
                if (L2Pressed) {
                    DonSetState(d, DONOGAN_STATE_BOW_ENTER);
                    d->bowMode = true;
                    break;
                }
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
                // --- Steep-slope check: swap to falling + start sliding ---
                if(onLoad)
                {
                    // --- Steep-slope check ---
                    Vector3 n = d->groundNormal;
                    float len = Vector3Length(n);
                    if (len < 1e-6f) n = (Vector3){ 0,1,0 };  // fallback if degenerate
                    else            n = Vector3Scale(n, 1.0f / len);

                    const Vector3 UP = { 0,1,0 };
                    float upDot = Vector3DotProduct(n, UP);   // -1..+1
                    bool  tooSteep = (upDot < d->slopeMinUpDot);

                    if (tooSteep) {
                        // downhill direction = gravity projected onto the plane
                        Vector3 g = (Vector3){ 0.0f, -1.0f, 0.0f };
                        Vector3 n = d->groundNormal;
                        Vector3 along = Vector3Subtract(g, Vector3Scale(n, Vector3DotProduct(g, n)));

                        // planar XZ push
                        Vector3 slideXZ = (Vector3){ along.x, 0.0f, along.z };
                        float m = Vector3Length(slideXZ);
                        if (m > 1e-4f) slideXZ = Vector3Scale(slideXZ, 1.0f / m);

                        Vector3 target = Vector3Scale(slideXZ, d->steepSlideMax);
                        d->velXZ = Vector3Lerp(d->velXZ, target, Clampf(d->steepSlideAccel * dt, 0.0f, 1.0f));
                        d->velXZ = Vector3Scale(d->velXZ, fmaxf(0.0f, 1.0f - d->steepSlideFriction * dt));

                        ResetTimer(&d->slideDwell);
                        StartTimer(&d->slideDwell);
                        d->onGround = false;        // very important: we are not “grounded” while sliding
                        d->velY = 0.0f;         // pinned to face (we don’t accumulate airborne vertical)
                        DonSetState(d, DONOGAN_STATE_SLIDE);
                        break;
                    }

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
    if (!d->animFinished) { d->animTime += dt; }

    if (d->curAnimId >= 0)
    {
        const ModelAnimation* A = &d->anims[d->curAnimId];
        int frameCount = (A) ? (int)A->frameCount : 1;
        if (frameCount < 1) frameCount = 1;

        if (d->animLoop) {
            d->curFrame = (d->curFrame + 2) % frameCount;
        }
        else {
            d->curFrame += (d->curAnimId == DONOGAN_ANIM_Jump_Land ? 11 : 2); //d->curFrame++; //do this twice because it feels slow, 10 for landing
            if (d->curFrame >= frameCount) {
                d->curFrame = frameCount - 1;
                d->animFinished = true;
            }
        }
        DonApplyFrame(d);
    }
    else
    {
        DonApplyProcFrame(d);
    }

    if (d->bowCur >= 0)
    {
        if (!d->bowFinished) { d->bowTime += dt; }

        const ModelAnimation* A = &d->bowAnimsRaw[d->bowCur];
        int frameCount = A ? (int)A->frameCount : 1;
        if (frameCount < 1) frameCount = 1;

        int rate = 1;
        if (d->bowCur==1) { rate = 5; }//the release animation should play faster
        if (d->bowLoop)  d->bowFrame = (d->bowFrame + rate) % frameCount;
        else {
            d->bowFrame += rate;
            if (d->bowFrame >= frameCount) { d->bowFrame = frameCount - 1; d->bowFinished = true; }
        }
        BowApplyFrame(d);
    }

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
