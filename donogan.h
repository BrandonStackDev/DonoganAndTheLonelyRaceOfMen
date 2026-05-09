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
#include "interact.h"
#include "game.h"
#include "texture.h"
//bubbles
#define DON_MAX_BUBBLES 32

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
    DONOGAN_STATE_AIR_R2_SPELL_SHOOT,
    DONOGAN_STATE_AIR_R1_HAND_STAND,
    DONOGAN_STATE_AIR_R1_RELEASE,
    DONOGAN_STATE_AIR_L2_SPELL_SLAM,
    DONOGAN_STATE_AIR_L1_GUITAR_SLAM,
    DONOGAN_STATE_GROUND_L1_GUITAR_DASH,
    DONOGAN_STATE_GROUND_L1_GUITAR_SWING,
    DONOGAN_STATE_SWIM_IDLE,
    DONOGAN_STATE_SWIM_MOVE,
    DONOGAN_STATE_BOW_ENTER,
    DONOGAN_STATE_BOW_AIM,
    DONOGAN_STATE_BOW_PULL,
    DONOGAN_STATE_BOW_REL,
    DONOGAN_STATE_BOW_EXIT,
    DONOGAN_STATE_SLIDE,
    DONOGAN_STATE_PUNCH_JAB_ENTER,
    DONOGAN_STATE_PUNCH_JAB,
    DONOGAN_STATE_PUNCH_CROSS_ENTER,
    DONOGAN_STATE_PUNCH_CROSS,
    DONOGAN_STATE_PUNCH_IDLE,
    DONOGAN_STATE_SPELL_ENTER,
    DONOGAN_STATE_SPELL_IDLE,
    DONOGAN_STATE_SPELL_EXIT,
    DONOGAN_STATE_SPELL_SHOOT,
    DONOGAN_STATE_MACHINE_TURN,
    DONOGAN_STATE_WRENCH_SWING,
    DONOGAN_STATE_HIT,
    DONOGAN_STATE_DEATH,
} DonoganState;


