#ifndef SHARK_H
#define SHARK_H

#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <math.h>

#include "texture.h"
#include "donogan.h"
#include "core.h"

// =============================
// CONFIG
// =============================
#define SHARK_MIN_WATER_DEPTH   20.0f
#define SHARK_SURFACE_CLEARANCE 0.6f
#define SHARK_BOTTOM_CLEARANCE  1.0f

#define SHARK_EAT_DISTANCE      20.0f
#define SHARK_ATTACK_DISTANCE   180.0f

#define SHARK_FAR_REPATH_DIST   6000.0f
#define SHARK_RING_RADIUS       6500.0f
#define SHARK_RING_REACHED_DIST 320.0f

// =============================
// STATE
// =============================
typedef enum {
    SHARK_STATE_WANDER = 0,
    SHARK_STATE_STALK,
    SHARK_STATE_ATTACK,
    SHARK_STATE_EAT
} SharkState;

// =============================
// SHARK STRUCT
// =============================
typedef struct {
    Model model;
    Texture2D tex;

    Vector3 pos;
    Vector3 goal;

    float yaw;
    float speed;

    float wanderSpeed;
    float stalkSpeed;

    float homeRadius;
    Vector3 home;

    float stateTime;
    SharkState state;

    float minWaterDepth;
    float surfaceClearance;
    float bottomClearance;

    float surfaceY; // ✅ FIXED: actual water surface

    float scale;

    // eat effect
    bool hasEaten;
    float eatTimer;

    float attackSpeed;
    BoundingBox origBox;
    BoundingBox box;
    float steerTimer;

    Model leg;
    Texture legText;
    Model bloodModel;
    Shader bloodShader;
    int bloodTimeLoc;
    int bloodVariantLoc;

    bool usingFarRing;
    Vector3 farRingGoal;

    ModelAnimation proc;
    int boneBodyMid;
    int boneBodyRear;
    int boneTailBase;
    int boneTailTip;
} Shark;


// =============================
// HELPERS
// =============================
//anim
static void SharkPoseResetToBind(const Model* m, ModelAnimation* p)
{
    for (int b = 0; b < p->boneCount; b++)
    {
        p->keyframePoses[0][b] = m->skeleton.bindPose[b];
    }
}

static void SharkSetFromBindPlusEuler(const Model* m, ModelAnimation* p, int idx, float ex, float ey, float ez)
{
    if (idx < 0) return;

    Quaternion dq = QuaternionFromEuler(ex, ey, ez);
    p->keyframePoses[0][idx].rotation = QuaternionMultiply(dq, m->skeleton.bindPose[idx].rotation);
}
//other
static float SharkDepthAtXZ(const Shark* s, float x, float z)
{
    float seabed = GetTerrainHeightFromMeshXZ(x, z);
    return s->surfaceY - seabed;
}
static Vector3 SharkForwardFromYaw(float yawDeg)
{
    float r = DEG2RAD * yawDeg;
    return (Vector3) { sinf(r), 0.0f, cosf(r) };
}
static Vector3 SharkPickGoal(const Shark* s)
{
    Vector3 best = s->home;
    bool found = false;
    float bestScore = -999999.0f;

    for (int i = 0; i < 32; i++)
    {
        float r = GetRandomValue(20, (int)s->homeRadius);
        float a = GetRandomValue(0, 360) * DEG2RAD;

        Vector3 p = {
            s->home.x + sinf(a) * r,
            0.0f,
            s->home.z + cosf(a) * r
        };

        float seabed = GetTerrainHeightFromMeshXZ(p.x, p.z);
        float depth = s->surfaceY - seabed;

        if (depth < s->minWaterDepth) continue;

        float minY = seabed + s->bottomClearance;
        float maxY = s->surfaceY - s->surfaceClearance;
        if (maxY <= minY) continue;

        p.y = GetRandomValue((int)minY, (int)maxY);

        float distFromHome = Vector3Distance(
            (Vector3) {
            p.x, 0, p.z
        },
            (Vector3) {
            s->home.x, 0, s->home.z
        });

        float score = depth * 2.0f + distFromHome * 0.25f;

        if (!found || score > bestScore)
        {
            best = p;
            bestScore = score;
            found = true;
        }
    }

    return found ? best : s->home;
}

