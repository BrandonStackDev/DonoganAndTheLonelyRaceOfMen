#ifndef BG_H
#define BG_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdbool.h>
//me
#include "timer.h"
#include "game.h"
#include "util.h"
#include "apples.h"
#include "texture.h"
#include "dust_puff.h"
#include "donogan.h"

#define MAX_BG_PER_TYPE_AT_ONCE 12

typedef enum {
    BG_NONE = -1,//probably do not use
    BG_GHOST,
    BG_YETI,
    BG_ROBO,
    BG_PUMPKIN_HOPPER,
    BG_SKELETON,
    BG_ALISTER,
    BG_MECH,
    BG_TYPE_COUNT
} BadGuyType;

//kill counters (sum for total)
int ghostKillCount;
int yetiKillCount;
int roboKillCount;
int skelKillCount;

typedef enum {
    ATTACK_PUNCH,
    ATTACK_BALL,
    ATTACK_FREEZE,
    ATTACK_THROW,
    ATTACK_ARROW,
    ATTACK_HOP,
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
            return d->level + 1;
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
    ALISTER_STATE_IDLE,
    ALISTER_STATE_TALK,
    ALISTER_STATE_COMMAND,
    ALISTER_STATE_RUN,
    ALISTER_STATE_HURT,
    ALISTER_STATE_RETURN,
    ALISTER_STATE_HIT,
    ALISTER_STATE_DEFEATED,
} AlisterState;
typedef enum {
    ALISTER_ANIM_DYING,
    ALISTER_ANIM_HIT,
    ALISTER_ANIM_IDLE,
    ALISTER_ANIM_RUN,
    ALISTER_ANIM_T_POSE,
    ALISTER_ANIM_TALK,
} AlisterAnimation;
typedef enum {
    MECH_STATE_IDLE,
    MECH_STATE_ACTIVE,
    MECH_STATE_FLY,     // fly to a hover point
    MECH_STATE_WARN,
    MECH_STATE_ATTACK,   // drop toward Donogan and hit
    MECH_STATE_FALLBACK,
    MECH_STATE_THROW_DON,     // NEW: close grab/throw recovery
    //MECH_STATE_STOMP_COMBO,   // NEW: starts queued stomp pattern
    MECH_STATE_DEFEATED,
} MechState;

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

typedef enum {
    HOPPER_STATE_SLEEP,
    HOPPER_STATE_WAIT,
    HOPPER_STATE_JUMP,
    HOPPER_STATE_HURT,
    HOPPER_STATE_DEAD
} HopperState;

typedef enum {
    ANIM_SKEL_DANCE = 0, // dance, 76 frames
    ANIM_SKEL_DEATH = 1, // death, 151 frames
    ANIM_SKEL_HIT = 2, // hit, 61 frames
    ANIM_SKEL_JUMP = 3, // jump, 151 frames
    ANIM_SKEL_KICK = 4, // kick, 91 frames
    ANIM_SKEL_PLAN = 5, // plan, 31 frames
    ANIM_SKEL_RISE = 6, // rise, 151 frames
    ANIM_SKEL_RUN = 7, // run, 48 frames
    ANIM_SKEL_SWIPE = 8, // swipe/grab, 81 frames
    ANIM_SKEL_TRIP = 9, // trip, 151 frames
    ANIM_SKEL_WALK = 10,// walk, 121 frames

    // aliases so your state code reads nicely
    ANIM_SKEL_IDLE = ANIM_SKEL_PLAN,
    ANIM_SKEL_JUMP_ATTACK = ANIM_SKEL_JUMP,
    ANIM_SKEL_GRAB_ATTACK = ANIM_SKEL_SWIPE,
    ANIM_SKEL_KICK_ATTACK = ANIM_SKEL_KICK,
    ANIM_SKEL_GET_UP = ANIM_SKEL_TRIP,

    ANIM_SKEL_COUNT = 11
} SkeletonAnimation;

typedef enum {
    SKELETON_STATE_RISE,
    SKELETON_STATE_PLAN,
    SKELETON_STATE_WALK,
    SKELETON_STATE_RUN,
    SKELETON_STATE_JUMP_ATTACK_START,
    SKELETON_STATE_JUMP_ATTACK_AIR,
    SKELETON_STATE_JUMP_ATTACK_LAND,
    SKELETON_STATE_GRAB_ATTACK,
    SKELETON_STATE_KICK_ATTACK,
    SKELETON_STATE_TRIP,
    SKELETON_STATE_DANCE,
    SKELETON_STATE_HIT,
    SKELETON_STATE_DEATH,
    SKELETON_STATE_DEAD
} SkeletonState;

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
    float scale, desiredScale;
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
    Vector3 warnPos;
    float warnTimer;
    float warnSpin;

    bool bounced;
    float groundY;
    bool onPlatform;
    bool attackLanded; // prevents one skeleton attack from hitting Don every frame

    bool ragdoll;
    float ragdollTimer;
    Vector3 ragdollSpinVel;
    float truckHitCooldown;
    float propHitCooldown;

    int mechStompIndex;
    bool mechStompCombo;
} BadGuy;
//instance of a bad guy, will borrow its model

int act_bg_count = 0;
int act_bg[MAX_BG_PER_TYPE_AT_ONCE * BG_TYPE_COUNT]; //store indexes of active bg's so we dont loop alot ever (except for spawning check...)
BadGuy * bg;
int total_bg_models_all_types, bg_count;
static bool gClarenceBossCheatStarted = false;
static BadGuy* gClarenceBossTarget;

void BG_SetAnimSafe(BadGuy* b, int animIndex, bool forceRestart);
#define MECH_WARN_TIME          3
#define MECH_WARN_SPIN_SPEED  145.0f
#define MECH_WARN_RADIUS       6.4f
Texture2D mechWarnTexture;

#define ALISTER_RUN_AWAY_DIST      34.0f
#define ALISTER_RUN_SPEED          26.0f
#define ALISTER_RUN_ARRIVE_DIST     3.5f

static inline Vector3 BG_ForwardFromYawDeg(float yawDeg);

static inline void Alister_StartRunAwayFromDonogan(BadGuy* b, Donogan* d)
{
    if (!b || !d) return;

    Vector3 away = Vector3Subtract(b->pos, d->pos);
    away.y = 0;

    if (Vector3LengthSqr(away) < 0.0001f)
    {
        away = BG_ForwardFromYawDeg(b->yaw);
    }
    else
    {
        away = Vector3Normalize(away);
    }

    Vector3 target = Vector3Add(b->pos, Vector3Scale(away, ALISTER_RUN_AWAY_DIST));

    // Optional: keep him from running too far from his post.
    if (Vector3DistanceSqr(target, b->spawnPoint) > b->spawnRadius * b->spawnRadius)
    {
        Vector3 backToPost = Vector3Subtract(b->spawnPoint, b->pos);
        backToPost.y = 0;

        if (Vector3LengthSqr(backToPost) > 0.0001f)
        {
            backToPost = Vector3Normalize(backToPost);
            target = Vector3Add(b->pos, Vector3Scale(backToPost, ALISTER_RUN_AWAY_DIST * 0.65f));
        }
    }

    float gy = BG_GroundY(target);
    if (gy > -9000)
    {
        target.y = gy + 3.7;
    }
    else
    {
        target.y = b->pos.y;
    }

    b->targetPos = target;
    b->targetYaw = BG_YawTo(b->pos, target);
    b->targetPitch = 0;
    b->targetRoll = 0;
    b->vel = (Vector3){ 0 };

    b->state = ALISTER_STATE_RUN;
}
static inline BoundingBox UpdateBoundingBoxFromFeet(BoundingBox orig, Vector3 feetPos)
{
    return (BoundingBox) {
        (Vector3) {
        feetPos.x + orig.min.x,
            feetPos.y + orig.min.y,
            feetPos.z + orig.min.z
    },
            (Vector3) {
            feetPos.x + orig.max.x,
                feetPos.y + orig.max.y,
                feetPos.z + orig.max.z
        }
    };
}

void InitBadGuyModels(Shader ghostShader)
{
    total_bg_models_all_types = MAX_BG_PER_TYPE_AT_ONCE * BG_TYPE_COUNT;
    //bgModelBorrower = (BadGuyBorrowModel*)malloc(sizeof(BadGuyBorrowModel) * total_bg_models_all_types);
    bgModelBorrower = calloc(total_bg_models_all_types, sizeof(BadGuyBorrowModel));
    if (!bgModelBorrower)
    {
        TraceLog(LOG_ERROR, "Failed to allocate enemy Model Borrower");
        return;
    }
    //ghost model can be shared
    Model ghost_model = LoadModel("models/ghost.obj");
    Texture ghost_tex = LoadMyTexture("textures/ghost.png");
    int yeti_animCount = 0;
    ModelAnimation* yeti_anims = LoadModelAnimations("models/yeti_anim_2.glb", &yeti_animCount);
    Model robo_model = LoadModel("models/robo.obj");
    Texture robo_tex = LoadMyTexture("textures/robo.png");
    int skel_animCount = 0;
    ModelAnimation* skel_anims = LoadModelAnimations("models/skeleton.glb", &skel_animCount);

    //alister and mech
    Model ali_model = LoadModel("models/alister.glb");
    Texture ali_tex = LoadMyTexture("textures/alister.png");
    int ali_animCount = 0;
    ModelAnimation* ali_anims = LoadModelAnimations("models/alister.glb",&ali_animCount);
    Model mech_model = LoadModel("models/mech.obj");
    Texture mech_tex = LoadMyTexture("textures/mech.png");
    mechWarnTexture = LoadMyTexture("textures/warn.png");

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
                bgModelBorrower[index].tex = LoadMyTexture("textures/yeti_skin_2.png");
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
            else if (bg_t == BG_PUMPKIN_HOPPER)
            {
                bgModelBorrower[index].model = LoadModel("models/hopper.obj");
                bgModelBorrower[index].tex = LoadMyTexture("textures/hopper.png"); // if you have one

                if (bgModelBorrower[index].model.materialCount > 0)
                {
                    bgModelBorrower[index].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture =
                        bgModelBorrower[index].tex;
                }

                bgModelBorrower[index].origBox = ScaleBoundingBox(GetModelBoundingBox(bgModelBorrower[index].model), 1.8);
            }
            else if (bg_t == BG_SKELETON)
            {
                bgModelBorrower[index].model = LoadModel("models/skeleton.glb");
                bgModelBorrower[index].tex = LoadMyTexture("textures/skeleton.png");

                if (bgModelBorrower[index].model.materialCount > 0 && bgModelBorrower[index].tex.id != 0)
                {
                    for (int m = 0; m < bgModelBorrower[index].model.materialCount; m++)
                    {
                        bgModelBorrower[index].model.materials[m].maps[MATERIAL_MAP_DIFFUSE].texture = bgModelBorrower[index].tex;
                    }
                }

                //bgModelBorrower[index].origBox = ScaleBoundingBox(GetModelBoundingBox(bgModelBorrower[index].model), 1.12);
                bgModelBorrower[index].origBox = (BoundingBox){(Vector3){ -1.10f,  0.20f, -1.10f },(Vector3){  1.10f,  5.80f,  1.10f }};

                bgModelBorrower[index].anims = skel_anims;
                bgModelBorrower[index].animCount = skel_animCount;
            }
            else if (bg_t == BG_ALISTER)
            {
                bgModelBorrower[index].model = ali_model;
                bgModelBorrower[index].tex = ali_tex;

                if (bgModelBorrower[index].model.materialCount > 0 && bgModelBorrower[index].tex.id != 0)
                {
                    for (int m = 0; m < bgModelBorrower[index].model.materialCount; m++)
                    {
                        bgModelBorrower[index].model.materials[m].maps[MATERIAL_MAP_DIFFUSE].texture = bgModelBorrower[index].tex;
                    }
                }

                bgModelBorrower[index].origBox = ScaleBoundingBox(GetModelBoundingBox(bgModelBorrower[index].model), 4);

                bgModelBorrower[index].anims = ali_anims;
                bgModelBorrower[index].animCount = ali_animCount;
            }
            else if (bg_t == BG_MECH)
            {
                bgModelBorrower[index].model = mech_model;
                bgModelBorrower[index].tex = mech_tex;

                if (bgModelBorrower[index].model.materialCount > 0 && bgModelBorrower[index].tex.id != 0)
                {
                    for (int m = 0; m < bgModelBorrower[index].model.materialCount; m++)
                    {
                        bgModelBorrower[index].model.materials[m].maps[MATERIAL_MAP_DIFFUSE].texture = bgModelBorrower[index].tex;
                    }
                }

                bgModelBorrower[index].origBox = ScaleBoundingBox(GetModelBoundingBox(bgModelBorrower[index].model), 12);

                /*bgModelBorrower[index].anims = skel_anims;
                bgModelBorrower[index].animCount = skel_animCount;*/
            }
        }
    }
}
//helper for managing all of these d@mn variables in the bg struct
static inline void BG_ClearRuntimeState(BadGuy* b)
{
    if (!b) return;

    b->attackLanded = false;

    b->ragdoll = false;
    b->ragdollTimer = 0;
    b->ragdollSpinVel = (Vector3){0};

    b->truckHitCooldown = 0;
    b->propHitCooldown = 0;

    b->vel = (Vector3){ 0 };

    b->animFrame = 0;
    b->curAnim = 0;
    b->animFPS = 24.0f;
    b->mechStompIndex = 0;
    b->mechStompCombo = false;
    b->steerTimer = 0;
}
//
static inline bool BG_ActiveIndexOK(int idx)
{
    return bg && idx >= 0 && idx < bg_count;
}

static inline bool BG_BorrowerIndexOK(int idx)
{
    return bgModelBorrower &&
        idx >= 0 &&
        idx < total_bg_models_all_types;
}
// === NEW: helper for ground
static inline float BG_GroundY(Vector3 p) {
    float g = GetTerrainHeightFromMeshXZ(p.x, p.z);
    return (g < -9000) ? p.y : g;
}

static inline void BG_UpdateMainBox(BadGuy* b)
{
    if (!b) return;
    if (!b->active || b->dead || b->gbm_index < 0) { return; } //guard

    BoundingBox orig = bgModelBorrower[b->gbm_index].origBox;

    if (b->type == BG_SKELETON)
    {
        b->box = UpdateBoundingBoxFromFeet(orig, b->pos);
    }
    else
    {
        b->box = UpdateBoundingBox(orig, b->pos);
    }
}

static inline bool BG_GroundCheckAndSnap(BadGuy* b, float snapUp, float snapDown, bool forceIfBelow)
{
    if (!b || b->gbm_index < 0) return false;

    float groundY = BG_GroundY(b->pos);
    float bottomY = BG_BoxBottomYAtPos(b, b->pos);

    b->groundY = groundY;

    float diff = bottomY - groundY;

    // If the bottom of the box is below ground, always allow a rescue when forceIfBelow is true.
    if (diff < 0)
    {
        if (forceIfBelow || fabsf(diff) <= snapUp)
        {
            b->pos.y += -diff;
            BG_UpdateMainBox(b);
            return true;
        }

        return false;
    }

    // If slightly above ground, snap down.
    if (diff <= snapDown)
    {
        b->pos.y -= diff;
        BG_UpdateMainBox(b);
        return true;
    }

    // Too high above ground; probably airborne.
    return false;
}
//truck ragdoll stuff
static inline float BG_BoxBottomYAtPos(BadGuy* b, Vector3 pos)
{
    if (!b || b->gbm_index < 0) return pos.y;

    BoundingBox orig = bgModelBorrower[b->gbm_index].origBox;
    return pos.y + orig.min.y;
}

static inline bool BG_IsActuallyDeadState(BadGuy* b)
{
    if (!b) return true;

    if (b->type == BG_YETI)
    {
        return b->state == YETI_STATE_DEAD || b->state == YETI_STATE_DYING;
    }

    if (b->type == BG_ROBO)
    {
        return b->state == ROBO_STATE_DEAD || b->state == ROBO_STATE_DYING;
    }

    if (b->type == BG_PUMPKIN_HOPPER)
    {
        return b->state == HOPPER_STATE_DEAD;
    }

    if (b->type == BG_SKELETON)
    {
        return b->state == SKELETON_STATE_DEAD || b->state == SKELETON_STATE_DEATH;
    }

    return b->dead;
}

static inline void BG_StartTruckRagdoll(BadGuy* b, Vector3 impulse, float spinPower)
{
    if (!b) return;
    if (b->type == BG_GHOST) return;
    if (BG_IsActuallyDeadState(b)) return;

    b->ragdoll = true;
    b->ragdollTimer = 01.85f + (float)GetRandomValue(10, 50) * 0.01f;
    b->truckHitCooldown = 0.35f;

    b->vel = impulse;

    if (b->vel.y < 7.0f)
    {
        b->vel.y = 7.0f;
    }

    // Keep it goofy but not insane.
    float sp = Clamp(spinPower * 2.4, 12.0f, 36.0f);

    b->ragdollSpinVel = (Vector3){
        (float)GetRandomValue(-90, 90) * sp * 0.08f,
        (float)GetRandomValue(-120, 120) * sp * 0.06f,
        (float)GetRandomValue(-160, 160) * sp * 0.08f
    };

    b->targetPos = b->pos;
    b->throwing = false;
    b->frozen = false;
    b->attackLanded = false;

    if (b->type == BG_YETI)
    {
        b->state = YETI_STATE_HIT;
        BG_SetAnimSafe(b, ANIM_YETI_ROAR, false);
    }
    else if (b->type == BG_ROBO)
    {
        b->state = ROBO_STATE_PLAN;
    }
    else if (b->type == BG_PUMPKIN_HOPPER)
    {
        b->state = HOPPER_STATE_HURT;
    }
    else if (b->type == BG_SKELETON)
    {
        b->state = SKELETON_STATE_HIT;
        BG_SetAnimSafe(b, ANIM_SKEL_HIT, true);
    }
}

