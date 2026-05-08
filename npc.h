#ifndef NPC_H
#define NPC_H

// --- npc.h additions ---
#include "donogan.h"   // so we can accept Donogan* in update/draw
// Includes
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h> 
#include <stdbool.h>
#include "texture.h"

// Type Definitions
typedef enum {
    NPC_MODEL_TYPE_DARREL,
    NPC_MODEL_TYPE_CHICKEN, //darrel is the name of the model + the name of the first character that we are using it with...
    NPC_MODEL_TYPE_LUCY,
    NPC_MODEL_TYPE_WIZARD,
    NPC_MODEL_TYPE_ABBY,
    NPC_MODEL_TYPE_CLERK,
    NPC_MODEL_TYPE_GALADRIEL,
    NPC_MODEL_TYPE_ROGER,
    NPC_MODEL_TYPE_GEOFF,
    NPC_MODEL_TYPE_MARY,
    NPC_MODEL_TYPE_JARED,
} NPC_Model_Type;

typedef enum {
    NPC_DARREL = 0, //a few npc's will use the darrel model, but only one darrel record
    NPC_CHICKEN,
    NPC_LUCY,
    NPC_NICK, //NPC_MODEL_TYPE_DARREL - mark repeats this way for yourself
    NPC_WIZARD,
    NPC_ABBY,
    NPC_CLERK,
    NPC_CLERK_2,
    NPC_CLERK_3,
    NPC_CLERK_4,
    NPC_CLERK_5,
    NPC_GALADRIEL,
    NPC_ROGER,
    NPC_GEOFF,
    NPC_MARY,
    NPC_JARED,
    NPC_TOTAL
} NPC_Type;

typedef enum {
    RESCUE_STATE_SCARED = 0,
    RESCUE_STATE_RUN,
    RESCUE_STATE_SAFE,
} RescueState;

typedef enum {
    CHICKEN_STATE_PLAN = 0,
    CHICKEN_STATE_WALK,
    CHICKEN_STATE_FOLLOW,
} ChickenState;

typedef enum {
    DARREL_STATE_HELLO = 0,
    DARREL_STATE_RUN,
    DARREL_STATE_CONFUSED,
    DARREL_STATE_TALK,
    DARREL_STATE_WALKING,
} DarrelState; 
//darrel is simple, so animations indexes match states and we use state for animation index, many npc's will be simple and can do it this way
//we dont care about shared models with shared animations, I will place the similar models far apart and cull on distance for update and draw

typedef enum {
    LUCY_STATE_HELLO = 0,
    LUCY_STATE_TALK,
    LUCY_STATE_RUN,
    LUCY_STATE_WALK,
    LUCY_STATE_JOG,
} LucyState;

typedef enum {
    WIZARD_STATE_HELLO = 0,
    WIZARD_STATE_TALK,
    WIZARD_STATE_FLY,
    WIZARD_STATE_DONE
} WizardState;

typedef enum {
    ABBY_STATE_HELLO = 0,
    ABBY_STATE_TALK
} AbbyState;

typedef enum {
    GAL_STATE_HELLO = 0,
} GaladrielState;

typedef struct {
    NPC_Type type;
    NPC_Model_Type modelType;
    int state;

    // === NEW: simple, general per-BG animation control
    int    curAnim;        // which animation index is playing
    float  animFrame;      // current frame within that animation, this is a float now...!
    float  animFPS;        // playback speed (frames/sec)
    ModelAnimation* anims; // shared pointer to per-type animations
    int    animCount;      // number of animations for this BG
    Model model;
    Texture tex;

    Vector3 pos;
    Vector3 tether;
    float yaw, pitch, roll;
    float scale;
    BoundingBox box, origBox;
    //
    Vector3 vel;
    float   speed, targetSpeed, minSpeed, maxSpeed, accel;
    float   targetYaw;
    Vector3 targetPos;

    //rescue missions stuff
    bool isRescue;
    RescueState r_state;
    int cartIndex;
} NPC;

NPC npcs[NPC_TOTAL];
#define NUM_CARTS 5
Model cartModel;
Texture cartTexture;
Vector3 cartPositions[NUM_CARTS];