static bool Shark_IsWaterDeepEnough(const Shark* s, float x, float z)
{
    float seabed = GetTerrainHeightFromMeshXZ(x, z);
    if (seabed < -9000.0f) return false;
    return ((s->surfaceY - seabed) >= s->minWaterDepth);
}

static Vector3 Shark_PickOuterRingGoalTowardTarget(const Shark* s, Vector3 target)
{
    // Use target direction from world center (0,0 in XZ) so we pick a ring point
    Vector3 flat = { target.x, 0.0f, target.z };
    float len = sqrtf(flat.x * flat.x + flat.z * flat.z);

    if (len < 0.001f) len = 1.0f;

    Vector3 dir = { flat.x / len, 0.0f, flat.z / len };

    // Try straight toward Donny first, then slight offsets around the ring
    const float offsetsDeg[] = { 0, 18, -18, 36, -36, 54, -54, 72, -72, 90, -90 };
    const int count = (int)(sizeof(offsetsDeg) / sizeof(offsetsDeg[0]));

    for (int i = 0; i < count; i++)
    {
        float a = DEG2RAD * offsetsDeg[i];
        float cs = cosf(a);
        float sn = sinf(a);

        Vector3 d = {
            dir.x * cs - dir.z * sn,
            0.0f,
            dir.x * sn + dir.z * cs
        };

        Vector3 p = {
            d.x * SHARK_RING_RADIUS,
            0.0f,
            d.z * SHARK_RING_RADIUS
        };

        if (!Shark_IsWaterDeepEnough(s, p.x, p.z)) continue;

        float seabed = GetTerrainHeightFromMeshXZ(p.x, p.z);
        float minY = seabed + s->bottomClearance;
        float maxY = s->surfaceY - s->surfaceClearance;
        p.y = Clamp(s->pos.y, minY, maxY);
        return p;
    }

    return s->home;
}
// =============================
// INIT
// =============================
static void InitShark(Shark* s, Vector3 home, float surfaceY)
{
    s->home = home;
    s->pos = home;
    s->goal = home;

    s->yaw = 0.0f;
    s->speed = 0.0f;

    s->wanderSpeed = 3.4f;
    s->stalkSpeed = 32.4f;

    s->homeRadius = 120.0f;

    s->state = SHARK_STATE_WANDER;
    s->stateTime = 0.0f;

    s->minWaterDepth = SHARK_MIN_WATER_DEPTH;
    s->surfaceClearance = SHARK_SURFACE_CLEARANCE;
    s->bottomClearance = SHARK_BOTTOM_CLEARANCE;

    s->surfaceY = surfaceY;

    s->scale = 1.6f;

    s->hasEaten = false;
    s->eatTimer = 0.0f;

    s->attackSpeed = 80.0f;
    s->steerTimer = 0.0f;

    s->leg= LoadModel("models/leg.obj");
    s->legText = LoadMyTexture("textures/leg.png");

    if (s->legText.id)
    {
        SetMaterialTexture(&s->leg.materials[0], MATERIAL_MAP_ALBEDO, s->legText);
    }

    s->boneBodyMid = 13; // spine.005
    s->boneBodyRear = 12; // spine.004
    s->boneTailBase = 2;  // spine.001
    s->boneTailTip = 3;  // spine
}