static inline void BG_UpdateTruckRagdoll(BadGuy* b, float dt)
{
    if (!b) return;
    if (b->gbm_index < 0) return;

    b->ragdollTimer -= dt;

    // fake physics
    b->vel.y -= 30 * dt;

    b->pos.x += b->vel.x * dt;
    b->pos.y += b->vel.y * dt;
    b->pos.z += b->vel.z * dt;

    // tumble visuals
    b->pitch += b->ragdollSpinVel.x * dt;
    b->yaw += b->ragdollSpinVel.y * dt;
    b->roll += b->ragdollSpinVel.z * dt;

    // horizontal drag
    float drag = 1.0f - 3.8f * dt;
    if (drag < 0.80f) drag = 0.80f;

    b->vel.x *= drag;
    b->vel.z *= drag;

    b->ragdollSpinVel.x *= drag;
    b->ragdollSpinVel.y *= drag;
    b->ragdollSpinVel.z *= drag;

    float groundY = BG_GroundY(b->pos);
    float bottomY = BG_BoxBottomYAtPos(b, b->pos);

    b->groundY = groundY;

    if (bottomY <= groundY)
    {
        // Move the whole bad guy up/down so the bottom of his box sits on the ground.
        b->pos.y += (groundY - bottomY);

        // little bounce once, then settle
        if (fabsf(b->vel.y) > 8.0f)
        {
            b->vel.y = fabsf(b->vel.y) * 0.20f;
            b->vel.x *= 0.65f;
            b->vel.z *= 0.65f;
        }
        else
        {
            b->vel.y = 0;
        }

        // ground friction
        b->vel.x *= 0.82f;
        b->vel.z *= 0.82f;
    }

    BG_UpdateMainBox(b);

    bool slowEnough =
        Vector3LengthSqr((Vector3) { b->vel.x, 0, b->vel.z }) < 1.5f &&
        fabsf(b->vel.y) < 1.0f;

    if (b->ragdollTimer <= 0 && slowEnough)
    {
        b->ragdoll = false;
        b->vel = (Vector3){ 0 };
        b->pitch = 0;
        b->roll = 0;
        b->health -= 25;
        if (b->type == BG_YETI)
        {
            b->state = YETI_STATE_PLANNING;
            BG_SetAnimSafe(b, ANIM_YETI_WALK, false);
        }
        else if (b->type == BG_ROBO)
        {
            b->state = ROBO_STATE_PLAN;
        }
        else if (b->type == BG_PUMPKIN_HOPPER)
        {
            b->state = HOPPER_STATE_WAIT;
            ResetTimer(&b->interactionTimer);
            StartTimer(&b->interactionTimer);
        }
        else if (b->type == BG_SKELETON)
        {
            b->state = SKELETON_STATE_PLAN;
            BG_SetAnimSafe(b, ANIM_SKEL_IDLE, false);
        }
        if (b->health <= 0)
        {
            b->health = 0;

            if (b->type == BG_YETI)
            {
                b->state = YETI_STATE_DYING;
                BG_SetAnimSafe(b, ANIM_YETI_ROAR, false);
            }
            else if (b->type == BG_ROBO)
            {
                b->state = ROBO_STATE_DYING;
            }
            else if (b->type == BG_PUMPKIN_HOPPER)
            {
                b->state = HOPPER_STATE_DEAD;
            }
            else if (b->type == BG_SKELETON)
            {
                b->state = SKELETON_STATE_DEATH;
                BG_SetAnimSafe(b, ANIM_SKEL_DEATH, true);
            }
        }
    }
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
static inline void BG_SetAnimSafe(BadGuy* b, int animIndex, bool forceRestart)
{
    if (!b || b->gbm_index < 0) return;
    if (!b->anims || b->animCount <= 0) return;

    if (animIndex < 0 || animIndex >= b->animCount)
    {
        animIndex = 0;
    }

    BG_SetAnim(b, animIndex, forceRestart);
}
static inline void BG_UpdateAnim(BadGuy* b, float dt) {
    if (!b || b->gbm_index < 0) return;
    if (!BG_BorrowerIndexOK(b->gbm_index)) return;
    if (b->animCount <= 0 || !b->anims) return;

    if (b->curAnim < 0 || b->curAnim >= b->animCount)
    {
        TraceLog(LOG_ERROR,
            "Bad BG curAnim=%d animCount=%d type=%d state=%d gbm=%d",
            b->curAnim, b->animCount, b->type, b->state, b->gbm_index);
        b->curAnim = 0;
        b->animFrame = 0;
    }

    const int a = b->curAnim;
    const ModelAnimation* A = &b->anims[a];
    if (A->keyframeCount <= 0) return;

    // advance frames
    float framesToAdvance = b->animFPS * dt;
    int oldFrame = b->animFrame;
    int adv = (int)(framesToAdvance + 0.5f);
    if (adv <= 0) { adv = 1; }
    b->animFrame += adv;

    // Loop everything except "one-shot" jumps (we'll clamp on ATTACK state)
    if (b->animFrame >= A->keyframeCount) {
        b->animFrame %= A->keyframeCount;
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
    if (b->animFPS <= 0) b->animFPS = 24.0f; // default
    b->curAnim = 0;
    b->animFrame = 0;
}
//draw stuff
// --- Quaternion helpers for BadGuy full-body rotation -----------------------
static inline Quaternion BG_ModelFixQuat(const BadGuy* b) {
    // Exporter/model local-axis fix (if needed). Ghost seems fine → 0.
    // If the ghost appears 90° off, try setting xFixDeg = -90.
    float xFixDeg = 0;
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

static inline void DrawMechWarningCircle(BadGuy* b)
{
    if (!b) return;
    if (b->type != BG_MECH) return;
    if (b->state != MECH_STATE_WARN) return;
    if (mechWarnTexture.id == 0) return;

    Vector3 c = b->warnPos;
    c.y += 0.16f; // small lift above ground to avoid z-fighting

    float half = MECH_WARN_RADIUS;
    float a = b->warnSpin * DEG2RAD;

    Vector3 right = {
        cosf(a) * half,
        0,
        sinf(a) * half
    };

    Vector3 forward = {
        -sinf(a) * half,
        0,
        cosf(a) * half
    };

    Vector3 p0 = Vector3Subtract(Vector3Subtract(c, right), forward);
    Vector3 p1 = Vector3Add(Vector3Subtract(c, right), forward);
    Vector3 p2 = Vector3Add(Vector3Add(c, right), forward);
    Vector3 p3 = Vector3Subtract(Vector3Add(c, right), forward);

    // Important: transparent decals should not write depth.
    rlDisableDepthMask();

    rlSetTexture(mechWarnTexture.id);
    rlBegin(RL_QUADS);
    rlColor4ub(255, 255, 255, 225);

    rlTexCoord2f(0, 1); rlVertex3f(p0.x, p0.y, p0.z);
    rlTexCoord2f(0, 0); rlVertex3f(p1.x, p1.y, p1.z);
    rlTexCoord2f(1, 0); rlVertex3f(p2.x, p2.y, p2.z);
    rlTexCoord2f(1, 1); rlVertex3f(p3.x, p3.y, p3.z);

    rlEnd();
    rlSetTexture(0);

    rlEnableDepthMask();
}
static inline void DrawBadGuy(BadGuy * b) {
    if (!b || !b->active || b->gbm_index < 0) return;
    Vector3 drawPos = b->pos;
    Model* M = &bgModelBorrower[b->gbm_index].model;

    Quaternion q = BG_BuildWorldQuat(b);
    Matrix R = QuaternionToMatrix(q);
    Matrix T = MatrixTranslate(drawPos.x, drawPos.y, drawPos.z);
    float s = (b->scale > 0) ? b->scale : 1.0f;
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
    if (d->pos.y + 15 < groundY && d->inHome) { groundY = b->spawnPoint.y; }//hack, for cinderSpire cave
    if (groundY < -9000) groundY = b->pos.y - 3.0f; // safe fallback
    float flyAGL = 3.0f;   // cruise height while flying
    float landAGL = 0.30f;  // near-ground for landing/wander
    float targetY = b->pos.y; // set per-state below
    float dxT = b->targetPos.x - b->pos.x;
    float dzT = b->targetPos.z - b->pos.z;
    float distToTarget = sqrtf(dxT * dxT + dzT * dzT);
    float yawToTarget = (distToTarget > 1e-4f) ? (RAD2DEG * atan2f(dxT, dzT)) : b->yaw;
    b->targetYaw = yawToTarget;
    //first check if he is outside of the activation radius, for ghosts they die if this is true
    float r = b->spawnRadius < 30 ? 200 : b->spawnRadius;
    if (Vector3DistanceSqr(b->pos, b->spawnPoint) > r*r)
    {
        b->targetPitch = 0;
        b->speed = 0.4f;
        b->targetPos = b->pos;
        b->targetPos.y = groundY - 20;
        b->state = GHOST_STATE_DEATH;
    }
    if (Vector3DistanceSqr(d->pos, b->spawnPoint) > b->spawnRadius * b->spawnRadius && !b->interactionTimer.running)//is donogan outside of our radius
    {
        StartTimer(&b->interactionTimer);
    }
    else if (HasTimerElapsed(&b->interactionTimer) && Vector3DistanceSqr(d->pos, b->spawnPoint) > b->spawnRadius * b->spawnRadius)
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
        if (Vector3DistanceSqr(d->pos, b->pos) < b->awareRadius * b->awareRadius || b->aware) //if aware of donogan
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
        if (Vector3DistanceSqr(b->targetPos, b->pos) < 64)
        {
            b->targetPitch = -10;
            b->state = GHOST_STATE_FLY_DEC;
        }
    }break;
    case GHOST_STATE_FLY_DEC: {
        if (Vector3DistanceSqr(b->targetPos, b->pos) < 9)
        {
            b->targetPitch = 0;
            b->state = GHOST_STATE_PLAN;
        }
    }break;
    case GHOST_STATE_WANDER: {
        if (Vector3DistanceSqr(b->targetPos, b->pos) < 16)
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
        if (Vector3DistanceSqr(b->targetPos, b->pos) < 9)
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
static inline void Yeti_KnockBackFromDonogan(BadGuy* b, Donogan* d)
{
    Vector3 away = Vector3Subtract(b->pos, d->pos);
    away.y = 0;

    if (Vector3Length(away) < 0.001f)
    {
        away = (Vector3){ sinf(d->yawY), 0, cosf(d->yawY) };
    }

    away = Vector3Normalize(away);

    b->vel = Vector3Scale(away, 10); // stronger than hopper
    //b->vel.y = 8.5f;                    // nice pop
    b->state = YETI_STATE_HIT;
}
static inline void BG_Update_Yeti(Donogan* d, BadGuy* b, float dt)
{
    if (Vector3DistanceSqr(d->pos, b->spawnPoint) > b->spawnRadius * b->spawnRadius && !b->interactionTimer.running)//is donogan outside of our radius
    {
        StartTimer(&b->interactionTimer);
    }
    else if (HasTimerElapsed(&b->interactionTimer) && Vector3DistanceSqr(d->pos, b->spawnPoint) > b->spawnRadius * b->spawnRadius)//if it expires and donny is still outisde the radius, kill him
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
        PlaySoundVol(monster2);
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
        float t = (dist > 0) ? fminf((b->speed * dt) / dist, 1.0f) : 1.0f;
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
        PlaySoundVol(monster2);
        // Advance physics
        b->pos.x += b->vel.x * dt * b->speed;
        b->pos.z += b->vel.z * dt * b->speed;
        b->vel.y += -24.0f * dt; // gravity
        b->pos.y += b->vel.y * dt;

        // Don't loop the JUMP anim: clamp last frame until we land
        if (b->anims && b->animCount > 0) {
            int a = b->curAnim;
            const int last = b->anims[a].keyframeCount > 0 ? b->anims[a].keyframeCount - 1 : 0;
            if (b->animFrame > last) b->animFrame = last;
        }

        // Land?
        float gy = BG_GroundY(b->pos);
        if (b->pos.y <= gy) {
            b->pos.y = gy;
            // Post-attack behavior: roar then continue hunting/roaming
            BG_SetAnim(b, ANIM_YETI_ROAR, true);
            b->state = YETI_STATE_PLANNING;
            if (Vector3DistanceSqr(d->pos, b->pos) < 25*25)
            {
                d->shook += dt;
                d->health -= 5;
                DonSetState(d,DONOGAN_STATE_HIT);
                StartTimer(&d->hitTimer);
            }
        }
    } break;

    case YETI_STATE_HIT:
    {
        PlaySoundVol(monster2);
        b->pos = Vector3Add(b->pos, Vector3Scale(b->vel, dt));
        b->vel.y -= 28.0f * dt;

        // slight horizontal damping
        b->vel.x *= 0.985f;
        b->vel.z *= 0.985f;

        if (b->pos.y <= groundY)
        {
            b->pos.y = groundY;
            b->vel = (Vector3){ 0 };

            if (b->health <= 0)
            {
                b->state = YETI_STATE_DEAD;
            }
            else
            {
                b->state = YETI_STATE_PLANNING;
            }
        }
    } break;

    case YETI_STATE_DYING:
        PlaySoundVol(monster2);
        b->pitch = Lerp(b->pitch, -90, Clamp(dt * 4.5f, 0.0f, 1.0f));
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
        if (b->animFrame >= b->anims[b->curAnim].keyframeCount - 1) {
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
    BG_UpdateMainBox(b);
}

static inline void BG_Update_Robo(Donogan* d, BadGuy* b, float dt)
{
    //pre guard stuff
    if (Vector3DistanceSqr(d->pos, b->spawnPoint) > b->spawnRadius * b->spawnRadius && !b->interactionTimer.running)//is donogan outside of our radius
    {
        StartTimer(&b->interactionTimer);
    }
    else if (HasTimerElapsed(&b->interactionTimer) && Vector3DistanceSqr(d->pos, b->spawnPoint) > b->spawnRadius * b->spawnRadius)//if it expires and donny is still outisde the radius, kill him
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
        b->aware = Vector3DistanceSqr(d->pos,b->pos)<b->awareRadius*b->awareRadius;
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
            if (b->aware && GetRandomValue(0, 4) == 4 && Vector3DistanceSqr(d->pos, b->pos) < 50*50) //they need to be close to shoot, other wise you can spawn one and get shot really far away...
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
        if (Vector3DistanceSqr(b->pos, b->targetPos) < 3.4 * 3.5){b->state = ROBO_STATE_PLAN;}
    } break;
    case ROBO_STATE_SHOOT: {
        // Spawn an instant beam aimed at Don’s torso (no collision tests)
        Vector3 from = (Vector3){ b->pos.x, b->pos.y + 0.4f, b->pos.z };   // slight vertical offset
        Vector3 to = (Vector3){ d->pos.x, d->pos.y + 1.2f, d->pos.z };   // Don chest-ish

        FireLaser(from, to, 0.16f);  // short flash; fades automatically
        PlaySoundVol(laserBeam);
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
        //if (b->vel.y == 0 && b->pos.y > gy + 0.02f) b->vel.y = -6.0f; // give it a push if stationary

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
        roboKillCount++;
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

static inline void Hopper_KnockBackFromDonogan(BadGuy* b, Donogan* d)
{
    Vector3 away = Vector3Subtract(b->pos, d->pos);
    away.y = 0;

    if (Vector3Length(away) < 0.001f)
    {
        away = (Vector3){ sinf(d->yawY), 0, cosf(d->yawY) };
    }

    away = Vector3Normalize(away);

    b->vel = Vector3Scale(away, 8.0f); // backwards shove
    //b->vel.y = 7.0f;                   // little pop upward
    b->state = HOPPER_STATE_HURT;
}

static inline void BG_Update_PumpkinHopper(Donogan* d, BadGuy* b, float dt)
{
    //float groundY = BG_GroundY(b->pos);
    float groundY = b->groundY;
    if (groundY < -9000)
    {
        groundY = BG_GroundY(b->pos);
    }
    if (b->health <= 0 && b->state != HOPPER_STATE_HURT){b->state = HOPPER_STATE_DEAD;}
    switch (b->state)
    {
    case HOPPER_STATE_SLEEP:
    {
        if (Vector3DistanceSqr(d->pos, b->pos) < b->awareRadius * b->awareRadius)
        {
            b->aware = true;
            b->state = HOPPER_STATE_WAIT;
            ResetTimer(&b->interactionTimer);
            StartTimer(&b->interactionTimer);
        }
    } break;

    case HOPPER_STATE_WAIT:
    {
        Vector3 toDon = Vector3Subtract(d->pos, b->pos);
        b->yaw = RAD2DEG * atan2f(toDon.x, toDon.z);

        if (HasTimerElapsed(&b->interactionTimer))
        {
            Vector3 dir = Vector3Normalize((Vector3) { toDon.x, 0, toDon.z });
            b->vel = Vector3Scale(dir, b->speed);
            b->vel.y = 10;
            b->state = HOPPER_STATE_JUMP;
        }
    } break;

    case HOPPER_STATE_JUMP:
    {
        b->pos = Vector3Add(b->pos, Vector3Scale(b->vel, dt));
        b->vel.y -= 28.0f * dt;

        if (b->pos.y <= groundY)
        {
            b->pos.y = groundY;
            b->vel = (Vector3){ 0 };
            b->state = HOPPER_STATE_WAIT;
            ResetTimer(&b->interactionTimer);
            StartTimer(&b->interactionTimer);
        }
        if (!b->onPlatform && b->spawnPoint.y > BG_GroundY(b->pos) + 10 && b->pos.y < b->spawnPoint.y - 25.0f)
        {
            b->health = 0;
            b->state = HOPPER_STATE_DEAD;
            break;
        }
    } break;
    case HOPPER_STATE_HURT:
    {
        b->pos = Vector3Add(b->pos, Vector3Scale(b->vel, dt));
        b->vel.y -= 28.0f * dt;

        // optional: slow horizontal drift
        b->vel.x *= 0.985f;
        b->vel.z *= 0.985f;

        if (b->pos.y <= groundY)
        {
            b->pos.y = groundY;
            b->vel = (Vector3){ 0 };

            if (b->health <= 0)
            {
                b->state = HOPPER_STATE_DEAD;
            }
            else
            {
                b->state = HOPPER_STATE_WAIT;
                ResetTimer(&b->interactionTimer);
                StartTimer(&b->interactionTimer);
            }
        }
        if (!b->onPlatform && b->spawnPoint.y > BG_GroundY(b->pos) + 10 && b->pos.y < b->spawnPoint.y - 25.0f)
        {
            b->health = 0;
            b->state = HOPPER_STATE_DEAD;
            break;
        }
    } break;
    case HOPPER_STATE_DEAD:
    {
        b->active = false;
        b->dead = true;
        if (b->gbm_index >= 0) { bgModelBorrower[b->gbm_index].isInUse = false; }
        b->gbm_index = -1;
        StartTimer(&b->respawnTimer);
        ResetTimer(&b->interactionTimer);
        d->xp += 12;
        DustPuff_Spawn(b->pos);
        return;
    } break;
    }
    BG_UpdateMainBox(b);
}

static inline Vector3 BG_FlatDirTo(Vector3 from, Vector3 to)
{
    Vector3 d = Vector3Subtract(to, from);
    d.y = 0;

    if (Vector3LengthSqr(d) < 0.0001f)
    {
        return (Vector3) { 0, 0, 1.0f };
    }

    return Vector3Normalize(d);
}

static inline float BG_YawTo(Vector3 from, Vector3 to)
{
    Vector3 d = Vector3Subtract(to, from);
    d.y = 0;

    if (Vector3LengthSqr(d) < 0.0001f)
    {
        return 0;
    }

    return RAD2DEG * atan2f(d.x, d.z);
}

static inline void Skeleton_PickWanderTarget(BadGuy* b)
{
    float r = (float)GetRandomValue(12, 38);
    float a = (float)GetRandomValue(0, 359) * DEG2RAD;

    Vector3 p = {
        b->spawnPoint.x + sinf(a) * r,
        b->spawnPoint.y,
        b->spawnPoint.z + cosf(a) * r
    };

    float gy = GetTerrainHeightFromMeshXZ(p.x, p.z);
    if (gy > -9000) p.y = gy;

    b->targetPos = p;
}

#define SKEL_TRIP_MIN_DIST          24.0f
#define SKEL_TRIP_CHANCE_PER_1000    1     // was 3, lower = much less tripping

#define SKEL_JUMP_MIN_DIST          10
#define SKEL_JUMP_MAX_DIST          22.0f

// Jump lands the skeleton root BEFORE Donogan,
// because the skeleton's body/arms extend forward during the animation.
// Positive means jump THROUGH Donogan past his position.
// Tune 2.0f - 5.0f.
#define SKEL_JUMP_THROUGH_DIST        3.5f

// Longer jump = more float and better animation sync.
#define SKEL_JUMP_TIME                0.92f

// Higher arc. Tune 10-16.
#define SKEL_JUMP_ARC_HEIGHT          13.0f

// Start physical movement a little later in the jump anim.
#define SKEL_JUMP_LEAVE_FRAME         48  //12

// Land around 2/3 into the 151-frame jump anim.
#define SKEL_JUMP_LAND_FRAME          100

#define SKEL_CLOSE_ATTACK_RANGE       7.8f   // was ~6.4/6.8; lets swipe start farther out
#define SKEL_KICK_BACKUP_DIST         1.8f   // how far kick slides backward
#define SKEL_KICK_BACKUP_UNTIL        0.32f  // first 32% of kick animation
// Swipe/grab has a forward step during the animation.
// Tune speed first, then range.
#define SKEL_SWIPE_FORWARD_SPEED      2.4f   // lower if it ends too close
#define SKEL_SWIPE_FORWARD_UNTIL      0.42f  // first 42% of swipe animation

#define SKEL_RUN_SPEED               6.8f
#define SKEL_WALK_SPEED              2.2f

#define SKEL_TOO_CLOSE_DIST          2.4f
#define SKEL_BACKUP_TO_DIST          5.2f
#define SKEL_BACKUP_SPEED            4.4f

#define SKEL_KICK_BACKUP_SPEED        0.35f  //slow

//attack defines
#define SKEL_BODY_PUSH                0.08f

#define SKEL_DON_HIT_DAMAGE_KICK       8
#define SKEL_DON_HIT_DAMAGE_SWIPE      7
#define SKEL_DON_HIT_DAMAGE_JUMP      12

#define SKEL_DON_KNOCKBACK_KICK       7.0f
#define SKEL_DON_KNOCKBACK_SWIPE      6.0f
#define SKEL_DON_KNOCKBACK_JUMP      10

#define SKEL_TAKE_HIT_KNOCKBACK       8.5f
#define SKEL_TAKE_WRENCH_KNOCKBACK   16.0f
#define SKEL_TAKE_HIT_UP              5.0f
#define SKEL_TAKE_WRENCH_UP           9.0f

#define SKEL_KICK_HIT_START           0.30f
#define SKEL_KICK_HIT_END             0.62f
#define SKEL_SWIPE_HIT_START          0.25f
#define SKEL_SWIPE_HIT_END            0.58f
#define SKEL_JUMP_HIT_START           0.38f
#define SKEL_JUMP_HIT_END             0.78f
//jesus, lots of defines lol

// Only update yaw toward Donogan when there is enough distance.
// This prevents twitchy spin when the skeleton root is basically inside Don.
#define SKEL_YAW_FACE_MIN_DIST       2.0f

static inline bool Skeleton_IsAttackState(int state)
{
    return state == SKELETON_STATE_JUMP_ATTACK_START ||
        state == SKELETON_STATE_JUMP_ATTACK_AIR ||
        state == SKELETON_STATE_JUMP_ATTACK_LAND ||
        state == SKELETON_STATE_GRAB_ATTACK ||
        state == SKELETON_STATE_KICK_ATTACK ||
        state == SKELETON_STATE_TRIP ||
        state == SKELETON_STATE_HIT ||
        state == SKELETON_STATE_DEATH ||
        state == SKELETON_STATE_DEAD ||
        state == SKELETON_STATE_DANCE;
}

static inline void Skeleton_StartJumpAttack(BadGuy* b, Donogan* d, float groundY)
{
    PlaySoundVol(monster);
    Vector3 toDon = Vector3Subtract(d->pos, b->pos);
    toDon.y = 0;

    float dist = Vector3Length(toDon);
    Vector3 dir = (dist > 0.001f)
        ? Vector3Scale(toDon, 1.0f / dist)
        : (Vector3) { 0, 0, 1.0f };

    // Land/finish THROUGH Donogan, not before him.
    // This should make the dive/clutch look more aggressive.
    Vector3 land = d->pos;
    land.x += dir.x * SKEL_JUMP_THROUGH_DIST;
    land.z += dir.z * SKEL_JUMP_THROUGH_DIST;
    land.y = groundY;

    b->targetPos = land;

    Vector3 delta = Vector3Subtract(land, b->pos);
    delta.y = 0;

    // Face Donogan at launch.
    b->yaw = BG_YawTo(b->pos, d->pos);
    b->targetYaw = b->yaw;

    b->vel.x = delta.x / SKEL_JUMP_TIME;
    b->vel.z = delta.z / SKEL_JUMP_TIME;

    // High initial arc.
    b->vel.y = (2.0f * SKEL_JUMP_ARC_HEIGHT) / SKEL_JUMP_TIME;
    b->attackLanded = false;
    b->state = SKELETON_STATE_JUMP_ATTACK_START;
    BG_SetAnimSafe(b, ANIM_SKEL_JUMP_ATTACK, true);
}

static inline bool BG_AnimNearEnd(BadGuy* b)
{
    if (!b || !b->anims || b->animCount <= 0) return true;
    if (b->curAnim < 0 || b->curAnim >= b->animCount) return true;

    int last = b->anims[b->curAnim].keyframeCount - 2;
    return b->animFrame >= last;
}

static inline float BG_AnimT(BadGuy* b)
{
    if (!b || !b->anims || b->animCount <= 0) return 0;
    if (b->curAnim < 0 || b->curAnim >= b->animCount) return 0;

    int frames = b->anims[b->curAnim].keyframeCount;
    if (frames <= 1) return 0;

    float t = (float)b->animFrame / (float)(frames - 1);
    if (t < 0) t = 0;
    if (t > 1.0f) t = 1.0f;
    return t;
}

static inline Vector3 BG_ForwardFromYawDeg(float yawDeg)
{
    float r = yawDeg * DEG2RAD;
    return (Vector3) { sinf(r), 0, cosf(r) };
}

static inline BoundingBox BG_MakeBoxCenterHalf(Vector3 c, Vector3 h)
{
    return (BoundingBox) {
        (Vector3) {
        c.x - h.x, c.y - h.y, c.z - h.z
    },
            (Vector3) {
            c.x + h.x, c.y + h.y, c.z + h.z
        }
    };
}

static inline BoundingBox Skeleton_MakeForwardAttackBox(BadGuy* b,
    float forward,
    float yOffset,
    Vector3 halfSize)
{
    Vector3 f = BG_ForwardFromYawDeg(b->yaw);

    Vector3 c = {
        b->pos.x + f.x * forward,
        b->pos.y + yOffset,
        b->pos.z + f.z * forward
    };

    return BG_MakeBoxCenterHalf(c, halfSize);
}

static inline BoundingBox Skeleton_KickBox(BadGuy* b)
{
    // Out in front, lower/mid body.
    return Skeleton_MakeForwardAttackBox(
        b,
        2.35f,                         // forward
        2.15f,                         // y center
        (Vector3) {
        1.25f, 1.15f, 1.25f
    } // half size
    );
}

static inline BoundingBox Skeleton_SwipeBox(BadGuy* b)
{
    // Wider upper-body grab/swipe zone.
    return Skeleton_MakeForwardAttackBox(
        b,
        2.10f,
        3.00f,
        (Vector3) {
        1.65f, 1.45f, 1.65f
    }
    );
}

static inline BoundingBox Skeleton_JumpAttackBox(BadGuy* b)
{
    // During jump, use a chunky box around the skeleton's body.
    // This is intentionally forgiving.
    Vector3 c = {
        b->pos.x,
        b->pos.y + 2.70f,
        b->pos.z
    };

    return BG_MakeBoxCenterHalf(c, (Vector3) { 1.75f, 2.10f, 1.75f });
}

static inline Vector3 Skeleton_DirFromSkeletonToDon(BadGuy* b, Donogan* d)
{
    Vector3 dir = Vector3Subtract(d->pos, b->pos);
    dir.y = 0;

    if (Vector3LengthSqr(dir) < 0.0001f)
    {
        dir = BG_ForwardFromYawDeg(b->yaw);
    }
    else
    {
        dir = Vector3Normalize(dir);
    }

    return dir;
}

static inline void Skeleton_PushDon(Donogan* d, Vector3 dir, float power)
{
    // Light immediate positional nudge so the hit visibly separates them.
    d->pos = Vector3Add(d->pos, Vector3Scale(dir, SKEL_BODY_PUSH * power));

    // Also seed velocity if Don's movement code respects velXZ.
    d->velXZ = Vector3Scale(dir, power);
    d->velY = fmaxf(d->velY, 4.5f);

    d->box = UpdateBoundingBox(d->origBB, d->pos);
    d->innerBox = UpdateBoundingBox(d->origInnerBB, d->pos);
    d->outerBox = UpdateBoundingBox(d->origOuterBB, d->pos);
}

static inline bool Skeleton_TryHitDon(Donogan* d, BadGuy* b,
    BoundingBox attackBox,
    int damage,
    float knockPower)
{
    if (!d || !b) return false;
    if (b->attackLanded) return false;

    if (!CheckCollisionBoxes(attackBox, d->box) &&
        !CheckCollisionBoxes(attackBox, d->outerBox))
    {
        return false;
    }

    if (!HasTimerElapsed(&d->hitTimer)) return false;

    Vector3 dir = Skeleton_DirFromSkeletonToDon(b, d);

    d->health -= damage;
    DonSetState(d, DONOGAN_STATE_HIT);
    StartTimer(&d->hitTimer);
    d->shook = fmaxf(d->shook, 0.30f);

    Skeleton_PushDon(d, dir, knockPower);

    b->attackLanded = true;
    return true;
}

static inline void Skeleton_KnockBackFromDonogan(BadGuy* b, Donogan* d, bool wrench)
{
    Vector3 dir = Vector3Subtract(b->pos, d->pos);
    dir.y = 0;

    if (Vector3LengthSqr(dir) < 0.0001f)
    {
        dir = (Vector3){ sinf(d->yawY), 0, cosf(d->yawY) };
    }
    else
    {
        dir = Vector3Normalize(dir);
    }

    float power = wrench ? SKEL_TAKE_WRENCH_KNOCKBACK : SKEL_TAKE_HIT_KNOCKBACK;
    float up = wrench ? SKEL_TAKE_WRENCH_UP : 0;

    b->vel = Vector3Scale(dir, power);
    b->vel.y = up;

    b->state = SKELETON_STATE_HIT;
    b->attackLanded = false;
    BG_SetAnimSafe(b, ANIM_SKEL_HIT, true);
}

static inline void BG_Update_Skeleton(Donogan* d, BadGuy* b, float dt)
{
    if (!d || !b) return;

    float groundY = GetTerrainHeightFromMeshXZ(b->pos.x, b->pos.z);
    if (groundY < -9000) groundY = b->spawnPoint.y;

    Vector3 toDon = Vector3Subtract(d->pos, b->pos);
    toDon.y = 0;
    float distDon = Vector3Length(toDon);

    if (b->health <= 0) { b->dead = true; b->state = SKELETON_STATE_DEATH; }

    switch (b->state)
    {
    case SKELETON_STATE_RISE:
    {
        b->animFPS = 8;
        BG_SetAnimSafe(b, ANIM_SKEL_RISE, false);
        if (b->animFrame > b->anims[b->curAnim].keyframeCount / 2)
        {
            b->pos.y += dt;
        }
        if (BG_AnimNearEnd(b) && b->pos.y >= groundY - 2)
        {
            b->pos.y = groundY;
            b->state = SKELETON_STATE_PLAN;
            b->animFPS = 24;
            BG_SetAnimSafe(b, ANIM_SKEL_IDLE, true);
        }
    } break;

    case SKELETON_STATE_PLAN:
    {
        b->pos.y = groundY;

        if (distDon < b->awareRadius)
        {
            b->aware = true;
            b->state = SKELETON_STATE_RUN;
            BG_SetAnimSafe(b, ANIM_SKEL_RUN, true);
            break;
        }

        // Aimless walking: pick a wander spot sometimes.
        if (Vector3DistanceSqr(b->pos, b->targetPos) < 4.0f * 4.0f ||
            GetRandomValue(0, 100) < 2)
        {
            Skeleton_PickWanderTarget(b);
        }

        b->state = SKELETON_STATE_WALK;
        BG_SetAnimSafe(b, ANIM_SKEL_WALK, true);
    } break;

    case SKELETON_STATE_WALK:
    {
        b->pos.y = groundY;

        if (distDon < b->awareRadius)
        {
            b->aware = true;
            b->state = SKELETON_STATE_RUN;
            BG_SetAnimSafe(b, ANIM_SKEL_RUN, true);
            break;
        }

        Vector3 dir = BG_FlatDirTo(b->pos, b->targetPos);
        b->yaw = BG_YawTo(b->pos, b->targetPos);

        b->pos = Vector3Add(b->pos, Vector3Scale(dir, 2.2f * dt));

        if (Vector3DistanceSqr(b->pos, b->targetPos) < 3.0f * 3.0f)
        {
            b->state = SKELETON_STATE_PLAN;
            BG_SetAnimSafe(b, ANIM_SKEL_IDLE, true);
        }
    } break;

    case SKELETON_STATE_RUN:
    {
        b->pos.y = groundY;

        // Face Don while running/chasing, but do not twitch-spin when nearly overlapping.
        if (distDon > SKEL_YAW_FACE_MIN_DIST)
        {
            b->yaw = BG_YawTo(b->pos, d->pos);
            b->targetYaw = b->yaw;
        }

        // If not attacking and too close, back up into a better attack range.
        // This stops him from crowding Donogan and making kick/swipe/jump look bad.
        if (!Skeleton_IsAttackState(b->state) && distDon < SKEL_TOO_CLOSE_DIST)
        {
            Vector3 away = Vector3Subtract(b->pos, d->pos);
            away.y = 0;

            if (Vector3LengthSqr(away) < 0.0001f)
            {
                // fallback: back away from current facing
                away = (Vector3){ -sinf(b->yaw * DEG2RAD), 0, -cosf(b->yaw * DEG2RAD) };
            }
            else
            {
                away = Vector3Normalize(away);
            }

            b->pos = Vector3Add(b->pos, Vector3Scale(away, SKEL_BACKUP_SPEED * dt));
            b->pos.y = groundY;

            // Keep run animation while shuffling backward for now.
            BG_SetAnimSafe(b, ANIM_SKEL_RUN, false);

            // Once he has created enough room, let normal attack logic happen next frame.
            break;
        }

        // Very rare trip, and only while he is still a decent distance away.
        if (distDon > SKEL_TRIP_MIN_DIST && GetRandomValue(0, 1000) < SKEL_TRIP_CHANCE_PER_1000)
        {
            b->state = SKELETON_STATE_TRIP;
            BG_SetAnimSafe(b, ANIM_SKEL_TRIP, true);
            break;
        }

        // Close-range attacks FIRST.
        if (distDon <= SKEL_CLOSE_ATTACK_RANGE)
        {
            b->targetPos = b->pos;
            b->attackLanded = false;
            if (GetRandomValue(0, 1) == 0)
            {
                b->state = SKELETON_STATE_KICK_ATTACK;
                BG_SetAnimSafe(b, ANIM_SKEL_KICK_ATTACK, true);
            }
            else
            {
                b->state = SKELETON_STATE_GRAB_ATTACK;
                BG_SetAnimSafe(b, ANIM_SKEL_GRAB_ATTACK, true);
            }

            break;
        }

        // Jump should start from further out, not from inside Donogan's personal space.
        if (distDon >= SKEL_JUMP_MIN_DIST && distDon <= SKEL_JUMP_MAX_DIST)
        {
            Skeleton_StartJumpAttack(b, d, groundY);
            break;
        }

        // Otherwise chase.
        Vector3 dir = BG_FlatDirTo(b->pos, d->pos);
        b->pos = Vector3Add(b->pos, Vector3Scale(dir, SKEL_RUN_SPEED * dt));

    } break;

    case SKELETON_STATE_JUMP_ATTACK_START:
    {
        // Wait until animation is a little underway, then leave ground.
        BG_SetAnimSafe(b, ANIM_SKEL_JUMP_ATTACK, false);

        if (b->animFrame >= SKEL_JUMP_LEAVE_FRAME)
        {
            b->state = SKELETON_STATE_JUMP_ATTACK_AIR;
        }
    } break;

    case SKELETON_STATE_JUMP_ATTACK_AIR:
    {
        // Do NOT update targetPos to Don here.
        // Do NOT turn toward Don here.
        // The jump is committed to the locked landing point.

        b->pos = Vector3Add(b->pos, Vector3Scale(b->vel, dt));

        // Arc gravity. Tune if he floats or drops too fast.
        b->vel.y -= (4.0f * SKEL_JUMP_ARC_HEIGHT) / (SKEL_JUMP_TIME * SKEL_JUMP_TIME) * dt;

        // Keep yaw locked toward original landing point.
        b->yaw = b->targetYaw;

        float animT = BG_AnimT(b);

        if (animT >= SKEL_JUMP_HIT_START && animT <= SKEL_JUMP_HIT_END)
        {
            bool hadHitAlready = b->attackLanded;

            Skeleton_TryHitDon(
                d,
                b,
                Skeleton_JumpAttackBox(b),
                SKEL_DON_HIT_DAMAGE_JUMP,
                SKEL_DON_KNOCKBACK_JUMP
            );

            if (!hadHitAlready && b->attackLanded)
            {
                b->vel = (Vector3){ 0 };
                b->pos.y = groundY;
                b->state = SKELETON_STATE_DANCE;
                BG_SetAnimSafe(b, ANIM_SKEL_DANCE, true);
                break;
            }
        }

        float groundHere = GetTerrainHeightFromMeshXZ(b->pos.x, b->pos.z);
        if (groundHere < -9000) groundHere = groundY;

        // Do not allow landing too early. Let the animation reach about 2/3 first.
        if (b->animFrame >= SKEL_JUMP_LAND_FRAME && b->pos.y <= groundHere)
        {
            b->pos.y = groundHere;
            b->vel = (Vector3){ 0 };
            b->state = SKELETON_STATE_JUMP_ATTACK_LAND;
        }
    } break;

    case SKELETON_STATE_JUMP_ATTACK_LAND:
    {
        b->pos.y = groundY;
        b->yaw = b->targetYaw;

        if (BG_AnimNearEnd(b))
        {
            b->state = SKELETON_STATE_PLAN;
            BG_SetAnimSafe(b, ANIM_SKEL_IDLE, true);
        }
    } break;

    case SKELETON_STATE_GRAB_ATTACK:
    {
        b->pos.y = groundY;

        if (distDon > SKEL_YAW_FACE_MIN_DIST)
        {
            b->yaw = BG_YawTo(b->pos, d->pos);
            b->targetYaw = b->yaw;
        }

        b->targetPos = b->pos;

        BG_SetAnimSafe(b, ANIM_SKEL_GRAB_ATTACK, false);

        float animT = BG_AnimT(b);
        if (animT >= SKEL_SWIPE_HIT_START && animT <= SKEL_SWIPE_HIT_END)
        {
            Skeleton_TryHitDon(
                d,
                b,
                Skeleton_SwipeBox(b),
                SKEL_DON_HIT_DAMAGE_SWIPE,
                SKEL_DON_KNOCKBACK_SWIPE
            );
        }

        if (animT <= SKEL_SWIPE_FORWARD_UNTIL)
        {
            Vector3 dir = BG_FlatDirTo(b->pos, d->pos);
            b->pos = Vector3Add(b->pos, Vector3Scale(dir, SKEL_SWIPE_FORWARD_SPEED * dt));
            b->pos.y = groundY;
        }

        if (BG_AnimNearEnd(b))
        {
            b->state = SKELETON_STATE_PLAN;
            BG_SetAnimSafe(b, ANIM_SKEL_IDLE, true);
        }
    } break;

    case SKELETON_STATE_KICK_ATTACK:
    {
        b->pos.y = groundY;

        if (distDon > SKEL_YAW_FACE_MIN_DIST)
        {
            b->yaw = BG_YawTo(b->pos, d->pos);
            b->targetYaw = b->yaw;
        }

        BG_SetAnimSafe(b, ANIM_SKEL_KICK_ATTACK, false);

        float animT = BG_AnimT(b);
        if (animT >= SKEL_KICK_HIT_START && animT <= SKEL_KICK_HIT_END)
        {
            Skeleton_TryHitDon(
                d,
                b,
                Skeleton_KickBox(b),
                SKEL_DON_HIT_DAMAGE_KICK,
                SKEL_DON_KNOCKBACK_KICK
            );
        }

        // Kick needs room: slide backward early in the animation.
        if (animT <= SKEL_KICK_BACKUP_UNTIL)
        {
            Vector3 away = Vector3Subtract(b->pos, d->pos);
            away.y = 0;

            if (Vector3LengthSqr(away) < 0.0001f)
            {
                away = (Vector3){ -sinf(b->yaw * DEG2RAD), 0, -cosf(b->yaw * DEG2RAD) };
            }
            else
            {
                away = Vector3Normalize(away);
            }
            b->pos = Vector3Add(b->pos, Vector3Scale(away, SKEL_KICK_BACKUP_SPEED * dt));
            b->pos.y = groundY;
        }

        b->targetPos = b->pos;

        if (BG_AnimNearEnd(b))
        {
            b->state = SKELETON_STATE_PLAN;
            BG_SetAnimSafe(b, ANIM_SKEL_IDLE, true);
        }
    } break;

    case SKELETON_STATE_TRIP:
    {
        BG_SetAnimSafe(b, ANIM_SKEL_TRIP, false);

        if (b->animCount <= 0 || b->animFrame > b->anims[b->curAnim].keyframeCount - 2)
        {
            b->state = SKELETON_STATE_PLAN;
            BG_SetAnimSafe(b, ANIM_SKEL_IDLE, true);
        }
    } break;

    case SKELETON_STATE_DANCE:
    {
        b->pos.y = groundY;
        BG_SetAnimSafe(b, ANIM_SKEL_DANCE, false);

        if (BG_AnimNearEnd(b))
        {
            b->attackLanded = false;
            b->state = SKELETON_STATE_PLAN;
            BG_SetAnimSafe(b, ANIM_SKEL_IDLE, true);
        }
    } break;

    case SKELETON_STATE_HIT:
    {
        BG_SetAnimSafe(b, ANIM_SKEL_HIT, false);

        // Knockback physics.
        b->pos = Vector3Add(b->pos, Vector3Scale(b->vel, dt));
        b->vel.y -= 24.0f * dt;

        b->vel.x *= 0.92f;
        b->vel.z *= 0.92f;

        float gy = GetTerrainHeightFromMeshXZ(b->pos.x, b->pos.z);
        if (gy < -9000) gy = groundY;

        if (b->pos.y <= gy)
        {
            b->pos.y = gy;

            if (fabsf(b->vel.y) < 2.0f)
            {
                b->vel.y = 0;
            }
        }

        if (b->health <= 0)
        {
            b->state = SKELETON_STATE_DEATH;
            b->vel = (Vector3){ 0 };
            BG_SetAnimSafe(b, ANIM_SKEL_DEATH, true);
            break;
        }

        if (BG_AnimNearEnd(b))
        {
            b->vel = (Vector3){ 0 };
            b->state = SKELETON_STATE_PLAN;
            BG_SetAnimSafe(b, ANIM_SKEL_IDLE, true);
        }
    } break;

    case SKELETON_STATE_DEATH:
    {
        BG_SetAnimSafe(b, ANIM_SKEL_DEATH, false);
        b->pos.y -= dt;
        if (b->animCount <= 0 || b->animFrame > b->anims[b->curAnim].keyframeCount - 4)
        {
            b->state = SKELETON_STATE_DEAD;
        }
        else { break; }
    }; // break; //ladder logic for this guy so we dont have a one frame draw issue

    case SKELETON_STATE_DEAD:
    {
        b->active = false;
        b->dead = true;

        if (b->gbm_index >= 0)
        {
            bgModelBorrower[b->gbm_index].isInUse = false;
        }

        b->gbm_index = -1;
        StartTimer(&b->respawnTimer);
        ResetTimer(&b->interactionTimer);
        d->xp += 60;
        skelKillCount++;
        return;
    } break;

    default:
    {
        b->state = SKELETON_STATE_PLAN;
    } break;
    }

    if (b->gbm_index >= 0)
    {
        BG_UpdateMainBox(b);
        BG_UpdateAnim(b, dt);
    }
}

#define MECH_FLY_HEIGHT          52
#define MECH_ATTACK_HEIGHT       14
#define MECH_FLY_SPEED           32.0f
#define MECH_ATTACK_SPEED        42.0f
#define MECH_REPLAN_DIST          7.0f
#define MECH_HIT_DAMAGE          18
#define MECH_HIT_SHAKE            0.65f
#define MECH_HIT_KNOCKBACK       16.0f

#define MECH_GUARD_RANGE          120.0f
#define MECH_THROW_RANGE           35.0f
#define MECH_THROW_UP             32.0f
#define MECH_THROW_MIN_SPEED      16.0f
#define MECH_THROW_MAX_SPEED      52.0f
#define MECH_THROW_COOLDOWN        5.0f
#define MECH_THROW_RECOVER_TIME    0.65f

#define MECH_STOMP_COUNT           2
#define MECH_STOMP_SIDE_DIST      22.0f
#define MECH_STOMP_FWD_DIST       10.0f
#define MECH_STOMP_LAND_HEIGHT     3.0f

#define MECH_FINAL_WARN_BONUS      0.80f

#define MECH_ARENA_CENTER ((Vector3){ -3794, 332, 1203 })
Vector3* aliPos;
Vector3* mechPos;

static inline Vector3 BG_MoveTowardVec3(Vector3 from, Vector3 to, float maxStep)
{
    Vector3 delta = Vector3Subtract(to, from);
    float len = Vector3Length(delta);

    if (len <= 0.0001f || len <= maxStep)
    {
        return to;
    }

    return Vector3Add(from, Vector3Scale(delta, maxStep / len));
}

static inline void Mech_PickFlyTarget(BadGuy* b, Donogan* d)
{
    float a = (float)GetRandomValue(0, 359) * DEG2RAD;
    float r = (float)GetRandomValue(28, 75);

    Vector3 p = {
        d->pos.x + sinf(a) * r,
        d->pos.y + MECH_FLY_HEIGHT,
        d->pos.z + cosf(a) * r
    };

    float gy = BG_GroundY(p);
    if (gy > -9000)
    {
        p.y = gy + MECH_FLY_HEIGHT;
    }

    b->targetPos = p;
    b->targetYaw = BG_YawTo(b->pos, p);
    b->targetPitch = 75; // keep your current mech orientation
    b->targetRoll = 0;

    b->state = MECH_STATE_FLY;
}
static inline bool Mech_DonInGuardZone(BadGuy* b, Donogan* d, float range)
{
    if (!b || !d || !aliPos) return false;

    float rangeSq = range * range;

    float donToAliSq = Vector3DistanceSqr(d->pos, *aliPos);
    float mechToAliSq = Vector3DistanceSqr(b->pos, *aliPos);

    return donToAliSq < rangeSq && donToAliSq < mechToAliSq;
}

static inline void Mech_ClampWarnPosToGround(BadGuy* b)
{
    if (!b) return;

    float gy = BG_GroundY(b->warnPos);
    if (gy > -9000)
    {
        b->warnPos.y = gy + 0.08f;
    }
}

static inline void Mech_StartThrowDon(BadGuy* b, Donogan* d)
{
    if (!b || !d) return;

    Vector3 arena = MECH_ARENA_CENTER;

    Vector3 dir = Vector3Subtract(arena, d->pos);
    if (Vector3LengthSqr(dir) < 0.0001f)
    {
        dir = Vector3Subtract(d->pos, b->pos);
        dir.y = 0;
    }

    if (Vector3LengthSqr(dir) < 0.0001f)
    {
        dir = BG_ForwardFromYawDeg(b->yaw);
    }
    else
    {
        dir = Vector3Normalize(dir);
    }

    float distToArena = Vector3Distance(
        (Vector3) {
        d->pos.x, 0, d->pos.z
    },
        (Vector3) {
        arena.x, 0, arena.z
    }
    );

    float throwSpeed = Clamp(distToArena * 1.35f, MECH_THROW_MIN_SPEED, MECH_THROW_MAX_SPEED);

    DonSetState(d, DONOGAN_STATE_AIR_ROLL);
    StartTimer(&d->hitTimer);

    d->onGround = false;
    d->gluedToPlatform = false;
    d->rollVel = Vector3Scale(dir, throwSpeed);
    d->velY = fmaxf(d->velY, MECH_THROW_UP);
    d->shook = fmaxf(d->shook, 0.75f);
    d->health -= 10;

    //d->state = DONOGAN_STATE_HIT;
    d->bowMode = false;
    d->inWater = false;

    b->targetYaw = BG_YawTo(b->pos, d->pos);
    b->targetPos = b->pos;
    b->targetPos.y = BG_GroundY(b->pos) + 7;

    b->warnTimer = MECH_THROW_RECOVER_TIME;
    b->steerTimer = MECH_THROW_COOLDOWN;
    b->attackLanded = true;

    b->state = MECH_STATE_THROW_DON;

    DustPuff_Spawn(d->pos);
    d->pos.y += 3;
}

static inline float StepYaw(float current, float target, float maxStep)
{
    float delta = target - current;

    //while (delta > PI) delta -= PI * 2.0f;
    //while (delta < -PI) delta += PI * 2.0f;

    if (delta > maxStep) delta = maxStep;
    if (delta < -maxStep) delta = -maxStep;

    return current + delta;
}
static inline float WrapRad(float a)
{
    while (a > PI) a -= PI * 2.0f;
    while (a < -PI) a += PI * 2.0f;
    return a;
}

static inline float StepYawRad(float current, float target, float maxStep)
{
    float delta = WrapRad(target - current);

    if (delta > maxStep) delta = maxStep;
    if (delta < -maxStep) delta = -maxStep;

    return WrapRad(current + delta);
}
static inline float WrapDeg(float a)
{
    while (a > 180.0f) a -= 360.0f;
    while (a < -180.0f) a += 360.0f;
    return a;
}

static inline float StepYawDeg(float current, float target, float maxStep)
{
    float delta = WrapDeg(target - current);

    if (delta > maxStep) delta = maxStep;
    if (delta < -maxStep) delta = -maxStep;

    return WrapDeg(current + delta);
}

static inline void Alister_TurnAndMoveForward(BadGuy* b, float dt, float speed)
{
    b->targetYaw = BG_YawTo(b->pos, b->targetPos);

    // degrees per second
    b->yaw = StepYawDeg(b->yaw, b->targetYaw, 180.0f * dt);

    float yawRad = b->yaw * DEG2RAD;

    Vector3 forward = {
        sinf(yawRad),
        0.0f,
        cosf(yawRad)
    };

    b->pos = Vector3Add(b->pos, Vector3Scale(forward, speed * dt));
}

static inline void BG_Update_Alister(Donogan* d, BadGuy* b, float dt)
{
    if (!b || !d) return;
    bool donNear = Vector3DistanceSqr(d->pos, b->pos) < 1000;
    bool donVeryNear = Vector3DistanceSqr(d->pos, b->pos) < 100;
    gClarenceBossTarget = b;

    int ca = b->curAnim;
    if (b->state == ALISTER_STATE_TALK) { BG_SetAnimSafe(b, ALISTER_ANIM_TALK, false); }
    else if (b->state == ALISTER_STATE_RUN || b->state == ALISTER_STATE_RETURN) { BG_SetAnimSafe(b, ALISTER_ANIM_RUN, false); }
    else if (b->state == ALISTER_STATE_HIT) { BG_SetAnimSafe(b, ALISTER_ANIM_HIT, false);}
    else if (b->state == ALISTER_STATE_DEFEATED) { BG_SetAnimSafe(b, ALISTER_ANIM_DYING, false);}
    else { BG_SetAnimSafe(b, ALISTER_ANIM_IDLE, false);}
    if (ca != b->curAnim) { b->animFrame = 0; }
    BG_UpdateAnim(b,dt);

    if (b->state < ALISTER_STATE_COMMAND)
    {
        aliPos = &b->pos;
        b->health = b->startHealth;
        d->alisterDead = false;
        if (donNear && Vector3DistanceSqr(b->pos, d->pos) > 4) 
        { 
            b->targetYaw = BG_YawTo(b->pos, d->pos);
            b->yaw = StepYawDeg(b->yaw, b->targetYaw, 180.0f * dt);
        }
        return;
    }

    if (d->alisterDead)
    {
        b->dead = true; b->active = false;
        return;
    }

    if (b->health <= 0 && b->state != ALISTER_STATE_DEFEATED)
    {
        if (!d->alisterEvilRevealed)
        {
            // Before the reveal / 10 shadow books, Alister cannot be defeated.
            b->health = 1;
            b->state = ALISTER_STATE_HURT;
            b->targetPos = b->pos;
            b->vel = (Vector3){ 0 };
        }
        else
        {
            b->state = ALISTER_STATE_DEFEATED;
            b->targetPos = b->pos;
            b->vel = (Vector3){ 0 };
            b->health = b->startHealth;
        }
    }

    switch (b->state)
    {
    case ALISTER_STATE_COMMAND:
    {
        b->targetYaw = BG_YawTo(b->pos, *mechPos);
        float gy = BG_GroundY(b->pos);
        if ((donVeryNear && Vector3DistanceSqr(b->spawnPoint, b->pos) > 50 * 60) || gy < WHALE_SURFACE + 5 || gy > WHALE_SURFACE + 38)
        {
            b->targetPos = b->spawnPoint;
            b->targetYaw = BG_YawTo(b->pos, b->spawnPoint);
            b->state = ALISTER_STATE_RETURN;
            b->steerTimer = 8;
        }
        if (donNear)
        {
            b->state = ALISTER_STATE_HURT;
        }
    } break;
    case ALISTER_STATE_HIT:
    {
        if (BG_AnimNearEnd(b))
        {
            b->state = ALISTER_STATE_HURT;
        }
    } break;
    case ALISTER_STATE_HURT:
    {
        Alister_StartRunAwayFromDonogan(b, d);
        b->steerTimer = 1;
    } break;
    case ALISTER_STATE_RETURN:
    {
        b->steerTimer -= dt;

        Alister_TurnAndMoveForward(b, dt, 12.0f);

        if ((Vector3DistanceSqr(b->pos, b->targetPos) < ALISTER_RUN_ARRIVE_DIST * ALISTER_RUN_ARRIVE_DIST)
            || b->steerTimer < 0)
        {
            b->steerTimer = 0;
            b->state = ALISTER_STATE_COMMAND;
            b->targetPos = b->pos;
        }
    } break;
    case ALISTER_STATE_RUN:
    {
        b->steerTimer -= dt;
        Alister_TurnAndMoveForward(b, dt, ALISTER_RUN_SPEED);
        float gy = BG_GroundY(b->pos);
        if ((donVeryNear && Vector3DistanceSqr(b->spawnPoint, b->pos) > 50 * 60) || gy < WHALE_SURFACE || gy > WHALE_SURFACE + 336) //PLAYER_FLOAT_Y_POSITION
        {
            b->targetPos = b->spawnPoint;
            b->targetYaw = BG_YawTo(b->pos, b->spawnPoint);
            b->state = ALISTER_STATE_RETURN;
            b->steerTimer = 8;
            break;
        }
        else if (donNear)
        {
            Alister_StartRunAwayFromDonogan(b, d);
            break;
        }
        if ((Vector3DistanceSqr(b->pos, b->targetPos) < ALISTER_RUN_ARRIVE_DIST * ALISTER_RUN_ARRIVE_DIST)
            || b->steerTimer < 0)
        {
            b->steerTimer = 0;
            // If the Mech encounter has started, go back to command.
            // Otherwise chill.
            b->state = ALISTER_STATE_COMMAND;
            b->targetPos = b->pos;
        }
    } break;

    case ALISTER_STATE_DEFEATED:
    {
        b->targetPitch = -90.0f;
        if (fabsf(b->pitch + 90.0f) < 2.0f)
        {
            d->alisterDead = true;
            toast = "You defeated Alister! The land of Mosswake thanks you!";
            StartTimer(&toastTimer);
            b->active = false;
            b->dead = true;

            if (b->gbm_index >= 0)
            {
                bgModelBorrower[b->gbm_index].isInUse = false;
            }

            b->gbm_index = -1;
            d->xp += 5000;
        }
    } break;

    default:
    {
        // Idle / talk
    } break;
    }
    float g_y = BG_GroundY(b->pos);
    if (g_y > -9000)
    {
        b->pos.y = g_y + 3.7;
    }
    b->yaw = WrapDeg(b->yaw);
    b->pitch = Lerp(b->pitch, b->targetPitch, dt);
    b->roll = Lerp(b->roll, b->targetRoll, dt);
    BG_UpdateMainBox(b);
}
#define MECH_Y_OFFSET 11

static inline void BG_Update_Mech(Donogan* d, BadGuy* b, float dt)
{
    if (!d || !b) return;
    if (b->steerTimer > 0)
    {
        b->steerTimer -= dt;
        if (b->steerTimer < 0) b->steerTimer = 0;
    }
    if (b->state < MECH_STATE_ACTIVE)
    {
        BG_UpdateMainBox(b);
        return;
    }
    float gy = BG_GroundY(b->pos);
    if (b->pos.y < gy + MECH_Y_OFFSET) //&& b->state != MECH_STATE_ATTACK
    {
        b->pos.y = gy + MECH_Y_OFFSET;
    }
    if (d->alisterDead)
    {
        b->state = MECH_STATE_DEFEATED;
    }
    if (Vector3DistanceSqr(*aliPos, *mechPos) > 400 * 400)
    {
        b->state = MECH_STATE_FALLBACK;
        b->steerTimer = 5;
    }
    TraceLog(LOG_WARNING, "Mech is in %d state", b->state);
    float distToDonSq = Vector3DistanceSqr(b->spawnPoint, d->pos);
    b->aware = distToDonSq < b->awareRadius * b->awareRadius;
    if (!b->aware) { b->state = MECH_STATE_FALLBACK; b->steerTimer = 5;}
    // Face Donogan unless we are flying to a chosen side point.
    b->targetYaw = BG_YawTo(b->pos, d->pos);

    switch (b->state)
    {
    case MECH_STATE_ACTIVE:
    {
        // ACTIVE is the planning state.
        b->attackLanded = false;

        // Stay awake, hover, then choose a fly point.
        if (!b->aware)
        {
            Vector3 idle = b->spawnPoint;
            idle.y = BG_GroundY(idle) + MECH_FLY_HEIGHT;

            b->targetPos = idle;
            b->pos = BG_MoveTowardVec3(b->pos, b->targetPos, MECH_FLY_SPEED * 0.35f * dt);
            b->yaw = Lerp(b->yaw, b->targetYaw, dt * 2.0f);
            break;
        }

        // Normal guard behavior: old 120*120 logic, now shared.
        Mech_PickFlyTarget(b, d);
        if (Mech_DonInGuardZone(b, d, MECH_THROW_RANGE))
        {
            b->steerTimer = 5;
            b->warnTimer = 3;
            b->state = MECH_STATE_THROW_DON;
        }
        else if (Mech_DonInGuardZone(b, d, MECH_GUARD_RANGE))
        {
            Vector3 toDonFromAli = Vector3Subtract(d->pos, *aliPos);
            toDonFromAli.y = 0;

            if (Vector3LengthSqr(toDonFromAli) < 0.0001f)
            {
                toDonFromAli = Vector3Subtract(d->pos, b->pos);
                toDonFromAli.y = 0;
            }

            if (Vector3LengthSqr(toDonFromAli) < 0.0001f)
            {
                toDonFromAli = BG_ForwardFromYawDeg(b->yaw);
            }
            else
            {
                toDonFromAli = Vector3Normalize(toDonFromAli);
            }

            Vector3 blockPath = Vector3Negate(Vector3Scale(toDonFromAli, 28));
            b->targetPos = Vector3Add(d->pos, blockPath);
            b->targetPos.y += 24;

            b->targetPitch = 15;
            b->targetYaw = BG_YawTo(b->pos, b->targetPos);
            b->state = MECH_STATE_FALLBACK;
            b->steerTimer = 5;
        }
    } break;
    case MECH_STATE_FALLBACK:
    {
        b->steerTimer -= dt;
        b->warnTimer = 0;
        b->pos = BG_MoveTowardVec3(b->pos, *aliPos, MECH_ATTACK_SPEED * 4.2 * dt);
        if (Vector3DistanceSqr(b->pos, *aliPos) < MECH_REPLAN_DIST * MECH_REPLAN_DIST * MECH_REPLAN_DIST || b->steerTimer <= 0)
        {
            b->state = MECH_STATE_ACTIVE;
        }
    } break;
    case MECH_STATE_FLY:
    {
        b->pos = BG_MoveTowardVec3(b->pos, b->targetPos, MECH_FLY_SPEED * dt);

        b->yaw = Lerp(b->yaw, b->targetYaw, dt * 3.0f);
        b->pitch = Lerp(b->pitch, b->targetPitch, dt * 3.0f);
        b->roll = Lerp(b->roll, b->targetRoll, dt * 3.0f);

        if (Vector3DistanceSqr(b->pos, b->targetPos) < MECH_REPLAN_DIST * MECH_REPLAN_DIST)
        {
            // Decide the attack position ONCE.
            b->warnPos = d->pos;

            float gy = BG_GroundY(b->warnPos);
            if (gy > -9000)
            {
                b->warnPos.y = gy + 0.08f; // tiny lift so it does not z-fight
            }

            // Mech waits above the target during warning.
            b->targetPos = b->warnPos;
            b->targetPos.y = b->warnPos.y + MECH_ATTACK_HEIGHT;

            b->warnTimer = MECH_WARN_TIME;
            if (gGame.diff == DIFF_EASY) { b->warnTimer++; }
            if (gGame.diff == DIFF_HARD) { b->warnTimer--; }
            b->warnSpin = 0;

            b->targetYaw = BG_YawTo(b->pos, b->warnPos);
            b->attackLanded = false;

            b->state = MECH_STATE_WARN;
        }
    } break;
    case MECH_STATE_THROW_DON:
    {
        b->warnTimer -= dt;
        b->targetPos = d->pos;
        b->pos = BG_MoveTowardVec3(b->pos, b->targetPos, MECH_ATTACK_SPEED * dt);

        b->targetYaw = BG_YawTo(b->pos, d->pos);
        b->yaw = Lerp(b->yaw, b->targetYaw, dt * 4.0f);
        b->pitch = Lerp(b->pitch, 15.0f, dt * 4.0f);
        b->roll = Lerp(b->roll, 0.0f, dt * 4.0f);
        if (CheckCollisionBoxes(b->box, d->outerBox))
        {
            // Close enough: throw Don back toward the battle arena center.
            bool throwRange =
                Mech_DonInGuardZone(b, d, MECH_THROW_RANGE) ||
                Vector3DistanceSqr(d->pos, b->pos) < MECH_THROW_RANGE * MECH_THROW_RANGE;

            if (throwRange && HasTimerElapsed(&d->hitTimer))
            {
                b->attackLanded = true;
                Mech_StartThrowDon(b, d);
                break;
            }
        }
        if (b->warnTimer <= 0 && b->steerTimer <= 0)
        {
            //b->state = MECH_STATE_STOMP_COMBO;
            b->state = MECH_STATE_FALLBACK;
            b->steerTimer = 5;
        }
    } break;
    case MECH_STATE_WARN:
    {
        b->warnTimer -= dt;
        b->warnSpin += MECH_WARN_SPIN_SPEED * dt;

        // Hover over the chosen attack point while warning.
        Vector3 hover = b->warnPos;
        hover.y += MECH_ATTACK_HEIGHT;

        b->targetPos = hover;
        b->pos = BG_MoveTowardVec3(b->pos, b->targetPos, MECH_FLY_SPEED * 0.65f * dt);

        b->targetYaw = BG_YawTo(b->pos, b->warnPos);
        b->yaw = Lerp(b->yaw, b->targetYaw, dt * 4.0f);
        b->pitch = Lerp(b->pitch, 15.0f, dt * 4.0f);
        b->roll = Lerp(b->roll, 0.0f, dt * 4.0f);

        if (b->warnTimer <= 0)
        {
            b->targetPos = b->warnPos;
            b->targetPos.y = b->warnPos.y + MECH_ATTACK_HEIGHT;

            b->attackLanded = false;
            b->state = MECH_STATE_ATTACK;
            b->warnTimer = MECH_WARN_TIME;
            b->steerTimer = 5;
        }
    } break;
    case MECH_STATE_ATTACK:
    {
        b->steerTimer -= dt;
        // Slight homing while attacking. This makes it scary but not perfectly unfair.
        Vector3 attackTarget = b->warnPos;
        attackTarget.y = b->warnPos.y + MECH_Y_OFFSET;

        b->targetPos = attackTarget;
        b->targetYaw = BG_YawTo(b->pos, b->warnPos);

        b->pos = BG_MoveTowardVec3(b->pos, b->targetPos, MECH_ATTACK_SPEED * dt);

        b->yaw = Lerp(b->yaw, b->targetYaw, dt * 5.0f);
        b->pitch = Lerp(b->pitch, 15.0f, dt * 4.0f);

        BG_UpdateMainBox(b);

        //bool closeEnough = Vector3DistanceSqr(b->pos, attackTarget) < 10.0f * 10.0f;
        bool boxHit = CheckCollisionBoxes(b->box, d->outerBox);

        if (!b->attackLanded && HasTimerElapsed(&d->hitTimer) && boxHit)
        {
            Vector3 dir = Vector3Subtract(d->pos, b->pos);
            dir.y = 0;

            if (Vector3LengthSqr(dir) < 0.0001f)
            {
                dir = BG_ForwardFromYawDeg(b->yaw);
            }
            else
            {
                dir = Vector3Normalize(dir);
            }

            d->health -= MECH_HIT_DAMAGE;
            DonSetState(d, DONOGAN_STATE_HIT);
            StartTimer(&d->hitTimer);

            d->shook = fmaxf(d->shook, MECH_HIT_SHAKE);
            d->velXZ = Vector3Scale(dir, MECH_HIT_KNOCKBACK);
            d->velY = fmaxf(d->velY, 7.0f);

            b->attackLanded = true;

            DustPuff_Spawn(d->pos);
        }

        if (Vector3DistanceSqr(b->pos, b->targetPos) < 5.0f * 4.0f || b->steerTimer <= 0)
        {
            b->targetPos = b->pos;
            b->targetPos.y = b->warnPos.y + MECH_FLY_HEIGHT;
            b->state = MECH_STATE_ACTIVE;
            b->warnTimer = MECH_WARN_TIME;
        }
    } break;

    case MECH_STATE_DEFEATED:
    {
        b->targetPos = b->spawnPoint;
        b->targetPitch = 0;
        b->targetRoll = 0;
        b->targetYaw = BG_YawTo(b->targetPos, d->pos);
        float targetY = BG_GroundY(b->pos) + MECH_Y_OFFSET;
        b->targetPos.y = targetY;
        b->pos = BG_MoveTowardVec3(b->pos, b->targetPos, 30.0f * dt);
        if (b->pos.y < targetY) {b->pos.y = targetY;}
    } break;

    default:
    {
        b->state = MECH_STATE_ACTIVE;
    } break;
    }

    b->yaw = Lerp(b->yaw, b->targetYaw, dt * 5.0f);
    b->pitch = Lerp(b->pitch, b->targetPitch, dt * 4.0f);
    b->roll = Lerp(b->roll, b->targetRoll, dt * 3.0f);
    BG_UpdateMainBox(b);
}

//create functions
BadGuy CreateMech(Vector3 pos)
{
    BadGuy b = { 0 };
    b.type = BG_MECH;
    b.spawnPoint = pos;
    b.spawnRadius = 10000;
    b.health = 10000000;
    b.startHealth = b.health;
    b.awareRadius = 700;
    b.tetherRadius = 700;
    b.gbm_index = -1;
    b.active = false;
    b.dead = false;
    b.aware = false;
    b.pos = pos;
    b.pitch = 90;
    b.scale = 12;
    b.speed = 1;
    //b.respawnTimer = CreateTimer(360);//6 minutes
    //b.interactionTimer = CreateTimer(120);//2 minutes
    b.drawColor = WHITE;
    BG_UpdateMainBox(&b);
    return b;
}
BadGuy CreateAlister(Vector3 pos)
{
    BadGuy b = { 0 };
    b.type = BG_ALISTER;
    b.spawnPoint = pos;
    b.spawnRadius = 10000;
    b.awareRadius = 700;
    b.tetherRadius = 700;
    b.gbm_index = -1;
    b.active = false;
    b.dead = false;
    b.aware = false;
    b.pos = pos;
    b.scale = 4;
    b.speed = 1;
    b.health = 800;
    b.startHealth = b.health;
    //b.respawnTimer = CreateTimer(360);//6 minutes
    //b.interactionTimer = CreateTimer(120);//2 minutes
    b.drawColor = WHITE;
    BG_UpdateMainBox(&b);
    return b;
}
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
    b.awareRadius = 100;
    b.tetherRadius = 40;
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
static inline BadGuy CreatePumpkinHopper(Vector3 pos)
{
    BadGuy b = { 0 };
    b.active = false;
    b.dead = false;
    b.aware = false;
    b.type = BG_PUMPKIN_HOPPER;
    b.spawnPoint = pos;
    b.pos = pos;
    b.scale = 0.001f;
    b.desiredScale = 1.8;
    b.state = HOPPER_STATE_SLEEP;
    b.spawnRadius = 120;
    b.awareRadius = 35.0f;
    b.speed = 8.0f;
    b.health = 20;
    b.startHealth = b.health;
    b.respawnTimer = CreateTimer(30);
    b.interactionTimer = CreateTimer(1.0f);
    b.gbm_index = -1;
    b.drawColor = WHITE;
    return b;
}

BadGuy CreateSkeleton(Vector3 pos)
{
    BadGuy b = { 0 };

    b.type = BG_SKELETON;
    b.spawnPoint = pos;
    b.spawnRadius = 180;   // activates when Don gets near
    b.awareRadius = 80;    // notices Don
    b.tetherRadius = 45.0f;

    b.gbm_index = -1;
    b.active = false;
    b.dead = false;
    b.aware = false;

    b.pos = pos;
    b.targetPos = pos;

    b.scale = 1.12;
    b.desiredScale = 1.12;

    b.speed = 6.0f;
    b.startHealth = 180;
    b.health = b.startHealth;

    b.state = SKELETON_STATE_RISE;

    b.animFPS = 24.0f;

    b.respawnTimer = CreateTimer(360);
    b.interactionTimer = CreateTimer(2.0f);

    b.drawColor = WHITE;

    return b;
}

//end of the file stuff, important!
void InitBadGuys(Shader ghostShader)
{
    InitBadGuyModels(ghostShader);
    bg_count = 394; //increment this, every time, you add, a bg...
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

    bg[43] = CreateYeti((Vector3) { -314, 880, 1779 });
    bg[44] = CreateYeti((Vector3) { -86, 740, 1932 });
    bg[45] = CreateYeti((Vector3) { -330, 830, 2712 });
    bg[46] = CreateYeti((Vector3) { -407, 782, 2977 });
    bg[47] = CreateYeti((Vector3) { -504, 688, 3462 });

    bg[48] = CreateGhost((Vector3) { -634, 560, 3708 });
    bg[49] = CreateGhost((Vector3) { 2981, 374, 247 });
    bg[50] = CreateGhost((Vector3) { 2696, 320, 252 });
    bg[51] = CreateGhost((Vector3) { 2439, 318, 195 });
    bg[52] = CreateGhost((Vector3) { 2286, 323, -95 });

    bg[53] = CreateRobo((Vector3) { 2722, 455, 1627 });
    bg[54] = CreateRobo((Vector3) { 2761, 451, 1628 });
    bg[55] = CreateRobo((Vector3) { 2764, 453, 1579 });
    bg[56] = CreateRobo((Vector3) { 2306, 481, 1591 });
    bg[57] = CreateRobo((Vector3) { 2230, 489, 1593 });

    //mountain top, yeti moutnain 2, part 1
    bg[58] = CreateYeti((Vector3) { -2201, 895, -592 });
    bg[59] = CreateYeti((Vector3) { -2083, 934, -374 });
    bg[60] = CreateYeti((Vector3) { -2092, 918, -497 });
    bg[61] = CreateYeti((Vector3) { -2040, 913, -646 });
    bg[62] = CreateRobo((Vector3) { -2026, 918, -600 });
    bg[63] = CreateRobo((Vector3) { -2007, 914, -573 });
    bg[64] = CreateRobo((Vector3) { -2094, 918, -515 });
    bg[65] = CreateGhost((Vector3) { -2094, 918, -646 });
    bg[66] = CreateGhost((Vector3) { -2007, 918, -497 });
    //yeti mountain 2 part 2, this will be hard
    //yetis
    bg[67] = CreateYeti((Vector3) { -2007.34, 948.71, -225.76 });
    bg[68] = CreateYeti((Vector3) { -2176.13, 922.35, -102.69 });
    bg[69] = CreateYeti((Vector3) { -2213.31, 914.82, -297.31 });
    bg[70] = CreateYeti((Vector3) { -2341.10, 896.00, -188.42 });
    bg[71] = CreateYeti((Vector3) { -2375.00, 892.24, -49.96 });
    bg[72] = CreateYeti((Vector3) { -2545.86, 862.12, -135.83 });
    bg[73] = CreateYeti((Vector3) { -2562.40, 857.79, -270.36 });
    bg[74] = CreateYeti((Vector3) { -2658.05, 835.76, -480.35 });
    bg[75] = CreateYeti((Vector3) { -2807.65, 813.18, -394.54 });
    bg[76] = CreateYeti((Vector3) { -3063.90, 752.17, -364.72 });
    bg[77] = CreateYeti((Vector3) { -3131.19, 752.94, -382.41 });

    //orbs
    bg[78] = CreateRobo((Vector3) { -3086.85, 752.94, -392.37 });
    bg[79] = CreateRobo((Vector3) { -2866.50, 801.29, -418.90 });
    bg[80] = CreateRobo((Vector3) { -2794.91, 816.94, -353.48 });
    bg[81] = CreateRobo((Vector3) { -2585.83, 852.27, -405.81 });
    bg[82] = CreateRobo((Vector3) { -2477.21, 806.41, -479.55 });
    bg[83] = CreateRobo((Vector3) { -2440.65, 811.14, -469.48 });
    bg[84] = CreateRobo((Vector3) { -2360.18, 822.55, -436.84 });
    bg[85] = CreateRobo((Vector3) { -2380.15, 825.39, -480.06 });
    bg[86] = CreateRobo((Vector3) { -2369.83, 891.80, -185.46 });
    bg[87] = CreateRobo((Vector3) { -2285.83, 904.04, -273.57 });
    bg[88] = CreateRobo((Vector3) { -2140.60, 929.88, -112.58 });

    //ghosts
    bg[89] = CreateGhost((Vector3) { -2122.75, 931.12, -260.41 });
    bg[90] = CreateGhost((Vector3) { -2367.44, 892.37, -110.10 });
    bg[91] = CreateGhost((Vector3) { -2486.79, 871.82, -243.77 });
    bg[92] = CreateGhost((Vector3) { -2633.73, 847.06, -217.27 });
    bg[93] = CreateGhost((Vector3) { -2662.45, 835.76, -489.27 });

    //near dons home (the first few ghosts...) and then lots of stuff in the forest outside the castle
    //GHOSTS
    bg[94] = CreateGhost((Vector3) { 2300.10, 376.70, 4527.03 });
    bg[95] = CreateGhost((Vector3) { 2180.39, 395.35, 4479.77 });
    bg[96] = CreateGhost((Vector3) { 2101.70, 377.21, 4620.84 });
    bg[97] = CreateGhost((Vector3) { 1717.42, 404.10, 4447.05 });
    //above is on start island, below is near the castle
    bg[98] = CreateGhost((Vector3) { 2877.78, 349.78, 207.21 });
    bg[99] = CreateGhost((Vector3) { 2799.20, 334.87, 266.73 });
    bg[100] = CreateGhost((Vector3) { 2740.46, 324.81, 260.23 });
    bg[101] = CreateGhost((Vector3) { 2691.21, 316.24, 120.75 });
    bg[102] = CreateGhost((Vector3) { 2619.31, 318.90, -216.24 });
    //ORBS
    bg[103] = CreateRobo((Vector3) { 2410.49, 312.47, -748.57 });
    bg[104] = CreateRobo((Vector3) { 2376.40, 312.47, -739.90 });
    bg[105] = CreateRobo((Vector3) { 2321.68, 312.47, -735.42 });
    bg[106] = CreateRobo((Vector3) { 2261.02, 316.24, -775.67 });
    bg[107] = CreateRobo((Vector3) { 2231.89, 316.24, -830.51 });
    bg[108] = CreateRobo((Vector3) { 2224.96, 320.00, -902.57 });
    bg[109] = CreateRobo((Vector3) { 2228.61, 320.00, -949.82 });
    bg[110] = CreateRobo((Vector3) { 2213.15, 320.00, -1000.84 });
    bg[111] = CreateRobo((Vector3) { 2220.57, 318.51, -1122.92 });
    bg[112] = CreateRobo((Vector3) { 2210.55, 316.24, -1155.30 });
    bg[113] = CreateRobo((Vector3) { 2219.12, 316.24, -1191.33 });
    //not sure exactly, but somewhere in here is orb island
    bg[114] = CreateRobo((Vector3) { 2238.47, 312.47, -1233.92 });
    bg[115] = CreateRobo((Vector3) { 2214.56, 312.47, -1285.44 });
    bg[116] = CreateRobo((Vector3) { 2248.70, 307.30, -1349.97 });
    bg[117] = CreateRobo((Vector3) { 2218.14, 308.71, -1380.00 });
    bg[118] = CreateRobo((Vector3) { 2220.10, 316.24, -1198.31 });
    bg[119] = CreateRobo((Vector3) { 2248.10, 320.00, -1017.31 });
    bg[120] = CreateRobo((Vector3) { 2313.92, 320.00, -882.06 });
    bg[121] = CreateRobo((Vector3) { 3207.65, 412.32, 305.29 });
    bg[122] = CreateRobo((Vector3) { 3207.43, 416.60, 222.02 });
    bg[123] = CreateRobo((Vector3) { 3344.50, 430.69, 121.06 });
    bg[124] = CreateRobo((Vector3) { 3364.01, 432.94, 55.04 });
    bg[125] = CreateRobo((Vector3) { 3344.18, 436.66, 6.49 });
    bg[126] = CreateRobo((Vector3) { 3612.64, 399.57, 146.15 });
    bg[127] = CreateRobo((Vector3) { 3670.18, 390.08, 176.65 });
    //ghosts
    bg[128] = CreateGhost((Vector3) { 3603.21, 402.07, 180.25 });
    bg[129] = CreateGhost((Vector3) { 3477.33, 417.88, 208.81 });
    bg[130] = CreateGhost((Vector3) { 3443.21, 420.89, 231.83 });
    bg[131] = CreateGhost((Vector3) { 3416.24, 421.65, 416.51 });
    //orbs
    bg[132] = CreateRobo((Vector3) { 3170.54, 418.48, 169.34 });
    bg[133] = CreateRobo((Vector3) { 2814.29, 345.95, 71.14 });
    bg[134] = CreateRobo((Vector3) { 2652.73, 324.56, -134.66 });
    bg[135] = CreateRobo((Vector3) { 2733.59, 352.75, -238.67 });
    //hoppers bridge
    bg[136] = CreatePumpkinHopper((Vector3) { 1650.05, 611.32, -1718.89 });
    bg[137] = CreatePumpkinHopper((Vector3) { 1618.74, 620.50, -1723.95 });
    bg[138] = CreatePumpkinHopper((Vector3) { 1574.60, 634.86, -1718.52 });
    //
    bg[139] = CreatePumpkinHopper((Vector3) { 1500.46, 659.49, -1739.17 });
    bg[140] = CreatePumpkinHopper((Vector3) { 1501.12, 659.49, -1747.64 });
    bg[141] = CreatePumpkinHopper((Vector3) { 1460.35, 672.09, -1743.29 });
    bg[142] = CreatePumpkinHopper((Vector3) { 1384.44, 698.50, -1752.68 });
    bg[143] = CreatePumpkinHopper((Vector3) { 1271.63, 737.28, -1767.41 });
    bg[144] = CreatePumpkinHopper((Vector3) { 1197.10, 764.23, -1777.97 });
    bg[145] = CreatePumpkinHopper((Vector3) { 1159.77, 777.87, -1782.11 });
    bg[146] = CreatePumpkinHopper((Vector3) { 1046.50, 819.42, -1796.91 });
    //hoppers on dons island
    bg[147] = CreatePumpkinHopper((Vector3) { 2882.77, 355.95, 4311.23 });
    bg[148] = CreatePumpkinHopper((Vector3) { 2652.06, 361.16, 4353.07 });
    bg[149] = CreatePumpkinHopper((Vector3) { 2625.59, 374.51, 4280.33 });
    bg[150] = CreatePumpkinHopper((Vector3) { 2488.96, 391.36, 4343.77 });
    bg[151] = CreatePumpkinHopper((Vector3) { 2436.51, 395.85, 4345.13 });
    bg[152] = CreatePumpkinHopper((Vector3) { 2341.39, 391.53, 4417.55 });
    bg[153] = CreatePumpkinHopper((Vector3) { 2315.52, 381.14, 4496.64 });
    //hoppers near ocean north past castle
    bg[154] = CreatePumpkinHopper((Vector3) { 2284.50f, 338.82f, -3194.66f });
    bg[155] = CreatePumpkinHopper((Vector3) { 2312.21f, 338.82f, -3239.63f });
    bg[156] = CreatePumpkinHopper((Vector3) { 2329.44f, 338.82f, -3271.89f });
    bg[157] = CreatePumpkinHopper((Vector3) { 2376.49f, 331.29f, -3281.79f });
    bg[158] = CreatePumpkinHopper((Vector3) { 2426.24f, 327.53f, -3448.19f });
    bg[159] = CreatePumpkinHopper((Vector3) { 2479.76f, 320.06f, -3481.24f });
    //skeletons initial
    bg[160] = CreateSkeleton((Vector3) { 2081.94f, 371.92f, 4659.33f });
    bg[161] = CreateSkeleton((Vector3) { 2338.47f, 436.71f, 4152.02f });
    bg[162] = CreateSkeleton((Vector3) { 1369.24f, 327.53f, 1515.23f });
    bg[163] = CreateSkeleton((Vector3) { 1356.75f, 323.76f, 1612.49f });
    bg[164] = CreateSkeleton((Vector3) { 1266.54f, 327.53f, 1637.80f });
    bg[165] = CreateSkeleton((Vector3) { 1139.63f, 335.06f, 1723.39f });
    bg[166] = CreateSkeleton((Vector3) { 1049.55f, 342.59f, 1678.52f });
    bg[167] = CreateSkeleton((Vector3) { 900.89f, 338.82f, 1618.25f });
    bg[168] = CreateSkeleton((Vector3) { 894.38f, 350.12f, 1516.07f });

    bg[169] = CreateSkeleton((Vector3) { 43.64f, 372.30f, -1486.26f });
    bg[170] = CreateSkeleton((Vector3) { -25.86f, 368.94f, -1588.60f });
    bg[171] = CreateSkeleton((Vector3) { 15.82f, 365.18f, -1654.45f });
    bg[172] = CreateSkeleton((Vector3) { -43.67f, 350.12f, -1875.42f });
    bg[173] = CreateSkeleton((Vector3) { -89.70f, 344.07f, -1878.76f });
    bg[174] = CreateSkeleton((Vector3) { -247.13f, 323.76f, -1941.97f });
    bg[175] = CreateSkeleton((Vector3) { -336.20f, 317.34f, -2219.33f });
    bg[176] = CreateSkeleton((Vector3) { -361.29f, 320.00f, -2217.44f });

    bg[177] = CreateSkeleton((Vector3) { -812.29f, 331.29f, -4314.53f });
    bg[178] = CreateSkeleton((Vector3) { -848.85f, 323.76f, -4360.31f });
    bg[179] = CreateSkeleton((Vector3) { -885.17f, 320.00f, -4370.65f });
    bg[180] = CreateSkeleton((Vector3) { -940.58f, 320.00f, -4392.25f });

    bg[181] = CreateSkeleton((Vector3) { -881.24f, 308.71f, -4662.05f });
    bg[182] = CreateSkeleton((Vector3) { -855.88f, 312.47f, -4705.63f });
    bg[183] = CreateSkeleton((Vector3) { -919.66f, 308.71f, -4734.47f });

    bg[184] = CreateSkeleton((Vector3) { 1873.65f, 420.87f, -2606.50f });
    bg[185] = CreateSkeleton((Vector3) { 1860.86f, 423.13f, -2646.33f });
    bg[186] = CreateSkeleton((Vector3) { 1956.70f, 381.79f, -2647.76f });
    bg[187] = CreateSkeleton((Vector3) { 1988.69f, 370.50f, -2701.86f });
    bg[188] = CreateSkeleton((Vector3) { 1963.52f, 385.05f, -2784.16f });

    bg[189] = CreateSkeleton((Vector3) { 1872.40f, 395.20f, -3161.17f });
    bg[190] = CreateSkeleton((Vector3) { 1578.17f, 323.06f, -3486.57f });
    bg[191] = CreateSkeleton((Vector3) { 1640.62f, 316.46f, -3519.04f });

    // skeletons outside cinderspire
    bg[192] = CreateSkeleton((Vector3) { -481.46f, 658.82f, -1186.00f });
    bg[193] = CreateSkeleton((Vector3) { -496.83f, 655.06f, -1261.21f });
    bg[194] = CreateSkeleton((Vector3) { -414.20f, 658.82f, -1254.08f });
    bg[195] = CreateSkeleton((Vector3) { -371.93f, 662.59f, -1219.08f });
    bg[196] = CreateSkeleton((Vector3) { -331.92f, 666.35f, -1186.96f });
    bg[197] = CreateSkeleton((Vector3) { -312.59f, 670.12f, -1157.08f });

    // ghosts inside
    bg[198] = CreateGhost((Vector3) { -404.37f, 613.10f, -1165.66f });
    bg[199] = CreateGhost((Vector3) { -439.83f, 613.02f, -1168.91f });
    bg[200] = CreateGhost((Vector3) { -453.03f, 612.99f, -1198.14f });
    bg[201] = CreateGhost((Vector3) { -410.32f, 613.09f, -1211.30f });
    bg[202] = CreateGhost((Vector3) { -386.41f, 613.14f, -1195.41f });
    bg[203] = CreateGhost((Vector3) { -397.82f, 568.59f, -1138.16f });
    bg[204] = CreateGhost((Vector3) { -378.18f, 567.13f, -1188.98f });
    bg[205] = CreateGhost((Vector3) { -417.35f, 567.04f, -1208.09f });
    bg[206] = CreateGhost((Vector3) { -445.06f, 566.98f, -1183.13f });
    for (int i = 198; i < 207; i++) { bg[i].spawnRadius = 22; }

    // skeletons outside the white temple
    bg[207] = CreateSkeleton((Vector3) { -2120.46f, 313.38f, 3331.87f });
    bg[208] = CreateSkeleton((Vector3) { -2066.84f, 312.47f, 3387.75f });
    bg[209] = CreateSkeleton((Vector3) { -2117.74f, 316.24f, 3628.95f });
    bg[210] = CreateSkeleton((Vector3) { -2289.84f, 316.24f, 3630.90f });

    //dream_land
    bg[211] = CreateSkeleton((Vector3) { -9999.00f, 10001.00f, -9999.00f });
    bg[212] = CreateSkeleton((Vector3) { -10018.44f, 10001.00f, -9956.30f });
    bg[213] = CreateSkeleton((Vector3) { -9993.18f, 10001.00f, -9933.52f });
    bg[214] = CreateSkeleton((Vector3) { -10019.24f, 10001.00f, -9899.24f });
    bg[215] = CreateSkeleton((Vector3) { -9988.51f, 10001.00f, -9877.22f });
    bg[216] = CreateSkeleton((Vector3) { -9976.92f, 10001.00f, -9898.58f });

    //alister and the mech...
    bg[217] = CreateAlister((Vector3) { -3788.46, 327.53, 1524.95 });
    bg[218] = CreateMech((Vector3) { -3761.57, 327.53, 1545.34 });

    //final stuff 1
        // new badguys: road / field loop
    bg[219] = CreateSkeleton((Vector3) { 838.69f, 323.76f, 2072.33f });
    bg[220] = CreateSkeleton((Vector3) { 865.91f, 316.24f, 2169.33f });
    bg[221] = CreateRobo((Vector3) { 1071.09f, 312.47f, 2104.01f }); // orb
    bg[222] = CreateSkeleton((Vector3) { 864.87f, 327.53f, 1953.42f });
    bg[223] = CreateGhost((Vector3) { 804.02f, 327.53f, 1929.30f });
    bg[224] = CreateSkeleton((Vector3) { 851.02f, 338.82f, 1750.64f });
    bg[225] = CreatePumpkinHopper((Vector3) { 788.65f, 337.23f, 1606.41f });
    bg[226] = CreateSkeleton((Vector3) { 770.72f, 357.65f, 1479.98f });
    bg[227] = CreateRobo((Vector3) { 749.43f, 372.11f, 1244.17f }); // orb
    bg[228] = CreateSkeleton((Vector3) { 726.27f, 380.24f, 1139.61f });
    bg[229] = CreateSkeleton((Vector3) { 784.48f, 389.12f, 1033.49f });
    bg[230] = CreateGhost((Vector3) { 829.09f, 388.47f, 969.90f });
    bg[231] = CreateSkeleton((Vector3) { 817.70f, 397.34f, 901.06f });
    bg[232] = CreateSkeleton((Vector3) { 871.25f, 393.61f, 773.38f });
    bg[233] = CreateRobo((Vector3) { 1015.22f, 393.60f, 356.24f }); // orb
    bg[234] = CreateSkeleton((Vector3) { 1019.07f, 390.83f, 327.57f });
    bg[235] = CreateGhost((Vector3) { 1032.31f, 385.89f, 305.81f });
    bg[236] = CreateSkeleton((Vector3) { 1046.64f, 382.52f, 297.45f });
    bg[237] = CreatePumpkinHopper((Vector3) { 1104.88f, 374.18f, 326.92f });
    bg[238] = CreateSkeleton((Vector3) { 1195.66f, 388.79f, 473.47f });
    bg[239] = CreateRobo((Vector3) { 1221.30f, 387.76f, 555.69f }); // orb
    bg[240] = CreateSkeleton((Vector3) { 1244.51f, 384.00f, 631.45f });
    bg[241] = CreateSkeleton((Vector3) { 1261.09f, 380.24f, 711.58f });
    bg[242] = CreateGhost((Vector3) { 1261.95f, 373.19f, 845.87f });
    bg[243] = CreateSkeleton((Vector3) { 1295.18f, 360.79f, 979.91f });
    bg[244] = CreateSkeleton((Vector3) { 1360.17f, 350.12f, 1115.42f });
    bg[245] = CreateRobo((Vector3) { 1264.02f, 335.06f, 1484.92f }); // orb
    bg[246] = CreateSkeleton((Vector3) { 1273.54f, 331.29f, 1560.37f });
    bg[247] = CreateGhost((Vector3) { 1131.51f, 342.59f, 1607.91f });
    bg[248] = CreateSkeleton((Vector3) { 1092.80f, 344.65f, 1618.04f });
    bg[249] = CreatePumpkinHopper((Vector3) { 1078.21f, 338.82f, 1702.59f });

    // new badguys: bridge / high path
    bg[250] = CreateSkeleton((Vector3) { 1720.88f, 547.15f, -1082.64f });
    bg[251] = CreateRobo((Vector3) { 1770.57f, 513.17f, -1119.26f }); // orb
    bg[252] = CreateSkeleton((Vector3) { 1817.42f, 476.78f, -1140.00f });
    bg[253] = CreateSkeleton((Vector3) { 1765.47f, 528.41f, -1581.54f });
    bg[254] = CreateGhost((Vector3) { 1788.23f, 514.89f, -1600.64f });
    bg[255] = CreateSkeleton((Vector3) { 1805.56f, 502.59f, -1623.49f });
    bg[256] = CreateSkeleton((Vector3) { 1788.70f, 512.10f, -1653.68f });
    bg[257] = CreateRobo((Vector3) { 1808.33f, 500.71f, -1678.88f }); // orb
    bg[258] = CreateSkeleton((Vector3) { 1837.14f, 476.50f, -1712.33f });
    bg[259] = CreateGhost((Vector3) { 1868.19f, 454.73f, -1733.17f });
    bg[260] = CreateSkeleton((Vector3) { 1875.24f, 446.98f, -1770.96f });
    bg[261] = CreatePumpkinHopper((Vector3) { 1890.08f, 435.21f, -1793.35f });
    bg[262] = CreateSkeleton((Vector3) { 1915.13f, 415.29f, -1793.52f });
    bg[263] = CreateRobo((Vector3) { 1938.39f, 393.82f, -1781.58f }); // orb
    bg[264] = CreateSkeleton((Vector3) { 1949.05f, 386.47f, -1775.32f });
    bg[265] = CreateSkeleton((Vector3) { 1969.56f, 367.64f, -1792.07f });
    bg[266] = CreateGhost((Vector3) { 1978.56f, 359.03f, -1849.50f });
    bg[267] = CreateSkeleton((Vector3) { 1973.27f, 350.12f, -2243.39f });
    bg[268] = CreateSkeleton((Vector3) { 1995.61f, 336.16f, -2328.58f });
    bg[269] = CreateRobo((Vector3) { 2040.10f, 334.17f, -2580.88f }); // orb
    bg[270] = CreateSkeleton((Vector3) { 2026.04f, 345.27f, -2607.09f });
    bg[271] = CreateGhost((Vector3) { 2043.83f, 340.19f, -2653.39f });
    bg[272] = CreateSkeleton((Vector3) { 2037.48f, 347.48f, -2700.70f });
    bg[273] = CreatePumpkinHopper((Vector3) { 2048.16f, 346.35f, -2745.88f });

    // new badguys: north ocean / hopper coast
    bg[274] = CreateSkeleton((Vector3) { 2319.35f, 319.75f, -3133.44f });
    bg[275] = CreateRobo((Vector3) { 2332.61f, 325.08f, -3170.51f }); // orb
    bg[276] = CreateSkeleton((Vector3) { 2367.36f, 319.31f, -3196.22f });
    bg[277] = CreateSkeleton((Vector3) { 2395.05f, 325.00f, -3264.46f });
    bg[278] = CreateGhost((Vector3) { 2393.45f, 332.83f, -3346.54f });
    bg[279] = CreateSkeleton((Vector3) { 2368.66f, 322.33f, -3510.31f });
    bg[280] = CreateSkeleton((Vector3) { 2335.76f, 318.87f, -3524.85f });
    bg[281] = CreateRobo((Vector3) { 2279.71f, 317.63f, -3530.12f }); // orb
    bg[282] = CreateSkeleton((Vector3) { 2250.75f, 319.32f, -3522.71f });
    bg[283] = CreateGhost((Vector3) { 2228.13f, 315.75f, -3538.38f });
    bg[284] = CreateSkeleton((Vector3) { 2142.28f, 321.70f, -3513.92f });
    bg[285] = CreatePumpkinHopper((Vector3) { 2129.18f, 328.28f, -3499.04f });
    bg[286] = CreateSkeleton((Vector3) { 2121.86f, 330.74f, -3490.06f });
    bg[287] = CreateRobo((Vector3) { 2102.87f, 337.73f, -3460.26f }); // orb
    bg[288] = CreateSkeleton((Vector3) { 2089.82f, 344.15f, -3433.01f });
    bg[289] = CreateSkeleton((Vector3) { 2061.10f, 347.14f, -3420.56f });
    bg[290] = CreateGhost((Vector3) { 2012.14f, 361.00f, -3361.40f });
    bg[291] = CreateSkeleton((Vector3) { 1974.66f, 371.01f, -3302.82f });
    bg[292] = CreateSkeleton((Vector3) { 1909.93f, 374.95f, -3302.65f });
    bg[293] = CreateRobo((Vector3) { 1894.61f, 371.73f, -3316.43f }); // orb
    bg[294] = CreateSkeleton((Vector3) { 1828.47f, 368.30f, -3346.63f });
    bg[295] = CreateGhost((Vector3) { 1724.42f, 363.21f, -3368.46f });
    bg[296] = CreateSkeleton((Vector3) { 1707.23f, 363.11f, -3368.75f });
    bg[297] = CreatePumpkinHopper((Vector3) { 1695.30f, 363.33f, -3367.89f });
    bg[298] = CreateSkeleton((Vector3) { 1683.06f, 359.44f, -3382.15f });
    bg[299] = CreateRobo((Vector3) { 1682.16f, 354.37f, -3392.44f }); // orb
    bg[300] = CreateSkeleton((Vector3) { 1675.78f, 350.14f, -3408.04f });
    bg[301] = CreateSkeleton((Vector3) { 1666.14f, 349.26f, -3411.81f });
    bg[302] = CreateGhost((Vector3) { 1646.54f, 349.82f, -3409.16f });
    bg[303] = CreateSkeleton((Vector3) { 1634.80f, 350.75f, -3405.23f });
    bg[304] = CreateSkeleton((Vector3) { 1586.39f, 343.04f, -3422.23f });
    bg[305] = CreateRobo((Vector3) { 1550.25f, 338.22f, -3426.52f }); // orb
    bg[306] = CreateSkeleton((Vector3) { 1492.36f, 336.57f, -3406.89f });
    bg[307] = CreateGhost((Vector3) { 1469.68f, 338.09f, -3394.89f });
    bg[308] = CreateSkeleton((Vector3) { 1453.65f, 340.78f, -3381.38f });
    bg[309] = CreatePumpkinHopper((Vector3) { 1443.66f, 339.98f, -3374.99f });
    bg[310] = CreateSkeleton((Vector3) { 1424.84f, 339.32f, -3373.93f });
    bg[311] = CreateRobo((Vector3) { 1413.76f, 338.82f, -3375.34f }); // orb
    bg[312] = CreateSkeleton((Vector3) { 1354.98f, 331.51f, -3386.55f });
    bg[313] = CreateSkeleton((Vector3) { 1314.94f, 331.29f, -3373.83f });
    bg[314] = CreateGhost((Vector3) { 1284.29f, 327.53f, -3397.88f });
    bg[315] = CreateSkeleton((Vector3) { 1263.87f, 323.76f, -3445.41f });
    bg[316] = CreateSkeleton((Vector3) { 1277.55f, 320.00f, -3483.52f });
    bg[317] = CreateRobo((Vector3) { 1283.34f, 316.24f, -3512.05f }); // orb
    bg[318] = CreateSkeleton((Vector3) { 1257.61f, 312.07f, -3537.90f });
    bg[319] = CreateGhost((Vector3) { -2228.96f, 493.18f, -1810.53f });
    bg[320] = CreateSkeleton((Vector3) { -2199.66f, 496.94f, -1797.44f });
    bg[321] = CreatePumpkinHopper((Vector3) { -2176.26f, 496.94f, -1766.08f });
    bg[322] = CreateSkeleton((Vector3) { -2146.94f, 500.71f, -1762.92f });
    bg[323] = CreateRobo((Vector3) { -2128.09f, 500.71f, -1817.56f }); // orb
    bg[324] = CreateSkeleton((Vector3) { -2118.18f, 496.94f, -1885.43f });

    // new badguys: cinder village / windmill fields
    bg[325] = CreateSkeleton((Vector3) { -2075.16f, 496.94f, -1928.03f });
    bg[326] = CreateGhost((Vector3) { -2037.87f, 500.71f, -1946.14f });
    bg[327] = CreateSkeleton((Vector3) { -2032.25f, 496.94f, -1977.31f });
    bg[328] = CreateSkeleton((Vector3) { -1991.25f, 495.91f, -2004.21f });
    bg[329] = CreateRobo((Vector3) { -1793.72f, 500.10f, -2051.01f }); // orb
    bg[330] = CreateSkeleton((Vector3) { -1803.53f, 496.94f, -2086.28f });
    bg[331] = CreateGhost((Vector3) { -1805.51f, 496.35f, -2115.21f });
    bg[332] = CreateSkeleton((Vector3) { -1786.49f, 493.18f, -2150.16f });
    bg[333] = CreatePumpkinHopper((Vector3) { -1786.71f, 490.09f, -2173.55f });
    bg[334] = CreateSkeleton((Vector3) { -1788.91f, 489.41f, -2182.82f });
    bg[335] = CreateRobo((Vector3) { -1808.99f, 489.41f, -2219.31f }); // orb
    bg[336] = CreateSkeleton((Vector3) { -1829.11f, 485.65f, -2243.58f });
    bg[337] = CreateSkeleton((Vector3) { -1837.95f, 485.65f, -2264.14f });
    bg[338] = CreateGhost((Vector3) { -1840.27f, 484.82f, -2275.91f });
    bg[339] = CreateSkeleton((Vector3) { -1847.66f, 481.88f, -2295.49f });
    bg[340] = CreateSkeleton((Vector3) { -1847.66f, 481.88f, -2295.49f });
    bg[341] = CreateRobo((Vector3) { -1837.93f, 481.88f, -2297.35f }); // orb
    bg[342] = CreateSkeleton((Vector3) { -1837.93f, 481.88f, -2297.35f });
    bg[343] = CreateGhost((Vector3) { -1826.01f, 481.88f, -2296.30f });
    bg[344] = CreateSkeleton((Vector3) { -1826.01f, 481.88f, -2296.30f });
    bg[345] = CreatePumpkinHopper((Vector3) { -1815.04f, 480.33f, -2294.60f });
    bg[346] = CreateSkeleton((Vector3) { -1815.04f, 480.33f, -2294.60f });
    bg[347] = CreateRobo((Vector3) { -1539.01f, 460.00f, -2128.11f }); // orb
    bg[348] = CreateSkeleton((Vector3) { -1509.33f, 458.59f, -2114.98f });
    bg[349] = CreateSkeleton((Vector3) { -1491.78f, 455.09f, -2118.12f });
    bg[350] = CreateGhost((Vector3) { -1481.58f, 453.68f, -2119.88f });
    bg[351] = CreateSkeleton((Vector3) { -1464.60f, 455.01f, -2113.91f });
    bg[352] = CreateSkeleton((Vector3) { -1458.30f, 455.53f, -2107.35f });

    // new badguys: ruins / lower path
    bg[353] = CreateRobo((Vector3) { -1446.85f, 457.55f, -2087.01f }); // orb
    bg[354] = CreateSkeleton((Vector3) { -1447.28f, 459.29f, -2078.36f });
    bg[355] = CreateGhost((Vector3) { -1443.07f, 460.10f, -2060.23f });
    bg[356] = CreateSkeleton((Vector3) { -1438.06f, 461.53f, -2054.18f });
    bg[357] = CreatePumpkinHopper((Vector3) { -1426.79f, 463.06f, -2045.29f });
    bg[358] = CreateSkeleton((Vector3) { -1419.53f, 463.06f, -2042.29f });
    bg[359] = CreateRobo((Vector3) { -1412.16f, 463.06f, -2039.55f }); // orb
    bg[360] = CreateSkeleton((Vector3) { -1396.25f, 462.66f, -2033.61f });
    bg[361] = CreateSkeleton((Vector3) { -1389.35f, 462.96f, -2032.32f });
    bg[362] = CreateGhost((Vector3) { -1381.65f, 463.06f, -2030.67f });
    bg[363] = CreateSkeleton((Vector3) { -1362.91f, 463.06f, -2027.34f });
    bg[364] = CreateSkeleton((Vector3) { -1355.90f, 462.19f, -2026.93f });
    bg[365] = CreateRobo((Vector3) { -1349.26f, 460.63f, -2027.03f }); // orb
    bg[366] = CreateSkeleton((Vector3) { -1327.81f, 459.29f, -2027.84f });
    bg[367] = CreateGhost((Vector3) { -1320.57f, 459.29f, -2029.47f });
    bg[368] = CreateSkeleton((Vector3) { -1314.07f, 459.29f, -2030.60f });
    bg[369] = CreatePumpkinHopper((Vector3) { -1307.41f, 458.23f, -2017.91f });
    bg[370] = CreateSkeleton((Vector3) { -1306.82f, 458.07f, -2010.08f });
    bg[371] = CreateRobo((Vector3) { -1308.67f, 458.49f, -2002.45f }); // orb
    bg[372] = CreateSkeleton((Vector3) { -1310.71f, 458.96f, -1994.42f });
    bg[373] = CreateSkeleton((Vector3) { -1314.26f, 459.29f, -1988.36f });
    bg[374] = CreateGhost((Vector3) { -1316.67f, 459.29f, -1981.77f });
    bg[375] = CreateSkeleton((Vector3) { -1318.16f, 459.29f, -1975.32f });
    bg[376] = CreateSkeleton((Vector3) { -1319.01f, 459.29f, -1968.77f });
    bg[377] = CreateRobo((Vector3) { -1319.07f, 459.29f, -1961.75f }); // orb
    bg[378] = CreateSkeleton((Vector3) { -1317.98f, 459.29f, -1955.24f });
    bg[379] = CreateGhost((Vector3) { -1163.26f, 471.61f, -1755.58f });
    bg[380] = CreateSkeleton((Vector3) { -1134.33f, 475.39f, -1773.59f });
    bg[381] = CreatePumpkinHopper((Vector3) { -1134.73f, 471.40f, -1822.24f });
    bg[382] = CreateSkeleton((Vector3) { -1133.37f, 469.06f, -1849.47f });
    bg[383] = CreateRobo((Vector3) { -1112.69f, 469.57f, -1867.20f }); // orb
    bg[384] = CreateSkeleton((Vector3) { -1065.65f, 474.35f, -1877.14f });

    // new badguys: skeleton road toward cinderspire
    bg[385] = CreateSkeleton((Vector3) { -1045.05f, 478.12f, -1851.12f });
    bg[386] = CreateGhost((Vector3) { -1027.08f, 484.83f, -1824.01f });
    bg[387] = CreateSkeleton((Vector3) { -994.06f, 481.88f, -1864.50f });
    bg[388] = CreateSkeleton((Vector3) { -348.48f, 312.47f, -1862.74f });
    bg[389] = CreateRobo((Vector3) { -329.81f, 314.69f, -1913.85f }); // orb
    bg[390] = CreateSkeleton((Vector3) { -349.74f, 319.27f, -1981.27f });
    bg[391] = CreateGhost((Vector3) { -357.72f, 331.29f, -2072.10f });
    bg[392] = CreateSkeleton((Vector3) { -371.92f, 327.53f, -2138.06f });
    bg[393] = CreatePumpkinHopper((Vector3) { -400.95f, 323.76f, -2195.82f });
}

static inline bool BG_HornRecentlyActive(Donogan* d)
{
    if (!d || !d->gs) return false;

    return d->gs->HonkedHornRecently.running &&
        !HasTimerElapsed(&d->gs->HonkedHornRecently);
}

static inline void BG_KillGhostFromHorn(Donogan* d, BadGuy* b)
{
    if (!d || !b) return;
    if (!b->active || b->dead) return;
    if (b->type != BG_GHOST) return;
    if (b->gbm_index < 0) return;

    DustPuff_Spawn(b->pos);

    b->active = false;
    b->dead = true;
    b->aware = false;
    b->frozen = false;
    b->throwing = false;
    b->state = GHOST_STATE_DEATH;

    bgModelBorrower[b->gbm_index].isInUse = false;
    b->gbm_index = -1;

    StartTimer(&b->respawnTimer);
    ResetTimer(&b->interactionTimer);

    d->xp += 10;
    ghostKillCount++;
}

static inline void BG_UpdateAll(Donogan *d, float dt)
{
    for (int b = 0; b < act_bg_count; b++) {
        int i = act_bg[b];
        if (!bg[i].active) { continue; }
        if (bg[i].type == BG_MECH && bg[i].state < MECH_STATE_ACTIVE) { continue; }
        if (Vector3DistanceSqr(d->pos, bg[i].pos) > 800*800
            && bg[i].type != BG_ALISTER 
            && bg[i].type != BG_MECH) //general guard to help enforce that when don is far away, bad guys get put away
        {
            bg[i].active = false;
            bg[i].dead = true;
            bgModelBorrower[bg[i].gbm_index].isInUse = false;
            bg[i].gbm_index = -1;
            d->xp += 1; //give don a point for it I guess
            continue;
        }
        //ragdoll
        if (bg[i].truckHitCooldown > 0)
        {
            bg[i].truckHitCooldown -= dt;
            if (bg[i].truckHitCooldown < 0) bg[i].truckHitCooldown = 0;
        }

        if (bg[i].ragdoll)
        {
            BG_UpdateTruckRagdoll(&bg[i], dt);
            continue;
        }
        // Truck horn kills nearby ghosts.
        if (bg[i].type == BG_GHOST && BG_HornRecentlyActive(d))
        {
            const float HORN_GHOST_KILL_RADIUS = 90;

            if (Vector3DistanceSqr(d->pos, bg[i].pos) <
                HORN_GHOST_KILL_RADIUS * HORN_GHOST_KILL_RADIUS)
            {
                BG_KillGhostFromHorn(d, &bg[i]);
                continue;
            }
        }
        //handle square spell
        if (d->squareThrowRequest && bg[i].frozen)
        {
            if (!IsSoundPlaying(throwFade)) { PlaySoundVol(throwFade); }
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
            if (Vector3Distance(d->pos, bg[i].pos) < (9+d->level)) //freeze radius grows with level
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
                        bg[i].frozen = false; //they die with a pff
                        DustPuff_Spawn(bg[i].pos);
                    }
                }
                if (d->spellTimer.running)
                {
                    //make em raise up
                    bg[i].pos.y += dt / 100;
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
        else if (bg[i].type == BG_PUMPKIN_HOPPER)
        {
            BG_Update_PumpkinHopper(d, &bg[i], dt);
            if (bg[i].scale < bg[i].desiredScale)
            {
                bg[i].scale += 0.0123;
            }
        }
        else if (bg[i].type == BG_SKELETON)
        {
            BG_Update_Skeleton(d, &bg[i], dt);
        }
        else if (bg[i].type == BG_ALISTER)
        {
            BG_Update_Alister(d, &bg[i], dt);
        }
        else if (bg[i].type == BG_MECH)
        {
            BG_Update_Mech(d, &bg[i], dt);
        }
        if (bg[i].active && bg[i].gbm_index >= 0)
        {
            //update boxes and height hacks
            BG_UpdateMainBox(&bg[i]);
            if (bg[i].type == BG_YETI) {//im sick of these mfn snakes on this mfn plane!
                bg[i].box.max.y += 4.5;
                bg[i].box.min.y += 4;
            }
            else if (bg[i].type == BG_PUMPKIN_HOPPER)
            {
                bg[i].box.max.y += 2.25;
                bg[i].box.min.y += 1.8321;
            }
        }
        else
        {
            continue;
        }
    }
    ////handle don and timer for square spell
    //if (d->spellTimer.running)
    //{
    //    //d->cached_yawY = d->yawY;//this does not work....?
    //}
    if (HasTimerElapsed(&d->spellTimer))
    {
        d->mana -= 1;
        StartTimer(&d->spellTimer);
        for (int i = 0; i < numCloseProps; i++)//berries, should not be here but...this was easiest...?
        {
            if ((*CloseProps[i]).type != MODEL_TREE_2 && (*CloseProps[i]).type != MODEL_TREE) { continue; }
            if ((*CloseProps[i]).type == MODEL_TREE_2)
            {
                if (!(*CloseProps[i]).hasBerries && Vector3DistanceSqr((*CloseProps[i]).pos, d->pos) < 144)
                {
                    (*CloseProps[i]).hasBerries = true;
                    PlaySoundVol(grow);
                }
            }
            else if((*CloseProps[i]).type == MODEL_TREE)//tree regular, single apple
            {
                if ((*CloseProps[i]).type == MODEL_TREE && Vector3DistanceSqr((*CloseProps[i]).pos, d->pos) < 144) {
                    if (SpawnAppleOnTree(CloseProps[i], 4.0f, 8.0f)) {
                        PlaySoundVol(grow);
                    }
                    else
                    {
                        TraceLog(LOG_INFO, "SpawnAppleOnTree: no candidate verts in band");
                    }
                }
            }
        }
    }
    if (d->squareThrowRequest) { d->squareThrowRequest = false; }
}


//only activate one per call
bool CheckSpawnAndActivateNext(Vector3 pos, Donogan * d)
{
    for (int b = 0; b < bg_count; b++)
    {
        if (bg[b].active) { continue; }//if its turned on, dont turn it on again
        if (d->alisterDead && bg[b].type == BG_ALISTER) { continue; }
        else 
        {
            if (Vector3DistanceSqr(pos, bg[b].spawnPoint) < bg[b].spawnRadius * bg[b].spawnRadius && (!bg[b].respawnTimer.running || HasTimerElapsed(&bg[b].respawnTimer)))
            {
                for (int i = 0; i < MAX_BG_PER_TYPE_AT_ONCE; i++)
                {
                    int index = i + (bg[b].type * MAX_BG_PER_TYPE_AT_ONCE);
                    if (bgModelBorrower[index].isInUse) { continue; }
                    BG_ClearRuntimeState(&(bg[b])); //clear important stuff
                    bgModelBorrower[index].isInUse = true;
                    bg[b].gbm_index = index;
                    bg[b].active = true;
                    bg[b].dead = false;
                    bg[b].aware = false;
                    bg[b].health = bg[b].startHealth;
                    bg[b].pos = bg[b].spawnPoint;
                    bg[b].targetPos = bg[b].spawnPoint;
                    BG_AttachBorrowed(&bg[b]);
                    if (bg[b].type == BG_GHOST) 
                    { 
                        bg[b].pos.y = GetTerrainHeightFromMeshXZ(bg[b].pos.x, bg[b].pos.z) - 30;
                        bg[b].state = GHOST_STATE_SPAWN;
                        PlaySoundVol(spawnGhost);
                    }
                    else if (bg[b].type == BG_YETI) {
                        // snap to ground and start in SPAWN (will fall into PLANNING next update)
                        bg[b].pos.y = GetTerrainHeightFromMeshXZ(bg[b].pos.x, bg[b].pos.z);
                        bg[b].state = YETI_STATE_SPAWN;
                        BG_SetAnim(&bg[b], ANIM_YETI_ROAR, true); // default roar on spawn
                    }
                    else if (bg[b].type == BG_ROBO)
                    {
                        bg[b].state = ROBO_STATE_SPAWN;
                        PlaySoundVol(spawnBot);
                    }
                    else if (bg[b].type == BG_PUMPKIN_HOPPER)
                    {
                        bg[b].pos = bg[b].spawnPoint;
                        bg[b].targetPos = bg[b].pos;
                        bg[b].groundY = bg[b].pos.y;
                        bg[b].state = HOPPER_STATE_SLEEP;
                    }
                    else if (bg[b].type == BG_SKELETON)
                    {
                        float gy = GetTerrainHeightFromMeshXZ(bg[b].spawnPoint.x, bg[b].spawnPoint.z);
                        if (gy < -9000) gy = bg[b].spawnPoint.y;

                        bg[b].spawnPoint.y = gy;
                        bg[b].pos = bg[b].spawnPoint;
                        bg[b].targetPos = bg[b].spawnPoint;
                        bg[b].pos.y -= 2.1;
                        bg[b].vel = (Vector3){ 0 };
                        bg[b].yaw = 0;
                        bg[b].pitch = 0;
                        bg[b].roll = 0;
                        bg[b].state = SKELETON_STATE_RISE;
                        BG_SetAnimSafe(&bg[b], ANIM_SKEL_RISE, true);
                    }
                    else if (bg[b].type == BG_ALISTER && !d->alisterDead)
                    {
                        float gy = GetTerrainHeightFromMeshXZ(bg[b].spawnPoint.x, bg[b].spawnPoint.z);
                        if (gy < -9000) gy = bg[b].spawnPoint.y;

                        bg[b].spawnPoint.y = gy;
                        bg[b].pos = bg[b].spawnPoint;
                        bg[b].targetPos = bg[b].spawnPoint;
                        bg[b].pos.y += 3.7;
                        bg[b].vel = (Vector3){ 0 };
                        bg[b].yaw = 0;
                        bg[b].pitch = 0;
                        bg[b].roll = 0;
                        bg[b].state = ALISTER_STATE_IDLE;
                        BG_SetAnim(&bg[b], ALISTER_ANIM_IDLE, true);
                        aliPos = &bg[b].pos;
                        BG_UpdateMainBox(&bg[b]);
                    }
                    else if (bg[b].type == BG_MECH)
                    {
                        float gy = GetTerrainHeightFromMeshXZ(bg[b].spawnPoint.x, bg[b].spawnPoint.z);
                        if (gy < -9000) gy = bg[b].spawnPoint.y;

                        bg[b].spawnPoint.y = gy;
                        bg[b].pos = bg[b].spawnPoint;
                        bg[b].targetPos = bg[b].spawnPoint;
                        bg[b].vel = (Vector3){ 0 };
                        bg[b].yaw = 0;
                        bg[b].pitch = 90;
                        bg[b].roll = 0;
                        bg[b].state = MECH_STATE_IDLE;
                        mechPos = &bg[b].pos;
                        BG_UpdateMainBox(&bg[b]);
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

#endif // BG_H