// Optional: tiny helpers
static inline float NPC_GroundY(Vector3 p) {
    float g = GetTerrainHeightFromMeshXZ(p.x, p.z);
    return (g < -9000.0f) ? p.y : g;
}
static inline float WrapAngleNpc(float a) {
    while (a > PI)  a -= 2.0f * PI;
    while (a < -PI) a += 2.0f * PI;
    return a;
}
static inline float TurnToward(float cur, float target, float maxStep) {
    float e = WrapAngleNpc(target - cur);
    if (e > maxStep) e = maxStep;
    if (e < -maxStep) e = -maxStep;
    return cur + e;
}

// --- General animation helpers (reusable) ---
static inline void NPC_AnimSet(NPC* n, int animIndex, bool forceReset, float fps) {
    if (!n || !n->anims || n->animCount <= 0) return;
    animIndex = (animIndex % n->animCount + n->animCount) % n->animCount;
    if (forceReset || n->curAnim != animIndex) {
        n->curAnim = animIndex;
        n->animFPS = (fps > 0.0f) ? fps : n->animFPS;
        n->animFrame = 0;
    }
}

//init the stuff
void InitAllNPC()
{
    //darrel
    Model darrel_model = LoadModel("models/darrel.glb");
    Texture darrel_tex = LoadMyTexture("textures/darrel.png");
    int darrel_animCount = 0;
    ModelAnimation * darrel_anims = LoadModelAnimations("models/darrel.glb", &darrel_animCount);
    //chicken
    Model chicken_model = LoadModel("models/chicken_run.glb");
    Texture chicken_tex = LoadMyTexture("textures/chicken.png");
    int chicken_animCount = 0;
    ModelAnimation* chicken_anims = LoadModelAnimations("models/chicken_run.glb", &chicken_animCount);
    //lucy
    Model lucy_model = LoadModel("models/lucy.glb");
    Texture lucy_tex = LoadMyTexture("textures/lucy.png");
    int lucy_animCount = 0;
    ModelAnimation* lucy_anims = LoadModelAnimations("models/lucy.glb", &lucy_animCount);
    //wizard
    Model wiz_model = LoadModel("models/wizard.obj");
    Texture wiz_tex = LoadMyTexture("textures/wizard.png");
    //lucy
    Model abby_model = LoadModel("models/abby.obj");
    Texture abby_tex = LoadMyTexture("textures/abby.png");
    int abby_animCount = 0;
    ModelAnimation* abby_anims = LoadModelAnimations("models/abby_anim.glb", &abby_animCount);
    //clerk and cart
    Model clerk_model = LoadModel("models/clerk.obj");
    Texture clerk_tex = LoadMyTexture("textures/clerk.png");
    cartModel = LoadModel("models/store.obj");
    cartTexture = LoadMyTexture("textures/store.png");
    cartModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = cartTexture;
    //galadriel
    Model gal_model = LoadModel("models/galadriel.obj");
    Texture gal_tex = LoadMyTexture("textures/galadriel.png");
    //3 last guys for now
    Model roger_model = LoadModel("models/roger.obj");
    Texture roger_tex = LoadMyTexture("textures/roger.png");
    Model geoff_model = LoadModel("models/geoff.obj");
    Texture geoff_tex = LoadMyTexture("textures/geoff.png");
    Model mary_model = LoadModel("models/mary.obj");
    Texture mary_tex = LoadMyTexture("textures/mary.png");
    //jared
    Model jared_model = LoadModel("models/jared.obj");
    Texture jared_tex = LoadMyTexture("textures/jared.png");
    //setup darrel
    npcs[NPC_DARREL].type = NPC_DARREL;
    npcs[NPC_DARREL].modelType = NPC_MODEL_TYPE_DARREL;
    npcs[NPC_DARREL].model = darrel_model; //models with animations have to have a unique model instance in raylib, otherwise they all display the same animation at the same time
    npcs[NPC_DARREL].tex = darrel_tex;
    npcs[NPC_DARREL].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = darrel_tex;
    npcs[NPC_DARREL].anims = darrel_anims;
    npcs[NPC_DARREL].animCount = darrel_animCount;
    npcs[NPC_DARREL].pos = (Vector3){ -346.16f, 496, -24.73f }; //test pos = (Vector3){ 3022.00f, 322.00f, 4042.42f };
    npcs[NPC_DARREL].targetPos = npcs[NPC_DARREL].pos;
    npcs[NPC_DARREL].scale = 3.8f;
    npcs[NPC_DARREL].yaw = 0.0f;
    npcs[NPC_DARREL].state = DARREL_STATE_CONFUSED;
    npcs[NPC_DARREL].curAnim = npcs[NPC_DARREL].state;
    npcs[NPC_DARREL].animFPS = 24.0f;
    npcs[NPC_DARREL].animFrame = 0.0f;
    NPC_AnimSet(&npcs[NPC_DARREL], npcs[NPC_DARREL].curAnim, true, npcs[NPC_DARREL].animFPS); // start correct clip
    //setup chicken
    npcs[NPC_CHICKEN].type = NPC_CHICKEN;
    npcs[NPC_CHICKEN].modelType = NPC_MODEL_TYPE_CHICKEN;
    npcs[NPC_CHICKEN].model = chicken_model;
    npcs[NPC_CHICKEN].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = chicken_tex;
    npcs[NPC_CHICKEN].anims = chicken_anims;
    npcs[NPC_CHICKEN].animCount = chicken_animCount;
    npcs[NPC_CHICKEN].pos = (Vector3){ 1887, 361, 4737 };
    npcs[NPC_CHICKEN].targetPos = npcs[NPC_CHICKEN].pos;
    npcs[NPC_CHICKEN].tether = npcs[NPC_CHICKEN].pos; //the chicken in regular non follow state is tethered so it doesnt wander too much
    npcs[NPC_CHICKEN].scale = 1.23f;
    npcs[NPC_CHICKEN].speed = 0.2f;
    npcs[NPC_CHICKEN].yaw = 0.0f;
    npcs[NPC_CHICKEN].state = CHICKEN_STATE_PLAN;
    npcs[NPC_CHICKEN].curAnim = 0; //only walk for the chicken
    npcs[NPC_CHICKEN].animFPS = 48.0f;
    npcs[NPC_CHICKEN].animFrame = 0.0f;
    NPC_AnimSet(&npcs[NPC_CHICKEN], npcs[NPC_CHICKEN].curAnim, true, npcs[NPC_CHICKEN].animFPS); // start correct clip
    //setup lucy
    npcs[NPC_LUCY].type = NPC_LUCY;
    npcs[NPC_LUCY].modelType = NPC_MODEL_TYPE_LUCY;
    npcs[NPC_LUCY].model = lucy_model;
    npcs[NPC_LUCY].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = lucy_tex;
    npcs[NPC_LUCY].anims = lucy_anims;
    npcs[NPC_LUCY].animCount = lucy_animCount;
    npcs[NPC_LUCY].pos = (Vector3){ 1886, 361, 4201 };
    npcs[NPC_LUCY].targetPos = npcs[NPC_LUCY].pos;
    npcs[NPC_LUCY].scale = 3.50f;
    npcs[NPC_LUCY].yaw = 0.0f;
    npcs[NPC_LUCY].state = LUCY_STATE_HELLO;
    npcs[NPC_LUCY].curAnim = 0; //only walk for the chicken
    npcs[NPC_LUCY].animFPS = 60.0f;
    npcs[NPC_LUCY].animFrame = 0.0f;
    NPC_AnimSet(&npcs[NPC_LUCY], npcs[NPC_LUCY].curAnim, true, npcs[NPC_LUCY].animFPS); // start correct clip
    //setup nick
    npcs[NPC_NICK].type = NPC_NICK;
    npcs[NPC_NICK].modelType = NPC_MODEL_TYPE_DARREL; //repeat for rescue mission
    npcs[NPC_NICK].model = darrel_model; //models with animations have to have a unique model instance in raylib, otherwise they all display the same animation at the same time
    npcs[NPC_NICK].tex = darrel_tex;
    npcs[NPC_NICK].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = darrel_tex;
    npcs[NPC_NICK].anims = darrel_anims;
    npcs[NPC_NICK].animCount = darrel_animCount;
    npcs[NPC_NICK].pos = (Vector3){ 2280.08, 320.00, -892.53 }; //test pos = (Vector3){ 3022.00f, 322.00f, 4042.42f }; //rescued at 2846.52, 323.76, -615.60
    npcs[NPC_NICK].targetPos = npcs[NPC_NICK].pos;
    npcs[NPC_NICK].scale = 3.8f;
    npcs[NPC_NICK].yaw = 0.0f;
    npcs[NPC_NICK].speed = 18.0f;
    npcs[NPC_NICK].isRescue = TRUE;
    npcs[NPC_NICK].r_state = RESCUE_STATE_SCARED;
    npcs[NPC_NICK].state = DARREL_STATE_CONFUSED;
    npcs[NPC_NICK].curAnim = npcs[NPC_NICK].state;
    npcs[NPC_NICK].animFPS = 24.0f;
    npcs[NPC_NICK].animFrame = 0.0f;
    NPC_AnimSet(&npcs[NPC_NICK], npcs[NPC_NICK].curAnim, true, npcs[NPC_NICK].animFPS); // start correct clip
    //setup wizard
    npcs[NPC_WIZARD].type = NPC_WIZARD;
    npcs[NPC_WIZARD].modelType = NPC_MODEL_TYPE_WIZARD;
    npcs[NPC_WIZARD].model = wiz_model; 
    npcs[NPC_WIZARD].tex = wiz_tex;
    npcs[NPC_WIZARD].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = wiz_tex;
    npcs[NPC_WIZARD].animCount = 0;
    npcs[NPC_WIZARD].pos = (Vector3){ 3020, 320, 4030 }; //test pos = (Vector3){ 3022.00f, 322.00f, 4042.42f };
    npcs[NPC_WIZARD].targetPos = npcs[NPC_WIZARD].pos;
    npcs[NPC_WIZARD].scale = 3.9f;
    npcs[NPC_WIZARD].yaw = 0.0f;
    npcs[NPC_WIZARD].speed = 6.0f;
    npcs[NPC_WIZARD].isRescue = false;
    npcs[NPC_WIZARD].state = WIZARD_STATE_HELLO;
    npcs[NPC_WIZARD].curAnim = 0;
    npcs[NPC_WIZARD].animFPS = 0;
    npcs[NPC_WIZARD].animFrame = 0.0f;
    //setup abby
    npcs[NPC_ABBY].type = NPC_ABBY;
    npcs[NPC_ABBY].modelType = NPC_MODEL_TYPE_ABBY;
    npcs[NPC_ABBY].model = abby_model;
    npcs[NPC_ABBY].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = abby_tex;
    npcs[NPC_ABBY].anims = abby_anims;
    npcs[NPC_ABBY].animCount = abby_animCount;
    npcs[NPC_ABBY].pos = (Vector3){ 2241.41, 335.06, -3347.53 };
    npcs[NPC_ABBY].targetPos = npcs[NPC_ABBY].pos;
    npcs[NPC_ABBY].scale = 3.50f;
    npcs[NPC_ABBY].yaw = 0.0f;
    npcs[NPC_ABBY].state = ABBY_STATE_HELLO;
    npcs[NPC_ABBY].curAnim = 0; //only walk for the chicken
    npcs[NPC_ABBY].animFPS = 60.0f;
    npcs[NPC_ABBY].animFrame = 0.0f;
    NPC_AnimSet(&npcs[NPC_ABBY], npcs[NPC_ABBY].curAnim, true, npcs[NPC_ABBY].animFPS); // start correct clip
    //setup clerk and cart
    npcs[NPC_CLERK].type = NPC_CLERK;
    npcs[NPC_CLERK].modelType = NPC_MODEL_TYPE_CLERK;
    npcs[NPC_CLERK].model = clerk_model;
    npcs[NPC_CLERK].tex = clerk_tex;
    npcs[NPC_CLERK].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = clerk_tex;
    npcs[NPC_CLERK].animCount = 0;
    npcs[NPC_CLERK].pos = (Vector3){ 2503.90, 338.82, 2335.35 };
    npcs[NPC_CLERK].targetPos = npcs[NPC_CLERK].pos;
    npcs[NPC_CLERK].scale = 3.9f;
    npcs[NPC_CLERK].yaw = 0.0f;
    npcs[NPC_CLERK].speed = 1.0f;
    npcs[NPC_CLERK].isRescue = false;
    npcs[NPC_CLERK].state = 0; //does not matter for this guy
    npcs[NPC_CLERK].curAnim = 0;
    npcs[NPC_CLERK].animFPS = 0;
    npcs[NPC_CLERK].animFrame = 0.0f;
    cartPositions[0] = (Vector3){ 2503.49, 344.2, 2343.99 };
    npcs[NPC_CLERK].cartIndex = 0;
    npcs[NPC_CLERK_2] = npcs[NPC_CLERK];
    npcs[NPC_CLERK_3] = npcs[NPC_CLERK];
    npcs[NPC_CLERK_4] = npcs[NPC_CLERK];
    npcs[NPC_CLERK_5] = npcs[NPC_CLERK];
    npcs[NPC_CLERK_2].cartIndex = 1;
    npcs[NPC_CLERK_3].cartIndex = 2;
    npcs[NPC_CLERK_4].cartIndex = 3;
    npcs[NPC_CLERK_5].cartIndex = 4;
    npcs[NPC_CLERK_2].pos = (Vector3){ -3882.33, 308.71, 3802.42 };
    cartPositions[1] = (Vector3){ -3887.88, 313.68, 3786.54 };
    npcs[NPC_CLERK_3].pos = (Vector3){ -1269.04, 382.81, 637.69 };
    cartPositions[2] = (Vector3){ -1288.39, 385.24, 641.19 };
    npcs[NPC_CLERK_4].pos = (Vector3){ -2520.94, 409.11, -2373.29 };
    cartPositions[3] = (Vector3){ -2528.19, 409.58, -2408.55 };
    npcs[NPC_CLERK_5].pos = (Vector3){ 1876.92, 384.00, -3060.90 };
    cartPositions[4] = (Vector3){ 1857.41, 398.71, -3082.70 };
    //setup galadriel
    npcs[NPC_GALADRIEL].type = NPC_GALADRIEL;
    npcs[NPC_GALADRIEL].modelType = NPC_MODEL_TYPE_GALADRIEL;
    npcs[NPC_GALADRIEL].model = gal_model;
    npcs[NPC_GALADRIEL].tex = gal_tex;
    npcs[NPC_GALADRIEL].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = gal_tex;
    npcs[NPC_GALADRIEL].animCount = 0;
    npcs[NPC_GALADRIEL].pos = (Vector3){ -396, 334, -4007 }; //test pos = (Vector3){ 3022.00f, 322.00f, 4042.42f };
    npcs[NPC_GALADRIEL].targetPos = npcs[NPC_GALADRIEL].pos;
    npcs[NPC_GALADRIEL].scale = 2.67f;
    npcs[NPC_GALADRIEL].yaw = 0.0f;
    npcs[NPC_GALADRIEL].speed = 6.0f;
    npcs[NPC_GALADRIEL].isRescue = false;
    npcs[NPC_GALADRIEL].state = WIZARD_STATE_HELLO;
    npcs[NPC_GALADRIEL].curAnim = 0;
    npcs[NPC_GALADRIEL].animFPS = 0;
    npcs[NPC_GALADRIEL].animFrame = 0.0f;
    //setup roger
    npcs[NPC_ROGER].type = NPC_ROGER;
    npcs[NPC_ROGER].modelType = NPC_MODEL_TYPE_ROGER;
    npcs[NPC_ROGER].model = roger_model;
    npcs[NPC_ROGER].tex = roger_tex;
    npcs[NPC_ROGER].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = roger_tex;
    npcs[NPC_ROGER].animCount = 0;
    npcs[NPC_ROGER].pos = (Vector3){ 2578.58, 339.06, 2320.00 }; //test pos = (Vector3){ 3022.00f, 322.00f, 4042.42f };
    npcs[NPC_ROGER].targetPos = npcs[NPC_ROGER].pos;
    npcs[NPC_ROGER].scale = 3.2;
    npcs[NPC_ROGER].yaw = 0.0f;
    npcs[NPC_ROGER].speed = 6.0f;
    npcs[NPC_ROGER].isRescue = false;
    npcs[NPC_ROGER].state = WIZARD_STATE_HELLO;
    npcs[NPC_ROGER].curAnim = 0;
    npcs[NPC_ROGER].animFPS = 0;
    npcs[NPC_ROGER].animFrame = 0.0f;
    //setup geoff
    npcs[NPC_GEOFF].type = NPC_GEOFF;
    npcs[NPC_GEOFF].modelType = NPC_MODEL_TYPE_GEOFF;
    npcs[NPC_GEOFF].model = geoff_model;
    npcs[NPC_GEOFF].tex = geoff_tex;
    npcs[NPC_GEOFF].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = geoff_tex;
    npcs[NPC_GEOFF].animCount = 0;
    npcs[NPC_GEOFF].pos = (Vector3){ -2603.73, 403, -2456.85 }; //test pos = (Vector3){ 3022.00f, 322.00f, 4042.42f };
    npcs[NPC_GEOFF].targetPos = npcs[NPC_GEOFF].pos;
    npcs[NPC_GEOFF].scale = 3.2;
    npcs[NPC_GEOFF].yaw = 0.0f;
    npcs[NPC_GEOFF].speed = 6.0f;
    npcs[NPC_GEOFF].isRescue = false;
    npcs[NPC_GEOFF].state = WIZARD_STATE_HELLO;
    npcs[NPC_GEOFF].curAnim = 0;
    npcs[NPC_GEOFF].animFPS = 0;
    npcs[NPC_GEOFF].animFrame = 0.0f;
    //setup mary
    npcs[NPC_MARY].type = NPC_MARY;
    npcs[NPC_MARY].modelType = NPC_MODEL_TYPE_MARY;
    npcs[NPC_MARY].model = mary_model;
    npcs[NPC_MARY].tex = mary_tex;
    npcs[NPC_MARY].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = mary_tex;
    npcs[NPC_MARY].animCount = 0;
    npcs[NPC_MARY].pos = (Vector3){ -2401.56, 448.24, -2137.88 }; //test pos = (Vector3){ 3022.00f, 322.00f, 4042.42f };
    npcs[NPC_MARY].targetPos = npcs[NPC_MARY].pos;
    npcs[NPC_MARY].scale = 3.2;
    npcs[NPC_MARY].yaw = 0.0f;
    npcs[NPC_MARY].speed = 6.0f;
    npcs[NPC_MARY].isRescue = false;
    npcs[NPC_MARY].state = WIZARD_STATE_HELLO;
    npcs[NPC_MARY].curAnim = 0;
    npcs[NPC_MARY].animFPS = 0;
    npcs[NPC_MARY].animFrame = 0.0f;
    //setup jared
    npcs[NPC_JARED].type = NPC_JARED;
    npcs[NPC_JARED].modelType = NPC_MODEL_TYPE_JARED;
    npcs[NPC_JARED].model = jared_model;
    npcs[NPC_JARED].tex = jared_tex;
    npcs[NPC_JARED].model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = jared_tex;
    npcs[NPC_JARED].pos = (Vector3){ -2401.56, 448.24, -2137.88 };
    npcs[NPC_JARED].scale = 3.2;
    npcs[NPC_JARED].isRescue = false;

}