static bool LoadShark(Shark* s)
{
    s->model = LoadModel("models/shark.glb");
    s->tex = LoadMyTexture("textures/shark.png");

    if (s->tex.id)
    {
        SetMaterialTexture(&s->model.materials[0], MATERIAL_MAP_ALBEDO, s->tex);
    }

    s->origBox = (BoundingBox){ (Vector3) {-6,-6,-6}, (Vector3) {6,6,6} };//ScaleBoundingBox(GetModelBoundingBox(s->model), 0.8f);
    Vector3 fwd = (Vector3){ sinf(DEG2RAD * s->yaw), 0.0f, cosf(DEG2RAD * s->yaw) };
    Vector3 nosePos = Vector3Add(s->pos, Vector3Scale(fwd, 10.0f)); // tweak 10.0f to match your shark length
    s->box = UpdateBoundingBox(s->origBox, nosePos);

    s->bloodShader = LoadShader("shaders/120/blood.vs", "shaders/120/blood.fs");
    s->bloodShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(s->bloodShader, "mvp");
    s->bloodShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(s->bloodShader, "matModel");

    s->bloodTimeLoc = GetShaderLocation(s->bloodShader, "uTime");
    s->bloodVariantLoc = GetShaderLocation(s->bloodShader, "uVariant");

    Mesh bloodMesh = GenMeshSphere(0.5f, 20, 20);
    s->bloodModel = LoadModelFromMesh(bloodMesh);
    s->bloodModel.materials[0].shader = s->bloodShader;
    s->bloodModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = (Color){ 180, 20, 20, 170 };

    s->proc.boneCount = s->model.skeleton.boneCount;
    //s->proc.bones = s->model.skeleton.bones; //raylib 6
    s->proc.keyframeCount = 1;
    s->proc.keyframePoses = MemAlloc(sizeof(Transform*) * 1);
    s->proc.keyframePoses[0] = MemAlloc(sizeof(Transform) * s->proc.boneCount);

    for (int b = 0; b < s->proc.boneCount; b++)
    {
        s->proc.keyframePoses[0][b] = s->model.skeleton.bindPose[b];
    }
    //PrintModelBones(&s->model); //return false to see it.
    return true;
}

