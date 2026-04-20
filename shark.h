#ifndef SHARK_H
#define SHARK_H

#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <math.h>

#include "texture.h"
#include "donogan.h"

// =============================
// CONFIG
// =============================
#define SHARK_MIN_WATER_DEPTH   20.0f
#define SHARK_SURFACE_CLEARANCE 0.6f
#define SHARK_BOTTOM_CLEARANCE  1.0f

#define SHARK_EAT_DISTANCE      20.0f
#define SHARK_ATTACK_DISTANCE   180.0f

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

} Shark;

// =============================
// HELPERS
// =============================
static float SharkDepthAtXZ(const Shark* s, float x, float z)
{
    float seabed = GetTerrainHeightFromMeshXZ(x, z);
    return s->surfaceY - seabed;
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
    s->stalkSpeed = 12.4f;

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

    s->attackSpeed = 64.0f;
    s->steerTimer = 0.0f;

    s->leg= LoadModel("models/leg.obj");
    s->legText = LoadMyTexture("textures/leg.png");

    if (s->legText.id)
    {
        SetMaterialTexture(&s->leg.materials[0], MATERIAL_MAP_ALBEDO, s->legText);
    }
}

static bool LoadShark(Shark* s)
{
    s->model = LoadModel("models/shark.glb");
    s->tex = LoadMyTexture("textures/shark.png");

    if (s->tex.id)
    {
        SetMaterialTexture(&s->model.materials[0], MATERIAL_MAP_ALBEDO, s->tex);
    }

    s->origBox = ScaleBoundingBox(GetModelBoundingBox(s->model), 0.8f);
    s->box = UpdateBoundingBox(s->origBox, s->pos);

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
    const float anglesDeg[] = { 0, 20, -20, 40, -40, 65, -65, 90, -90 };
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
    float minY = seabed + s->bottomClearance;
    float maxY = s->surfaceY - s->surfaceClearance;
    best.y = Clamp(s->pos.y, minY, maxY);

    return best;
}
// =============================
// UPDATE
// =============================
static void Shark_Update(Shark* s, Donogan* d, float dt)
{
    s->stateTime += dt;
    float seabed = GetTerrainHeightFromMeshXZ(s->pos.x, s->pos.z);
    // =============================
    // DEPTH CHECK (XZ restriction)
    // =============================
    if ((s->surfaceY - seabed) < s->minWaterDepth)
    {
        s->goal = s->home;
        s->state = SHARK_STATE_WANDER;
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

        if (Vector3Distance(s->pos, s->goal) < 5.0f || s->stateTime > 6.0f)
        {
            s->goal = SharkPickGoal(s);
            s->stateTime = 0.0f;
        }

        if (canHunt)
        {
            s->state = SHARK_STATE_STALK;
            s->stateTime = 0.0f;
        }
    }
    else if (s->state == SHARK_STATE_STALK)
    {
        Vector3 toDon = Vector3Subtract(d->pos, s->pos);
        toDon.y = 0.0f; // keep chase calmer in yaw
        float dist = Vector3Length(toDon);

        if (dist > 0.01f)
        {
            s->steerTimer += dt;
            if (s->steerTimer > 0.25f)
            {
                //*s->goal = Shark_PickSteerGoalTowardTarget(s, d->pos);
                s->steerTimer = 0.0f;
                Vector3 steer = Shark_PickSteerGoalTowardTarget(s, d->pos);
                if (Vector3Distance(steer, s->home) < 1.0f)
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

        if (dist < SHARK_ATTACK_DISTANCE)
        {
            s->state = SHARK_STATE_ATTACK;
            s->stateTime = 0.0f;
            s->hasEaten = false;

            // lock a lunge target straight through Donny
            Vector3 toDon = Vector3Subtract(d->pos, s->pos);
            toDon.y = 0.0f;
            if (Vector3Length(toDon) > 0.01f)
            {
                Vector3 attackDir = Vector3Normalize(toDon);
                s->goal = Vector3Add(d->pos, Vector3Scale(attackDir, 28.0f));
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
        s->box = UpdateBoundingBox(s->origBox, s->pos);

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

            if (!d->eatenByShark)
            {
                d->eatenByShark = true;
                StartTimer(&d->eatenTimer);
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
    s->box = UpdateBoundingBox(s->origBox, s->pos);
}

// =============================
// DRAW
// =============================
static void Shark_Draw(Shark* s, Donogan *d)
{
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
    if (d->eatenByShark)
    {
        DrawModel(s->leg,d->pos,1,WHITE);
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
}

#endif