// ---------- Anim IDs present in your GLB (+procedural negatives)----------
typedef enum {
    DONOGAN_ANIM_PROC_GROUND_L1_GUITAR_SWING = -15,
    DONOGAN_ANIM_PROC_GROUND_L1_GUITAR_DASH = -14,
    DONOGAN_ANIM_PROC_AIR_L1_GUITAR_SLAM = -13,
    DONOGAN_ANIM_PROC_AIR_L2_SPHERE_SLAM = -12,
    DONOGAN_ANIM_PROC_AIR_R1_RELEASE = -11,
    DONOGAN_ANIM_PROC_AIR_R1_HAND_STAND = -10,
    DONOGAN_ANIM_PROC_AIR_R2_SPELL_SHOOT = -9,
    DONOGAN_ANIM_PROC_WRENCH_SWING = -8,
    DONOGAN_ANIM_PROC_MACHINE_TURN = -7,
    DONOGAN_ANIM_PROC_SPELL_SHOOT = -6,
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
    BoundingBox origBox, box;
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
    SPELL_KFG_SHOOT,
    MACHINE_KFG_TURN,
    WRENCH_KFG_SWING,
    AIR_R2_KFG_SHOOT,
    AIR_R1_KFG_HAND_STAND,
    AIR_R1_KFG_RELEASE,
    AIR_L2_KFG_SPHERE_SLAM,
    AIR_L1_KFG_GUITAR_SLAM,
    GROUND_L1_KFG_GUITAR_DASH,
    GROUND_L1_KFG_GUITAR_SWING,
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
#define DON_POS_HISTORY_MAX 32
// ---------- Donogan runtime ----------
typedef struct {
    // Animation & model
    Model model;
    Texture2D tex;
    //BOW
    Model bowModel;
    Texture2D bowTex;
    // (optional, for later attachment tuning)
    Vector3 bowOffset;       // local offset from Donogan origin
    Vector3 bowEulerDeg;     // local rotation (degrees)
    float   bowScale;
    int bowBoneIndex;
    bool     bowMode, hasBow;
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


    //WRENCH
    bool     wrenchMode, hasWrench;
    Model wrenchModel;
    Texture2D wrenchTex;

    //GUITAR!!!!
    // Ground guitar attack
    int guitarGroundTargetIndex;
    Vector3 guitarGroundTargetPos;
    Vector3 guitarGroundDashStart;
    float guitarGroundDashTimer;
    bool guitarGroundHitDone;
    //GUITAR
    Model guitarModel;
    Texture2D guitarTex;
    float guitarScale;
    Vector3 guitarGripOffset;
    Vector3 guitarGripEulerDeg;

    // Current animated/proc pose cache.
    // Used for attaching held props to bones.
    Transform poseNow[DON_BONE_COUNT];
    bool poseNowValid;

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
    Vector3 pos, oldPos;
    float   yawY;       // face direction (radians, Y axis)
    float   scale;
    float   modelYawX;  // axis fix if needed, baked into model.transform

    // Bounds/info
    BoundingBox firstBB;
    BoundingBox origBB, box;
    BoundingBox origInnerBB, innerBox;
    BoundingBox origOuterBB, outerBox;
    Vector3 bbCenter;

    // Tunables
    float walkSpeed;    // meters/sec
    float runSpeed;     // meters/sec
    float turnSpeed;    // radians/sec (face direction)

    // Jump timing
    bool prevCross;     // for edge detection (X/Jump)
    bool prevCircle;     //
    bool prevSquare;
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
    bool prevL1;
    bool prevR1;

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

    Arrow arrows[MAX_ARROWS];
    int   arrowHead;          // ring buffer cursor
    float arrowLen;           // meters, total arrow length
    float arrowShaftR;        // radius of shaft
    float arrowHeadLen;       // cone length
    float arrowHeadR;         // cone base radius
    float arrowDrag;          // 1/sec, aerodynamic damping
    float arrowMaxLife;       // seconds until auto-despawn
    Vector3 arrowOffset;       // local offset from Donogan origin
    float arrowGravity;        // m/s^2, negative; e.g. -12 (not -40)
    float arrowDragForward;    // 1/sec, drag along flight direction (small)
    float arrowDragPerp;       // 1/sec, drag perpendicular to flight (bigger)
    float arrowNoCollideTime;  // seconds to ignore ground right after launch
    float bowTurnSpeed;
    float bowDrawTLatch;        // last pull amount [0..1]
    float bowReleaseCamHold;    // seconds to keep aim camera active after release

    //for conversation and interaction
    bool isTalking;
    Timer talkStartTimer;
    Timer interactionLimitTimer;
    TALK_TYPE who;

    //for hit state
    Timer hitTimer;
    Color drawColor;

    //health and mana
    int health, mana, maxHealth, maxMana, xp, level;
    Timer spellTimer;
    float cached_yawY;
    float shook;
    bool squareThrowRequest;
    GameState *gs;

    //has things?
    bool unlockedTruck;

    // moving-platform glue
    bool jumpPressedEdge;   // set each frame from crossPressed
    bool gluedToPlatform;   // true while mover glue owns Donny
    int  gluedPlatId;       // plats[] index, -1 when none
    //shark
    bool eatenByShark;
    Timer eatenTimer;
    //in home
    bool inHome;
    //punching
    BoundingBox punchBox;
    bool punching;
    //rocket/hover.space mode for truck
    bool canHasCheeseburger;
    //wizard
    bool talkedToBlueWizard;
    //money. its a crime.
    float money;
    int  galBooksGiven;
    bool hasGuitar;
    int  aliBooksGiven;
    bool alisterEvilRevealed;

    //jump attacks
    bool ja_l1_unlocked;
    bool ja_r1_unlocked;
    bool ja_l2_unlocked;
    bool ja_r2_unlocked;
    //history
    Vector3 posHistory[DON_POS_HISTORY_MAX];
    int posHistoryHead;
    int posHistoryCount;
} Donogan;

typedef struct SpellBall {
    Vector3 pos, vel;
    float   radius, growRate;
    float   life;      // seconds
    int     alive;
} SpellBall;

#define MAX_BALLS 32
SpellBall balls[MAX_BALLS] = { 0 };

typedef struct AirL2SlamSphere {
    bool active;
    bool lockedToGround;

    Vector3 pos;

    float radius;
    float preImpactMaxRadius;
    float maxRadius;

    float airGrowRate;
    float groundGrowRate;

    float groundY;

    // After it reaches max size, linger briefly, then vanish.
    float life;

    // Damage pulse so it does not damage every single frame.
    float damageTimer;
    bool damagePulse;
} AirL2SlamSphere;

static AirL2SlamSphere gAirL2Slam = { 0 };

//protos
Vector3 RotYawOffset(Vector3 localOff, float yaw, float scale, bool useScale);
void DonGetProcBlendKeys(const Donogan* d, const KeyFrameGroup* G, int* outKeyA, int* outKeyB, float* outT);
static void DonSetState(Donogan* d, DonoganState s);

static inline bool DonAirL2SlamIsActive(void)
{
    return gAirL2Slam.active;
}

static inline bool DonAirL2SlamCanDamage(void)
{
    // Let it damage once it is big enough or once it hits the ground.
    return gAirL2Slam.active &&
        (gAirL2Slam.lockedToGround || gAirL2Slam.radius >= 1.25f);
}

static inline Vector3 DonAirL2SlamHandPos(const Donogan* d)
{
    // Fake "near fingertip" position.
    // x = Don's right, y = up, z = forward.
    Vector3 off = { 0.95f, 5.55f, 0.10f };
    return Vector3Add(d->pos, RotYawOffset(off, d->yawY, d->scale, false));
}

static inline void DonAirL2SlamSpawnAtHand(const Donogan* d)
{
    gAirL2Slam.active = true;
    gAirL2Slam.lockedToGround = false;

    gAirL2Slam.pos = DonAirL2SlamHandPos(d);

    gAirL2Slam.radius = 0.40f;
    gAirL2Slam.preImpactMaxRadius = 2.40f;
    gAirL2Slam.maxRadius = 24.0f;

    gAirL2Slam.airGrowRate = 7.0f;
    gAirL2Slam.groundGrowRate = 18.0f;

    gAirL2Slam.groundY = d->groundY;
    gAirL2Slam.life = 0.18f;

    gAirL2Slam.damageTimer = 0;
    gAirL2Slam.damagePulse = true;
}

static inline void DonAirL2SlamFollowHand(const Donogan* d)
{
    if (!gAirL2Slam.active) return;
    if (gAirL2Slam.lockedToGround) return;

    gAirL2Slam.pos = DonAirL2SlamHandPos(d);
}

static inline void DonAirL2SlamLockToGround(const Donogan* d)
{
    if (!gAirL2Slam.active) return;

    gAirL2Slam.lockedToGround = true;
    gAirL2Slam.groundY = d->groundY;

    // Center follows Don's XZ impact point.
    gAirL2Slam.pos.x = d->pos.x;
    gAirL2Slam.pos.z = d->pos.z;

    // Keep the sphere visually coming out of the ground instead of totally buried.
    gAirL2Slam.pos.y = gAirL2Slam.groundY + gAirL2Slam.radius * 0.35f;

    // Immediate damage pulse on impact.
    gAirL2Slam.damageTimer = 0;
    gAirL2Slam.damagePulse = true;
}

static inline void UpdateAirL2SlamSphere(float dt)
{
    gAirL2Slam.damagePulse = false;

    if (!gAirL2Slam.active) return;

    gAirL2Slam.damageTimer -= dt;
    if (gAirL2Slam.damageTimer <= 0)
    {
        gAirL2Slam.damagePulse = true;
        gAirL2Slam.damageTimer = 0.13f;
    }

    if (!gAirL2Slam.lockedToGround)
    {
        gAirL2Slam.radius += gAirL2Slam.airGrowRate * dt;

        if (gAirL2Slam.radius > gAirL2Slam.preImpactMaxRadius)
        {
            gAirL2Slam.radius = gAirL2Slam.preImpactMaxRadius;
        }

        return;
    }

    gAirL2Slam.radius += gAirL2Slam.groundGrowRate * dt;

    if (gAirL2Slam.radius >= gAirL2Slam.maxRadius)
    {
        gAirL2Slam.radius = gAirL2Slam.maxRadius;
        gAirL2Slam.life -= dt;

        if (gAirL2Slam.life <= 0)
        {
            gAirL2Slam.active = false;
            gAirL2Slam.lockedToGround = false;
            gAirL2Slam.damagePulse = false;
        }
    }

    // As it grows, keep it rising out of the ground.
    gAirL2Slam.pos.y = gAirL2Slam.groundY + gAirL2Slam.radius * 0.35f;
}
static bool ShowGuitar(Donogan* d)
{
    if (!d) return false;

    return d->state == DONOGAN_STATE_AIR_L1_GUITAR_SLAM ||
        d->state == DONOGAN_STATE_GROUND_L1_GUITAR_DASH ||
        d->state == DONOGAN_STATE_GROUND_L1_GUITAR_SWING;
}
static inline bool DonIsAirAttackState(const Donogan* d)
{
    if (!d) return false;

    return d->state == DONOGAN_STATE_AIR_R2_SPELL_SHOOT ||
        d->state == DONOGAN_STATE_AIR_R1_HAND_STAND ||
        d->state == DONOGAN_STATE_AIR_R1_RELEASE ||
        d->state == DONOGAN_STATE_AIR_L2_SPELL_SLAM ||
        d->state == DONOGAN_STATE_AIR_L1_GUITAR_SLAM;
}

static inline bool DonCanTakeBadGuyTouchDamage(const Donogan* d)
{
    if (!d) return true;

    if (d->state == DONOGAN_STATE_AIR_R1_RELEASE) { return false; }
    if (d->state == DONOGAN_STATE_AIR_R1_HAND_STAND) { return false; }
    if (d->state == DONOGAN_STATE_AIR_L2_SPELL_SLAM) { return false; }
    if (d->state == DONOGAN_STATE_AIR_L1_GUITAR_SLAM) { return false; }
    if (d->state == DONOGAN_STATE_GROUND_L1_GUITAR_DASH) { return false; }
    if (d->state == DONOGAN_STATE_GROUND_L1_GUITAR_SWING) { return false; }

    return true;
}
static inline bool Don_TryStartAirL2SphereSlam(Donogan* d)
{
    if (!d) return false;
    if (!d->ja_l2_unlocked) return false;
    if (d->mana < 20) return false;

    d->mana -= 20;

    DonAirL2SlamSpawnAtHand(d);

    d->gluedToPlatform = false;
    d->gluedPlatId = -1;

    // Slam downward.
    if (d->velY > -10) d->velY = -10;

    // Keep a little drift but mostly commit to the slam.
    d->velXZ = Vector3Scale(d->velXZ, 0.30f);
    d->rollVel = (Vector3){ 0 };

    d->onGround = false;

    DonSetState(d, DONOGAN_STATE_AIR_L2_SPELL_SLAM);
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////
static inline void Don_UpdateBoxes(Donogan* d)
{
    if (!d) return;

    d->box = UpdateBoundingBox(d->origBB, d->pos);
    d->innerBox = UpdateBoundingBox(d->origInnerBB, d->pos);
    d->outerBox = UpdateBoundingBox(d->origOuterBB, d->pos);
}
static inline Transform DonGetPoseOrBind(const Donogan* d, int boneId)
{
    Transform t = { 0 };

    if (!d) return t;

    if (boneId < 0 || boneId >= d->model.skeleton.boneCount)
    {
        return t;
    }

    if (d->poseNowValid && boneId < DON_BONE_COUNT)
    {
        return d->poseNow[boneId];
    }

    if (d->model.skeleton.bindPose)
    {
        return d->model.skeleton.bindPose[boneId];
    }

    return t;
}

static inline Matrix DonTransformToMatrix(Transform t)
{
    Matrix R = QuaternionToMatrix(QuaternionNormalize(t.rotation));
    Matrix T = MatrixTranslate(t.translation.x, t.translation.y, t.translation.z);

    // Matches the style used elsewhere: local rotation, then local translation.
    return MatrixMultiply(R, T);
}

static inline Matrix DonBuildBoneAttachmentMatrix(
    const Donogan* d,
    int boneId,
    Vector3 localOffset,
    Vector3 localEulerDeg,
    float itemScale
)
{
    if (!d)
    {
        return MatrixIdentity();
    }

    Transform bt = DonGetPoseOrBind(d, boneId);

    Matrix Sitem = MatrixScale(itemScale, itemScale, itemScale);

    Matrix Ritem = MatrixRotateXYZ((Vector3) {
        localEulerDeg.x* DEG2RAD,
            localEulerDeg.y* DEG2RAD,
            localEulerDeg.z* DEG2RAD
    });

    Matrix Titem = MatrixTranslate(localOffset.x, localOffset.y, localOffset.z);

    Matrix Mbone = DonTransformToMatrix(bt);

    Matrix Schar = MatrixScale(d->scale, d->scale, d->scale);
    Matrix Rchar = d->model.transform;
    Matrix Tchar = MatrixTranslate(d->pos.x, d->pos.y, d->pos.z);

    // item local -> grip offset -> bone pose -> Donogan scale -> Donogan transform -> world
    Matrix itemLocal = MatrixMultiply(Sitem, MatrixMultiply(Ritem, Titem));
    Matrix charWorld = MatrixMultiply(Schar, MatrixMultiply(Rchar, Tchar));

    return MatrixMultiply(itemLocal, MatrixMultiply(Mbone, charWorld));
}
static inline void Don_ResetPositionHistory(Donogan* d)
{
    if (!d) return;

    d->posHistoryHead = 0;
    d->posHistoryCount = DON_POS_HISTORY_MAX;

    for (int i = 0; i < DON_POS_HISTORY_MAX; i++)
    {
        d->posHistory[i] = d->pos;
    }
}

static inline void Don_RecordPositionHistory(Donogan* d)
{
    if (!d) return;

    d->posHistory[d->posHistoryHead] = d->pos;
    d->posHistoryHead = (d->posHistoryHead + 1) % DON_POS_HISTORY_MAX;

    if (d->posHistoryCount < DON_POS_HISTORY_MAX)
    {
        d->posHistoryCount++;
    }
}

static inline Vector3 Don_GetHistoryPosition(const Donogan* d, int framesBack)
{
    if (!d) return (Vector3) { 0 };

    if (framesBack < 1) framesBack = 1;
    if (framesBack > d->posHistoryCount) framesBack = d->posHistoryCount;
    if (framesBack > DON_POS_HISTORY_MAX) framesBack = DON_POS_HISTORY_MAX;

    int idx = d->posHistoryHead - framesBack;
    while (idx < 0) idx += DON_POS_HISTORY_MAX;

    return d->posHistory[idx];
}

Vector3 RotYawOffset(Vector3 localOff, float yaw, float scale, bool useScale);
// Optional external targeting hook.
// preview.c can set this after bg.h is included.
// Return true if a target was found, false to use straight-down fallback.
typedef bool (*DonAirR2FindTargetFn)(const Donogan* d, Vector3 spawn, Vector3* outTarget);

static DonAirR2FindTargetFn gDonAirR2FindTargetFn = NULL;

static inline void Don_SetAirR2FindTargetHook(DonAirR2FindTargetFn fn)
{
    gDonAirR2FindTargetFn = fn;
}
typedef bool (*DonGroundGuitarFindTargetFn)(const Donogan* d, int* outIndex, Vector3* outTarget);

static DonGroundGuitarFindTargetFn gDonGroundGuitarFindTargetFn = NULL;

static inline void Don_SetGroundGuitarFindTargetHook(DonGroundGuitarFindTargetFn fn)
{
    gDonGroundGuitarFindTargetFn = fn;
}

static inline void DonFacePointXZ(Donogan* d, Vector3 p)
{
    if (!d) return;

    Vector3 dir = Vector3Subtract(p, d->pos);
    dir.y = 0.0f;

    if (Vector3LengthSqr(dir) > 0.0001f)
    {
        d->yawY = atan2f(dir.x, dir.z);
        d->model.transform = MatrixRotateY(d->yawY);
    }
}

static inline bool DonGroundGuitarHitWindow(const Donogan* d)
{
    return d &&
        d->state == DONOGAN_STATE_GROUND_L1_GUITAR_SWING &&
        d->animTime >= 0.16f &&
        d->animTime <= 0.36f &&
        !d->guitarGroundHitDone &&
        d->guitarGroundTargetIndex >= 0;
}

static inline bool Don_TryStartGroundL1GuitarDash(Donogan* d)
{
    if (!d) return false;
    if (!d->hasGuitar) return false;
    //if (!d->ja_l1_unlocked) return false;

    int targetIndex = -1;
    Vector3 targetPos = { 0 };
    bool foundTarget = false;

    if (gDonGroundGuitarFindTargetFn)
    {
        foundTarget = gDonGroundGuitarFindTargetFn(d, &targetIndex, &targetPos);
    }

    // No target? Still do the move.
    // Dash 12 feet straight forward, then swing at empty air.
    if (!foundTarget)
    {
        Vector3 fwd = { sinf(d->yawY), 0.0f, cosf(d->yawY) };

        targetIndex = -1;
        targetPos = Vector3Add(d->pos, Vector3Scale(fwd, 12.0f));
        targetPos.y = d->pos.y;
    }

    d->guitarGroundTargetIndex = targetIndex;
    d->guitarGroundTargetPos = targetPos;
    d->guitarGroundDashStart = d->pos;
    d->guitarGroundDashTimer = 0.0f;
    d->guitarGroundHitDone = false;

    d->velY = 0.0f;
    d->velXZ = (Vector3){ 0 };
    d->rollVel = (Vector3){ 0 };
    d->onGround = true;

    DonFacePointXZ(d, targetPos);
    DonSetState(d, DONOGAN_STATE_GROUND_L1_GUITAR_DASH);

    return true;
}
static void SpawnAirR2DownBall(const Donogan* d, SpellBall* b)
{
    // Same basic spell ball, but spawned from above/chest/hand area.
    Vector3 spawnOff = { 0, 2.6f, 0.25f };
    Vector3 spawn = Vector3Add(d->pos, RotYawOffset(spawnOff, d->yawY, d->scale, false));

    const float ballSpeed = 38.0f;

    Vector3 dir = { 0, -1.0f, 0 };

    // Ask preview/bg side for a target.
    // If none found, keep the old straight-down shot.
    if (gDonAirR2FindTargetFn)
    {
        Vector3 target = { 0 };

        if (gDonAirR2FindTargetFn(d, spawn, &target))
        {
            Vector3 toTarget = Vector3Subtract(target, spawn);

            if (Vector3LengthSqr(toTarget) > 0.0001f)
            {
                dir = Vector3Normalize(toTarget);
            }
        }
    }

    b->pos = spawn;
    b->vel = Vector3Scale(dir, ballSpeed);

    // Same values as regular spell ball.
    b->radius = 0.35f;
    b->growRate = 3.5f;
    b->life = 3.12f;
    b->alive = 1;
}

static inline bool Don_TryFireAirR2Spell(Donogan* d)
{
    if (!d) return false;
    if (!d->ja_r2_unlocked) return false;
    if (d->mana < 8) return false;

    for (int i = 0; i < MAX_BALLS; ++i)
    {
        if (!balls[i].alive)
        {
            SpawnAirR2DownBall(d, &balls[i]);
            d->mana -= 8;
            return true;
        }
    }

    return false;
}
// Assets (adjust if needed)
static const char* GLB = "models/donogan_anim.glb";
static const char* GLB_ANIM = "models/donogan_anim.glb";
static const char* PNG = "textures/donogan.png";
static const char* BOW_GLB = "models/bow.glb";
static const char* BOW_PNG = "textures/bow.png";

static inline bool DonIsPunching(const Donogan* d)
{
    return d->state == DONOGAN_STATE_PUNCH_JAB ||
        d->state == DONOGAN_STATE_PUNCH_CROSS;
}
static inline BoundingBox DonMakePunchBox(const Donogan* d)
{
    Vector3 fwd = {
        sinf(d->yawY),
        0,
        cosf(d->yawY)
    };

    float size = 2.2f;      // cube size, tune this
    float reach = 2.1f;     // distance forward, tune this

    Vector3 c = Vector3Add(d->pos, Vector3Scale(fwd, reach));
    c.y = d->outerBox.min.y + 0.55f * (d->outerBox.max.y - d->outerBox.min.y);

    return (BoundingBox) {
        { c.x - size * 0.5f, c.y - size * 0.5f, c.z - size * 0.5f },
        { c.x + size * 0.5f, c.y + size * 0.5f, c.z + size * 0.5f }
    };
}
//wrench
static inline bool DonIsWrenchSwinging(const Donogan* d)
{
    return d->state == DONOGAN_STATE_WRENCH_SWING &&
        d->animTime >= 0.10f &&
        d->animTime <= 0.32f;
}

static inline BoundingBox DonMakeWrenchBox(const Donogan* d)
{
    Vector3 fwd = { sinf(d->yawY), 0, cosf(d->yawY) };

    float size = 4.2f;
    float reach = 3.0f;

    Vector3 c = Vector3Add(d->pos, Vector3Scale(fwd, reach));
    c.y = d->outerBox.min.y + 0.60f * (d->outerBox.max.y - d->outerBox.min.y);

    return (BoundingBox) {
        { c.x - size * 0.5f, c.y - size * 0.5f, c.z - size * 0.5f },
        { c.x + size * 0.5f, c.y + size * 0.5f, c.z + size * 0.5f }
    };
}
//guitar
static inline bool DonIsGuitarSlamming(const Donogan* d)
{
    return d &&
        d->state == DONOGAN_STATE_AIR_L1_GUITAR_SLAM &&
        d->animTime >= 0.14f &&
        d->animTime <= 0.50f;
}

static inline BoundingBox DonMakeGuitarSlamBox(const Donogan* d)
{
    Vector3 fwd = { sinf(d->yawY), 0.0f, cosf(d->yawY) };

    // Bigger than wrench by about 2-4.
    float sizeXZ = 7.5f;
    float sizeY = 4.25f;
    float reach = 4.35f;

    Vector3 c = Vector3Add(d->pos, Vector3Scale(fwd, reach));

    float h = d->outerBox.max.y - d->outerBox.min.y;

    // In front of feet / lower body.
    c.y = d->outerBox.min.y + h * 0.32f;

    return (BoundingBox) {
        { c.x - sizeXZ * 0.5f, c.y - sizeY * 0.5f, c.z - sizeXZ * 0.5f },
        { c.x + sizeXZ * 0.5f, c.y + sizeY * 0.5f, c.z + sizeXZ * 0.5f }
    };
}

static inline bool Don_TryStartAirL1GuitarSlam(Donogan* d)
{
    if (!d) return false;
    if (!d->ja_l1_unlocked) return false;
    if (!d->hasGuitar) return false;

    d->gluedToPlatform = false;
    d->gluedPlatId = -1;

    // Commit down, but not as violently as L2 sphere slam.
    if (d->velY > -7.5f) d->velY = -7.5f;

    d->velXZ = Vector3Scale(d->velXZ, 0.45f);
    d->rollVel = (Vector3){ 0 };
    d->onGround = false;

    DonSetState(d, DONOGAN_STATE_AIR_L1_GUITAR_SLAM);
    return true;
}
//lasers
// ===== Robo Lasers ===========================================================
#define MAX_LASERS 16

typedef struct {
    int alive;
    Vector3 a, b;       // beam endpoints (spawn time “from” and “to”)
    float life, maxLife;
    float width;
} Laser;

static Laser gLasers[MAX_LASERS];
static int   gLaserHead = 0;

static inline void LasersInit(void) {
    for (int i = 0; i < MAX_LASERS; ++i) gLasers[i].alive = 0;
    gLaserHead = 0;
}

// Short-lived flash beam; damage is handled by the shooter (Robo) on fire.
static inline void FireLaser(Vector3 from, Vector3 to, float flashLife) {
    Laser* L = &gLasers[gLaserHead++ % MAX_LASERS];
    L->alive = 1;
    L->a = from;
    L->b = to;
    L->life = flashLife;
    L->maxLife = flashLife;
    L->width = 0.06f;         // tweak visual thickness
}

static inline void UpdateLasers(float dt) {
    for (int i = 0; i < MAX_LASERS; ++i) {
        Laser* L = &gLasers[i];
        if (!L->alive) continue;
        L->life -= dt;
        if (L->life <= 0) L->alive = 0;
    }
}

static inline void DrawLasers(void) {
    BeginBlendMode(BLEND_ADDITIVE);
    for (int i = 0; i < MAX_LASERS; ++i) {
        Laser* L = &gLasers[i];
        if (!L->alive) continue;

        float t = (L->maxLife > 0) ? (L->life / L->maxLife) : 0;
        float w = L->width * (0.65f + 0.35f * t); // small shrink over time

        // soft outer pass (cheap glow)
        Color halo = (Color){ 200, 60, 40, (unsigned char)(90 * t) };
        DrawCylinderEx(L->a, L->b, w * 2.2f, w * 2.2f, 8, halo);

        // bright core
        Color core = (Color){ 255, 100, 100, (unsigned char)(220 * t) };
        DrawCylinderEx(L->a, L->b, w, w, 10, core);

        // end caps
        DrawSphere(L->a, w * 0.9f, (Color) { 255, 180, 200, (unsigned char)(180 * t) });
        DrawSphere(L->b, w * 1.2f, core);
    }
    EndBlendMode();
}

// ============================================================================

//end lasers

// Feet world Y using model-space BB (only Y-rotation on model transform, so Y extent is stable)
static inline float DonFeetWorldY(const Donogan* d) {
    return d->pos.y + d->firstBB.min.y * d->scale;
}
static inline void DonSnapToGround(Donogan* d) {
    d->pos.y = d->groundY - d->firstBB.min.y * d->scale; // place feet exactly on ground
    d->velY = 0;
    d->onGround = true;
    d->box = UpdateBoundingBox(d->origBB,d->pos);
}
//proc anim
static inline float StepBlend01(float cur, float target, float rate, float dt, InterpolateFunc f)
{
    float t = rate * dt;
    if (t > 1.0f) t = 1.0f;
    return f ? f(&target, &cur, &t) : (cur + (target - cur) * t);
}

// Helper to fill one KeyFrame with zeros + identity rotation
// Fill one KeyFrameBone with zeros + identity rotation + linear interpolator
static inline void KfBoneZero(KeyFrameBone* kb, DonBone bone) {
    kb->boneId = bone;
    kb->rate = 0;                      // not used yet, but kept for future
    kb->pos = (Vector3){ 0,0,0 }; // delta translation (local)
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
// Forward from yaw+pitch (positive pitch looks UP)
static inline Vector3 DonAimForward(const Donogan* d, float upBiasDeg)
{
    float cy = cosf(d->yawY), sy = sinf(d->yawY);
    float cp = cosf(d->camPitch), sp = sinf(d->camPitch);

    // camera → world forward (opposite of target→camera):
    Vector3 dir = (Vector3){ -sy * cp, -sp, -cy * cp };

    if (upBiasDeg != 0) dir.y += sinf(DEG2RAD * upBiasDeg);
    return Vector3Normalize(dir);
}

// Rotate a local offset (x=right, y=up, z=forward) by yaw and return world offset
static inline Vector3 RotYawOffset(Vector3 localOff, float yaw, float scale, bool useScale)
{
    float cy = cosf(yaw), sy = sinf(yaw);
    Vector3 right = (Vector3){ cy, 0, -sy };
    Vector3 fwd = (Vector3){ sy, 0,  cy };
    Vector3 up = (Vector3){ 0, 1.0f, 0 };

    if (useScale) localOff = (Vector3){ localOff.x * scale, localOff.y * scale, localOff.z * scale };

    // world-space offset that rotates with yaw
    Vector3 offW = Vector3Add(
        Vector3Add(Vector3Scale(right, localOff.x),
            Vector3Scale(up, localOff.y)),
        Vector3Scale(fwd, localOff.z));

    return offW;
}
// Cheap ballistic predict to first ground hit (uses arrowGravity)
static Vector3 PredictArrowImpact(const Donogan* d,
    Vector3 origin, Vector3 dir,
    float speed, float tMax)
{
    Vector3 pos = origin;
    Vector3 vel = Vector3Scale(dir, speed);
    const float g = d->arrowGravity;     // negative
    const float dt = 1.0f / 120;        // fine-grained but cheap

    for (float t = 0; t < tMax; t += dt) {
        // integrate
        pos = Vector3Add(pos, Vector3Scale(vel, dt));
        vel.y += g * dt;

        // ground test
        float gy = GetTerrainHeightFromMeshXZ(pos.x, pos.z);
        if (pos.y <= gy + 0.01f) {
            pos.y = gy + 0.01f;
            return pos;
        }
    }
    // Fallback: far point along current vel
    return Vector3Add(origin, Vector3Scale(dir, speed * tMax));
}

static void DonInitArrows(Donogan* d) {
    d->arrowHead = 0;
    d->arrowLen = 1.989f;
    d->arrowShaftR = 0.028f;
    d->arrowHeadLen = 0.18f;
    d->arrowHeadR = 0.035f;
    d->arrowDrag = 0.15f;
    d->arrowMaxLife = 12.0f;
    d->arrowOffset = (Vector3){3,3.12,0};
    d->arrowGravity = -12.0f;   // MUCH lighter than character gravity
    d->arrowDragForward = 0.03f;    // keep forward speed
    d->arrowDragPerp = 3.0f;     // damp side/vertical wobble
    d->arrowNoCollideTime = 0.06f;  // avoid self-grounding at spawn
    for (int i = 0; i < MAX_ARROWS; i++) 
    { 
        d->arrows[i].alive = 0;
        d->arrows[i].origBox.max = (Vector3){ 0.2f , 0.2f , 0.2f };
        d->arrows[i].origBox.min = (Vector3){ -0.2f , -0.2f , -0.2f };
    }
}

static void DonFireArrow(Donogan* d, Vector3 spawn, Vector3 dir, float speed) {
    Arrow* a = &d->arrows[d->arrowHead++ % MAX_ARROWS];
    a->alive = 1; a->stuck = 0;
    a->pos = spawn;                     // tip starts at spawn
    a->vel = Vector3Scale(Vector3Normalize(dir), speed);
    a->life = d->arrowMaxLife;
    a->dir = Vector3Normalize(dir);
}

static void DonUpdateArrows(Donogan* d, float dt) {
    for (int i = 0; i < MAX_ARROWS; i++) {
        Arrow* a = &d->arrows[i];
        if (!a->alive) continue;

        if (!a->stuck) {
            a->box = UpdateBoundingBox(a->origBox, Vector3Add(a->pos, Vector3Scale(Vector3Normalize(a->dir),d->arrowLen)));//update the arrow bounding box
            // ... your gravity + anisotropic drag ...
            // move tip
            a->pos = Vector3Add(a->pos, Vector3Scale(a->vel, dt));

            // ground hit?
            float gy = GetTerrainHeightFromMeshXZ(a->pos.x, a->pos.z);
            if (a->pos.y <= gy + 0.01f) {
                // 1) lock the facing at the moment of impact
                Vector3 hitDir = Vector3Normalize(a->vel);
                if (Vector3Length(hitDir) < 1e-5f) hitDir = a->dir; // fallback
                a->dir = hitDir;

                // (optional) ensure it doesn’t lie perfectly flat:
                float minUpDot = 0.12f; // ~7° up minimum; tweak if needed
                float upDot = Vector3DotProduct(a->dir, (Vector3) { 0, 1, 0 });
                if (upDot < minUpDot) {
                    // blend a bit toward up just to “stick out”
                    float t = (minUpDot - upDot) * 0.6f;
                    a->dir = Vector3Normalize(Vector3Lerp(a->dir, (Vector3) { 0, 1, 0 }, t));
                }

                // 2) pin the tip just at/above the surface and stop
                a->pos.y = gy - 0.4f;                // tiny hover to avoid z-fight
                a->stuck = true;
                a->vel = (Vector3){ 0 };
            }
        }

        a->life -= dt;
        if (a->life <= 0) a->alive = 0;
    }
}


// Remove per-bone scales (set to 1) and zero any root translations so our hand attach controls placement.
static void BowStripScaleAndRootOffset(Donogan* d)
{
    if (!d || !d->bowAnimsRaw || d->bowAnimCount == 0) return;

    for (unsigned k = 0; k < d->bowAnimCount; ++k) {
        ModelAnimation* A = &d->bowAnimsRaw[k];
        if (!A || !A->keyframePoses) continue;

        for (int f = 0; f < (int)A->keyframeCount; ++f) {
            Transform* F = A->keyframePoses[f];

            bool firstRootDone = false;
            for (int b = 0; b < (int)A->boneCount; ++b) {
                // 1) kill per-bone scale
                F[b].scale = (Vector3){ 1,1,1 };

                // 2) zero translation on the *first* root only
                if (d->bowModel.skeleton.bones[b].parent == -1) {
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
        d->bowTime = 0;
        d->bowFrame = 0;
    }
}

static void BowApplyFrame(Donogan* d)
{
    if (!d || d->bowCur < 0 || !d->bowAnimsRaw) return;
    const ModelAnimation* A = &d->bowAnimsRaw[d->bowCur];
    int fc = (int)A->keyframeCount; if (fc < 1) fc = 1;

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
        DON_BONE_DEF_F_INDEX02_L, 
        DON_BONE_DEF_F_MIDDLE02_L, 
        DON_BONE_DEF_F_PINKY02_L, 
        DON_BONE_DEF_F_RING02_L
    };
    const int NUM_BOW_BONES = (int)(sizeof(BOW_BONES) / sizeof(BOW_BONES[0]));

    // --- ENTER ---
    KeyFrameGroup* g0 = &d->kfGroups[BOW_KFG_ENTER];
    g0->state = DONOGAN_STATE_BOW_ENTER;
    g0->anim = DONOGAN_ANIM_PROC_BOW_ENTER;
    g0->maxKey = 1;
    g0->curKey = 0;
    KfMakeZeroKey(&g0->keyFrames[0], 0, BOW_BONES, NUM_BOW_BONES);
    g0->keyFrames[0].kfBones[0].rot = QuatXYZDeg(0, 0, -85.0f);
    g0->keyFrames[0].kfBones[1].rot = QuatXYZDeg(0, 0,  85.0f);

    // --- AIM ---
    KeyFrameGroup* g1 = &d->kfGroups[BOW_KFG_AIM];
    g1->state = DONOGAN_STATE_BOW_AIM;
    g1->anim = DONOGAN_ANIM_PROC_BOW_AIM;
    g1->maxKey = 1;
    g1->curKey = 0;
    KfMakeZeroKey(&g1->keyFrames[0], 0, BOW_BONES, NUM_BOW_BONES);
    g1->keyFrames[0].kfBones[0].rot = QuatXYZDeg(-2.0f, -88.0f, -12.0f);
    g1->keyFrames[0].kfBones[1].rot = QuatXYZDeg(45, 85, 120);
    g1->keyFrames[0].kfBones[2].rot = QuatXYZDeg(0,0,0);
    g1->keyFrames[0].kfBones[3].rot = QuatXYZDeg(0, 76.0f, 0);
    g1->keyFrames[0].kfBones[4].rot = QuatXYZDeg(75.0f, 0, 0);
    g1->keyFrames[0].kfBones[5].rot = QuatXYZDeg(75.0f, 0, 0);
    g1->keyFrames[0].kfBones[6].rot = QuatXYZDeg(75.0f, 0, 0);
    g1->keyFrames[0].kfBones[7].rot = QuatXYZDeg(75.0f, 0, 0);

    // PULL
    KeyFrameGroup* g2 = &d->kfGroups[BOW_KFG_PULL];
    g2->state = DONOGAN_STATE_BOW_PULL;
    g2->anim = DONOGAN_ANIM_PROC_BOW_PULL;
    g2->maxKey = 1;
    g2->curKey = 0;
    KfMakeZeroKey(&g2->keyFrames[0], 0, BOW_BONES, NUM_BOW_BONES);
    g2->keyFrames[0].kfBones[0].rot = QuatXYZDeg(-2.0f, -88.0f, -11.0f);
    g2->keyFrames[0].kfBones[1].rot = QuatXYZDeg(0, 0, 100);
    g2->keyFrames[0].kfBones[2].rot = QuatXYZDeg(135, 0, 0);
    g2->keyFrames[0].kfBones[3].rot = QuatXYZDeg(0, 76.0f, 0);
    g2->keyFrames[0].kfBones[4].rot = QuatXYZDeg(76.0f, 0, 0);
    g2->keyFrames[0].kfBones[5].rot = QuatXYZDeg(76.0f, 0, 0);
    g2->keyFrames[0].kfBones[6].rot = QuatXYZDeg(76.0f, 0, 0);
    g2->keyFrames[0].kfBones[7].rot = QuatXYZDeg(76.0f, 0, 0);

    // RELEASE
    KeyFrameGroup* g3 = &d->kfGroups[BOW_KFG_REL];
    g3->state = DONOGAN_STATE_BOW_REL;
    g3->anim = DONOGAN_ANIM_PROC_BOW_REL;
    g3->maxKey = 1;
    g3->curKey = 0;
    KfMakeZeroKey(&g3->keyFrames[0], 0, BOW_BONES, NUM_BOW_BONES);
    g3->keyFrames[0].kfBones[0].rot = QuatXYZDeg(-2.0f, -88.0f, -10);
    g3->keyFrames[0].kfBones[1].rot = QuatXYZDeg(0, 0, 100);
    g3->keyFrames[0].kfBones[2].rot = QuatXYZDeg(135, 0, -10);
    g3->keyFrames[0].kfBones[3].rot = QuatXYZDeg(0, 76.0f, 0);
    g3->keyFrames[0].kfBones[4].rot = QuatXYZDeg(74.0f, 0, 0);
    g3->keyFrames[0].kfBones[5].rot = QuatXYZDeg(74.0f, 0, 0);
    g3->keyFrames[0].kfBones[6].rot = QuatXYZDeg(75.0f, 0, 0);
    g3->keyFrames[0].kfBones[7].rot = QuatXYZDeg(75.0f, 0, 0);

    // --- EXIT ---
    KeyFrameGroup* g4 = &d->kfGroups[BOW_KFG_EXIT];
    g4->state = DONOGAN_STATE_BOW_EXIT;
    g4->anim = DONOGAN_ANIM_PROC_BOW_EXIT;
    g4->maxKey = 1;
    g4->curKey = 0;
    KfMakeZeroKey(&g4->keyFrames[0], 0, BOW_BONES, NUM_BOW_BONES);
    g4->keyFrames[0].kfBones[0].rot = QuatXYZDeg(0, 0, -85.0f);
    g4->keyFrames[0].kfBones[1].rot = QuatXYZDeg(0, 0, 85.0f);
}

static void DonInitSpellShootKeyframeGroups(Donogan* d)
{
    // Bones we’ll drive for the bow pose (you can add fingers later):
    const DonBone BONES[] = {
        DON_BONE_DEF_UPPER_ARM_R,
        DON_BONE_DEF_UPPER_ARM_L,
        DON_BONE_DEF_HAND_R,
        DON_BONE_DEF_HAND_L,
    };
    const int NUM_BONES = (int)(sizeof(BONES) / sizeof(BONES[0]));

    // --- ENTER ---
    KeyFrameGroup* g0 = &d->kfGroups[SPELL_KFG_SHOOT];
    g0->state = DONOGAN_STATE_SPELL_SHOOT;
    g0->anim = DONOGAN_ANIM_PROC_SPELL_SHOOT;
    g0->maxKey = 1;
    g0->curKey = 0;
    KfMakeZeroKey(&g0->keyFrames[0], 1.0f, BONES, NUM_BONES);
    g0->keyFrames[0].kfBones[0].rot = QuatXYZDeg(95, 0, -15);
    g0->keyFrames[0].kfBones[1].rot = QuatXYZDeg(100, -120, 10);
    g0->keyFrames[0].kfBones[2].rot = QuatXYZDeg(0, 0, -70);
    g0->keyFrames[0].kfBones[3].rot = QuatXYZDeg(0, 0, 60);
}
//by his chest
static inline Vector3 DonAirR2PivotLocal(const Donogan* d)
{
    // Pivot in MODEL LOCAL SPACE.
    // Since DrawModel applies d->scale later, do NOT multiply this by d->scale.
    BoundingBox bb = d->firstBB;

    float h = bb.max.y - bb.min.y;

    Vector3 p;
    p.x = (bb.min.x + bb.max.x) * 0.5f;

    // Chest-ish flip center:
    // 2/3 from feet == 1/3 from head.
    p.y = bb.min.y + h * (2.0f / 3.0f);

    p.z = (bb.min.z + bb.max.z) * 0.5f;

    return p;
}

static inline Vector3 DonRootFrontFlipPivotCompensate(float pitchDeg, Vector3 pivot)
{
    // Pure front flip compensation.
    // For rotation around local X:
    //
    // y' = y*cos(a) - z*sin(a)
    // z' = y*sin(a) + z*cos(a)
    //
    // To keep the pivot visually fixed:
    // pos = pivot - rotatedPivot
    //
    // This is the circle correction you were talking about.

    float a = pitchDeg * DEG2RAD;
    float s = sinf(a);
    float c = cosf(a);

    Vector3 pos = { 0 };

    // X does not change for pure X-axis pitch.
    pos.x = 0;

    // Keep pivot's Y from bobbing around the flip circle.
    pos.y = pivot.y - (pivot.y * c - pivot.z * s);

    // Keep pivot's forward/back position from drawing a circle.
    pos.z = pivot.z - (pivot.y * s + pivot.z * c);

    return pos;
}

static inline Vector3 DonRootPivotCompensate(Quaternion q, Vector3 pivot)
{
    // General pivot compensation in MODEL/BIND space:
    // pos = pivot - rotatedPivot
    Vector3 rp = Vector3RotateByQuaternion(pivot, q);
    return Vector3Subtract(pivot, rp);
}

static inline Vector3 DonRootCompToKbLocal(const Donogan* d, Vector3 desiredComp)
{
    // DonApplyPoseFk() does:
    //
    //     deltaLocal = Rotate(KB->pos, prevRot);
    //     newPos = prev.translation + deltaLocal;
    //
    // So if we want "desiredComp" to be the final applied translation,
    // we must pre-unrotate it here.
    if (!d || !d->model.skeleton.bindPose) return desiredComp;

    Quaternion prevRot = d->model.skeleton.bindPose[DON_BONE_ROOT].rotation;
    Quaternion invPrevRot = QuaternionInvert(prevRot);

    return Vector3RotateByQuaternion(desiredComp, invPrevRot);
}

static inline void DonAirJumpAttackSetRoot(KeyFrame* kf, const Donogan* d, float pitchDeg, float yawDeg, float rollDeg)
{
    Quaternion q = QuatXYZDeg(pitchDeg, yawDeg, rollDeg);
    Vector3 pivot = DonAirR2PivotLocal(d);

    // First compute the compensation in the obvious/model space.
    Vector3 desiredComp = DonRootPivotCompensate(q, pivot);

    // Then convert it into the local space expected by DonApplyPoseFk().
    Vector3 kbPos = DonRootCompToKbLocal(d, desiredComp);

    kf->kfBones[0].rot = q;
    kf->kfBones[0].pos = kbPos;
}
static void DonInitAirR2SpellKeyframeGroups(Donogan* d)
{
    const DonBone BONES[] = {
        // Whole-body visual rotation.
        // This is NOT gameplay yaw. This is just the root bone pose for this proc anim.
        DON_BONE_ROOT,

        DON_BONE_DEF_UPPER_ARM_L,
        DON_BONE_DEF_FOREARM_L,
        DON_BONE_DEF_HAND_L,

        DON_BONE_DEF_UPPER_ARM_R,
        DON_BONE_DEF_FOREARM_R,
        DON_BONE_DEF_HAND_R,

        DON_BONE_DEF_THIGH_L,
        DON_BONE_DEF_SHIN_L,
        DON_BONE_DEF_THIGH_R,
        DON_BONE_DEF_SHIN_R,
    };

    const int NUM_BONES = (int)(sizeof(BONES) / sizeof(BONES[0]));

    KeyFrameGroup* g = &d->kfGroups[AIR_R2_KFG_SHOOT];
    g->state = DONOGAN_STATE_AIR_R2_SPELL_SHOOT;
    g->anim = DONOGAN_ANIM_PROC_AIR_R2_SPELL_SHOOT;
    g->maxKey = 4;
    g->curKey = 0;

    KfMakeZeroKey(&g->keyFrames[0], 0.00f, BONES, NUM_BONES);
    KfMakeZeroKey(&g->keyFrames[1], 0.18f, BONES, NUM_BONES);
    KfMakeZeroKey(&g->keyFrames[2], 0.36f, BONES, NUM_BONES);
    KfMakeZeroKey(&g->keyFrames[3], 0.54f, BONES, NUM_BONES);

    // Index map:
    // 0  root
    // 1  upper_arm_L
    // 2  forearm_L
    // 3  hand_L
    // 4  upper_arm_R
    // 5  forearm_R
    // 6  hand_R
    // 7  thigh_L
    // 8  shin_L
    // 9  thigh_R
    // 10 shin_R

    // ------------------------------------------------------------
    // KEY 0: start tuck
    // ------------------------------------------------------------

    // Root: pivot-corrected whole-body rotation.
    DonAirJumpAttackSetRoot(&g->keyFrames[0], d, 0, 0, 0);

    // Arms start moving forward/down.
    g->keyFrames[0].kfBones[1].rot = QuatXYZDeg(35, -8, 8);
    g->keyFrames[0].kfBones[2].rot = QuatXYZDeg(20, 0, 0);
    g->keyFrames[0].kfBones[3].rot = QuatXYZDeg(0, 0, 8);

    g->keyFrames[0].kfBones[4].rot = QuatXYZDeg(35, 8, -8);
    g->keyFrames[0].kfBones[5].rot = QuatXYZDeg(20, 0, 0);
    g->keyFrames[0].kfBones[6].rot = QuatXYZDeg(0, 0, -8);

    // Knees: moderate tuck.
    g->keyFrames[0].kfBones[7].rot = QuatXYZDeg(-35, 0, 4);  // thigh L
    g->keyFrames[0].kfBones[8].rot = QuatXYZDeg(70, 0, 0);  // shin L
    g->keyFrames[0].kfBones[9].rot = QuatXYZDeg(-35, 0, -4);  // thigh R
    g->keyFrames[0].kfBones[10].rot = QuatXYZDeg(70, 0, 0);  // shin R

    // ------------------------------------------------------------
    // KEY 1: main flip/tuck
    // ------------------------------------------------------------

    // X = front flip pitch
    // Y = twist/yaw
    // Z = side roll/aerial flavor
    DonAirJumpAttackSetRoot(&g->keyFrames[1], d, 120, 0,0);

    g->keyFrames[1].kfBones[1].rot = QuatXYZDeg(75, -10, 12);
    g->keyFrames[1].kfBones[2].rot = QuatXYZDeg(45, 0, 0);
    g->keyFrames[1].kfBones[3].rot = QuatXYZDeg(0, 0, 16);

    g->keyFrames[1].kfBones[4].rot = QuatXYZDeg(75, 10, -12);
    g->keyFrames[1].kfBones[5].rot = QuatXYZDeg(45, 0, 0);
    g->keyFrames[1].kfBones[6].rot = QuatXYZDeg(0, 0, -16);

    // Main tuck.
    g->keyFrames[1].kfBones[7].rot = QuatXYZDeg(-65, 0, 6);  // thigh L
    g->keyFrames[1].kfBones[8].rot = QuatXYZDeg(105, 0, 0);  // shin L
    g->keyFrames[1].kfBones[9].rot = QuatXYZDeg(-65, 0, -6);  // thigh R
    g->keyFrames[1].kfBones[10].rot = QuatXYZDeg(105, 0, 0);  // shin R

    // ------------------------------------------------------------
    // KEY 2: release / strongest rotation
    // ------------------------------------------------------------

    // Almost one full flip with some twist/roll.
    DonAirJumpAttackSetRoot(&g->keyFrames[2], d, 270, 0,0);

    // Arms reach out/down for the shot.
    g->keyFrames[2].kfBones[1].rot = QuatXYZDeg(125, -8, 12);
    g->keyFrames[2].kfBones[2].rot = QuatXYZDeg(35, 0, 0);
    g->keyFrames[2].kfBones[3].rot = QuatXYZDeg(0, 0, 16);

    g->keyFrames[2].kfBones[4].rot = QuatXYZDeg(125, 8, -12);
    g->keyFrames[2].kfBones[5].rot = QuatXYZDeg(35, 0, 0);
    g->keyFrames[2].kfBones[6].rot = QuatXYZDeg(0, 0, -16);

    // Loosen legs a little for the shot.
    g->keyFrames[2].kfBones[7].rot = QuatXYZDeg(-45, 0, 4);  // thigh L
    g->keyFrames[2].kfBones[8].rot = QuatXYZDeg(75, 0, 0);  // shin L
    g->keyFrames[2].kfBones[9].rot = QuatXYZDeg(-45, 0, -4);  // thigh R
    g->keyFrames[2].kfBones[10].rot = QuatXYZDeg(75, 0, 0);  // shin R

    // ------------------------------------------------------------
    // KEY 3: recover
    // ------------------------------------------------------------

    // Full pitch rotation, but pivot-corrected so he rotates around center.
    DonAirJumpAttackSetRoot(&g->keyFrames[3], d, 360, 0, 0);

    g->keyFrames[3].kfBones[1].rot = QuatXYZDeg(25, 0, 0);
    g->keyFrames[3].kfBones[2].rot = QuatXYZDeg(10, 0, 0);
    g->keyFrames[3].kfBones[3].rot = QuatXYZDeg(0, 0, 0);

    g->keyFrames[3].kfBones[4].rot = QuatXYZDeg(25, 0, 0);
    g->keyFrames[3].kfBones[5].rot = QuatXYZDeg(10, 0, 0);
    g->keyFrames[3].kfBones[6].rot = QuatXYZDeg(0, 0, 0);

    g->keyFrames[3].kfBones[7].rot = QuatXYZDeg(-10, 0, 0);  // thigh L
    g->keyFrames[3].kfBones[8].rot = QuatXYZDeg(20, 0, 0);  // shin L
    g->keyFrames[3].kfBones[9].rot = QuatXYZDeg(-10, 0, 0);  // thigh R
    g->keyFrames[3].kfBones[10].rot = QuatXYZDeg(20, 0, 0);  // shin R
}
static void DonInitAirR1HandstandKeyframeGroups(Donogan* d)
{
    const DonBone BONES[] = {
        DON_BONE_ROOT,

        DON_BONE_DEF_UPPER_ARM_L,
        DON_BONE_DEF_FOREARM_L,
        DON_BONE_DEF_HAND_L,

        DON_BONE_DEF_UPPER_ARM_R,
        DON_BONE_DEF_FOREARM_R,
        DON_BONE_DEF_HAND_R,

        DON_BONE_DEF_THIGH_L,
        DON_BONE_DEF_SHIN_L,
        DON_BONE_DEF_FOOT_L,

        DON_BONE_DEF_THIGH_R,
        DON_BONE_DEF_SHIN_R,
        DON_BONE_DEF_FOOT_R,
    };

    const int NUM_BONES = (int)(sizeof(BONES) / sizeof(BONES[0]));

    // ------------------------------------------------------------
    // PHASE 1: flip down into handstand and HOLD
    // ------------------------------------------------------------
    KeyFrameGroup* g = &d->kfGroups[AIR_R1_KFG_HAND_STAND];
    g->state = DONOGAN_STATE_AIR_R1_HAND_STAND;
    g->anim = DONOGAN_ANIM_PROC_AIR_R1_HAND_STAND;
    g->maxKey = 4;
    g->curKey = 0;

    KfMakeZeroKey(&g->keyFrames[0], 0.00f, BONES, NUM_BONES);
    KfMakeZeroKey(&g->keyFrames[1], 0.14f, BONES, NUM_BONES);
    KfMakeZeroKey(&g->keyFrames[2], 0.30f, BONES, NUM_BONES);
    KfMakeZeroKey(&g->keyFrames[3], 0.46f, BONES, NUM_BONES);

    // Index map:
    // 0  root
    // 1  upper_arm_L
    // 2  forearm_L
    // 3  hand_L
    // 4  upper_arm_R
    // 5  forearm_R
    // 6  hand_R
    // 7  thigh_L
    // 8  shin_L
    // 9  foot_L
    // 10 thigh_R
    // 11 shin_R
    // 12 foot_R

    // KEY 0: start like R2 flip
    DonAirJumpAttackSetRoot(&g->keyFrames[0], d, 0, 0, 0);

    g->keyFrames[0].kfBones[1].rot = QuatXYZDeg(45, -6, 8);
    g->keyFrames[0].kfBones[2].rot = QuatXYZDeg(20, 0, 0);
    g->keyFrames[0].kfBones[3].rot = QuatXYZDeg(0, 0, 8);

    g->keyFrames[0].kfBones[4].rot = QuatXYZDeg(45, 6, -8);
    g->keyFrames[0].kfBones[5].rot = QuatXYZDeg(20, 0, 0);
    g->keyFrames[0].kfBones[6].rot = QuatXYZDeg(0, 0, -8);

    g->keyFrames[0].kfBones[7].rot = QuatXYZDeg(-30, 0, 4);
    g->keyFrames[0].kfBones[8].rot = QuatXYZDeg(55, 0, 0);
    g->keyFrames[0].kfBones[9].rot = QuatXYZDeg(0, 0, 0);

    g->keyFrames[0].kfBones[10].rot = QuatXYZDeg(-30, 0, -4);
    g->keyFrames[0].kfBones[11].rot = QuatXYZDeg(55, 0, 0);
    g->keyFrames[0].kfBones[12].rot = QuatXYZDeg(0, 0, 0);

    // KEY 1: dive forward/down
    DonAirJumpAttackSetRoot(&g->keyFrames[1], d, 105.0f, 0, 0);

    g->keyFrames[1].kfBones[1].rot = QuatXYZDeg(115, -4, 6);
    g->keyFrames[1].kfBones[2].rot = QuatXYZDeg(15, 0, 0);
    g->keyFrames[1].kfBones[3].rot = QuatXYZDeg(0, 0, 5);

    g->keyFrames[1].kfBones[4].rot = QuatXYZDeg(115, 4, -6);
    g->keyFrames[1].kfBones[5].rot = QuatXYZDeg(15, 0, 0);
    g->keyFrames[1].kfBones[6].rot = QuatXYZDeg(0, 0, -5);

    g->keyFrames[1].kfBones[7].rot = QuatXYZDeg(-45, 0, 6);
    g->keyFrames[1].kfBones[8].rot = QuatXYZDeg(75, 0, 0);
    g->keyFrames[1].kfBones[9].rot = QuatXYZDeg(10, 0, 0);

    g->keyFrames[1].kfBones[10].rot = QuatXYZDeg(-45, 0, -6);
    g->keyFrames[1].kfBones[11].rot = QuatXYZDeg(75, 0, 0);
    g->keyFrames[1].kfBones[12].rot = QuatXYZDeg(10, 0, 0);

    // KEY 2: almost handstand
    DonAirJumpAttackSetRoot(&g->keyFrames[2], d, 170, 0, 0);

    g->keyFrames[2].kfBones[1].rot = QuatXYZDeg(160, -2, 4);
    g->keyFrames[2].kfBones[2].rot = QuatXYZDeg(5, 0, 0);
    g->keyFrames[2].kfBones[3].rot = QuatXYZDeg(0, 0, 0);

    g->keyFrames[2].kfBones[4].rot = QuatXYZDeg(160, 2, -4);
    g->keyFrames[2].kfBones[5].rot = QuatXYZDeg(5, 0, 0);
    g->keyFrames[2].kfBones[6].rot = QuatXYZDeg(0, 0, 0);

    g->keyFrames[2].kfBones[7].rot = QuatXYZDeg(-18, 0, 8);
    g->keyFrames[2].kfBones[8].rot = QuatXYZDeg(20, 0, 0);
    g->keyFrames[2].kfBones[9].rot = QuatXYZDeg(12, 0, 0);

    g->keyFrames[2].kfBones[10].rot = QuatXYZDeg(-18, 0, -8);
    g->keyFrames[2].kfBones[11].rot = QuatXYZDeg(20, 0, 0);
    g->keyFrames[2].kfBones[12].rot = QuatXYZDeg(12, 0, 0);

    // KEY 3: full handstand HOLD pose
    DonAirJumpAttackSetRoot(&g->keyFrames[3], d, 180, 0, 0);

    g->keyFrames[3].kfBones[1].rot = QuatXYZDeg(168, -2, 2);
    g->keyFrames[3].kfBones[2].rot = QuatXYZDeg(0, 0, 0);
    g->keyFrames[3].kfBones[3].rot = QuatXYZDeg(0, 0, 0);

    g->keyFrames[3].kfBones[4].rot = QuatXYZDeg(168, 2, -2);
    g->keyFrames[3].kfBones[5].rot = QuatXYZDeg(0, 0, 0);
    g->keyFrames[3].kfBones[6].rot = QuatXYZDeg(0, 0, 0);

    // Legs mostly straight while upside down.
    g->keyFrames[3].kfBones[7].rot = QuatXYZDeg(-8, 0, 8);
    g->keyFrames[3].kfBones[8].rot = QuatXYZDeg(10, 0, 0);
    g->keyFrames[3].kfBones[9].rot = QuatXYZDeg(8, 0, 0);

    g->keyFrames[3].kfBones[10].rot = QuatXYZDeg(-8, 0, -8);
    g->keyFrames[3].kfBones[11].rot = QuatXYZDeg(10, 0, 0);
    g->keyFrames[3].kfBones[12].rot = QuatXYZDeg(8, 0, 0);

    // ------------------------------------------------------------
    // PHASE 2: bounce release, rights himself
    // ------------------------------------------------------------
    KeyFrameGroup* r = &d->kfGroups[AIR_R1_KFG_RELEASE];
    r->state = DONOGAN_STATE_AIR_R1_RELEASE;
    r->anim = DONOGAN_ANIM_PROC_AIR_R1_RELEASE;
    r->maxKey = 4;
    r->curKey = 0;

    KfMakeZeroKey(&r->keyFrames[0], 0.00f, BONES, NUM_BONES);
    KfMakeZeroKey(&r->keyFrames[1], 0.12f, BONES, NUM_BONES);
    KfMakeZeroKey(&r->keyFrames[2], 0.27f, BONES, NUM_BONES);
    KfMakeZeroKey(&r->keyFrames[3], 0.42f, BONES, NUM_BONES);

    // KEY 0: starts from handstand
    DonAirJumpAttackSetRoot(&r->keyFrames[0], d, 180, 0, 0);

    r->keyFrames[0].kfBones[1].rot = QuatXYZDeg(168, -2, 2);
    r->keyFrames[0].kfBones[2].rot = QuatXYZDeg(0, 0, 0);
    r->keyFrames[0].kfBones[3].rot = QuatXYZDeg(0, 0, 0);

    r->keyFrames[0].kfBones[4].rot = QuatXYZDeg(168, 2, -2);
    r->keyFrames[0].kfBones[5].rot = QuatXYZDeg(0, 0, 0);
    r->keyFrames[0].kfBones[6].rot = QuatXYZDeg(0, 0, 0);

    r->keyFrames[0].kfBones[7].rot = QuatXYZDeg(-8, 0, 8);
    r->keyFrames[0].kfBones[8].rot = QuatXYZDeg(10, 0, 0);
    r->keyFrames[0].kfBones[9].rot = QuatXYZDeg(8, 0, 0);
    r->keyFrames[0].kfBones[10].rot = QuatXYZDeg(-8, 0, -8);
    r->keyFrames[0].kfBones[11].rot = QuatXYZDeg(10, 0, 0);
    r->keyFrames[0].kfBones[12].rot = QuatXYZDeg(8, 0, 0);

    // KEY 1: spring away from handstand
    DonAirJumpAttackSetRoot(&r->keyFrames[1], d, 245.0f, 0, 0);

    r->keyFrames[1].kfBones[1].rot = QuatXYZDeg(120, -4, 5);
    r->keyFrames[1].kfBones[2].rot = QuatXYZDeg(12, 0, 0);
    r->keyFrames[1].kfBones[3].rot = QuatXYZDeg(0, 0, 4);

    r->keyFrames[1].kfBones[4].rot = QuatXYZDeg(120, 4, -5);
    r->keyFrames[1].kfBones[5].rot = QuatXYZDeg(12, 0, 0);
    r->keyFrames[1].kfBones[6].rot = QuatXYZDeg(0, 0, -4);

    r->keyFrames[1].kfBones[7].rot = QuatXYZDeg(-20, 0, 6);
    r->keyFrames[1].kfBones[8].rot = QuatXYZDeg(35, 0, 0);
    r->keyFrames[1].kfBones[9].rot = QuatXYZDeg(8, 0, 0);
    r->keyFrames[1].kfBones[10].rot = QuatXYZDeg(-20, 0, -6);
    r->keyFrames[1].kfBones[11].rot = QuatXYZDeg(35, 0, 0);
    r->keyFrames[1].kfBones[12].rot = QuatXYZDeg(8, 0, 0);

    // KEY 2: almost upright
    DonAirJumpAttackSetRoot(&r->keyFrames[2], d, 315.0f, 0, 0);

    r->keyFrames[2].kfBones[1].rot = QuatXYZDeg(70, -4, 6);
    r->keyFrames[2].kfBones[2].rot = QuatXYZDeg(20, 0, 0);
    r->keyFrames[2].kfBones[3].rot = QuatXYZDeg(0, 0, 5);

    r->keyFrames[2].kfBones[4].rot = QuatXYZDeg(70, 4, -6);
    r->keyFrames[2].kfBones[5].rot = QuatXYZDeg(20, 0, 0);
    r->keyFrames[2].kfBones[6].rot = QuatXYZDeg(0, 0, -5);

    r->keyFrames[2].kfBones[7].rot = QuatXYZDeg(-25, 0, 4);
    r->keyFrames[2].kfBones[8].rot = QuatXYZDeg(55, 0, 0);
    r->keyFrames[2].kfBones[9].rot = QuatXYZDeg(0, 0, 0);
    r->keyFrames[2].kfBones[10].rot = QuatXYZDeg(-25, 0, -4);
    r->keyFrames[2].kfBones[11].rot = QuatXYZDeg(55, 0, 0);
    r->keyFrames[2].kfBones[12].rot = QuatXYZDeg(0, 0, 0);

    // KEY 3: upright / return toward normal jump
    DonAirJumpAttackSetRoot(&r->keyFrames[3], d, 360, 0, 0);

    r->keyFrames[3].kfBones[1].rot = QuatXYZDeg(25, 0, 0);
    r->keyFrames[3].kfBones[2].rot = QuatXYZDeg(10, 0, 0);
    r->keyFrames[3].kfBones[3].rot = QuatXYZDeg(0, 0, 0);

    r->keyFrames[3].kfBones[4].rot = QuatXYZDeg(25, 0, 0);
    r->keyFrames[3].kfBones[5].rot = QuatXYZDeg(10, 0, 0);
    r->keyFrames[3].kfBones[6].rot = QuatXYZDeg(0, 0, 0);

    r->keyFrames[3].kfBones[7].rot = QuatXYZDeg(-10, 0, 0);
    r->keyFrames[3].kfBones[8].rot = QuatXYZDeg(20, 0, 0);
    r->keyFrames[3].kfBones[9].rot = QuatXYZDeg(0, 0, 0);
    r->keyFrames[3].kfBones[10].rot = QuatXYZDeg(-10, 0, 0);
    r->keyFrames[3].kfBones[11].rot = QuatXYZDeg(20, 0, 0);
    r->keyFrames[3].kfBones[12].rot = QuatXYZDeg(0, 0, 0);
}

static void DonInitAirL2SphereSlamKeyframeGroups(Donogan* d)
{
    const DonBone BONES[] = {
        DON_BONE_ROOT,

        DON_BONE_DEF_SPINE002,
        DON_BONE_DEF_SPINE003,

        DON_BONE_DEF_UPPER_ARM_R,
        DON_BONE_DEF_FOREARM_R,
        DON_BONE_DEF_HAND_R,

        DON_BONE_DEF_UPPER_ARM_L,
        DON_BONE_DEF_FOREARM_L,
        DON_BONE_DEF_HAND_L,

        DON_BONE_DEF_THIGH_L,
        DON_BONE_DEF_SHIN_L,
        DON_BONE_DEF_THIGH_R,
        DON_BONE_DEF_SHIN_R,
    };

    const int NUM_BONES = (int)(sizeof(BONES) / sizeof(BONES[0]));

    KeyFrameGroup* g = &d->kfGroups[AIR_L2_KFG_SPHERE_SLAM];
    g->state = DONOGAN_STATE_AIR_L2_SPELL_SLAM;
    g->anim = DONOGAN_ANIM_PROC_AIR_L2_SPHERE_SLAM;
    g->maxKey = 4;
    g->curKey = 0;

    KfMakeZeroKey(&g->keyFrames[0], 0.00f, BONES, NUM_BONES);
    KfMakeZeroKey(&g->keyFrames[1], 0.16f, BONES, NUM_BONES);
    KfMakeZeroKey(&g->keyFrames[2], 0.34f, BONES, NUM_BONES);
    KfMakeZeroKey(&g->keyFrames[3], 0.52f, BONES, NUM_BONES);

    // Index map:
    // 0 root
    // 1 spine002
    // 2 spine003
    // 3 upper_arm_R
    // 4 forearm_R
    // 5 hand_R
    // 6 upper_arm_L
    // 7 forearm_L
    // 8 hand_L
    // 9 thigh_L
    // 10 shin_L
    // 11 thigh_R
    // 12 shin_R

    // KEY 0: start raising arm
    DonAirJumpAttackSetRoot(&g->keyFrames[0], d, 0, 0, 0);

    g->keyFrames[0].kfBones[1].rot = QuatXYZDeg(0, 0, 0);
    g->keyFrames[0].kfBones[2].rot = QuatXYZDeg(0, 0, 0);

    g->keyFrames[0].kfBones[3].rot = QuatXYZDeg(70, 0, -20);
    g->keyFrames[0].kfBones[4].rot = QuatXYZDeg(25, 0, 0);
    g->keyFrames[0].kfBones[5].rot = QuatXYZDeg(0, 0, -8);

    g->keyFrames[0].kfBones[6].rot = QuatXYZDeg(20, 0, 12);
    g->keyFrames[0].kfBones[7].rot = QuatXYZDeg(10, 0, 0);
    g->keyFrames[0].kfBones[8].rot = QuatXYZDeg(0, 0, 0);

    g->keyFrames[0].kfBones[9].rot = QuatXYZDeg(-20, 0, 4);
    g->keyFrames[0].kfBones[10].rot = QuatXYZDeg(40, 0, 0);
    g->keyFrames[0].kfBones[11].rot = QuatXYZDeg(-20, 0, -4);
    g->keyFrames[0].kfBones[12].rot = QuatXYZDeg(40, 0, 0);

    // KEY 1: arm almost straight up, knees tucked, spin starts
    DonAirJumpAttackSetRoot(&g->keyFrames[1], d, 120,0,0);

    g->keyFrames[1].kfBones[1].rot = QuatXYZDeg(0, 0, -4);
    g->keyFrames[1].kfBones[2].rot = QuatXYZDeg(0, 0, -8);

    g->keyFrames[1].kfBones[3].rot = QuatXYZDeg(145, 0, -18);
    g->keyFrames[1].kfBones[4].rot = QuatXYZDeg(12, 0, 0);
    g->keyFrames[1].kfBones[5].rot = QuatXYZDeg(0, 0, -4);

    g->keyFrames[1].kfBones[6].rot = QuatXYZDeg(35, 0, 20);
    g->keyFrames[1].kfBones[7].rot = QuatXYZDeg(15, 0, 0);
    g->keyFrames[1].kfBones[8].rot = QuatXYZDeg(0, 0, 0);

    g->keyFrames[1].kfBones[9].rot = QuatXYZDeg(-60, 0, 6);
    g->keyFrames[1].kfBones[10].rot = QuatXYZDeg(100, 0, 0);
    g->keyFrames[1].kfBones[11].rot = QuatXYZDeg(-60, 0, -6);
    g->keyFrames[1].kfBones[12].rot = QuatXYZDeg(100, 0, 0);

    // KEY 2: full spin, sphere overhead, falling hard
    DonAirJumpAttackSetRoot(&g->keyFrames[2], d, 250, 0, 0);

    g->keyFrames[2].kfBones[1].rot = QuatXYZDeg(8, 0, -8);
    g->keyFrames[2].kfBones[2].rot = QuatXYZDeg(12, 0, -12);

    g->keyFrames[2].kfBones[3].rot = QuatXYZDeg(165, 0, -10);
    g->keyFrames[2].kfBones[4].rot = QuatXYZDeg(4, 0, 0);
    g->keyFrames[2].kfBones[5].rot = QuatXYZDeg(0, 0, 0);

    g->keyFrames[2].kfBones[6].rot = QuatXYZDeg(45, 0, 26);
    g->keyFrames[2].kfBones[7].rot = QuatXYZDeg(20, 0, 0);
    g->keyFrames[2].kfBones[8].rot = QuatXYZDeg(0, 0, 0);

    g->keyFrames[2].kfBones[9].rot = QuatXYZDeg(-70, 0, 8);
    g->keyFrames[2].kfBones[10].rot = QuatXYZDeg(110, 0, 0);
    g->keyFrames[2].kfBones[11].rot = QuatXYZDeg(-70, 0, -8);
    g->keyFrames[2].kfBones[12].rot = QuatXYZDeg(110, 0, 0);

    // KEY 3: slam/follow-through, full 360
    DonAirJumpAttackSetRoot(&g->keyFrames[3], d, 360, 0, 0);

    g->keyFrames[3].kfBones[1].rot = QuatXYZDeg(18, 0, 0);
    g->keyFrames[3].kfBones[2].rot = QuatXYZDeg(25, 0, 0);

    g->keyFrames[3].kfBones[3].rot = QuatXYZDeg(130, 0, -4);
    g->keyFrames[3].kfBones[4].rot = QuatXYZDeg(15, 0, 0);
    g->keyFrames[3].kfBones[5].rot = QuatXYZDeg(0, 0, 0);

    g->keyFrames[3].kfBones[6].rot = QuatXYZDeg(30, 0, 10);
    g->keyFrames[3].kfBones[7].rot = QuatXYZDeg(10, 0, 0);
    g->keyFrames[3].kfBones[8].rot = QuatXYZDeg(0, 0, 0);

    g->keyFrames[3].kfBones[9].rot = QuatXYZDeg(-30, 0, 4);
    g->keyFrames[3].kfBones[10].rot = QuatXYZDeg(45, 0, 0);
    g->keyFrames[3].kfBones[11].rot = QuatXYZDeg(-30, 0, -4);
    g->keyFrames[3].kfBones[12].rot = QuatXYZDeg(45, 0, 0);
}
static void DonInitAirL1GuitarSlamKeyframeGroups(Donogan* d)
{
    const DonBone BONES[] = {
        DON_BONE_ROOT,

        DON_BONE_DEF_SPINE002,
        DON_BONE_DEF_SPINE003,

        DON_BONE_DEF_UPPER_ARM_L,
        DON_BONE_DEF_FOREARM_L,
        DON_BONE_DEF_HAND_L,

        DON_BONE_DEF_UPPER_ARM_R,
        DON_BONE_DEF_FOREARM_R,
        DON_BONE_DEF_HAND_R,

        DON_BONE_DEF_THIGH_L,
        DON_BONE_DEF_SHIN_L,
        DON_BONE_DEF_FOOT_L,

        DON_BONE_DEF_THIGH_R,
        DON_BONE_DEF_SHIN_R,
        DON_BONE_DEF_FOOT_R,
    };

    const int NUM_BONES = (int)(sizeof(BONES) / sizeof(BONES[0]));

    KeyFrameGroup* g = &d->kfGroups[AIR_L1_KFG_GUITAR_SLAM];
    g->state = DONOGAN_STATE_AIR_L1_GUITAR_SLAM;
    g->anim = DONOGAN_ANIM_PROC_AIR_L1_GUITAR_SLAM;
    g->maxKey = 4;
    g->curKey = 0;

    /*KfMakeZeroKey(&g->keyFrames[0], 0.00f, BONES, NUM_BONES);
    KfMakeZeroKey(&g->keyFrames[1], 0.14f, BONES, NUM_BONES);
    KfMakeZeroKey(&g->keyFrames[2], 0.34f, BONES, NUM_BONES);
    KfMakeZeroKey(&g->keyFrames[3], 0.56f, BONES, NUM_BONES);*/
    KfMakeZeroKey(&g->keyFrames[0], 0.00f, BONES, NUM_BONES);
    KfMakeZeroKey(&g->keyFrames[1], 0.24f, BONES, NUM_BONES);
    KfMakeZeroKey(&g->keyFrames[2], 0.55f, BONES, NUM_BONES);
    KfMakeZeroKey(&g->keyFrames[3], 0.86f, BONES, NUM_BONES);

    // Index map:
    // 0  root
    // 1  spine002
    // 2  spine003
    // 3  upper_arm_L
    // 4  forearm_L
    // 5  hand_L
    // 6  upper_arm_R
    // 7  forearm_R
    // 8  hand_R
    // 9  thigh_L
    // 10 shin_L
    // 11 foot_L
    // 12 thigh_R
    // 13 shin_R
    // 14 foot_R

    // ------------------------------------------------------------
    // KEY 0: prepare, slight forward load, guitar up but not in face
    // ------------------------------------------------------------
    DonAirJumpAttackSetRoot(&g->keyFrames[0], d, 0.0f, 0.0f, 0.0f);

    // Forward lean instead of backward lean.
    g->keyFrames[0].kfBones[1].rot = QuatXYZDeg(6, 0, 0);
    g->keyFrames[0].kfBones[2].rot = QuatXYZDeg(10, 0, 4);

    // Left arm: guitar starts up and to Don's left side.
    // This keeps the guitar held out/sideways, not buried in his face.
    g->keyFrames[0].kfBones[3].rot = QuatXYZDeg(70, -18, 82);   // upper_arm_L
    g->keyFrames[0].kfBones[4].rot = QuatXYZDeg(22, 0, 0);   // forearm_L
    g->keyFrames[0].kfBones[5].rot = QuatXYZDeg(0, 0, 0);   // hand_L

    // Right arm loosely follows, like he is preparing the slam.
    g->keyFrames[0].kfBones[6].rot = QuatXYZDeg(35, 10, -32);  // upper_arm_R
    g->keyFrames[0].kfBones[7].rot = QuatXYZDeg(18, 0, 0);  // forearm_R
    g->keyFrames[0].kfBones[8].rot = QuatXYZDeg(0, 0, 0);  // hand_R

    // Legs: flipped major X direction from the first pass.
    g->keyFrames[0].kfBones[9].rot = QuatXYZDeg(-25, 0, -20);
    g->keyFrames[0].kfBones[10].rot = QuatXYZDeg(28, 0, 0);
    g->keyFrames[0].kfBones[11].rot = QuatXYZDeg(0, 0, -10);

    g->keyFrames[0].kfBones[12].rot = QuatXYZDeg(-25, 0, 20);
    g->keyFrames[0].kfBones[13].rot = QuatXYZDeg(28, 0, 0);
    g->keyFrames[0].kfBones[14].rot = QuatXYZDeg(0, 0, 10);

    // ------------------------------------------------------------
    // KEY 1: wind-up, forward-leaning, legs spread/back
    // ------------------------------------------------------------
    DonAirJumpAttackSetRoot(&g->keyFrames[1], d, 18.0f, 0.0f, 0.0f);

    g->keyFrames[1].kfBones[1].rot = QuatXYZDeg(20, 0, -4);
    g->keyFrames[1].kfBones[2].rot = QuatXYZDeg(28, 0, -8);

    // Wind-up: guitar hand high and a bit back/side, not waving in front.
    // Wind-up: guitar goes higher/side, like he is about to smash downward.
    g->keyFrames[1].kfBones[3].rot = QuatXYZDeg(105, -26, 92);  // upper_arm_L
    g->keyFrames[1].kfBones[4].rot = QuatXYZDeg(28, 0, 0);  // forearm_L
    g->keyFrames[1].kfBones[5].rot = QuatXYZDeg(0, 0, 4);  // hand_L

    // Right arm comes up slightly for balance.
    g->keyFrames[1].kfBones[6].rot = QuatXYZDeg(48, 12, -38);  // upper_arm_R
    g->keyFrames[1].kfBones[7].rot = QuatXYZDeg(24, 0, 0);  // forearm_R
    g->keyFrames[1].kfBones[8].rot = QuatXYZDeg(0, 0, 0);  // hand_R

    g->keyFrames[1].kfBones[9].rot = QuatXYZDeg(-48, 0, -36);
    g->keyFrames[1].kfBones[10].rot = QuatXYZDeg(48, 0, 0);
    g->keyFrames[1].kfBones[11].rot = QuatXYZDeg(0, 0, -18);

    g->keyFrames[1].kfBones[12].rot = QuatXYZDeg(-48, 0, 36);
    g->keyFrames[1].kfBones[13].rot = QuatXYZDeg(48, 0, 0);
    g->keyFrames[1].kfBones[14].rot = QuatXYZDeg(0, 0, 18);

    // ------------------------------------------------------------
    // KEY 2: main slam, hard forward lean, guitar driven down/front
    // ------------------------------------------------------------
    DonAirJumpAttackSetRoot(&g->keyFrames[2], d, 45.0f, 0.0f, 0.0f);

    g->keyFrames[2].kfBones[1].rot = QuatXYZDeg(36, 0, 2);
    g->keyFrames[2].kfBones[2].rot = QuatXYZDeg(48, 0, 8);

    // Main slam: left hand should pull the attached guitar down in front of feet.
    // Main slam: left hand/guitar comes down into the middle in front of him.
    // Lower upper_arm X + bigger forearm X = hand/guitar comes down instead of staying high.
    g->keyFrames[2].kfBones[3].rot = QuatXYZDeg(28, -8, 34);    // upper_arm_L
    g->keyFrames[2].kfBones[4].rot = QuatXYZDeg(112, 0, 0);    // forearm_L
    g->keyFrames[2].kfBones[5].rot = QuatXYZDeg(0, 0, -10);   // hand_L

    // Right arm follows the slam toward center, like a big two-arm smash pose.
    g->keyFrames[2].kfBones[6].rot = QuatXYZDeg(38, -8, -24);   // upper_arm_R
    g->keyFrames[2].kfBones[7].rot = QuatXYZDeg(72, 0, 0);   // forearm_R
    g->keyFrames[2].kfBones[8].rot = QuatXYZDeg(0, 0, 8);   // hand_R

    // Legs kicked back/wide.
    g->keyFrames[2].kfBones[9].rot = QuatXYZDeg(-62, 0, -44);
    g->keyFrames[2].kfBones[10].rot = QuatXYZDeg(66, 0, 0);
    g->keyFrames[2].kfBones[11].rot = QuatXYZDeg(8, 0, -22);

    g->keyFrames[2].kfBones[12].rot = QuatXYZDeg(-62, 0, 44);
    g->keyFrames[2].kfBones[13].rot = QuatXYZDeg(66, 0, 0);
    g->keyFrames[2].kfBones[14].rot = QuatXYZDeg(8, 0, 22);

    // ------------------------------------------------------------
    // KEY 3: recover, still a little forward
    // ------------------------------------------------------------
    DonAirJumpAttackSetRoot(&g->keyFrames[3], d, 15.0f, 0.0f, 0.0f);

    g->keyFrames[3].kfBones[1].rot = QuatXYZDeg(10, 0, 0);
    g->keyFrames[3].kfBones[2].rot = QuatXYZDeg(14, 0, 0);

    // Recover: guitar comes back up a little, still forward/center-ish.
    g->keyFrames[3].kfBones[3].rot = QuatXYZDeg(45, -10, 48);   // upper_arm_L
    g->keyFrames[3].kfBones[4].rot = QuatXYZDeg(62, 0, 0);   // forearm_L
    g->keyFrames[3].kfBones[5].rot = QuatXYZDeg(0, 0, -4);   // hand_L

    // Right arm relaxes back out.
    g->keyFrames[3].kfBones[6].rot = QuatXYZDeg(24, 0, -18);   // upper_arm_R
    g->keyFrames[3].kfBones[7].rot = QuatXYZDeg(28, 0, 0);   // forearm_R
    g->keyFrames[3].kfBones[8].rot = QuatXYZDeg(0, 0, 0);   // hand_R

    g->keyFrames[3].kfBones[9].rot = QuatXYZDeg(-18, 0, -16);
    g->keyFrames[3].kfBones[10].rot = QuatXYZDeg(25, 0, 0);
    g->keyFrames[3].kfBones[11].rot = QuatXYZDeg(0, 0, -8);

    g->keyFrames[3].kfBones[12].rot = QuatXYZDeg(-18, 0, 16);
    g->keyFrames[3].kfBones[13].rot = QuatXYZDeg(25, 0, 0);
    g->keyFrames[3].kfBones[14].rot = QuatXYZDeg(0, 0, 8);
}
static void DonInitMachineTurnKeyframeGroups(Donogan* d)
{
    const DonBone BONES[] = {
        DON_BONE_DEF_SPINE003,

        DON_BONE_DEF_SHOULDER_R,
        DON_BONE_DEF_UPPER_ARM_R,
        DON_BONE_DEF_FOREARM_R,
        DON_BONE_DEF_HAND_R,

        DON_BONE_DEF_SHOULDER_L,
        DON_BONE_DEF_UPPER_ARM_L
    };
    const int NUM_BONES = (int)(sizeof(BONES) / sizeof(BONES[0]));

    KeyFrameGroup* g = &d->kfGroups[MACHINE_KFG_TURN];
    g->state = DONOGAN_STATE_MACHINE_TURN;
    g->anim = DONOGAN_ANIM_PROC_MACHINE_TURN;
    g->maxKey = 4;
    g->curKey = 0;

    KfMakeZeroKey(&g->keyFrames[0], 0.08f, BONES, NUM_BONES);
    KfMakeZeroKey(&g->keyFrames[1], 0.18f, BONES, NUM_BONES);
    KfMakeZeroKey(&g->keyFrames[2], 0.12f, BONES, NUM_BONES);
    KfMakeZeroKey(&g->keyFrames[3], 0.08f, BONES, NUM_BONES);

    // key 0 = arm reaches across body / setup
    // key 0 = arm reaches across body / setup
    // key 0 = arm out at side, forearm bent inward / setup
    // key 0 = arm at side, forearm slightly across middle / setup
    // key 0 = arm down and slightly in front / setup
    g->keyFrames[0].kfBones[0].rot = QuatXYZDeg(0, 0, 0);        // spine003 tiny turn
    g->keyFrames[0].kfBones[1].rot = QuatXYZDeg(0, 0, 0);        // shoulderR neutral (lower arm)
    g->keyFrames[0].kfBones[2].rot = QuatXYZDeg(76, 0, 0);      // upper_armR forward (NOT side)
    g->keyFrames[0].kfBones[3].rot = QuatXYZDeg(0, 35, 0);       // forearm across middle //ads the kink, is there a way around it?
    g->keyFrames[0].kfBones[4].rot = QuatXYZDeg(0, 22, 0);       // hand follows
    g->keyFrames[0].kfBones[5].rot = QuatXYZDeg(0, 0, 0);        //lfeft arm stuff
    g->keyFrames[0].kfBones[6].rot = QuatXYZDeg(0, 0, -80);

    // key 1 = twist A
    g->keyFrames[1].kfBones[0].rot = QuatXYZDeg(0, 0, 0);
    g->keyFrames[1].kfBones[1].rot = QuatXYZDeg(0, 0, 0);
    g->keyFrames[1].kfBones[2].rot = QuatXYZDeg(78, 0, 0);      // keep forward placement
    g->keyFrames[1].kfBones[3].rot = QuatXYZDeg(0, 35, 0);       ////ads the kink, is there a way around it?
    g->keyFrames[1].kfBones[4].rot = QuatXYZDeg(0, 32, 0);
    g->keyFrames[1].kfBones[5].rot = QuatXYZDeg(0, 0, 0);        //lfeft arm stuff
    g->keyFrames[1].kfBones[6].rot = QuatXYZDeg(0, 0, -80);

    // key 2 = twist B
    g->keyFrames[2].kfBones[0].rot = QuatXYZDeg(0, 0, 0);
    g->keyFrames[2].kfBones[1].rot = QuatXYZDeg(0, 0, 0);
    g->keyFrames[2].kfBones[2].rot = QuatXYZDeg(80, 0, 0);
    g->keyFrames[2].kfBones[3].rot = QuatXYZDeg(0, 35, 0);       ////ads the kink, is there a way around it?
    g->keyFrames[2].kfBones[4].rot = QuatXYZDeg(0, 12, 0);
    g->keyFrames[2].kfBones[5].rot = QuatXYZDeg(0, 0, 0);        //lfeft arm stuff
    g->keyFrames[2].kfBones[6].rot = QuatXYZDeg(0, 0, -80);

    // key 3 = settle
    g->keyFrames[3].kfBones[0].rot = QuatXYZDeg(0, 0, 0);
    g->keyFrames[3].kfBones[1].rot = QuatXYZDeg(0, 0, 0);
    g->keyFrames[3].kfBones[2].rot = QuatXYZDeg(78, 0, 0);
    g->keyFrames[3].kfBones[3].rot = QuatXYZDeg(0, 35, 0); //ads the kink, is there a way around it?
    g->keyFrames[3].kfBones[4].rot = QuatXYZDeg(0, 20, 0);
    g->keyFrames[3].kfBones[5].rot = QuatXYZDeg(0, 0, 0);        //lfeft arm stuff
    g->keyFrames[3].kfBones[6].rot = QuatXYZDeg(0, 0, -80);
}
static void DonInitGroundL1GuitarKeyframeGroups(Donogan* d)
{
    const DonBone BONES[] = {
        DON_BONE_ROOT,

        DON_BONE_DEF_SPINE002,
        DON_BONE_DEF_SPINE003,

        DON_BONE_DEF_UPPER_ARM_L,
        DON_BONE_DEF_FOREARM_L,
        DON_BONE_DEF_HAND_L,

        DON_BONE_DEF_UPPER_ARM_R,
        DON_BONE_DEF_FOREARM_R,
        DON_BONE_DEF_HAND_R,

        DON_BONE_DEF_THIGH_L,
        DON_BONE_DEF_SHIN_L,
        DON_BONE_DEF_FOOT_L,

        DON_BONE_DEF_THIGH_R,
        DON_BONE_DEF_SHIN_R,
        DON_BONE_DEF_FOOT_R,
    };

    const int NUM_BONES = (int)(sizeof(BONES) / sizeof(BONES[0]));

    // ---------------------------------------------------------------------
    // DASH POSE
    // ---------------------------------------------------------------------
    KeyFrameGroup* dash = &d->kfGroups[GROUND_L1_KFG_GUITAR_DASH];
    dash->state = DONOGAN_STATE_GROUND_L1_GUITAR_DASH;
    dash->anim = DONOGAN_ANIM_PROC_GROUND_L1_GUITAR_DASH;
    dash->maxKey = 2;
    dash->curKey = 0;

    KfMakeZeroKey(&dash->keyFrames[0], 0.00f, BONES, NUM_BONES);
    KfMakeZeroKey(&dash->keyFrames[1], 0.22f, BONES, NUM_BONES);

    for (int k = 0; k < 2; k++)
    {
        // Root / body: forward dash lean
        dash->keyFrames[k].kfBones[0].rot = QuatXYZDeg(8, 0, 0);
        dash->keyFrames[k].kfBones[1].rot = QuatXYZDeg(12, 0, 0);
        dash->keyFrames[k].kfBones[2].rot = QuatXYZDeg(18, 0, 0);

        // Left arm bent at side, holding guitar low/ready
        dash->keyFrames[k].kfBones[3].rot = QuatXYZDeg(34, -18, 58);
        dash->keyFrames[k].kfBones[4].rot = QuatXYZDeg(82, 0, 0);
        dash->keyFrames[k].kfBones[5].rot = QuatXYZDeg(0, 0, -8);

        // Right arm mostly still at his side.
        // Small elbow bend only; no big forward pose.
        dash->keyFrames[k].kfBones[6].rot = QuatXYZDeg(12, 0, -10);  // upper_arm_R
        dash->keyFrames[k].kfBones[7].rot = QuatXYZDeg(24, 0, 0);  // forearm_R
        dash->keyFrames[k].kfBones[8].rot = QuatXYZDeg(0, 0, 2);  // hand_R

        // Left planting leg: closer to center, a little back, mild bend
        dash->keyFrames[k].kfBones[9].rot = QuatXYZDeg(28, 0, -4);  // thigh_L
        dash->keyFrames[k].kfBones[10].rot = QuatXYZDeg(18, 0, 0);  // shin_L
        dash->keyFrames[k].kfBones[11].rot = QuatXYZDeg(0, 0, -2);  // foot_L

        // Right/front leg: forward/up, knee bent, but not spread way out
        dash->keyFrames[k].kfBones[12].rot = QuatXYZDeg(-40, 0, 6);  // thigh_R
        dash->keyFrames[k].kfBones[13].rot = QuatXYZDeg(58, 0, 0);  // shin_R
        dash->keyFrames[k].kfBones[14].rot = QuatXYZDeg(4, 0, 2);  // foot_R
    }

    // ---------------------------------------------------------------------
    // SWING: guitar starts low-left, swings up through center
    // ---------------------------------------------------------------------
    KeyFrameGroup* swing = &d->kfGroups[GROUND_L1_KFG_GUITAR_SWING];
    swing->state = DONOGAN_STATE_GROUND_L1_GUITAR_SWING;
    swing->anim = DONOGAN_ANIM_PROC_GROUND_L1_GUITAR_SWING;
    swing->maxKey = 4;
    swing->curKey = 0;

    KfMakeZeroKey(&swing->keyFrames[0], 0.00f, BONES, NUM_BONES);
    KfMakeZeroKey(&swing->keyFrames[1], 0.06f, BONES, NUM_BONES);
    KfMakeZeroKey(&swing->keyFrames[2], 0.10f, BONES, NUM_BONES);
    KfMakeZeroKey(&swing->keyFrames[3], 0.28f, BONES, NUM_BONES);

    // KEY 0: guitar low-left, ready to rip upward
    swing->keyFrames[0].kfBones[0].rot = QuatXYZDeg(0, 0, 0);
    swing->keyFrames[0].kfBones[1].rot = QuatXYZDeg(6, 0, -6);
    swing->keyFrames[0].kfBones[2].rot = QuatXYZDeg(10, 0, -10);

    swing->keyFrames[0].kfBones[3].rot = QuatXYZDeg(28, -24, 70);
    swing->keyFrames[0].kfBones[4].rot = QuatXYZDeg(96, 0, 0);
    swing->keyFrames[0].kfBones[5].rot = QuatXYZDeg(0, 0, -12);

    swing->keyFrames[0].kfBones[6].rot = QuatXYZDeg(38, 8, -22);
    swing->keyFrames[0].kfBones[7].rot = QuatXYZDeg(46, 0, 0);
    swing->keyFrames[0].kfBones[8].rot = QuatXYZDeg(0, 0, 0);

    // Legs planted after dash
    swing->keyFrames[0].kfBones[9].rot = QuatXYZDeg(-18, 0, -14);
    swing->keyFrames[0].kfBones[10].rot = QuatXYZDeg(24, 0, 0);
    swing->keyFrames[0].kfBones[11].rot = QuatXYZDeg(0, 0, -6);
    swing->keyFrames[0].kfBones[12].rot = QuatXYZDeg(14, 0, 14);
    swing->keyFrames[0].kfBones[13].rot = QuatXYZDeg(-18, 0, 0);
    swing->keyFrames[0].kfBones[14].rot = QuatXYZDeg(0, 0, 6);

    // KEY 1: through center / hit moment
    swing->keyFrames[1].kfBones[0].rot = QuatXYZDeg(0, 0, 0);
    swing->keyFrames[1].kfBones[1].rot = QuatXYZDeg(14, 0, 0);
    swing->keyFrames[1].kfBones[2].rot = QuatXYZDeg(24, 0, 4);

    swing->keyFrames[1].kfBones[3].rot = QuatXYZDeg(58, -8, 28);
    swing->keyFrames[1].kfBones[4].rot = QuatXYZDeg(72, 0, 0);
    swing->keyFrames[1].kfBones[5].rot = QuatXYZDeg(0, 0, 8);

    swing->keyFrames[1].kfBones[6].rot = QuatXYZDeg(44, -8, -18);
    swing->keyFrames[1].kfBones[7].rot = QuatXYZDeg(70, 0, 0);
    swing->keyFrames[1].kfBones[8].rot = QuatXYZDeg(0, 0, 8);

    swing->keyFrames[1].kfBones[9].rot = QuatXYZDeg(-12, 0, -18);
    swing->keyFrames[1].kfBones[10].rot = QuatXYZDeg(20, 0, 0);
    swing->keyFrames[1].kfBones[11].rot = QuatXYZDeg(0, 0, -8);
    swing->keyFrames[1].kfBones[12].rot = QuatXYZDeg(18, 0, 18);
    swing->keyFrames[1].kfBones[13].rot = QuatXYZDeg(-22, 0, 0);
    swing->keyFrames[1].kfBones[14].rot = QuatXYZDeg(0, 0, 8);

    // KEY 2: follow-through upward
    swing->keyFrames[2].kfBones[0].rot = QuatXYZDeg(0, 0, 0);
    swing->keyFrames[2].kfBones[1].rot = QuatXYZDeg(4, 0, 8);
    swing->keyFrames[2].kfBones[2].rot = QuatXYZDeg(8, 0, 14);

    swing->keyFrames[2].kfBones[3].rot = QuatXYZDeg(100, 10, -16);
    swing->keyFrames[2].kfBones[4].rot = QuatXYZDeg(38, 0, 0);
    swing->keyFrames[2].kfBones[5].rot = QuatXYZDeg(0, 0, 16);

    swing->keyFrames[2].kfBones[6].rot = QuatXYZDeg(30, -4, -10);
    swing->keyFrames[2].kfBones[7].rot = QuatXYZDeg(30, 0, 0);
    swing->keyFrames[2].kfBones[8].rot = QuatXYZDeg(0, 0, 0);

    swing->keyFrames[2].kfBones[9].rot = QuatXYZDeg(-8, 0, -10);
    swing->keyFrames[2].kfBones[10].rot = QuatXYZDeg(12, 0, 0);
    swing->keyFrames[2].kfBones[11].rot = QuatXYZDeg(0, 0, -4);
    swing->keyFrames[2].kfBones[12].rot = QuatXYZDeg(8, 0, 10);
    swing->keyFrames[2].kfBones[13].rot = QuatXYZDeg(-10, 0, 0);
    swing->keyFrames[2].kfBones[14].rot = QuatXYZDeg(0, 0, 4);

    // KEY 3: recover
    swing->keyFrames[3].kfBones[0].rot = QuatXYZDeg(0, 0, 0);
    swing->keyFrames[3].kfBones[1].rot = QuatXYZDeg(2, 0, 0);
    swing->keyFrames[3].kfBones[2].rot = QuatXYZDeg(4, 0, 0);

    // KEY 3: recover / follow-through
    swing->keyFrames[3].kfBones[3].rot = QuatXYZDeg(0, 0, 0);  // upper_arm_L
    swing->keyFrames[3].kfBones[4].rot = QuatXYZDeg(0, 0, 0);  // forearm_L
    swing->keyFrames[3].kfBones[5].rot = QuatXYZDeg(0, 0, 0);  // hand_L

    swing->keyFrames[3].kfBones[6].rot = QuatXYZDeg(18, 0, -8);
    swing->keyFrames[3].kfBones[7].rot = QuatXYZDeg(22, 0, 0);
    swing->keyFrames[3].kfBones[8].rot = QuatXYZDeg(0, 0, 0);

    swing->keyFrames[3].kfBones[9].rot = QuatXYZDeg(0, 0, -4);
    swing->keyFrames[3].kfBones[10].rot = QuatXYZDeg(8, 0, 0);
    swing->keyFrames[3].kfBones[11].rot = QuatXYZDeg(0, 0, 0);
    swing->keyFrames[3].kfBones[12].rot = QuatXYZDeg(0, 0, 4);
    swing->keyFrames[3].kfBones[13].rot = QuatXYZDeg(8, 0, 0);
    swing->keyFrames[3].kfBones[14].rot = QuatXYZDeg(0, 0, 0);
}
static void DonInitWrenchSwingKf(Donogan* d)
{
    DonBone bones[] = {
        DON_BONE_DEF_SPINE002,
        DON_BONE_DEF_SPINE003,
        DON_BONE_DEF_SHOULDER_R,
        DON_BONE_DEF_UPPER_ARM_R,
        DON_BONE_DEF_FOREARM_R,
        DON_BONE_DEF_HAND_R,
        DON_BONE_DEF_SHOULDER_L,
        DON_BONE_DEF_UPPER_ARM_L,
        DON_BONE_DEF_FOREARM_L,
        DON_BONE_DEF_HAND_L
    };

    KeyFrameGroup* g = &d->kfGroups[WRENCH_KFG_SWING];
    g->state = DONOGAN_STATE_WRENCH_SWING;
    g->anim = DONOGAN_ANIM_PROC_WRENCH_SWING;
    g->maxKey = 4;
    g->curKey = 0;

    for (int k = 0; k < 4; k++) {
        KfMakeZeroKey(&g->keyFrames[k], k * 0.1f, bones, 10);
    }

    // ---------------------------------------------------------------------
// WRENCH SWING:
// Goal: chest-height right-to-left sweep across Donogan's body.
//
// Index map:
// 0 spine002
// 1 spine003
// 2 shoulderR
// 3 upper_armR
// 4 forearmR
// 5 handR
// 6 shoulderL
// 7 upper_armL
// 8 forearmL
// 9 handL
//
// Important:
// Keep upper_armR X modest. Big X is what sends the arm up through head.
// Use mostly Y/Z to sweep across the chest.
// ---------------------------------------------------------------------

// KEY 0: wind-up on Donogan's right side, chest height
    g->keyFrames[0].kfBones[0].rot = QuatXYZDeg(0, -4, 0);
    g->keyFrames[0].kfBones[1].rot = QuatXYZDeg(0, -6, 0);

    g->keyFrames[0].kfBones[2].rot = QuatXYZDeg(0, 0, 0);
    g->keyFrames[0].kfBones[3].rot = QuatXYZDeg(22, -20, -12); // was -38, less far right
    g->keyFrames[0].kfBones[4].rot = QuatXYZDeg(16, 24, 0);    // was 34
    g->keyFrames[0].kfBones[5].rot = QuatXYZDeg(0, 0, -18);    // was -28

    // Left arm gets out of the way a little
    g->keyFrames[0].kfBones[6].rot = QuatXYZDeg(0, 0, 0);
    g->keyFrames[0].kfBones[7].rot = QuatXYZDeg(10, 0, -28);
    g->keyFrames[0].kfBones[8].rot = QuatXYZDeg(8, 0, 0);
    g->keyFrames[0].kfBones[9].rot = QuatXYZDeg(0, 0, 0);


    // KEY 1: entering the strike, wrench in front of chest
    // KEY 1: entering strike, already crossing center chest
    g->keyFrames[1].kfBones[0].rot = QuatXYZDeg(0, 10, 0);
    g->keyFrames[1].kfBones[1].rot = QuatXYZDeg(0, 15, 0);

    g->keyFrames[1].kfBones[2].rot = QuatXYZDeg(0, 0, 0);
    g->keyFrames[1].kfBones[3].rot = QuatXYZDeg(24, 28, -4); // was 12
    g->keyFrames[1].kfBones[4].rot = QuatXYZDeg(12, 58, 0);  // was 48
    g->keyFrames[1].kfBones[5].rot = QuatXYZDeg(0, 0, 0);    // was -10

    g->keyFrames[1].kfBones[6].rot = QuatXYZDeg(0, 0, 0);
    g->keyFrames[1].kfBones[7].rot = QuatXYZDeg(10, 0, -35);
    g->keyFrames[1].kfBones[8].rot = QuatXYZDeg(8, 0, 0);
    g->keyFrames[1].kfBones[9].rot = QuatXYZDeg(0, 0, 0);


    // KEY 2: strongest hit, wrench crosses to Donogan's left side
    // KEY 2: strongest hit, wrench crosses farther to Donogan's left
    g->keyFrames[2].kfBones[0].rot = QuatXYZDeg(0, 24, 0);
    g->keyFrames[2].kfBones[1].rot = QuatXYZDeg(0, 34, 0);

    g->keyFrames[2].kfBones[2].rot = QuatXYZDeg(0, 0, 0);
    g->keyFrames[2].kfBones[3].rot = QuatXYZDeg(22, 82, 14); // was 58
    g->keyFrames[2].kfBones[4].rot = QuatXYZDeg(8, 82, 0);   // was 62
    g->keyFrames[2].kfBones[5].rot = QuatXYZDeg(0, 0, 26);   // was 18

    // Left arm tucks farther away from the wrench path
    g->keyFrames[2].kfBones[6].rot = QuatXYZDeg(0, 0, 0);
    g->keyFrames[2].kfBones[7].rot = QuatXYZDeg(12, 0, -48);
    g->keyFrames[2].kfBones[8].rot = QuatXYZDeg(12, 0, 0);
    g->keyFrames[2].kfBones[9].rot = QuatXYZDeg(0, 0, 0);


    // KEY 3: settle/recover, still low, no head clipping
    // KEY 3: settle/recover, still slightly left of center
    g->keyFrames[3].kfBones[0].rot = QuatXYZDeg(0, 8, 0);
    g->keyFrames[3].kfBones[1].rot = QuatXYZDeg(0, 12, 0);

    g->keyFrames[3].kfBones[2].rot = QuatXYZDeg(0, 0, 0);
    g->keyFrames[3].kfBones[3].rot = QuatXYZDeg(18, 38, 6); // was 24
    g->keyFrames[3].kfBones[4].rot = QuatXYZDeg(8, 48, 0);  // was 34
    g->keyFrames[3].kfBones[5].rot = QuatXYZDeg(0, 0, 12);  // was 8

    g->keyFrames[3].kfBones[6].rot = QuatXYZDeg(0, 0, 0);
    g->keyFrames[3].kfBones[7].rot = QuatXYZDeg(8, 0, -24);
    g->keyFrames[3].kfBones[8].rot = QuatXYZDeg(6, 0, 0);
    g->keyFrames[3].kfBones[9].rot = QuatXYZDeg(0, 0, 0);
}

// Choose the active keyframe group based on current proc anim
static inline KeyFrameGroup* DonActiveKfGroup(Donogan* d) {
    switch (d->curAnimId) {
    case DONOGAN_ANIM_PROC_BOW_ENTER: return &d->kfGroups[BOW_KFG_ENTER];
    case DONOGAN_ANIM_PROC_BOW_AIM:   return &d->kfGroups[BOW_KFG_AIM];
    case DONOGAN_ANIM_PROC_BOW_PULL:  return &d->kfGroups[BOW_KFG_PULL];  // NEW
    case DONOGAN_ANIM_PROC_BOW_REL:   return &d->kfGroups[BOW_KFG_REL];   // NEW
    case DONOGAN_ANIM_PROC_BOW_EXIT:  return &d->kfGroups[BOW_KFG_EXIT];
    case DONOGAN_ANIM_PROC_SPELL_SHOOT:  return &d->kfGroups[SPELL_KFG_SHOOT];
    case DONOGAN_ANIM_PROC_MACHINE_TURN: return &d->kfGroups[MACHINE_KFG_TURN];//NEW for machines
    case DONOGAN_ANIM_PROC_WRENCH_SWING: return &d->kfGroups[WRENCH_KFG_SWING];//wrench attack
    case DONOGAN_ANIM_PROC_AIR_R2_SPELL_SHOOT:   return &d->kfGroups[AIR_R2_KFG_SHOOT];
    case DONOGAN_ANIM_PROC_AIR_R1_HAND_STAND:   return &d->kfGroups[AIR_R1_KFG_HAND_STAND];
    case DONOGAN_ANIM_PROC_AIR_R1_RELEASE:   return &d->kfGroups[AIR_R1_KFG_RELEASE];
    case DONOGAN_ANIM_PROC_AIR_L2_SPHERE_SLAM: return &d->kfGroups[AIR_L2_KFG_SPHERE_SLAM];
    case DONOGAN_ANIM_PROC_AIR_L1_GUITAR_SLAM: return &d->kfGroups[AIR_L1_KFG_GUITAR_SLAM];
    case DONOGAN_ANIM_PROC_GROUND_L1_GUITAR_DASH: return &d->kfGroups[GROUND_L1_KFG_GUITAR_DASH];
    case DONOGAN_ANIM_PROC_GROUND_L1_GUITAR_SWING: return &d->kfGroups[GROUND_L1_KFG_GUITAR_SWING];
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
static inline void DonUpdateBowBlend(Donogan* d, float dt)
{
    // Decide the target for the blend
    float target = (d->state == DONOGAN_STATE_BOW_EXIT) ? 0 : 1.0f;

    // Use the active keyframe group's first bone to pick a rate + interpol
    float rate = 8.0f;                      // sensible default
    InterpolateFunc f = LerpFloat;          // you already have this
    KeyFrameGroup* G = DonActiveKfGroup(d); // picks the BOW_* group from curAnimId
    if (G && G->maxKey > 0 && G->keyFrames[G->curKey].maxBones > 0) {
        const KeyFrameBone* k0 = &G->keyFrames[G->curKey].kfBones[0];
        if (k0->rate > 0) rate = k0->rate;
        if (k0->interpol)    f = k0->interpol;
    }

    d->bowBlend = StepBlend01(d->bowBlend, target, rate, dt, f);
}
///actually SRT
static inline Matrix SRT(Transform t) {
    Matrix S = MatrixScale(t.scale.x, t.scale.y, t.scale.z);
    Matrix R = QuaternionToMatrix(t.rotation);
    Matrix T = MatrixTranslate(t.translation.x, t.translation.y, t.translation.z);
    return MatrixMultiply(S, MatrixMultiply(R, T)); // S*R*T
}
static Matrix BoneWorldFromPose(const Donogan* d, const Transform* pose, int bone) {
    Matrix M = MatrixIdentity();
    for (int b = bone; b != -1; b = d->model.skeleton.bones[b].parent) {
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
    int p = model->skeleton.bones[boneId].parent;
    while (p >= 0) {
        Matrix Mp = SRT(locals[p]);
        M = MatrixMultiply(Mp, M); // world = parentWorld * local
        p = model->skeleton.bones[p].parent;
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
    int parent = model->skeleton.bones[boneId].parent;
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
    int p = d->model.skeleton.bones[bone].parent;
    while (p >= 0) {                                     // climb to root
        q = QuaternionNormalize(QuaternionMultiply(pose[p].rotation, q));
        p = d->model.skeleton.bones[p].parent;
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

    if (det < 0) {
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
    if (trace > 0) {
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
    if (boneId < 0 || boneId >= d->model.skeleton.boneCount) return;

    int parent = d->model.skeleton.bones[boneId].parent;

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
        const Quaternion qBindP = d->model.skeleton.bindPose[parent].rotation;
        const Quaternion qCurP = out[parent].rotation;
        const Quaternion qDeltaP = QuaternionNormalize(QuaternionMultiply(qCurP, QuaternionInvert(qBindP)));
        // Rebuild CHILD from bind locals using the parent's delta (no stretch):
        const Quaternion childBindRot = d->model.skeleton.bindPose[boneId].rotation;
        // World position: parent world + rotated local bind offset
        Vector3 childBindWorld = d->model.skeleton.bindPose[boneId].translation;
        Vector3 parentBindWorld = d->model.skeleton.bindPose[parent].translation;
        Vector3 childRelBindWorld = Vector3Subtract(childBindWorld, parentBindWorld);
        Vector3 childRel = Vector3RotateByQuaternion(childRelBindWorld, qDeltaP);
        out[boneId].translation = Vector3Add(out[parent].translation, childRel);
        // World rotation: parent delta * child's bind local rotation
        out[boneId].rotation = QuaternionNormalize(QuaternionMultiply(qDeltaP, childBindRot));
    }

    // Recurse
    for (int i = 0; i < d->model.skeleton.boneCount; ++i)
    {
        if (d->model.skeleton.bones[i].parent == boneId)
            DonApplyPoseFk(rootBoneId, i, d, KB, out);
    }
}


// Apply current group's current key (single key for now) as deltas on top of bind pose
//static void DonApplyProcPoseFromKF(Donogan* d)
//{
//    if (!d || d->model.skeleton.boneCount <= 0 || !d->model.skeleton.bindPose) { return; }
//
//    const int bc = d->model.skeleton.boneCount;
//    // Temp frame (1 frame) – simple and clear
//    Transform* out = (Transform*)MemAlloc(sizeof(Transform) * bc);
//    if (!out) return;
//
//    // Base = bind pose (later, you can switch this to a cached GLB pose to avoid "snap")
//    for (int i = 0; i < bc; ++i) { out[i] = d->model.skeleton.bindPose[i]; }
//    float alpha = 1.0f;
//    if (d->curAnimId == DONOGAN_ANIM_PROC_BOW_ENTER ||
//        d->curAnimId == DONOGAN_ANIM_PROC_BOW_AIM ||
//        d->curAnimId == DONOGAN_ANIM_PROC_BOW_PULL ||
//        d->curAnimId == DONOGAN_ANIM_PROC_BOW_REL ||
//        d->curAnimId == DONOGAN_ANIM_PROC_BOW_EXIT)
//    {
//        alpha = d->bowBlend;
//    }
//    KeyFrameGroup* G = DonActiveKfGroup(d);
//    if (G && G->maxKey > 0) {
//        const KeyFrame* K = &G->keyFrames[G->curKey]; // one key for now
//        for (int i = 0; i < K->maxBones; ++i) {
//            KeyFrameBone* KB = &K->kfBones[i];
//            int b = (int)KB->boneId;
//            if (b >= 0 && b < bc) {
//                // Scale the keyframe’s delta by alpha
//                KeyFrameBone tmp = *KB;
//                tmp.pos = Vector3Scale(KB->pos, alpha);
//                tmp.rot = QuaternionSlerp(QuaternionIdentity(), KB->rot, alpha);
//
//                // Apply as usual
//                DonApplyPoseFk(b, b, d, &tmp, out);
//            }
//        }
//    }
//
//    // Build 1-frame "animation" and push it
//    Transform* framesArr[1] = { out };
//    ModelAnimation A1;
//    A1.boneCount = bc;
//    A1.keyframeCount = 1;
//    //A1.bones = d->model.skeleton.bones; //raylib 6
//    A1.keyframePoses = framesArr;
//
//    UpdateModelAnimation(d->model, A1, 0);
//    MemFree(out);
//}
static void DonApplyProcPoseFromKF(Donogan* d)
{
    if (!d || d->model.skeleton.boneCount <= 0 || !d->model.skeleton.bindPose) return;

    const int bc = d->model.skeleton.boneCount;

    Transform* out = (Transform*)MemAlloc(sizeof(Transform) * bc);
    if (!out) return;

    for (int i = 0; i < bc; ++i)
    {
        out[i] = d->model.skeleton.bindPose[i];
    }

    float alpha = 1.0f;
    if (d->curAnimId == DONOGAN_ANIM_PROC_BOW_ENTER ||
        d->curAnimId == DONOGAN_ANIM_PROC_BOW_AIM ||
        d->curAnimId == DONOGAN_ANIM_PROC_BOW_PULL ||
        d->curAnimId == DONOGAN_ANIM_PROC_BOW_REL ||
        d->curAnimId == DONOGAN_ANIM_PROC_BOW_EXIT)
    {
        alpha = d->bowBlend;
    }

    KeyFrameGroup* G = DonActiveKfGroup(d);
    if (G && G->maxKey > 0)
    {
        int keyA = 0, keyB = 0;
        float keyT = 0;
        DonGetProcBlendKeys(d, G, &keyA, &keyB, &keyT);

        const KeyFrame* A = &G->keyFrames[keyA];
        const KeyFrame* B = &G->keyFrames[keyB];

        for (int i = 0; i < A->maxBones; ++i)
        {
            KeyFrameBone tmp = A->kfBones[i];

            if (keyA != keyB && i < B->maxBones && B->kfBones[i].boneId == tmp.boneId)
            {
                tmp.pos = Vector3Lerp(A->kfBones[i].pos, B->kfBones[i].pos, keyT);
                tmp.rot = QuaternionSlerp(A->kfBones[i].rot, B->kfBones[i].rot, keyT);
            }

            tmp.pos = Vector3Scale(tmp.pos, alpha);
            tmp.rot = QuaternionSlerp(QuaternionIdentity(), tmp.rot, alpha);

            DonApplyPoseFk((int)tmp.boneId, (int)tmp.boneId, d, &tmp, out);
        }
    }

    Transform* framesArr[1] = { out };
    ModelAnimation A1;
    A1.boneCount = bc;
    A1.keyframeCount = 1;
    A1.keyframePoses = framesArr;

    UpdateModelAnimation(d->model, A1, 0);

    // Cache current pose so attached props can follow bones.
    // This is especially important for guitar/bow/etc.
    int copyCount = bc;
    if (copyCount > DON_BONE_COUNT) copyCount = DON_BONE_COUNT;

    for (int i = 0; i < copyCount; i++)
    {
        d->poseNow[i] = out[i];
    }

    d->poseNowValid = true;

    MemFree(out);
}
// Tunable durations for the one-shot proc anims
#ifndef BOW_ENTER_T
#define BOW_ENTER_T 0.25f
#endif
#ifndef BOW_EXIT_T
#define BOW_EXIT_T  0.20f
#endif
static inline float ProcClamp01(float x)
{
    if (x < 0) return 0;
    if (x > 1.0f) return 1.0f;
    return x;
}

static void DonGetProcBlendKeys(const Donogan* d, const KeyFrameGroup* G,
    int* outKeyA, int* outKeyB, float* outT)
{
    *outKeyA = 0;
    *outKeyB = 0;
    *outT = 0;

    if (!d || !G || G->maxKey <= 0) return;
    if (G->maxKey == 1) return;

    float t = d->animTime;

    if (t <= G->keyFrames[0].time)
    {
        *outKeyA = 0;
        *outKeyB = 0;
        *outT = 0;
        return;
    }

    for (int i = 0; i < G->maxKey - 1; ++i)
    {
        float t0 = G->keyFrames[i].time;
        float t1 = G->keyFrames[i + 1].time;

        if (t <= t1)
        {
            float denom = (t1 - t0);
            float blend = (denom > 0.0001f) ? ((t - t0) / denom) : 1.0f;

            *outKeyA = i;
            *outKeyB = i + 1;
            *outT = ProcClamp01(blend);
            return;
        }
    }

    *outKeyA = G->maxKey - 1;
    *outKeyB = G->maxKey - 1;
    *outT = 0;
}
// Minimal “procedural anim stepper”:
// - ENTER/EXIT finish after fixed time
// - AIM never finishes (loops/holds)
// No bone posing here; this is only to unblock the state machine.
static void DonApplyProcFrame(Donogan* d)
{
    if (!d) return;

    // 1) Apply current procedural pose from keyframes
    DonApplyProcPoseFromKF(d);
    KeyFrameGroup* G = DonActiveKfGroup(d);
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
    case DONOGAN_ANIM_PROC_SPELL_SHOOT:
        if (d->animTime >= 0.4f) d->animFinished = true;
        break;
    case DONOGAN_ANIM_PROC_MACHINE_TURN:
    {
        // 0.48 sec total: setup + three back/forth twists + settle
        const float phase = 0.16f;
        float t = d->animTime;

        if (t < phase) { G->curKey = 0; } // setup
        else if (t < phase * 2.0f) G->curKey = 1;
        else if (t < phase * 3.0f) G->curKey = 2;
        else if (t < phase * 4.0f) G->curKey = 1;
        else if (t < phase * 5.0f) G->curKey = 2;
        else if (t < phase * 6.0f) G->curKey = 1;
        else if (t < phase * 7.0f) G->curKey = 2;
        else if (t < phase * 8.0f) G->curKey = 3;
        else d->animFinished = true;
    } break;
    case DONOGAN_ANIM_PROC_WRENCH_SWING:
    {
        if (!G) { d->animFinished = true; break; }

        const float phase = 0.10f;
        float t = d->animTime;

        if (t < phase * 1.0f) G->curKey = 0;
        else if (t < phase * 2.0f) G->curKey = 1;
        else if (t < phase * 3.0f) G->curKey = 2;
        else if (t < phase * 4.0f) G->curKey = 3;
        else d->animFinished = true;
    } break;
    case DONOGAN_ANIM_PROC_AIR_R2_SPELL_SHOOT:
    {
        if (!G) { d->animFinished = true; break; }

        const float phase = 0.18f;
        float t = d->animTime;

        if (t < phase * 1.0f)      G->curKey = 0;
        else if (t < phase * 2.0f) G->curKey = 1;
        else if (t < phase * 3.0f) G->curKey = 2;
        else if (t < phase * 4.0f) G->curKey = 3;
        else d->animFinished = true;
    } break;
    case DONOGAN_ANIM_PROC_AIR_R1_HAND_STAND:
    {
        if (!G) { d->animFinished = true; break; }

        // This one does NOT finish by itself.
        // It flips into handstand, then holds key 3 until ground / platform / badguy impact.
        float t = d->animTime;

        if (t < 0.14f)      G->curKey = 0;
        else if (t < 0.30f) G->curKey = 1;
        else if (t < 0.46f) G->curKey = 2;
        else                G->curKey = 3;

        d->animFinished = false;
    } break;
    case DONOGAN_ANIM_PROC_AIR_R1_RELEASE:
    {
        if (!G) { d->animFinished = true; break; }

        float t = d->animTime;

        if (t < 0.12f)      G->curKey = 0;
        else if (t < 0.27f) G->curKey = 1;
        else if (t < 0.42f) G->curKey = 2;
        else if (t < 0.54f) G->curKey = 3;
        else                d->animFinished = true;
    } break;
    case DONOGAN_ANIM_PROC_AIR_L2_SPHERE_SLAM:
    {
        if (!G) { d->animFinished = true; break; }

        float t = d->animTime;

        if (t < 0.16f)      G->curKey = 0;
        else if (t < 0.34f) G->curKey = 1;
        else if (t < 0.52f) G->curKey = 2;
        else                G->curKey = 3;

        // Hold final slam pose until we actually hit ground/platform/home-floor/etc.
        d->animFinished = false;
    } break;
    case DONOGAN_ANIM_PROC_AIR_L1_GUITAR_SLAM:
    {
        if (!G) { d->animFinished = true; break; }

        float t = d->animTime;

        if (t < 0.14f)      G->curKey = 0;
        else if (t < 0.34f) G->curKey = 1;
        else if (t < 0.56f) G->curKey = 2;
        else if (t < 0.72f) G->curKey = 3;
        else                d->animFinished = true;
    } break;
    case DONOGAN_ANIM_PROC_GROUND_L1_GUITAR_DASH:
    {
        if (!G) { d->animFinished = true; break; }

        G->curKey = 0;
        d->animFinished = false;
    } break;

    case DONOGAN_ANIM_PROC_GROUND_L1_GUITAR_SWING:
    {
        if (!G) { d->animFinished = true; break; }

        float t = d->animTime;

        if (t < 0.13f)      G->curKey = 0;
        else if (t < 0.30f) G->curKey = 1;
        else if (t < 0.52f) G->curKey = 2;
        else if (t < 0.68f) G->curKey = 3;
        else                d->animFinished = true;
    } break;
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
    if (m->skeleton.boneCount <= 0 || !m->skeleton.bones) {
        printf("PrintModelBones: no bones\n");
        return;
    }
    printf("=== Bones (%d) ===\n", m->skeleton.boneCount);

    // Print all roots (parent == -1), then recurse into children
    for (int i = 0; i < m->skeleton.boneCount; ++i) {
        if (m->skeleton.bones[i].parent == -1) {
            PrintBoneRecursive(m, i, 0);
        }
    }

    // Also list any unparented-but-non-root anomalies (just in case)
    for (int i = 0; i < m->skeleton.boneCount; ++i) {
        if (m->skeleton.bones[i].parent >= m->skeleton.boneCount) {
            printf("[WARN] Bone %d ('%s') has invalid parent index %d\n",
                i, m->skeleton.bones[i].name, m->skeleton.bones[i].parent);
        }
    }
}

static void PrintBoneRecursive(const Model* m, int boneIndex, int depth) {
    const BoneInfo* bi = &m->skeleton.bones[boneIndex];
    // Indent
    for (int d = 0; d < depth; ++d) printf("  ");

    // Header line with index/name/parent
    printf("|-[%3d] '%s'  parent=%d", boneIndex, bi->name, bi->parent);

    // Bind pose (if available)
    if (m->skeleton.bindPose) {
        const Transform tp = m->skeleton.bindPose[boneIndex];
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
    for (int j = 0; j < m->skeleton.boneCount; ++j) {
        if (m->skeleton.bones[j].parent == boneIndex) {
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
static int AnimBoneIndexByName(const Model* model, const char* name)
{
    if (!model || !name) return -1;

    int count = model->skeleton.boneCount;
    BoneInfo* bones = model->skeleton.bones;

    for (int i = 0; i < count; i++)
    {
        if (bones[i].name && strcmp(bones[i].name, name) == 0) { return i; } 
    }

    return -1;
}

static ModelAnimation BuildRemapped(const Model* model, const ModelAnimation* src) {
    ModelAnimation out = { 0 };
    out.boneCount = model->skeleton.boneCount;
    out.keyframeCount = src->keyframeCount;
    //out.bones = model->bones; // target order = model//raylib 6
    out.keyframePoses = (Transform**)MemAlloc(sizeof(Transform*) * out.keyframeCount);
    for (int f = 0; f < (int)out.keyframeCount; f++) {
        out.keyframePoses[f] = (Transform*)MemAlloc(sizeof(Transform) * out.boneCount);
        for (int mb = 0; mb < model->skeleton.boneCount; mb++) {
            const char* mname = model->skeleton.bones[mb].name;
            int ab = AnimBoneIndexByName(model, mname);
            out.keyframePoses[f][mb] = (ab >= 0) ? src->keyframePoses[f][ab] : model->skeleton.bindPose[mb];
        }
    }
    return out;
}

static void FreeRemapped(ModelAnimation* a) {
    if (!a) return;
    for (int f = 0; f < (int)a->keyframeCount; ++f) if (a->keyframePoses[f]) MemFree(a->keyframePoses[f]);
    if (a->keyframePoses) MemFree(a->keyframePoses);
}

static inline float frand01(void) {
    return (float)GetRandomValue(0, 1000) * (1.0f / 1000);
}

// Approx butt world-space anchor: ~55% up from feet, nudged backward along facing
static inline Vector3 DonButtWorld(const Donogan* d) {
    float height = (d->firstBB.max.y - d->firstBB.min.y) * d->scale;
    float feetY = DonFeetWorldY(d);                           // you already have this helper
    float buttY = feetY + 0.55f * height;                       // “hips”
    Vector3 fwd = (Vector3){ sinf(d->yawY), 0, cosf(d->yawY) }; // your yaw-only forward
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
        Vector3 fwd = (Vector3){ sinf(d->yawY), 0, cosf(d->yawY) }; // yaw forward
        Vector3 side = (Vector3){ cosf(d->yawY), 0,-sinf(d->yawY) };
        // New (spreads faster)
        Vector3 vel = Vector3Add(Vector3Scale(up, 1.4f + 1.2f * frand01()),
            Vector3Add(Vector3Scale(fwd, -0.7f * strength),
                Vector3Scale(side, (frand01() - 0.5f) * 1.6f)));
        b->pos = base;
        b->vel = vel;
        b->radius = 0.03f + 0.04f * frand01();
        b->life = 0;
        b->maxLife = 0.9f + 0.5f * frand01();
        b->alive = 1;
        b->origBox = (BoundingBox){ (Vector3) { -1,-1,-1 },(Vector3) { 1,1,1 } };
        b->box = UpdateBoundingBox(b->origBox, b->pos);
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
        b->box = UpdateBoundingBox(b->origBox, b->pos);
    }
}
//helper for drawing
static Matrix DonBoneGlobalMatrix(const Donogan* d, int boneIndex) {
    Matrix M = MatrixIdentity();
    const Transform* frame = d->anims[d->curAnimId].keyframePoses[d->curFrame];
    // Walk up the hierarchy, multiplying local (scale*rot*trans) at each parent
    for (int b = boneIndex; b != -1; b = d->model.skeleton.bones[b].parent) {
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

    d.tex = LoadMyTexture(PNG);
    if (d.tex.id != 0) {
        for (int i = 0; i < d.model.materialCount; i++)
            SetMaterialTexture(&d.model.materials[i], MATERIAL_MAP_ALBEDO, d.tex);
    }

    //bow
    d.bowModel = LoadModel(BOW_GLB);
    d.bowAnimsRaw = LoadModelAnimations(BOW_GLB, &d.bowAnimCount);
    d.bowTex = LoadMyTexture(BOW_PNG);
    SetMaterialTexture(&d.bowModel.materials[0], MATERIAL_MAP_ALBEDO, d.bowTex);
    d.bowOffset = (Vector3){ 1.8f, 2.92f, 0.1f };  // start at exact Donogan origin
    d.bowEulerDeg = (Vector3){ 0, 180, 0 };
    d.bowScale = 0.76f;
    //d.bowBoneIndex = DON_BONE_DEF_SPINE002; //DON_BONE_DEF_HAND_L;
    // attach to LEFT HAND now
    d.bowBoneIndex = DON_BONE_DEF_HAND_L;

    // bow mode state
    d.bowMode = false;
    d.hasBow = false;
    d.prevL2Held = false;
    d.bowBlend = 0;

    //things
    d.unlockedTruck = false;

    //wrench load
    d.wrenchMode = false;
    d.hasWrench = false; //this will need to persist
    d.wrenchModel = LoadModel("models/wrench2.obj");
    d.wrenchTex = LoadMyTexture("textures/wrench.png");
    SetMaterialTexture(&d.wrenchModel.materials[0], MATERIAL_MAP_ALBEDO, d.wrenchTex);

    //guitar load
    d.guitarModel = LoadModel("models/guitar.obj");
    d.guitarTex = LoadMyTexture("textures/guitar.png");
    SetMaterialTexture(&d.guitarModel.materials[0], MATERIAL_MAP_ALBEDO, d.guitarTex);

    // Since the OBJ origin is already near the neck/headstock grip,
    // start with nearly zero offset.
    d.guitarScale = 0.52f;
    d.guitarGripOffset = (Vector3){ -0.010f, -0.008f, 0.016f };
    // Rotate the guitar slightly so the body is not straight through Don's torso.
    d.guitarGripEulerDeg = (Vector3){ 180, 0, 0 };

    d.poseNowValid = false;
    for (int i = 0; i < DON_BONE_COUNT; i++)
    {
        d.poseNow[i] = (Transform){ 0 };
    }

    // Load animations and build remapped copies by bone name
    d.animsRaw = LoadModelAnimations(GLB_ANIM, &d.animCount);
    if (d.animCount > 0) {
        d.anims = (ModelAnimation*)MemAlloc(sizeof(ModelAnimation) * d.animCount);
        for (unsigned i = 0; i < d.animCount; ++i) d.anims[i] = BuildRemapped(&d.model, &d.animsRaw[i]);
    }

    // Bounds + autoscale to ~2m tall
    //float height = d.firstBB.max.y - d.firstBB.min.y;
    //d.scale = (height > 0.0001f) ? Clampf(2.0f / height, 0.01f, 100) : 1.0f;
    d.scale = 2.8;
    // Pose/orient
    d.modelYawX = 0; // set -90 if needed; we’ll bake it into model.transform below
    d.model.transform = MatrixMultiply(d.model.transform, MatrixRotateX(DEG2RAD * d.modelYawX));
    d.pos = (Vector3){ 0 };
    d.yawY = 0;
    d.firstBB = GetMeshBoundingBox(d.model.meshes[0]);
    d.bbCenter = Vector3Scale(Vector3Add(d.firstBB.min, d.firstBB.max), 0.5f);
    d.origBB = ScaleBoundingBox(d.firstBB, d.scale);
    d.origInnerBB = d.origBB;
    d.origOuterBB = d.origBB;
    d.origInnerBB.max.x = d.origInnerBB.max.z;
    d.origInnerBB.min.x = d.origInnerBB.min.z;
    d.origOuterBB.max.z = d.origOuterBB.max.x;
    d.origOuterBB.min.z = d.origOuterBB.min.x;

    // State/anim defaults
    d.state = DONOGAN_STATE_IDLE;
    d.curAnimId = DONOGAN_ANIM_Idle_Loop;
    d.animLoop = true;
    d.animFinished = false;
    d.animTime = 0;
    d.curFrame = 0;
    d.animFps = 24.0f; // nominal

    //bow stuff for animation
    d.bowCur      = -1;
    d.bowFrame = 0;
    d.bowLoop = false;
    d.bowFinished = true;
    d.bowTime = 0;
    d.bowFps = 24.0f;

    // Movement tunables
    d.walkSpeed = 12.2f;
    d.runSpeed = 24.8f;
    d.turnSpeed = DEG2RAD * 540; // turn quickly to face motion
    d.runningHeld = false;

    // Jump timing
    d.prevCross = false;
    d.jumpTimer = 0;
    d.minAirTime = 0.28f;

    // --- Physics defaults ---
    d.groundY = 0;
    d.gravity = -40;  // gamey gravity; tweak  (-9.81 feels floaty with 24fps anims)
    d.jumpSpeed = 12.0f;    // ~1.5m jump apex with gravity=-20
    d.runJumpSpeed = 20;    //
    d.velY = 0;
    d.onGround = false;
    d.startToLoopTime = 0.18f; // how long Jump_Start should play before switching to Jump_Loop
    d.velXZ = (Vector3){ 0,0,0 };   // <-- start with no horizontal velocity

    //water swimming
    d.inWater = false;
    d.swimSpeed = 13.666f;
    d.swimTurnSpeed = DEG2RAD * 240;
    //d.swimFloatOffset = 0.90f;   // ~chest at surface

    d.groundEps = 0.81f;
    d.stepDownTolerance = 0.35f;  // roughly ankle height – tweak to taste
    d.liftoffBump = d.stepDownTolerance + (d.groundEps/4.0f) + 0.01f;

    d.fallGapThreshold = 1.20f;   // your “only fall if > 1.2f”
    d.stepUpMax = 0.60f;   // how high he can “step up” instantly
    d.slopeFollowRate = 0;    // 0 = snap; try 12.0f for smoothing

    d.swimMoveEnter = 0.14f;  // enter when stick > 14%
    d.swimMoveExit = 0.08f;  // stay moving until < 8%

    d.stepUpRate = 6.0f;   // climbs up to 6 m/s
    d.stepUpMaxInstant = 0.25f;  // allows a small pop for jaggy ground

    d.swimEnterToExitLock = CreateTimer(0.42f);//very short

    d.runLock = false;
    d.prevL3 = false;

    d.camPitch = 0;
    d.waterY = PLAYER_FLOAT_Y_POSITION;      // start same; preview will set both properly
    d.seabedY = d.groundY;

    d.swimDiveVel = (Vector3){ 0 };
    d.swimDiveBurst = 58.88f;   // try 8–14
    d.swimDiveDrag = 3.732f;    // higher = stops sooner
    //d.swimMinClear = 0.25f;   // ~ankle clearance
    
    // roll burst
    d.rollVel = (Vector3){ 0 };
    d.rollBurst = 4.0f;   // ~1.25x your run speed; tweak 12–20
    d.rollDrag = 6.5f;    // 1/sec; 6–10 gives a snappy decel

    d.groundNormal = (Vector3){ 0,1,0 };   // safe default

    d.slopeMinUpDot = 0.65f;     // ~75°+ becomes “too steep”
    d.steepSlideAccel = 8.0f;      // ramp into the slide quickly
    d.steepSlideMax = d.runSpeed * 1.1134f;
    d.steepSlideFriction = 1.6f;    // decay a bit each frame
    d.slideDwell = CreateTimer(0.25f);
    d.talkStartTimer = CreateTimer(0.2222f);
    d.interactionLimitTimer = CreateTimer(1.00f);
    StartTimer(&d.interactionLimitTimer);
    d.spellTimer = CreateTimer(0.2f);
    d.cached_yawY = 0;
    //bow speed turn
    d.bowTurnSpeed = DEG2RAD * 90; // turn quickly to face motion
    d.bowDrawTLatch = 0;
    d.bowReleaseCamHold = 0;

    d.hitTimer = CreateTimer(2.4f);
    d.drawColor = WHITE;

    d.gluedToPlatform = false;
    d.gluedPlatId = -1;
    d.jumpPressedEdge = false;

    d.maxHealth = 100;
    d.health = 100;
    d.mana = 100;
    d.maxMana = 100;
    d.shook = 0;
    d.money = 0;
    d.hasGuitar = false;
    d.galBooksGiven = 0;
    d.aliBooksGiven = 0;
    //jump attacks
    d.ja_l1_unlocked = false;
    d.ja_r1_unlocked = false;
    d.ja_l2_unlocked = false;
    d.ja_r2_unlocked = false;
    d.eatenByShark = false;
    d.eatenTimer = CreateTimer(5.00f);
    //PrintModelBones(&d.model);
    //PrintModelBones(&d.bowModel);
    //proc anim setup
    BowStripScaleAndRootOffset(&d);
    DonInitBowKeyframeGroups(&d);
    DonInitSpellShootKeyframeGroups(&d);
    DonInitAirR2SpellKeyframeGroups(&d);
    DonInitAirR1HandstandKeyframeGroups(&d);
    DonInitAirL2SphereSlamKeyframeGroups(&d);
    DonInitAirL1GuitarSlamKeyframeGroups(&d);
    DonInitGroundL1GuitarKeyframeGroups(&d);
    DonInitMachineTurnKeyframeGroups(&d);
    DonInitWrenchSwingKf(&d);
    DonInitArrows(&d);

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
        d->animTime = 0;
        d->curFrame = 0;
    }
}

static void DonApplyFrame(Donogan* d)
{
    if (!d || d->animCount == 0 || !d->anims) return;
    const ModelAnimation* A = &d->anims[d->curAnimId];
    int fc = (int)A->keyframeCount; if (fc < 1) fc = 1;

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
    case DONOGAN_STATE_SLIDE:       return DONOGAN_ANIM_Jump_Loop; // sliding //
    case DONOGAN_STATE_PUNCH_JAB_ENTER:         return DONOGAN_ANIM_Punch_Enter;
    case DONOGAN_STATE_PUNCH_JAB:               return DONOGAN_ANIM_Punch_Jab;
    case DONOGAN_STATE_PUNCH_CROSS_ENTER:       return DONOGAN_ANIM_Punch_Enter;
    case DONOGAN_STATE_PUNCH_CROSS:             return DONOGAN_ANIM_Punch_Cross;
    case DONOGAN_STATE_PUNCH_IDLE:              return DONOGAN_ANIM_Punch_Enter;
    case DONOGAN_STATE_SPELL_ENTER:              return DONOGAN_ANIM_Spell_Simple_Enter;
    case DONOGAN_STATE_SPELL_IDLE:              return DONOGAN_ANIM_Spell_Simple_Idle_Loop;
    case DONOGAN_STATE_SPELL_EXIT:              return DONOGAN_ANIM_Spell_Simple_Exit;
    case DONOGAN_STATE_SPELL_SHOOT:             return DONOGAN_ANIM_PROC_SPELL_SHOOT;
    case DONOGAN_STATE_AIR_R2_SPELL_SHOOT:      return DONOGAN_ANIM_PROC_AIR_R2_SPELL_SHOOT;
    case DONOGAN_STATE_AIR_R1_HAND_STAND:      return DONOGAN_ANIM_PROC_AIR_R1_HAND_STAND;
    case DONOGAN_STATE_AIR_R1_RELEASE:      return DONOGAN_ANIM_PROC_AIR_R1_RELEASE;
    case DONOGAN_STATE_AIR_L2_SPELL_SLAM:  return DONOGAN_ANIM_PROC_AIR_L2_SPHERE_SLAM;
    case DONOGAN_STATE_AIR_L1_GUITAR_SLAM: return DONOGAN_ANIM_PROC_AIR_L1_GUITAR_SLAM;
    case DONOGAN_STATE_GROUND_L1_GUITAR_DASH: return DONOGAN_ANIM_PROC_GROUND_L1_GUITAR_DASH;
    case DONOGAN_STATE_GROUND_L1_GUITAR_SWING: return DONOGAN_ANIM_PROC_GROUND_L1_GUITAR_SWING;
    case DONOGAN_STATE_MACHINE_TURN:      return DONOGAN_ANIM_PROC_MACHINE_TURN;
    case DONOGAN_STATE_WRENCH_SWING:      return DONOGAN_ANIM_PROC_WRENCH_SWING;
    case DONOGAN_STATE_HIT:         return DONOGAN_ANIM_Hit_Chest;
    case DONOGAN_STATE_DEATH:       return DONOGAN_ANIM_Death01;
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
                    || s == DONOGAN_STATE_BOW_AIM
                    || s == DONOGAN_STATE_SLIDE
                    || s == DONOGAN_STATE_SPELL_IDLE);
    DonPlay(d, AnimForState(s), loop, true);

    if (s == DONOGAN_STATE_JUMPING) d->jumpTimer = 0;
}
//r1 and other jump attack helpers
static inline bool DonIsAirR1HandstandAttack(const Donogan* d)
{
    if (!d) return false;

    return d->state == DONOGAN_STATE_AIR_R1_HAND_STAND &&
        d->animTime >= 0.10f;
}

static inline BoundingBox DonMakeAirR1HandstandBox(const Donogan* d)
{
    // Big forgiving attack box around Don while he is diving/handstanding.
    // This does not rotate with the visual model; it is gameplay-friendly.
    Vector3 c = d->pos;

    float h = d->outerBox.max.y - d->outerBox.min.y;

    c.y = d->outerBox.min.y + h * 0.48f;

    Vector3 half = {
        1.85f,
        2.35f,
        1.85f
    };

    return (BoundingBox) {
        { c.x - half.x, c.y - half.y, c.z - half.z },
        { c.x + half.x, c.y + half.y, c.z + half.z }
    };
}

static inline void DonStartAirR1Release(Donogan* d)
{
    if (!d) return;

    // Release any moving-platform glue before bouncing upward.
    d->gluedToPlatform = false;
    d->gluedPlatId = -1;

    // Slightly higher-than-normal pop.
    d->velY = d->jumpSpeed * 1.5f;

    // Keep some motion, but do not launch him wildly sideways.
    d->velXZ = Vector3Scale(d->velXZ, 0.45f);
    d->rollVel = (Vector3){ 0 };

    d->onGround = false;

    // Small lift so he does not instantly retrigger the same ground/platform.
    d->pos.y += d->liftoffBump * 0.85f;

    Don_UpdateBoxes(d);
    DonSetState(d, DONOGAN_STATE_AIR_R1_RELEASE);
}
//water helpers------------------------------------------------------------------------------------
static inline void DonClampToWater(Donogan* d) {
    // Keep the body riding at the surface
    float surfaceY = d->waterY; // treat groundY as water level 
    d->pos.y = surfaceY - d->firstBB.min.y * d->scale; // +d->swimFloatOffset;
    d->velY = 0;
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

float GetAnimationRate(DonoganAnim anim)
{
    if(anim == DONOGAN_ANIM_Jump_Land) { return 11; }
    else if (DONOGAN_ANIM_Punch_Enter) { return 5; }
    else if (DONOGAN_ANIM_Punch_Jab) { return 6; }
    else if (DONOGAN_ANIM_Punch_Cross) { return 4; }
    else { return 2; }
}

static void SpawnBall(const Donogan* d, SpellBall* b) {
    Vector3 spawnOff = (Vector3){ 0.25f, 3.1f, 0.6f }; // tweak: right, up, forward from Donny
    Vector3 spawn = Vector3Add(d->pos, RotYawOffset(spawnOff, d->yawY, d->scale, false));
    Vector3 dir = Vector3Negate(DonAimForward(d, 12.34));
    dir.y = -dir.y;
    b->pos = spawn;
    b->vel = Vector3Scale(dir, 28.0f); // speed
    b->radius = 0.35f;
    b->growRate = 3.5f; // m/s growth
    b->life = 3.12f;
    b->alive = 1;
}

void UpdateBalls(float dt) {
    for (int i = 0; i < MAX_BALLS; i++) {
        if (!balls[i].alive) continue;
        balls[i].pos = Vector3Add(balls[i].pos, Vector3Scale(balls[i].vel, dt));
        balls[i].radius += balls[i].growRate * dt;
        balls[i].life -= dt;
        if (balls[i].life <= 0) balls[i].alive = 0;
    }
    UpdateAirL2SlamSphere(dt);
}

// ---------- Per-frame update (controller → state → anim/frame) ----------
static void DonUpdate(Donogan* d, const ControllerData* pad, float dt, bool freeze, bool disableRoll)
{
    if (!d) return;
    if (d->health <= 0)
    {
        DonSetState(d, DONOGAN_STATE_DEATH);
    }
    if (d->eatenByShark)
    {
        if (HasTimerElapsed(&d->eatenTimer))
        {
            d->eatenByShark = false;
            d->health = 0;
            d->inWater = false;
            d->pos = (Vector3){ 2973.70f, 322.00f, 4042.42f };
            DonSetState(d, DONOGAN_STATE_DEATH);
        }
        else
        {
            freeze = true;
        }
    }
    if (!freeze)
    {
        //d->squareThrowRequest = false;
        // --- Input ---
        bool padPresent = (pad != NULL);
        float lx = padPresent ? pad->normLX : 0;
        float ly = padPresent ? pad->normLY : 0;
        bool cross = padPresent ? pad->btnCross : false;
        if (d->gs->menuOpen || d->isTalking)
        {
            cross = 0;
        }
        bool circle = padPresent ? pad->btnCircle : false;
        bool L3 = padPresent ? pad->btnL3 : false;
       
        bool L2 = padPresent ? pad->btnL2 : false;
        bool L2JumpPressed = L2 && !d->prevL2;
        bool L2Pressed = d->hasBow && L2 && !d->prevL2; //prevent the bow mode if not have bow
        bool L2Released = !L2 && d->prevL2;

        bool R2 = padPresent ? pad->btnR2 : false;
        bool R2Pressed = (R2 && !d->prevR2);
        bool R2Released = (!R2 && d->prevR2);
        bool L1 = padPresent ? pad->btnL1 : false;
        bool R1 = padPresent ? pad->btnR1 : false;
        bool L1Pressed = (L1 && !d->prevL1);
        bool R1Pressed = (R1 && !d->prevR1);
        d->prevR2 = R2;
        d->prevL2 = L2;
        d->prevR1 = R1;
        d->prevL1 = L1;
        bool square = padPresent ? pad->btnSquare : false;
        bool squarePressed = square && !d->prevSquare;
        bool squareReleased = !square && d->prevSquare;
        d->prevSquare = square;

        //get bow ready
        if (d-> bowMode && R2Pressed && d->bowAnimCount >= 1) {
            BowPlay(d, 0, false, true);   // 0 = PULL, one-shot
        }
        if (d->bowMode && R2Released && d->bowAnimCount >= 2) {
            d->bowReleaseCamHold = 0.25f;
            BowPlay(d, 1, false, true);   // 1 = RELEASE, one-shot
            // --- spawn an arrow ---
            // camera-derived forward (you already compute this for swimming)
            float cy = cosf(d->yawY), sy = sinf(d->yawY);
            float cp = cosf(d->camPitch), sp = sinf(d->camPitch);
            Vector3 fwd = DonAimForward(d, 0);
            Vector3 right = (Vector3){ cy, 0, -sy };
            Vector3 up = (Vector3){ 0,1,0 };

            // spawn near right face / bow notch
            // 1) rotate offset by Donny's yaw so the spawn rides with his facing
            Vector3 offW = RotYawOffset(d->arrowOffset, d->yawY, d->scale, /*useScale=*/false);
            Vector3 spawn = Vector3Add(d->pos, offW);

            // draw strength -> speed: simple “held time” mapping (tweak)
            float drawT = Clamp(d->bowTime * 2.0f, 0, 1.0f); // ~0.5s to full
            float speed = Lerp(100, 200, drawT);

            DonFireArrow(d, spawn, fwd, speed);
        }
        // Edge for X (jump)
        bool crossPressed = (cross && !d->prevCross);
        d->prevCross = cross;
        d->jumpPressedEdge = crossPressed;
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
                // stick → world planar direction
                Vector3 moveXZ = fwd;
                float m = Vector3Length(moveXZ);
                if (m > 0.001f) {
                    moveXZ = Vector3Scale(moveXZ, 1.0f / m);
                    d->pos = Vector3Add(d->pos, Vector3Scale(moveXZ, d->swimSpeed * dt)); // or dtLoc
                }
            }

            //4) Apply dive velocity
            d->pos = Vector3Add(d->pos, Vector3Scale(d->swimDiveVel, dt));
            // drag
            float drag = fmaxf(0, 1.0f - d->swimDiveDrag * dt);
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
                    if (d->jumpTimer >= d->startToLoopTime || d->velY <= 0) {
                        DonSetState(d, DONOGAN_STATE_JUMPING); // loops
                    }
                }
                else if (d->state == DONOGAN_STATE_JUMPING &&
                    !d->bowMode &&
                    R1Pressed &&
                    d->ja_r1_unlocked)
                {
                    DonSetState(d, DONOGAN_STATE_AIR_R1_HAND_STAND);

                    // Start the dive. If he was rising, stall it hard.
                    if (d->velY > -6.0f) d->velY = -6.0f;

                    // Keep some XZ movement, but reduce control/drift.
                    d->velXZ = Vector3Scale(d->velXZ, 0.65f);

                    break;
                }
                else if (d->state == DONOGAN_STATE_JUMPING &&
                    !d->bowMode &&
                    R2Pressed &&
                    d->ja_r2_unlocked &&
                    d->mana >= 8)
                {
                    if (Don_TryFireAirR2Spell(d))
                    {
                        DonSetState(d, DONOGAN_STATE_AIR_R2_SPELL_SHOOT);

                        // Optional: small hover/stall so the move reads clearly.
                        if (d->velY < 0) d->velY *= 0.35f;

                        break;
                    }
                }
                else if (d->state == DONOGAN_STATE_JUMPING &&
                    !d->bowMode &&
                    L2JumpPressed &&
                    d->ja_l2_unlocked &&
                    d->mana >= 20)
                {
                    if (Don_TryStartAirL2SphereSlam(d))
                    {
                        break;
                    }
                }
                else if (d->state == DONOGAN_STATE_JUMPING &&
                    !d->bowMode &&
                    L1Pressed &&
                    d->ja_l1_unlocked &&
                    d->hasGuitar)
                {
                    if (Don_TryStartAirL1GuitarSlam(d))
                    {
                        break;
                    }
                }
                // Land if feet cross ground while falling
                if (d->velY <= 0 && DonFeetWorldY(d) <= d->groundY) {
                    DonSnapToGround(d);
                    DonSetState(d, DONOGAN_STATE_JUMP_LAND); // one-shot
                }
                d->pos = Vector3Add(d->pos, Vector3Scale(d->velXZ, (dt) * (d->runningHeld ? d->runSpeed : d->walkSpeed)));

                if (circlePressed && !d->onGround && !disableRoll)
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
                float drag = fmaxf(0, 1.0f - d->rollDrag * dt);
                d->rollVel = Vector3Scale(d->rollVel, drag);
                //stick to ground
                // // --- Ground stick logic ---
                float targetY = d->groundY - d->firstBB.min.y * d->scale;
                float dy = targetY - d->pos.y;

                //if (dy >= 0)
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
                float drag = fmaxf(0, 1.0f - d->rollDrag * dt);
                d->rollVel = Vector3Scale(d->rollVel, drag);

                // If we touch ground during/after air roll, snap & exit
                if (d->velY <= 0 && DonFeetWorldY(d) <= d->groundY) {
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

            case DONOGAN_STATE_BOW_PULL: {
                float curT = Clamp(d->bowTime * 2.0f, 0, 1.0f);  // same mapping you use elsewhere
                d->bowDrawTLatch = curT;
                if (L2Released) { DonSetState(d, DONOGAN_STATE_BOW_EXIT); break; } // optional: or go REL then EXIT
                if (R2Released) { DonSetState(d, DONOGAN_STATE_BOW_REL);  break; } // NEW
                // keep holding PULL while R2 held
            }    break;

            case DONOGAN_STATE_BOW_REL: {
                // optionally allow L2 release here to chain to EXIT after REL finishes
                if (d->animFinished) {
                    if (!L2) DonSetState(d, DONOGAN_STATE_BOW_EXIT);
                    else     DonSetState(d, DONOGAN_STATE_BOW_AIM); // back to aiming
                }
            }   break;

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
                                       
            case DONOGAN_STATE_MACHINE_TURN: //work with machine
            {
                d->velXZ = (Vector3){ 0 };
                d->rollVel = (Vector3){ 0 };
                d->onGround = true;

                float targetY = d->groundY - d->firstBB.min.y * d->scale;
                d->pos.y = targetY;

                if (d->animFinished)
                {
                    DonSetState(d, DONOGAN_STATE_IDLE);
                }
                break;
            }
            case DONOGAN_STATE_PUNCH_JAB_ENTER: {
                // wait until the enter animation finishes, then throw the jab
                if (d->animFinished) {
                    DonSetState(d, DONOGAN_STATE_PUNCH_JAB);
                }
            } break;

            case DONOGAN_STATE_PUNCH_JAB: {
                // when the jab clip ends, return to locomotion
                if (d->animFinished) {
                    float moveMag = sqrtf(lx * lx + ly * ly);
                    if (moveMag > 0.1f)
                    {
                        bool wantsRun = d->runLock || d->runningHeld;
                        DonSetState(d, wantsRun ? DONOGAN_STATE_RUN : DONOGAN_STATE_WALK);
                    }
                    else
                    {
                        DonSetState(d, DONOGAN_STATE_PUNCH_IDLE);
                    }
                }
            } break;

            case DONOGAN_STATE_PUNCH_CROSS_ENTER: {
                if (d->animFinished) {
                    DonSetState(d, DONOGAN_STATE_PUNCH_CROSS);
                }
            } break;

            case DONOGAN_STATE_PUNCH_CROSS: {
                if (d->animFinished) {
                    float moveMag = sqrtf(lx * lx + ly * ly);
                    if (moveMag > 0.1f)
                    {
                        bool wantsRun = d->runLock || d->runningHeld;
                        DonSetState(d, wantsRun ? DONOGAN_STATE_RUN : DONOGAN_STATE_WALK);
                    }
                    else
                    {
                        DonSetState(d, DONOGAN_STATE_PUNCH_IDLE);
                    }
                }
            } break;

            case DONOGAN_STATE_PUNCH_IDLE: {
                if (d->animFinished) {
                    float moveMag = sqrtf(lx * lx + ly * ly);
                    if (L1Pressed) { DonSetState(d, DONOGAN_STATE_PUNCH_JAB); }
                    else if (R1Pressed) {
                        if (d->hasWrench) DonSetState(d, DONOGAN_STATE_WRENCH_SWING);
                        else              DonSetState(d, DONOGAN_STATE_PUNCH_CROSS);
                    }
                    else if (moveMag > 0.1f) {
                        bool wantsRun = d->runLock || d->runningHeld;
                        DonSetState(d, wantsRun ? DONOGAN_STATE_RUN : DONOGAN_STATE_WALK);
                    }
                    else if (L2Pressed) {
                        DonSetState(d, DONOGAN_STATE_BOW_ENTER);
                        d->bowMode = true;
                    }
                    else if (crossPressed) {
                        d->velY = d->runningHeld ? d->runJumpSpeed : d->jumpSpeed;
                        d->pos.y += d->liftoffBump;
                        DonSetState(d, DONOGAN_STATE_JUMP_START);
                    }
                    else if (circlePressed && !disableRoll) {
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
                    }
                    else if (!d->bowMode && //what if you dont have the wrench...?
                        L1Pressed &&
                        d->hasGuitar &&
                        d->ja_l1_unlocked &&
                        (d->state == DONOGAN_STATE_IDLE ||
                            d->state == DONOGAN_STATE_WALK ||
                            d->state == DONOGAN_STATE_RUN))
                    {
                        if (Don_TryStartGroundL1GuitarDash(d))
                        {
                            break;
                        }
                    }
                    else if (squarePressed) {
                        DonSetState(d, DONOGAN_STATE_SPELL_ENTER);
                    }
                    else if (!d->bowMode && R2Pressed && d->mana >= 8)
                    {
                        // Find a free slot and spawn
                        for (int i = 0; i < MAX_BALLS; ++i) {
                            if (!balls[i].alive) {
                                SpawnBall(d, &balls[i]);  // uses Donogan pos/yaw & aim
                                break;
                            }
                        }
                        DonSetState(d, DONOGAN_STATE_SPELL_SHOOT);
                        d->mana -= 8;
                    }
                }
            } break;
            case DONOGAN_STATE_WRENCH_SWING: {
                d->velXZ = (Vector3){ 0 };
                d->rollVel = (Vector3){ 0 };

                if (d->animFinished) {
                    float moveMag = sqrtf(lx * lx + ly * ly);
                    if (moveMag > 0.1f)
                    {
                        bool wantsRun = d->runLock || d->runningHeld;
                        DonSetState(d, wantsRun ? DONOGAN_STATE_RUN : DONOGAN_STATE_WALK);
                    }
                    else
                    {
                        DonSetState(d, DONOGAN_STATE_PUNCH_IDLE);
                    }
                }
            } break;
            case DONOGAN_STATE_SLIDE: //sliding....slide...
            {
                // Treat like ...
                d->onGround = false;

                // 5a) Push downhill along the plane (project gravity onto plane)
                Vector3 g = (Vector3){ 0, d->gravity, 0 }; // gravity is negative
                float gdotn = Vector3DotProduct(g, d->groundNormal);
                Vector3 aSlide = Vector3Subtract(g, Vector3Scale(d->groundNormal, gdotn)); // parallel to plane
                Vector3 aXZ = (Vector3){ aSlide.x, 0, aSlide.z };

                // accelerate + friction + clamp
                d->velXZ = Vector3Add(d->velXZ, Vector3Scale(aXZ, d->steepSlideAccel * dt));
                float sp = Vector3Length(d->velXZ);
                if (sp > d->steepSlideMax && sp > 1e-5f) {
                    d->velXZ = Vector3Scale(d->velXZ, d->steepSlideMax / sp);
                }
                d->velXZ = Vector3Scale(d->velXZ, fmaxf(0, 1.0f - d->steepSlideFriction * dt));

                d->pos.x += d->velXZ.x * dt;
                d->pos.z += d->velXZ.z * dt;

                // 5b) Maintain a tiny gap above the ground so we never "land"
                // 3) RE-SAMPLE surface at the NEW XZ and HARD-STICK Y to it
                float newGroundY = GetTerrainHeightFromMeshXZ(d->pos.x, d->pos.z);
                Vector3 newN = GetTerrainNormalFromMeshXZ(d->pos.x, d->pos.z);

                // If there's no ground under the new XZ, we truly left the wall: go to air.
                if (newGroundY <= -9000 || Vector3Length(newN) < 1e-6f) {
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
                d->velY = 0;

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
                if (d->groundY < -9000 && HasTimerElapsed(&d->slideDwell)) {
                    d->velY = 0;
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
                if (circlePressed && !disableRoll) {
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
                if (!d->bowMode &&
                    L1Pressed &&
                    d->hasGuitar &&
                    d->ja_l1_unlocked &&
                    (d->state == DONOGAN_STATE_IDLE ||
                        d->state == DONOGAN_STATE_WALK ||
                        d->state == DONOGAN_STATE_RUN))
                {
                    if (Don_TryStartGroundL1GuitarDash(d))
                    {
                        break;
                    }
                }
            } break;

            case DONOGAN_STATE_SPELL_ENTER: {
                // Enter is non-loop; when it finishes, go to loop if still holding,
                // otherwise immediately play Exit.
                d->cached_yawY = d->yawY;
                StartTimer(&d->spellTimer);
                if (d->animFinished) {
                    if (square) DonSetState(d, DONOGAN_STATE_SPELL_IDLE);
                    else        DonSetState(d, DONOGAN_STATE_SPELL_EXIT);
                }
            } break;

            case DONOGAN_STATE_SPELL_IDLE: {
                // Hold to stay; release to exit
                if (squareReleased || d->mana<=0) {
                    DonSetState(d, DONOGAN_STATE_SPELL_EXIT);
                    break;
                }
                // (Optional) you can freeze/slow locomotion here if desired.
            } break;

            case DONOGAN_STATE_SPELL_EXIT: {
                float deltaDeg = d->yawY - d->cached_yawY;
                if (fabsf(deltaDeg) > (3.6 * PI))
                {
                    d->squareThrowRequest = true;
                    TraceLog(LOG_INFO, "squareThrowRequest set");
                }
                ResetTimer(&d->spellTimer);
                // Exit is non-loop; when done, return to locomotion like other one-shots
                if (d->animFinished) {
                    float moveMag = sqrtf(lx * lx + ly * ly);
                    if (moveMag > 0.1f)
                    {
                        DonSetState(d, d->runningHeld ? DONOGAN_STATE_RUN : DONOGAN_STATE_WALK);
                    }
                    else
                    {
                        DonSetState(d, DONOGAN_STATE_IDLE);
                    }
                }
            } break;

            case DONOGAN_STATE_SPELL_SHOOT: {
                if (d->animFinished) {
                    DonSetState(d, DONOGAN_STATE_IDLE);
                }
            } break;
            case DONOGAN_STATE_AIR_R2_SPELL_SHOOT:
            {
                // Keep normal airborne physics during the attack.
                d->velY += d->gravity * dt;
                d->pos.y += d->velY * dt;

                // Keep some air drift, but slightly reduced while casting.
                d->pos = Vector3Add(
                    d->pos,
                    Vector3Scale(d->velXZ, dt * (d->runningHeld ? d->runSpeed : d->walkSpeed) * 0.65f)
                );

                // Landing cancels into land state.
                if (d->velY <= 0 && DonFeetWorldY(d) <= d->groundY)
                {
                    DonSnapToGround(d);
                    DonSetState(d, DONOGAN_STATE_JUMP_LAND);
                    break;
                }

                // If the proc move finishes and we are still airborne, go back to jump loop.
                if (d->animFinished)
                {
                    DonSetState(d, DONOGAN_STATE_JUMPING);
                }
            } break;
            case DONOGAN_STATE_AIR_R1_HAND_STAND:
            {
                // Dive downward into handstand.
                // Gravity still applies, but once the pose is underway we force a strong downward drive.
                d->velY += d->gravity * dt * 1.15f;

                if (d->animTime > 0.12f && d->velY > -24.0f)
                {
                    d->velY -= 52.0f * dt;
                    if (d->velY < -24.0f) d->velY = -24.0f;
                }

                d->pos.y += d->velY * dt;

                // Reduced air drift.
                d->pos = Vector3Add(
                    d->pos,
                    Vector3Scale(d->velXZ, dt * (d->runningHeld ? d->runSpeed : d->walkSpeed) * 0.35f)
                );

                Don_UpdateBoxes(d);

                // Anything that has already set groundY can trigger the bounce:
                // terrain, home floors, platforms, water wheel, etc.
                if (d->velY <= 0 && d->outerBox.min.y <= d->groundY + 0.15f)
                {
                    DonSnapToGround(d);
                    DonStartAirR1Release(d);
                    break;
                }

            } break;
            case DONOGAN_STATE_AIR_R1_RELEASE:
            {
                // Bounce upward and right himself.
                d->velY += d->gravity * dt;
                d->pos.y += d->velY * dt;

                d->pos = Vector3Add(
                    d->pos,
                    Vector3Scale(d->velXZ, dt * (d->runningHeld ? d->runSpeed : d->walkSpeed) * 0.55f)
                );

                Don_UpdateBoxes(d);

                // If he hits ground again before the release anim finishes,
                // break out into normal landing.
                if (d->velY <= 0 && d->outerBox.min.y <= d->groundY + 0.10f)
                {
                    DonSnapToGround(d);
                    DonSetState(d, DONOGAN_STATE_JUMP_LAND);
                    break;
                }

                // If the righting animation finishes while still airborne,
                // return to normal jump loop.
                if (d->animFinished)
                {
                    DonSetState(d, DONOGAN_STATE_JUMPING);
                }

            } break;
            case DONOGAN_STATE_AIR_L2_SPELL_SLAM:
            {
                // Keep the sphere near the raised fingertip until impact.
                DonAirL2SlamFollowHand(d);

                // Fast committed slam downward.
                d->velY += d->gravity * dt * 1.35f;

                if (d->velY > -32.0f)
                {
                    d->velY -= 80 * dt;
                    if (d->velY < -32.0f) d->velY = -32.0f;
                }

                d->pos.y += d->velY * dt;

                // Small XZ drift.
                d->pos = Vector3Add(
                    d->pos,
                    Vector3Scale(d->velXZ, dt * (d->runningHeld ? d->runSpeed : d->walkSpeed) * 0.25f)
                );

                Don_UpdateBoxes(d);

                // Anything that sets groundY counts:
                // terrain, authored home floors, platforms, etc.
                if (d->velY <= 0 && d->outerBox.min.y <= d->groundY + 0.18f)
                {
                    DonSnapToGround(d);

                    // Slam sphere becomes the ground shock sphere.
                    DonAirL2SlamLockToGround(d);

                    // Exit into normal land anim while the sphere keeps growing independently.
                    DonSetState(d, DONOGAN_STATE_JUMP_LAND);
                    break;
                }

            } break;
            case DONOGAN_STATE_AIR_L1_GUITAR_SLAM:
            {
                // Guitar slam: committed downward motion, not as violent as L2.
                d->velY += d->gravity * dt * 1.12f;

                if (d->animTime > 0.12f && d->velY > -22.0f)
                {
                    d->velY -= 44.0f * dt;
                    if (d->velY < -22.0f) d->velY = -22.0f;
                }

                d->pos.y += d->velY * dt;

                d->pos = Vector3Add(
                    d->pos,
                    Vector3Scale(d->velXZ, dt * (d->runningHeld ? d->runSpeed : d->walkSpeed) * 0.38f)
                );

                Don_UpdateBoxes(d);

                if (d->velY <= 0.0f && d->outerBox.min.y <= d->groundY + 0.16f)
                {
                    DonSnapToGround(d);
                    DonSetState(d, DONOGAN_STATE_JUMP_LAND);
                    break;
                }

                if (d->animFinished)
                {
                    DonSetState(d, DONOGAN_STATE_JUMPING);
                }

            } break;
            case DONOGAN_STATE_GROUND_L1_GUITAR_DASH:
            {
                d->guitarGroundDashTimer += dt;

                DonFacePointXZ(d, d->guitarGroundTargetPos);

                Vector3 toTarget = Vector3Subtract(d->guitarGroundTargetPos, d->guitarGroundDashStart);
                toTarget.y = 0.0f;

                Vector3 dir = toTarget;

                if (Vector3LengthSqr(dir) > 0.0001f)
                {
                    dir = Vector3Normalize(dir);
                }
                else
                {
                    dir = (Vector3){ sinf(d->yawY), 0.0f, cosf(d->yawY) };
                }

                // Stop slightly in front of the bad guy.
                // If we have a target, stop slightly in front of it.
                // If no target, dash all the way to the fake 12-foot point.
                float stopShort = (d->guitarGroundTargetIndex >= 0) ? 3.0f : 0.0f;

                Vector3 stopPos = Vector3Subtract(d->guitarGroundTargetPos,Vector3Scale(dir, stopShort));

                stopPos.y = d->pos.y;

                const float dashTime = 0.22f;

                float t = d->guitarGroundDashTimer / dashTime;
                if (t > 1.0f) t = 1.0f;

                // Smooth but still snappy.
                float s = t * t * (3.0f - 2.0f * t);

                d->pos = Vector3Lerp(d->guitarGroundDashStart, stopPos, s);

                // Stay grounded.
                d->velY = 0.0f;
                d->onGround = true;
                d->pos.y = d->groundY - d->firstBB.min.y * d->scale;

                Don_UpdateBoxes(d);

                if (t >= 1.0f)
                {
                    DonFacePointXZ(d, d->guitarGroundTargetPos);
                    DonSetState(d, DONOGAN_STATE_GROUND_L1_GUITAR_SWING);
                    break;
                }

            } break;

            case DONOGAN_STATE_GROUND_L1_GUITAR_SWING:
            {
                DonFacePointXZ(d, d->guitarGroundTargetPos);

                d->velY = 0.0f;
                d->velXZ = (Vector3){ 0 };
                d->onGround = true;
                d->pos.y = d->groundY - d->firstBB.min.y * d->scale;

                Don_UpdateBoxes(d);

                if (d->animFinished)
                {
                    d->guitarGroundTargetIndex = -1;
                    d->guitarGroundHitDone = false;

                    if (d->runningHeld)
                    {
                        DonSetState(d, DONOGAN_STATE_RUN);
                    }
                    else
                    {
                        DonSetState(d, DONOGAN_STATE_IDLE);
                    }

                    break;
                }

            } break;
            case DONOGAN_STATE_HIT:
                d->bowMode = false;
                d->bowCur = -1;
                d->bowFinished = true;
                d->bowReleaseCamHold = 0;
                d->runLock = false;
                d->runningHeld = false;
                d->squareThrowRequest = false;
                ResetTimer(&d->spellTimer);
                if (d->animFinished) { DonSetState(d, DONOGAN_STATE_IDLE);}
                break;

            case DONOGAN_STATE_DEATH:
                d->bowMode = false;
                d->bowCur = -1;
                d->bowFinished = true;
                d->bowReleaseCamHold = 0;
                d->squareThrowRequest = false;
                
                ResetTimer(&d->spellTimer);
                if (d->animFinished) { 
                    DonSetState(d, DONOGAN_STATE_IDLE);
                    d->pos = (Vector3){ 2973.70f, 322.00f, 4042.42f };//start at home position after death
                    d->health = d->maxHealth;
                    d->mana = d->maxMana;
                    d->inWater = false;
                    PlaySoundVol(donScream);
                }
                break;

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
                    d->jumpTimer = 0;
                    DonSetState(d, DONOGAN_STATE_JUMP_START);
                    break;
                }

                // roll:
                if (circlePressed && d->onGround && !disableRoll) {
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
                if (!d->bowMode &&
                    L1Pressed &&
                    d->hasGuitar &&
                    d->ja_l1_unlocked &&
                    (d->state == DONOGAN_STATE_IDLE ||
                        d->state == DONOGAN_STATE_WALK ||
                        d->state == DONOGAN_STATE_RUN))
                {
                    if (Don_TryStartGroundL1GuitarDash(d))
                    {
                        break;
                    }
                }
                // --- Steep-slope check: swap to falling + start sliding ---
                if(onLoad && !d->gluedToPlatform && d->onGround)
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
                        Vector3 g = (Vector3){ 0, -1.0f, 0 };
                        Vector3 n = d->groundNormal;
                        Vector3 along = Vector3Subtract(g, Vector3Scale(n, Vector3DotProduct(g, n)));

                        // planar XZ push
                        Vector3 slideXZ = (Vector3){ along.x, 0, along.z };
                        float m = Vector3Length(slideXZ);
                        if (m > 1e-4f) slideXZ = Vector3Scale(slideXZ, 1.0f / m);

                        Vector3 target = Vector3Scale(slideXZ, d->steepSlideMax);
                        d->velXZ = Vector3Lerp(d->velXZ, target, Clampf(d->steepSlideAccel * dt, 0, 1.0f));
                        d->velXZ = Vector3Scale(d->velXZ, fmaxf(0, 1.0f - d->steepSlideFriction * dt));

                        ResetTimer(&d->slideDwell);
                        StartTimer(&d->slideDwell);
                        d->onGround = false;        // very important: we are not “grounded” while sliding
                        d->velY = 0;         // pinned to face (we don’t accumulate airborne vertical)
                        DonSetState(d, DONOGAN_STATE_SLIDE);
                        break;
                    }

                }
                if (squarePressed) {
                    // If you have other modes like bow active, clear them here if needed
                    // d->bowMode = false;
                    DonSetState(d, DONOGAN_STATE_SPELL_ENTER);
                    break;
                }
                else if (!d->bowMode && R2Pressed && d->mana >= 8)
                {
                    // Find a free slot and spawn
                    for (int i = 0; i < MAX_BALLS; ++i) {
                        if (!balls[i].alive) {
                            SpawnBall(d, &balls[i]);  // uses Donogan pos/yaw & aim
                            break;
                        }
                    }
                    DonSetState(d, DONOGAN_STATE_SPELL_SHOOT);
                    d->mana -= 8;
                    break;
                }
                // --- Ground stick logic ---
                float targetY = d->groundY - d->firstBB.min.y * d->scale;
                float dy = targetY - d->pos.y;

                if (dy >= 0) {
                    float maxUpThisFrame = d->stepUpMaxInstant + d->stepUpRate * dt;
                    float climb = (dy < maxUpThisFrame) ? dy : maxUpThisFrame;
                    d->pos.y += climb;
                    d->onGround = true;
                }
                else {
                    float drop = -dy;
                    if (drop <= d->fallGapThreshold) {
                        // follow downward (snap or smooth if you’ve set slopeFollowRate)
                        if (d->slopeFollowRate <= 0) d->pos.y = targetY;
                        else d->pos.y -= drop * Clampf(d->slopeFollowRate * dt, 0, 1.0f);
                        d->onGround = true;
                    }
                    else if(!d->gluedToPlatform) {
                        // big cliff: go airborne
                        d->onGround = false;
                        d->velY = 0;
                        DonSetState(d, DONOGAN_STATE_JUMPING);
                        break;
                    }
                }
                if (!d->bowMode && d->onGround && L1Pressed) {
                    DonSetState(d, DONOGAN_STATE_PUNCH_JAB_ENTER);
                    break;
                }
                if (!d->bowMode && d->onGround && R1Pressed) {
                    if (R1Pressed) {
                        if (d->hasWrench) DonSetState(d, DONOGAN_STATE_WRENCH_SWING);
                        else              DonSetState(d, DONOGAN_STATE_PUNCH_CROSS_ENTER);
                    }
                    //DonSetState(d, DONOGAN_STATE_PUNCH_CROSS_ENTER);
                    break;
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

    if (d->curAnimId >= 0 && d->animCount>0)
    {
        const ModelAnimation* A = &d->anims[d->curAnimId];
        int frameCount = (A) ? (int)A->keyframeCount : 1;
        if (frameCount < 1) frameCount = 1;

        if (d->animLoop) {
            d->curFrame = (d->curFrame + 2) % frameCount;
        }
        else {
            d->curFrame += (GetAnimationRate(d->curAnimId)); //d->curFrame++; //do this twice because it feels slow, 10 for landing
            if (d->curFrame >= frameCount) {
                d->curFrame = frameCount - 1;
                d->animFinished = true;
            }
        }
        DonApplyFrame(d);
    }
    else
    {
        DonUpdateBowBlend(d,dt);
        DonApplyProcFrame(d);
    }

    if (d->bowCur >= 0)
    {
        if (!d->bowFinished) { d->bowTime += dt; }

        const ModelAnimation* A = &d->bowAnimsRaw[d->bowCur];
        int frameCount = A ? (int)A->keyframeCount : 1;
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
    if (d->bowReleaseCamHold > 0) d->bowReleaseCamHold -= dt;
    DonUpdateArrows(d, dt);
    UpdateBalls(dt);
    UpdateLasers(dt);
    d->box = UpdateBoundingBox(d->origBB, (Vector3) {d->pos.x, d->pos.y + 2.22f, d->pos.z});
    d->innerBox = UpdateBoundingBox(d->origInnerBB, (Vector3) { d->pos.x, d->pos.y + 2.22f, d->pos.z });
    d->outerBox = UpdateBoundingBox(d->origOuterBB, (Vector3) { d->pos.x, d->pos.y + 2.22f, d->pos.z });
    //always need health and mana no lower than 0
    if (d->mana < 0) { d->mana = 0; }
    if (d->health < 0) { d->health = 0; }
}

#include "rlgl.h"  // at top of preview.c

// ... in your 3D draw loop, after DrawModel(don.model, ...) ...
void DonDrawBubbles(const Donogan* d, bool displayBoxes) {
    // draw transparent without writing depth to avoid sorting artifacts
    rlDisableDepthMask();
    for (int i = 0; i < DON_MAX_BUBBLES; i++) {
        const Bubble* b = &d->bubbles[i];
        if (!b->alive) continue;
        float t = b->life / b->maxLife;             // 0..1
        unsigned char a = (unsigned char)((1.0f - t) * 160); // fade out
        Color c = (Color){ 180, 220, 255, a };      // light blue with alpha
        DrawSphereEx(b->pos, b->radius, 8, 8, c);   // small, simple sphere
        if (displayBoxes) { DrawBoundingBox(b->box, BLUE); }
    }
    rlEnableDepthMask();
}

static inline void DrawArrow3D(Vector3 tip, Vector3 dir, float totalLen,
    float shaftR, float headLen, float headR,
    Color shaftCol, Color headCol)
{
    if (totalLen <= 0) return;
    Vector3 f = Vector3Normalize(dir);
    if (Vector3Length(f) < 1e-6f) f = (Vector3){ 0,0,1 };

    float Ls = fmaxf(0, totalLen - headLen);      // shaft length
    Vector3 tail = Vector3Add(tip, Vector3Scale(f, -totalLen));
    Vector3 headBase = Vector3Add(tip, Vector3Scale(f, -headLen));
    Vector3 shaftEnd = headBase;

    // shaft: cylinder
    DrawCylinderEx(tail, shaftEnd, shaftR, shaftR, 8, shaftCol);
    // head: cone (radius -> 0 at tip)
    DrawCylinderEx(headBase, tip, headR, 0, 16, headCol);
    // optional little tip bead
    DrawSphere(tip, headR * 0.22f, headCol);
}

static void DonDrawArrows(const Donogan* d) {
    for (int i = 0; i < MAX_ARROWS; i++) {
        const Arrow* a = &d->arrows[i];
        if (!a->alive) continue;
        // Direction from velocity if flying; fall back to facing
        Vector3 dir = a->stuck ? a->dir : Vector3Normalize(a->vel);
        DrawArrow3D(
            a->pos, dir,
            d->arrowLen,
            d->arrowShaftR,
            d->arrowHeadLen,
            d->arrowHeadR,
            (Color) {180, 140, 90, 255},   // shaft
            (Color) {60, 60, 60, 255}      // head
        );
    }
}

void DrawBalls(Camera3D cam, Model ball, Shader lightningBall) {
    // uniforms shared by all balls this frame
    float time = GetTime();
    SetShaderValue(lightningBall, GetShaderLocation(lightningBall, "uTime"), &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(lightningBall, GetShaderLocation(lightningBall, "uViewPos"), &cam.position, SHADER_UNIFORM_VEC3);
    Vector3 color = { 0.45f, 0.75f, 1.0f }; // icy blue
    SetShaderValue(lightningBall, GetShaderLocation(lightningBall, "uColor"), &color, SHADER_UNIFORM_VEC3);
    float intensity = 1.8f, dispAmp = 0.08f, nscale = 3.0f;
    SetShaderValue(lightningBall, GetShaderLocation(lightningBall, "uIntensity"), &intensity, SHADER_UNIFORM_FLOAT);
    SetShaderValue(lightningBall, GetShaderLocation(lightningBall, "uDispAmp"), &dispAmp, SHADER_UNIFORM_FLOAT);
    SetShaderValue(lightningBall, GetShaderLocation(lightningBall, "uNoiseScale"), &nscale, SHADER_UNIFORM_FLOAT);

    BeginBlendMode(BLEND_ADDITIVE); // make it bloom with itself
    for (int i = 0; i < MAX_BALLS; i++) {
        if (!balls[i].alive) continue;
        Matrix m = MatrixMultiply(
            MatrixScale(balls[i].radius, balls[i].radius, balls[i].radius),
            MatrixTranslate(balls[i].pos.x, balls[i].pos.y, balls[i].pos.z)
        );
        DrawMesh(ball.meshes[0], ball.materials[0], m);
    }
    if (gAirL2Slam.active)
    {
        Vector3 slamColor = { 0.85f, 0.55f, 1.0f };
        float slamIntensity = 2.4f;
        float slamDispAmp = 0.14f;
        float slamNoiseScale = 2.2f;

        SetShaderValue(lightningBall, GetShaderLocation(lightningBall, "uColor"), &slamColor, SHADER_UNIFORM_VEC3);
        SetShaderValue(lightningBall, GetShaderLocation(lightningBall, "uIntensity"), &slamIntensity, SHADER_UNIFORM_FLOAT);
        SetShaderValue(lightningBall, GetShaderLocation(lightningBall, "uDispAmp"), &slamDispAmp, SHADER_UNIFORM_FLOAT);
        SetShaderValue(lightningBall, GetShaderLocation(lightningBall, "uNoiseScale"), &slamNoiseScale, SHADER_UNIFORM_FLOAT);

        Matrix m = MatrixMultiply(
            MatrixScale(gAirL2Slam.radius, gAirL2Slam.radius, gAirL2Slam.radius),
            MatrixTranslate(gAirL2Slam.pos.x, gAirL2Slam.pos.y, gAirL2Slam.pos.z)
        );

        DrawMesh(ball.meshes[0], ball.materials[0], m);
    }
    EndBlendMode();
}

static void DrawDonShadow(Donogan* d)
{
    if (!d) return;

    float gy = GetTerrainHeightFromMeshXZ(d->pos.x, d->pos.z);
    if (gy < -9000) return;

    Vector3 n = GetTerrainNormalFromMeshXZ(d->pos.x, d->pos.z);
    if (Vector3Length(n) < 0.001f) n = (Vector3){ 0, 1, 0 };
    n = Vector3Normalize(n);

    float heightAboveGround = d->pos.y - gy;

    float radius = 0.8f;
    float alpha = 220;

    if (heightAboveGround > 1.0f)
    {
        radius = Clamp(0.8f - heightAboveGround * 0.08f, 0.001f, 0.8f);
        alpha = Clamp(220 - heightAboveGround * 12.0f, 25.0f, 220);
    }

    Vector3 center = (Vector3){ d->pos.x, gy, d->pos.z };

    // lift off the ground along the slope normal to avoid z-fighting
    center = Vector3Add(center, Vector3Scale(n, 0.05f));

    Vector3 a = Vector3Subtract(center, Vector3Scale(n, 0.01f));
    Vector3 b = Vector3Add(center, Vector3Scale(n, 0.01f));

    DrawCylinderEx(
        a,
        b,
        radius,
        radius,
        18,
        (Color) {
        0, 0, 0, (unsigned char)alpha
    }
    );
}

#endif // DONOGAN_H