static float SharkAngleWrapDeg(float a)
{
    while (a > 180.0f) a -= 360.0f;
    while (a < -180.0f) a += 360.0f;
    return a;
}
static Vector3 Shark_PickSteerGoalTowardTarget(const Shark* s, Vector3 target)
{
    Vector3 toTarget = Vector3Subtract(target, s->pos);
    toTarget.y = 0.0f;

    if (Vector3Length(toTarget) < 0.01f) return s->pos;

    Vector3 forward = Vector3Normalize(toTarget);

    // probe distance ahead
    const float probeDist = 36.0f;

    // candidate turn angles in degrees, ordered by preference
    const float anglesDeg[] = { 120, -120, 90, -90, 20, -20, 40, -40, 65, -65, 0 };
    const int count = (int)(sizeof(anglesDeg) / sizeof(anglesDeg[0]));

    Vector3 best = s->pos;
    bool found = false;
    float bestScore = -999999.0f;

    for (int i = 0; i < count; i++)
    {
        float a = DEG2RAD * anglesDeg[i];
        float cs = cosf(a);
        float sn = sinf(a);

        Vector3 dir = {
            forward.x * cs - forward.z * sn,
            0.0f,
            forward.x * sn + forward.z * cs
        };

        Vector3 p = Vector3Add(s->pos, Vector3Scale(dir, probeDist));

        if (!Shark_IsWaterDeepEnough(s, p.x, p.z)) continue;

        // score: prefer directions that still point toward target
        Vector3 toP = Vector3Subtract(p, s->pos);
        toP.y = 0.0f;
        toP = Vector3Normalize(toP);

        float score = Vector3DotProduct(toP, forward);

        if (!found || score > bestScore)
        {
            best = p;
            bestScore = score;
            found = true;
        }
    }

    if (!found)
    {
        // no valid steer found: fall back to home
        return s->home;
    }

    // choose a legal Y for that X/Z
    float seabed = GetTerrainHeightFromMeshXZ(best.x, best.z);
    float minY = seabed + 7 + s->bottomClearance;
    float maxY = s->surfaceY - s->surfaceClearance;
    best.y = Clamp(s->pos.y, minY, maxY);

    return best;
}
// =============================
// UPDATE
// =============================
static void Shark_Update(Shark* s, Donogan* d, float dt)
{
    if (s->usingFarRing)
    {
        s->usingFarRing = false;
        s->state = SHARK_STATE_STALK;
        s->stateTime = 0.0f;
    }
    else if (Vector3DistanceSqr(s->pos, d->pos) > SHARK_FAR_REPATH_DIST * SHARK_FAR_REPATH_DIST)
    {
        s->usingFarRing = true;
        s->farRingGoal = Shark_PickOuterRingGoalTowardTarget(s, d->pos);
        s->goal = s->farRingGoal;
        s->pos = s->goal;
        s->state = SHARK_STATE_STALK;
        s->stateTime = 0.0f;
        return;
    }
    s->stateTime += dt;
    float seabed = GetTerrainHeightFromMeshXZ(s->pos.x, s->pos.z);
    // =============================
    // DEPTH CHECK (XZ restriction)
    // =============================
    if ((s->surfaceY - seabed) < s->minWaterDepth)
    {
        if (s->state == SHARK_STATE_STALK || s->state == SHARK_STATE_ATTACK)
        {
            if (Vector3DistanceSqr(s->pos, d->pos) > SHARK_FAR_REPATH_DIST * SHARK_FAR_REPATH_DIST)
            { 
                s->usingFarRing = true; 
                s->farRingGoal = Shark_PickOuterRingGoalTowardTarget(s, d->pos);
                s->goal = s->farRingGoal;
                s->pos = s->goal;
                s->state = SHARK_STATE_STALK;
                s->stateTime = 0.0f;
                s->usingFarRing = false;
            }
            else
            {
                s->goal = s->home;
                s->state = SHARK_STATE_WANDER;
                s->stateTime = 0.0f;
                s->steerTimer = 0.0f;
            }
        }
        else
        {
            s->goal = s->home;
            s->state = SHARK_STATE_WANDER;
            s->stateTime = 0.0f;
            s->steerTimer = 0.0f;
        }
    }

    bool canHunt = d->inWater &&
        ((d->waterY - d->seabedY) >= s->minWaterDepth) &&
        !d->eatenByShark;

    // =============================
    // STATE MACHINE
    // =============================
    if (s->state == SHARK_STATE_WANDER)
    {
        s->speed = s->wanderSpeed;

        if (Vector3DistanceSqr(s->pos, s->goal) < 25.0f || s->stateTime > 6.0f)
        {
            s->goal = SharkPickGoal(s);
            s->stateTime = 0.0f;
        }

        if (canHunt)
        {
            s->state = SHARK_STATE_STALK;
            s->stateTime = 0.0f;
            s->usingFarRing = false;
            s->steerTimer = 0.0f;
            s->goal = Shark_PickSteerGoalTowardTarget(s, d->pos);
        }
    }
    else if (s->state == SHARK_STATE_STALK)
    {
        Vector3 toDon = Vector3Subtract(d->pos, s->pos);
        toDon.y = 0.0f; // keep chase calmer in yaw
        float dist = Vector3Length(toDon);
        float worldDistToDon = Vector3DistanceSqr(s->pos, d->pos);

        if (worldDistToDon > SHARK_FAR_REPATH_DIST * SHARK_FAR_REPATH_DIST && !s->usingFarRing && !Shark_IsWaterDeepEnough(s, s->pos.x, s->pos.z))
        {
            if (Vector3DistanceSqr(s->pos, d->pos) > SHARK_FAR_REPATH_DIST * SHARK_FAR_REPATH_DIST) { s->usingFarRing = true; }
            s->farRingGoal = Shark_PickOuterRingGoalTowardTarget(s, d->pos);
            s->goal = s->farRingGoal;
            s->pos = s->goal;
            s->speed = 128;
        }
        if (s->usingFarRing)
        {
            s->goal = s->farRingGoal;
            s->pos = s->goal;

            if (Vector3DistanceSqr(s->pos, s->farRingGoal) < SHARK_RING_REACHED_DIST * SHARK_RING_REACHED_DIST)
            {
                s->usingFarRing = false;
                s->steerTimer = 0.0f;
                s->stateTime = 0.0f;   // optional but helpful
                s->goal = Shark_PickSteerGoalTowardTarget(s, d->pos);   // immediate re-acquire
            }

            s->speed = s->stalkSpeed;
        }
        else
        {
            if (dist > 0.01f)
            {
                s->steerTimer += dt;
                if (s->steerTimer > 0.25f)
                {
                    //*s->goal = Shark_PickSteerGoalTowardTarget(s, d->pos);
                    s->steerTimer = 0.0f;
                    Vector3 steer = Shark_PickSteerGoalTowardTarget(s, d->pos);
                    if (Vector3DistanceSqr(steer, s->home) < 1.0f) //todo: Vector3Distance-finder 
                    {
                        s->state = SHARK_STATE_WANDER;
                        s->stateTime = 0.0f;
                        s->goal = SharkPickGoal(s);
                    }
                    else
                    {
                        s->goal = steer;
                    }
                }
                s->speed = s->stalkSpeed;
            }
        }
        
        if (dist < SHARK_ATTACK_DISTANCE)
        {
            s->state = SHARK_STATE_ATTACK;
            s->stateTime = 0.0f;
            s->hasEaten = false;
            float r = s->yaw;
            Vector3 fwd = (Vector3){ sinf(r), 0.0f, cosf(r) };
            Vector3 toDon = Vector3Subtract(d->pos, Vector3Add(s->pos, Vector3Scale(fwd, 8)));
            toDon.y = 0.0f;

            if (Vector3Length(toDon) > 0.01f)
            {
                Vector3 attackDir = Vector3Normalize(toDon);
                s->goal = Vector3Add(d->pos, Vector3Scale(attackDir, 12.0f));
                s->goal.y = d->pos.y;
            }
        }

        if (!canHunt)
        {
            s->state = SHARK_STATE_WANDER;
            s->stateTime = 0.0f;
            s->goal = SharkPickGoal(s);
        }
    }
    else if (s->state == SHARK_STATE_ATTACK)
    {
        s->speed = s->attackSpeed;

        // keep the lunge mostly flat and direct
        Vector3 flatToGoal = Vector3Subtract(s->goal, s->pos);
        flatToGoal.y = 0.0f;

        // update box before testing
        Vector3 fwd = (Vector3){ sinf(DEG2RAD * s->yaw), 0.0f, cosf(DEG2RAD * s->yaw) };
        Vector3 nosePos = Vector3Add(s->pos, Vector3Scale(fwd, 10.0f)); // tweak 10.0f to match your shark length
        s->box = UpdateBoundingBox(s->origBox, nosePos);

        // if we hit Donny, eat him
        if (!d->eatenByShark && CheckCollisionBoxes(s->box, d->outerBox))
        {
            s->state = SHARK_STATE_EAT;
            s->stateTime = 0.0f;
            s->eatTimer = 0.0f;
            s->hasEaten = false;
        }
        else if (Vector3Length(flatToGoal) < 6.0f || s->stateTime > 1.5f)
        {
            // missed the attack, go home / wander again
            s->state = SHARK_STATE_WANDER;
            s->stateTime = 0.0f;
            s->goal = SharkPickGoal(s);
        }
    }
    else if (s->state == SHARK_STATE_EAT)
    {
        // stop the shark from trying to chase through Donny
        s->speed = 16.0f;

        if (!s->hasEaten)
        {
            s->hasEaten = true;
            PlaySoundVol(sharkGulp);
            if (!d->eatenByShark)
            {
                d->eatenByShark = true;
                StartTimer(&d->eatenTimer);
                s->goal = SharkPickGoal(s);
            }
        }

        s->eatTimer += dt;

        if (s->eatTimer > 0.35f)
        {
            s->state = SHARK_STATE_WANDER;
            s->stateTime = 0.0f;
            s->goal = SharkPickGoal(s);
            s->eatTimer = 0.0f;
        }
    }

    // =============================
    // MOVEMENT
    // =============================
    Vector3 dir = Vector3Subtract(s->goal, s->pos);

    if (Vector3Length(dir) > 0.01f)
    {
        dir = Vector3Normalize(dir);
        s->pos = Vector3Add(s->pos, Vector3Scale(dir, s->speed * dt));
        float targetYaw = atan2f(dir.x, dir.z) * RAD2DEG;
        float yawDiff = SharkAngleWrapDeg(targetYaw - s->yaw);
        float maxTurn = 90.0f * dt; // deg/sec
        if (yawDiff > maxTurn) yawDiff = maxTurn;
        if (yawDiff < -maxTurn) yawDiff = -maxTurn;
        s->yaw += yawDiff;
    }

    // clamp vertically ONLY (not XZ)
    float minY = seabed + s->bottomClearance;
    float maxY = s->surfaceY - s->surfaceClearance;

    s->pos.y = Clamp(s->pos.y, minY, maxY);
    Vector3 fwd = (Vector3){ sinf(DEG2RAD * s->yaw), 0.0f, cosf(DEG2RAD * s->yaw) };
    Vector3 nosePos = Vector3Add(s->pos, Vector3Scale(fwd, 10.0f)); // tweak 10.0f to match your shark length
    s->box = UpdateBoundingBox(s->origBox, nosePos);
    
    //anims
    float t = (float)GetTime();

    float ampBody = 0.0f;
    float ampTail = 0.0f;
    float freq = 0.0f;

    if (s->state == SHARK_STATE_WANDER)
    {
        ampBody = DEG2RAD * 2.0f;
        ampTail = DEG2RAD * 5.0f;
        freq = 2.0f;
    }
    else if (s->state == SHARK_STATE_STALK)
    {
        ampBody = DEG2RAD * 4.0f;
        ampTail = DEG2RAD * 10.0f;
        freq = 3.0f;
    }
    else if (s->state == SHARK_STATE_ATTACK)
    {
        ampBody = DEG2RAD * 6.0f;
        ampTail = DEG2RAD * 14.0f;
        freq = 5.0f;
    }
    else
    {
        ampBody = DEG2RAD * 3.0f;
        ampTail = DEG2RAD * 8.0f;
        freq = 2.0f;
    }

    float bodyRearYaw = sinf(t * freq + 0.1f) * ampBody;
    float bodyMidYaw = sinf(t * freq + 0.25f) * ampBody * 1.12f;
    float tailBaseYaw = sinf(t * freq + 0.50f) * ampTail;
    float tailTipYaw = sinf(t * freq + 0.75f) * ampTail * 1.3f;

    SharkPoseResetToBind(&s->model, &s->proc);

    // side-to-side motion = yaw around local Y
    SharkSetFromBindPlusEuler(&s->model, &s->proc, s->boneBodyRear, 0.0f, bodyRearYaw, 0.0f);
    SharkSetFromBindPlusEuler(&s->model, &s->proc, s->boneBodyMid, 0.0f, bodyMidYaw, 0.0f);
    SharkSetFromBindPlusEuler(&s->model, &s->proc, s->boneTailBase, 0.0f, tailBaseYaw, 0.0f);
    SharkSetFromBindPlusEuler(&s->model, &s->proc, s->boneTailTip, 0.0f, tailTipYaw, 0.0f);//yes?
    SharkSetFromBindPlusEuler(&s->model, &s->proc, 4, 0.0f, tailTipYaw, 0.0f);//yes?
    SharkSetFromBindPlusEuler(&s->model, &s->proc, 5, 0.0f, tailTipYaw, 0.0f);//yes?
    SharkSetFromBindPlusEuler(&s->model, &s->proc, 6, 0.0f, tailTipYaw, 0.0f);//yes?
    SharkSetFromBindPlusEuler(&s->model, &s->proc, 7, 0.0f, tailTipYaw, 0.0f);//yes?
    SharkSetFromBindPlusEuler(&s->model, &s->proc, 8, 0.0f, tailTipYaw, 0.0f);//yes?

    UpdateModelAnimation(s->model, s->proc, 0);
}