bool IsModelAnimationValidMe(Model model, ModelAnimation anim)
{
    int result = true;

    if (model.skeleton.boneCount != anim.boneCount)
    {
        result = false;
    }
    return result;
}
// was: static inline void NPC_AnimTick(NPC* n, float dt)
static inline bool NPC_AnimTick(NPC* n, float dt) {
    if (!n || !n->anims || n->animCount <= 0) return false;
    ModelAnimation* a = &n->anims[n->curAnim];
    if (a->keyframeCount <= 0) return false;

    float prev = n->animFrame;
    n->animFrame += n->animFPS * dt;

    bool looped = false;
    if (n->animFrame >= a->keyframeCount) {
        n->animFrame = fmodf(n->animFrame, (float)a->keyframeCount);
        looped = true;
    }
    if (IsModelAnimationValidMe(n->model, *a)) {
        UpdateModelAnimation(n->model, *a, (int)n->animFrame);
    }
    return looped;
}


// --- Case-specific handler for Darrel ---
static inline void NPC_Update_Simple(NPC* n, const Donogan* d, float dt, bool looped) 
{
    if (n->modelType == NPC_MODEL_TYPE_CLERK) { n->pos.y += 3.68; }
    else if (n->type == NPC_DARREL) { n->pos.y -= 0.2f; }
    else if (n->modelType == NPC_MODEL_TYPE_ABBY) { n->pos.y += 3; }
    else if (n->modelType == NPC_MODEL_TYPE_GALADRIEL) { n->pos.y += 2.5; }
    else if (n->modelType == NPC_MODEL_TYPE_ROGER) { n->pos.y += 3.33; }
    else if (n->modelType == NPC_MODEL_TYPE_GEOFF) { n->pos.y += 3.33; }
    else if (n->modelType == NPC_MODEL_TYPE_MARY) { n->pos.y += 3.33; }
    // Face Donogan
    float targetYaw = atan2f(d->pos.x - n->pos.x, d->pos.z - n->pos.z);
    n->yaw = TurnToward(n->yaw, targetYaw, dt * 6.0f); // gentle turn rate
    ModelAnimation* a = &n->anims[n->curAnim]; //todo: I think I can get rid of this...
    if (looped)
    {
        n->curAnim = n->state;
        NPC_AnimSet(n, n->curAnim, true, n->animFPS);
    }
}

static inline void NPC_Update_Wiz(NPC* n, const Donogan* d, float dt)
{
    if (n->state == WIZARD_STATE_FLY)
    {
        n->pos = Vector3Lerp(n->pos, n->targetPos, dt * 0.25f);

        float targetYaw = atan2f(n->targetPos.x - n->pos.x, n->targetPos.z - n->pos.z);
        n->yaw = TurnToward(n->yaw, targetYaw, dt * 6.0f);

        if (Vector3DistanceSqr(n->pos, d->pos) > 600*600.0f)
        {
            n->state = WIZARD_STATE_DONE;
        }

        return;
    }

    float targetYaw = atan2f(d->pos.x - n->pos.x, d->pos.z - n->pos.z);
    n->yaw = TurnToward(n->yaw, targetYaw, dt * 6.0f);
}

static inline void NPC_Update_Rescue(NPC* n, const Donogan* d, float dt, bool looped)
{
    if (n->r_state == RESCUE_STATE_RUN)
    {
        // Face target
        float targetYaw = atan2f(n->targetPos.x - n->pos.x, n->targetPos.z - n->pos.z);
        n->yaw = TurnToward(n->yaw, targetYaw, dt * 6.0f); // gentle turn rate
        //smooth speed move
        Vector3 d = { n->targetPos.x - n->pos.x, 0.0f, n->targetPos.z - n->pos.z };
        float len = sqrtf(d.x * d.x + d.z * d.z);
        if (len > 0.0001f) {
            float step = fminf(n->speed * dt, len) / len; // normalize & clamp
            n->pos.x += d.x * step;
            n->pos.z += d.z * step;
        }
        //set ground for y
        n->pos.y = NPC_GroundY(n->pos);
        if (Vector3DistanceSqr(n->pos, n->targetPos) < 64.0f)
        {
            n->r_state = RESCUE_STATE_SAFE;
            if (n->type == NPC_NICK)
            {
                n->state = DARREL_STATE_HELLO;
            }
        }
    }
    else
    {
        float targetYaw = atan2f(d->pos.x - n->pos.x, d->pos.z - n->pos.z);
        n->yaw = TurnToward(n->yaw, targetYaw, dt * 6.0f); // gentle turn rate
    }
    
    if (looped)
    {
        n->curAnim = n->state;
        NPC_AnimSet(n, n->curAnim, true, n->animFPS);
    }
}