// =============================
// DRAW
// =============================
static void Shark_Draw(Shark* s, Donogan *d)
{
    if (Vector3Distance(s->pos, d->pos) > 1024) { return; }
    DrawModelEx(
        s->model,
        s->pos,
        (Vector3) {
        0, 1, 0
        },
            s->yaw,
            (Vector3) {
            s->scale, s->scale, s->scale
        },
        WHITE
    );
    //DrawBoundingBox(s->box, RED); //todo: remove
}
static void Shark_Draw_Extremities(Shark* s, Donogan* d)
{
    if (d->eatenByShark)
    {
        Vector3 fwd = SharkForwardFromYaw(s->yaw);
        Vector3 left = (Vector3){ -fwd.z, 0.0f, fwd.x };

        Vector3 legPos = d->pos;
        legPos.y += 4;
        /*legPos = Vector3Add(legPos, Vector3Scale(left, 3.0f));
        legPos = Vector3Add(legPos, Vector3Scale(fwd, 6.0f));*/

        float t = (float)GetTime();
        float bob = sinf(t * 2.4f) * 0.35f;
        float sink = s->eatTimer * 1.5f;

        legPos.y = d->pos.y - 0.15f + bob - sink;
        if (legPos.y > (s->surfaceY - 5)) { legPos.y = s->surfaceY - 5; }

        DrawModelEx(
            s->leg,
            legPos,
            (Vector3) {
            0, 1, 0
        },
            s->yaw + 90.0f,
            (Vector3) {
            1, 1, 1
        },
            WHITE
        );

        //draw the blood
        float time = (float)GetTime();
        SetShaderValue(s->bloodShader, s->bloodTimeLoc, &time, SHADER_UNIFORM_FLOAT);

        Vector3 bloodBase = legPos;
        bloodBase.y += 0.06f;   // top of the leg, tweak this

        BeginBlendMode(BLEND_ADDITIVE);
        rlDisableDepthMask();

        for (int i = 0; i < 12; i++)
        {
            float fi = (float)i;
            SetShaderValue(s->bloodShader, s->bloodVariantLoc, &fi, SHADER_UNIFORM_FLOAT);

            float a = time * (0.7f + 0.13f * fi) + fi * 0.8f;
            float rad = 0.45f + 0.22f * sinf(time * 1.7f + fi * 1.3f);

            Vector3 p = bloodBase;
            p.x += cosf(a) * (0.25f + 0.05f * fi);
            p.z += sinf(a) * (0.30f + 0.12f * fi);
            p.y += 0.15f * fi + sinf(time * 2.2f + fi * 1.9f) * 0.18f;

            float scale = 0.35f + 0.08f * sinf(time * 2.8f + fi);

            DrawModelEx(
                s->bloodModel,
                p,
                (Vector3) {
                0, 1, 0
            },
                fi * 37.0f + time * 30.0f,
                (Vector3) {
                scale, scale * 1.25f, scale
            },
                (Color) {
                180, 20, 20, 255
            }
            );
        }

        rlEnableDepthMask();
        EndBlendMode();
    }
}

// =============================
// CLEANUP
// =============================
static void FreeShark(Shark* s)
{
    UnloadModel(s->model);
    UnloadTexture(s->tex);
    UnloadModel(s->leg);
    UnloadTexture(s->legText);
    UnloadModel(s->bloodModel);
    UnloadShader(s->bloodShader);
    if (s->proc.keyframePoses)
    {
        if (s->proc.keyframePoses[0]) MemFree(s->proc.keyframePoses[0]);
        MemFree(s->proc.keyframePoses);
        s->proc.keyframePoses = NULL;
    }
}

#endif