//cases for chicken
static inline void NPC_Update_Chicken(NPC* n, const Donogan* d, float dt, bool looped) 
{
    //handle states
    if (n->state == CHICKEN_STATE_PLAN)
    {
        float r = (float)GetRandomValue(2, 7);
        float a = (float)GetRandomValue(0, 359) * DEG2RAD;
        n-> targetPos = (Vector3) { n->tether.x + sinf(a) * r, n->pos.y, n->tether.z + cosf(a) * r };
        //needs to be on relativly flat ground so we identify when we are close to the target
        n->state = CHICKEN_STATE_WALK;
    }
    else if (n->state == CHICKEN_STATE_WALK) 
    {
        if (Vector3DistanceSqr(n->pos, n->targetPos) < 2.4*2.4)
        {
            n->state = CHICKEN_STATE_PLAN;
        }
    }
    else if (n->state == CHICKEN_STATE_FOLLOW) 
    {
        n->targetPos = d->pos;
        n->targetPos.x += 5;
        n->targetPos.z += 4;
    }
    else { return; } //not a valid state, dont update the chicken...
    //lerp target pos
    n->pos = Vector3Lerp(n->pos, n->targetPos, dt*n->speed);
    // Face Target and adjust after lerp for ground again
    float targetYaw = atan2f(n->targetPos.x - n->pos.x, n->targetPos.z - n->pos.z);
    n->yaw = TurnToward(n->yaw, targetYaw, dt * 6.0f); // gentle turn rate
    n->pos.y = NPC_GroundY(n->pos);
}

// --- General per-NPC update entry point ---
static inline void NPC_Update(NPC* n, const Donogan* d, float dt) 
{
    if (!n || !d) return;

    // Distance cull (skip everything if too far)
    float dist = Vector3Distance(n->pos, d->pos);
    float cutoff = 600.0f; //was 1000
    if (dist > cutoff) return;

    //put them on the ground always
    if (n->type != NPC_WIZARD)
    {
        n->pos.y = NPC_GroundY(n->pos);
    }
    // Tick animation & refresh box
    bool looped = NPC_AnimTick(n, dt);
    // Case dispatch
    switch (n->type) {
    case NPC_DARREL: NPC_Update_Simple(n, d, dt, looped); break;
    case NPC_CHICKEN: NPC_Update_Chicken(n, d, dt, looped); break;
    case NPC_LUCY: NPC_Update_Simple(n, d, dt, looped); break;
    case NPC_NICK: NPC_Update_Rescue(n, d, dt, looped); break;
    case NPC_WIZARD: NPC_Update_Wiz(n,d,dt); break;
    case NPC_ABBY: NPC_Update_Simple(n, d, dt, looped); break;
    case NPC_CLERK: NPC_Update_Simple(n, d, dt, looped); break;
    case NPC_GALADRIEL: NPC_Update_Simple(n, d, dt, looped); break;
    case NPC_ROGER: NPC_Update_Simple(n, d, dt, looped); break;
    case NPC_GEOFF: NPC_Update_Simple(n, d, dt, looped); break;
    case NPC_MARY: NPC_Update_Simple(n, d, dt, looped); break;
    default: break; //jared
    }
    //n->box = UpdateBoundingBox(n->origBox, n->pos);
}

// --- Minimal draw (preview will do the frustum check before calling this) ---
static inline void NPC_Draw(const NPC* n) 
{
    if (!n) return;
    DrawModelEx(n->model, n->pos, (Vector3) { 0, 1, 0 }, RAD2DEG* n->yaw,
        (Vector3) {
        n->scale, n->scale, n->scale
    }, WHITE);
    // Optionally: DrawBoundingBox(n->box, YELLOW);
    if (n->modelType == NPC_MODEL_TYPE_CLERK)
    {
        DrawModel(cartModel,cartPositions[n->cartIndex],7,WHITE);
    }
}


#endif // NPC_H
