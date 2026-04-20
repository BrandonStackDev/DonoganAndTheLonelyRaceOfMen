#ifndef SHARK_H
#define SHARK_H

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "texture.h"
#include "donogan.h"
#include "collision.h"
#include "timer.h"

// -----------------------------------------------------------------------------
// shark.h
// One procedural shark that wanders in deep water, slowly tracks Donogan while
// he is swimming, and "eats" him if it gets close enough.
//
// Notes:
// - No imported animations are used.
// - Bone posing is procedural, similar in spirit to whale.h.
// - Bone lookup is name-based so it can survive Blender/Rigify export changes.
// - Tail swings LEFT/RIGHT (yaw-ish), not up/down like the whale.
// -----------------------------------------------------------------------------

#ifndef SHARK_KILL_DISTANCE
#define SHARK_KILL_DISTANCE 200.0f
#endif

#ifndef SHARK_MIN_WATER_DEPTH
#define SHARK_MIN_WATER_DEPTH 20.0f
#endif

#ifndef SHARK_SURFACE_CLEARANCE
#define SHARK_SURFACE_CLEARANCE 1.25f
#endif

#ifndef SHARK_BOTTOM_CLEARANCE
#define SHARK_BOTTOM_CLEARANCE 3.5f
#endif

#ifndef SHARK_HIDE_VICTIM_TIME
#define SHARK_HIDE_VICTIM_TIME 1.0f
#endif

typedef enum SharkState {
    SHARK_STATE_WANDER = 0,
    SHARK_STATE_STALK,
    SHARK_STATE_BITE,
    SHARK_STATE_RECOVER
} SharkState;

typedef struct SharkBones {
    int root;
    int spine001;
    int spine002;
    int spine003;
    int chest;
    int neck;
    int head;
    int jaw;
    int tailBase;
    int tailMid;
    int tailTip;
    int finBack;
} SharkBones;

typedef struct Shark {
    Model model;
    Texture2D tex;
    ModelAnimation proc;   // one-frame procedural pose

    Vector3 pos;
    Vector3 home;
    Vector3 goal;
    Vector3 vel;

    float yaw;             // degrees
    float pitch;           // degrees
    float roll;            // degrees
    float speed;           // current scalar swim speed
    float scale;

    float xFixDeg;         // exporter/model local-axis fix
    float yFixPos;         // optional draw offset

    float homeRadius;
    float wanderSpeed;
    float stalkSpeed;
    float turnRateYaw;
    float turnRatePitch;
    float arriveRadius;

    float minWaterDepth;
    float surfaceClearance;
    float bottomClearance;

    float stateTime;
    float animTime;
    SharkState state;

    Timer planTimer;
    Timer biteRecoverTimer;
    Timer victimHideTimer;
    bool victimHidden;

    SharkBones bones;
} Shark;

// -----------------------------------------------------------------------------
// Small helpers
// -----------------------------------------------------------------------------
static inline float Shark_Clamp01(float x) { return (x < 0.0f) ? 0.0f : (x > 1.0f ? 1.0f : x); }
static inline float Shark_WrapDeg(float a) { while (a > 180.0f) a -= 360.0f; while (a < -180.0f) a += 360.0f; return a; }
static inline float Shark_LerpDeg(float a, float b, float k) { float d = Shark_WrapDeg(b - a); return a + d * k; }
static inline float Shark_Frand(float a, float b) { return a + (b - a) * ((float)GetRandomValue(0, 1000000) / 1000000.0f); }

static inline void Shark_PoseResetToBind(const Model* m, ModelAnimation* p)
{
    if (!m || !p || !p->framePoses || !p->framePoses[0]) return;
    for (int b = 0; b < p->boneCount; ++b) p->framePoses[0][b] = m->bindPose[b];
}

static inline void Shark_SetFromBindPlusEuler(const Model* m, ModelAnimation* p, int idx, float ex, float ey, float ez)
{
    if (!m || !p || idx < 0 || idx >= p->boneCount) return;
    Quaternion dq = QuaternionFromEuler(ex, ey, ez);
    p->framePoses[0][idx].rotation = QuaternionMultiply(dq, m->bindPose[idx].rotation);
}

static inline int Shark_FindBoneByExactName(const Model* m, const char* name)
{
    if (!m || !name) return -1;
    for (int i = 0; i < m->boneCount; ++i) {
        if (strcmp(m->bones[i].name, name) == 0) return i;
    }
    return -1;
}

static inline int Shark_FindBoneFirstMatch(const Model* m, const char** names, int count)
{
    if (!m || !names) return -1;
    for (int n = 0; n < count; ++n) {
        int idx = Shark_FindBoneByExactName(m, names[n]);
        if (idx >= 0) return idx;
    }
    return -1;
}

static inline void Shark_CacheBones(Shark* s)
{
    static const char* kRoot[]     = { "root", "Root", "rig", "Armature" };
    static const char* kSp1[]      = { "DEF-spine", "DEF-spine001", "spine", "spine.001", "spine001" };
    static const char* kSp2[]      = { "DEF-spine001", "DEF-spine002", "spine.002", "spine002" };
    static const char* kSp3[]      = { "DEF-spine002", "DEF-spine003", "spine.003", "spine003", "chest" };
    static const char* kChest[]    = { "DEF-spine003", "DEF-chest", "chest", "Chest" };
    static const char* kNeck[]     = { "DEF-neck", "neck", "Neck" };
    static const char* kHead[]     = { "DEF-head", "head", "Head" };
    static const char* kJaw[]      = { "DEF-jaw", "jaw", "Jaw", "mouth", "Mouth" };
    static const char* kTailBase[] = { "DEF-tail.001", "DEF-tail001", "tail.001", "tail001", "tail_base", "tail_base.001" };
    static const char* kTailMid[]  = { "DEF-tail.002", "DEF-tail002", "tail.002", "tail002", "tail_mid" };
    static const char* kTailTip[]  = { "DEF-tail.003", "DEF-tail003", "tail.003", "tail003", "tail_tip", "tail" };
    static const char* kFinBack[]  = { "DEF-fin_back", "fin_back", "back_fin", "dorsal", "dorsal_fin" };

    s->bones.root     = Shark_FindBoneFirstMatch(&s->model, kRoot,     (int)(sizeof(kRoot)     / sizeof(kRoot[0])));
    s->bones.spine001 = Shark_FindBoneFirstMatch(&s->model, kSp1,      (int)(sizeof(kSp1)      / sizeof(kSp1[0])));
    s->bones.spine002 = Shark_FindBoneFirstMatch(&s->model, kSp2,      (int)(sizeof(kSp2)      / sizeof(kSp2[0])));
    s->bones.spine003 = Shark_FindBoneFirstMatch(&s->model, kSp3,      (int)(sizeof(kSp3)      / sizeof(kSp3[0])));
    s->bones.chest    = Shark_FindBoneFirstMatch(&s->model, kChest,    (int)(sizeof(kChest)    / sizeof(kChest[0])));
    s->bones.neck     = Shark_FindBoneFirstMatch(&s->model, kNeck,     (int)(sizeof(kNeck)     / sizeof(kNeck[0])));
    s->bones.head     = Shark_FindBoneFirstMatch(&s->model, kHead,     (int)(sizeof(kHead)     / sizeof(kHead[0])));
    s->bones.jaw      = Shark_FindBoneFirstMatch(&s->model, kJaw,      (int)(sizeof(kJaw)      / sizeof(kJaw[0])));
    s->bones.tailBase = Shark_FindBoneFirstMatch(&s->model, kTailBase, (int)(sizeof(kTailBase) / sizeof(kTailBase[0])));
    s->bones.tailMid  = Shark_FindBoneFirstMatch(&s->model, kTailMid,  (int)(sizeof(kTailMid)  / sizeof(kTailMid[0])));
    s->bones.tailTip  = Shark_FindBoneFirstMatch(&s->model, kTailTip,  (int)(sizeof(kTailTip)  / sizeof(kTailTip[0])));
    s->bones.finBack  = Shark_FindBoneFirstMatch(&s->model, kFinBack,  (int)(sizeof(kFinBack)  / sizeof(kFinBack[0])));

    TraceLog(LOG_INFO,
        "Shark bones: root=%d sp1=%d sp2=%d sp3=%d chest=%d neck=%d head=%d jaw=%d tailA=%d tailB=%d tailC=%d finBack=%d",
        s->bones.root, s->bones.spine001, s->bones.spine002, s->bones.spine003,
        s->bones.chest, s->bones.neck, s->bones.head, s->bones.jaw,
        s->bones.tailBase, s->bones.tailMid, s->bones.tailTip, s->bones.finBack);
}

static inline Quaternion Shark_BuildWorldQuat(const Shark* s)
{
    Quaternion qFix   = QuaternionFromAxisAngle((Vector3){ 1, 0, 0 }, DEG2RAD * s->xFixDeg);
    Quaternion qYaw   = QuaternionFromAxisAngle((Vector3){ 0, 1, 0 }, DEG2RAD * s->yaw);
    Quaternion qPitch = QuaternionFromAxisAngle((Vector3){ 1, 0, 0 }, DEG2RAD * s->pitch);
    Quaternion qRoll  = QuaternionFromAxisAngle((Vector3){ 0, 0, 1 }, DEG2RAD * s->roll);
    Quaternion qWorld = QuaternionMultiply(QuaternionMultiply(qYaw, qPitch), qRoll);
    return QuaternionMultiply(qWorld, qFix);
}

static inline void Shark_Draw(const Shark* s)
{
    if (!s) return;

    Quaternion q = Shark_BuildWorldQuat(s);
    Matrix R = QuaternionToMatrix(q);
    Matrix T = MatrixTranslate(s->pos.x, s->pos.y + s->yFixPos, s->pos.z);
    Matrix S = MatrixScale(s->scale, s->scale, s->scale);
    Matrix world = MatrixMultiply(S, MatrixMultiply(R, T));

    rlPushMatrix();
    rlMultMatrixf(MatrixToFloatV(world).v);
    DrawModel(s->model, (Vector3){ 0, 0, 0 }, 1.0f, WHITE);
    rlPopMatrix();
}

static inline bool Shark_WaterDeepEnoughAtXZ(float x, float z, float surfaceY, float minDepth)
{
    float seabed = GetTerrainHeightFromMeshXZ(x, z);
    if (seabed < -9000.0f) return false;
    return (surfaceY - seabed) >= minDepth;
}

static inline float Shark_ClampYToWater(const Shark* s, float y, float seabedY)
{
    float minY = seabedY + s->bottomClearance;
    float maxY = s->home.y; // fallback if surface bad
    maxY = maxY < (seabedY + s->bottomClearance + 0.25f) ? (seabedY + s->bottomClearance + 0.25f) : maxY;
    maxY = fminf(maxY, s->home.y + 1000.0f);
    maxY = fminf(maxY, s->home.y + 1000.0f); // harmless guard
    return Clamp(y, minY, s->home.y + 1000.0f);
}

static inline Vector3 Shark_PickWanderGoal(const Shark* s)
{
    Vector3 g = s->home;

    for (int tries = 0; tries < 48; ++tries) {
        float a = DEG2RAD * (float)GetRandomValue(0, 359);
        float r = Shark_Frand(s->homeRadius * 0.15f, s->homeRadius);
        g.x = s->home.x + sinf(a) * r;
        g.z = s->home.z + cosf(a) * r;

        float seabed = GetTerrainHeightFromMeshXZ(g.x, g.z);
        if (seabed < -9000.0f) continue;

        float depth = s->home.y - seabed;
        if (depth < s->minWaterDepth) continue;

        float minY = seabed + s->bottomClearance;
        float maxY = s->home.y - s->surfaceClearance;
        if (maxY <= minY) continue;

        g.y = Shark_Frand(minY + 1.0f, maxY);
        return g;
    }

    return s->home;
}

static inline void Shark_FacePoint(Shark* s, Vector3 target, float dt)
{
    Vector3 to = Vector3Subtract(target, s->pos);
    float horiz = sqrtf(to.x * to.x + to.z * to.z) + 1e-6f;
    float yawTarget = RAD2DEG * atan2f(to.x, to.z);
    float pitchTarget = RAD2DEG * -atan2f(to.y, horiz);

    float kYaw = Shark_Clamp01(s->turnRateYaw * dt);
    float kPitch = Shark_Clamp01(s->turnRatePitch * dt);

    s->yaw = Shark_LerpDeg(s->yaw, yawTarget, kYaw);
    s->pitch = Shark_LerpDeg(s->pitch, pitchTarget, kPitch);
    s->roll = Shark_LerpDeg(s->roll, Clamp(-Shark_WrapDeg(yawTarget - s->yaw) * 0.25f, -12.0f, 12.0f), Shark_Clamp01(1.5f * dt));
}

static inline void Shark_MoveToward(Shark* s, Vector3 target, float dt)
{
    Vector3 to = Vector3Subtract(target, s->pos);
    float d = Vector3Length(to);
    if (d < 1e-6f) return;

    float maxStep = s->speed * dt * 10.0f;
    if (d <= maxStep) {
        s->pos = target;
        return;
    }

    s->pos = Vector3Add(s->pos, Vector3Scale(to, maxStep / d));
}

static inline void Shark_ApplySwimBones(Shark* s, float t, float intensity)
{
    // Sharks wag side-to-side. So the tail uses Y rotation most strongly.
    float beat = sinf(t * 3.6f);
    float beat2 = sinf(t * 7.2f);
    float tailA = DEG2RAD * (10.0f * intensity) * beat;
    float tailB = DEG2RAD * (18.0f * intensity) * beat;
    float tailC = DEG2RAD * (26.0f * intensity) * beat;
    float spineA = DEG2RAD * (2.0f  * intensity) * beat;
    float spineB = DEG2RAD * (3.5f  * intensity) * beat;
    float headC  = DEG2RAD * (1.5f  * intensity) * -beat;

    Shark_SetFromBindPlusEuler(&s->model, &s->proc, s->bones.spine001, 0, spineA, 0);
    Shark_SetFromBindPlusEuler(&s->model, &s->proc, s->bones.spine002, 0, spineB, 0);
    Shark_SetFromBindPlusEuler(&s->model, &s->proc, s->bones.spine003, 0, spineA * 0.7f, 0);
    Shark_SetFromBindPlusEuler(&s->model, &s->proc, s->bones.chest,    0, spineA * 0.4f, 0);
    Shark_SetFromBindPlusEuler(&s->model, &s->proc, s->bones.neck,     0, headC * 0.4f, 0);
    Shark_SetFromBindPlusEuler(&s->model, &s->proc, s->bones.head,     0, headC, 0);

    Shark_SetFromBindPlusEuler(&s->model, &s->proc, s->bones.tailBase, 0, tailA, 0);
    Shark_SetFromBindPlusEuler(&s->model, &s->proc, s->bones.tailMid,  0, tailB, 0);
    Shark_SetFromBindPlusEuler(&s->model, &s->proc, s->bones.tailTip,  0, tailC, 0);

    Shark_SetFromBindPlusEuler(&s->model, &s->proc, s->bones.finBack,
        DEG2RAD * (1.5f * intensity) * beat2,
        0,
        DEG2RAD * (1.0f * intensity) * beat);
}

static inline void Shark_ApplyBiteBones(Shark* s, float t)
{
    float snap = sinf(Clamp(t * 10.0f, 0.0f, PI));
    Shark_ApplySwimBones(s, t, 1.35f);
    Shark_SetFromBindPlusEuler(&s->model, &s->proc, s->bones.jaw, DEG2RAD * (-22.0f * snap), 0, 0);
    Shark_SetFromBindPlusEuler(&s->model, &s->proc, s->bones.head, DEG2RAD * (4.0f * snap), 0, 0);
    Shark_SetFromBindPlusEuler(&s->model, &s->proc, s->bones.neck, DEG2RAD * (2.5f * snap), 0, 0);
}

// -----------------------------------------------------------------------------
// Lifecycle
// -----------------------------------------------------------------------------
static inline bool LoadShark(Shark* s)
{
    if (!s) return false;

    s->model = LoadModel("models/shark.glb");
    s->tex = LoadMyTexture("textures/shark.png");
    if (s->model.materialCount > 0 && s->tex.id) {
        SetMaterialTexture(&s->model.materials[0], MATERIAL_MAP_ALBEDO, s->tex);
    }

    s->proc.boneCount = s->model.boneCount;
    s->proc.bones = s->model.bones;
    s->proc.frameCount = 1;
    s->proc.framePoses = MemAlloc(sizeof(Transform*) * 1);
    if (!s->proc.framePoses) return false;
    s->proc.framePoses[0] = MemAlloc(sizeof(Transform) * s->proc.boneCount);
    if (!s->proc.framePoses[0]) return false;

    Shark_PoseResetToBind(&s->model, &s->proc);
    Shark_CacheBones(s);
    return true;
}

static inline void InitShark(Shark* s, Vector3 home, float surfaceY)
{
    if (!s) return;
    memset(s, 0, sizeof(*s));

    float seabed = GetTerrainHeightFromMeshXZ(home.x, home.z);
    if (seabed < -9000.0f) seabed = home.y - 80.0f;

    s->home = home;
    s->pos = home;
    s->goal = home;
    s->yaw = 0.0f;
    s->pitch = 0.0f;
    s->roll = 0.0f;
    s->scale = 6.0f;
    s->xFixDeg = -90.0f;
    s->yFixPos = 0.0f;

    s->homeRadius = 260.0f;
    s->wanderSpeed = 0.42f;
    s->stalkSpeed = 0.82f;
    s->speed = s->wanderSpeed;
    s->turnRateYaw = 1.6f;
    s->turnRatePitch = 1.1f;
    s->arriveRadius = 6.0f;

    s->minWaterDepth = SHARK_MIN_WATER_DEPTH;
    s->surfaceClearance = SHARK_SURFACE_CLEARANCE;
    s->bottomClearance = SHARK_BOTTOM_CLEARANCE;

    s->state = SHARK_STATE_WANDER;
    s->stateTime = 0.0f;
    s->animTime = 0.0f;

    s->planTimer = CreateTimer(2.8f);
    s->biteRecoverTimer = CreateTimer(1.25f);
    s->victimHideTimer = CreateTimer(SHARK_HIDE_VICTIM_TIME);
    StartTimer(&s->planTimer);

    float minY = seabed + s->bottomClearance;
    float maxY = surfaceY - s->surfaceClearance;
    if (maxY > minY) s->pos.y = Clamp(home.y, minY, maxY);
}

static inline void FreeShark(Shark* s)
{
    if (!s) return;
    if (s->proc.framePoses) {
        if (s->proc.framePoses[0]) MemFree(s->proc.framePoses[0]);
        MemFree(s->proc.framePoses);
        s->proc.framePoses = NULL;
    }
    if (s->tex.id) UnloadTexture(s->tex);
    if (s->model.meshCount > 0) UnloadModel(s->model);
}

// -----------------------------------------------------------------------------
// Core update
// -----------------------------------------------------------------------------
static inline void Shark_Update(Shark* s, Donogan* d, float dt)
{
    if (!s || !d) return;

    s->stateTime += dt;
    s->animTime += dt;

    // Undo temporary victim invisibility.
    if (s->victimHidden && HasTimerElapsed(&s->victimHideTimer)) {
        s->victimHidden = false;
        d->drawColor = WHITE;
        ResetTimer(&s->victimHideTimer);
    }

    float seabedHere = GetTerrainHeightFromMeshXZ(s->pos.x, s->pos.z);
    if (seabedHere < -9000.0f) seabedHere = s->pos.y - 50.0f;

    bool sharkWaterValid = ((s->home.y - seabedHere) >= s->minWaterDepth);
    bool donInDeepEnoughWater = d->inWater && ((d->waterY - d->seabedY) >= s->minWaterDepth);

    float surfaceY = d->waterY;
    float minY = seabedHere + s->bottomClearance;
    float maxY = surfaceY - s->surfaceClearance;
    if (maxY <= minY) maxY = minY + 0.25f;

    if (s->pos.y < minY) s->pos.y = minY;
    if (s->pos.y > maxY) s->pos.y = maxY;

    float distToDon = Vector3Distance(s->pos, d->pos);
    bool canHuntDon = donInDeepEnoughWater && sharkWaterValid;

    if (canHuntDon && distToDon <= SHARK_KILL_DISTANCE && s->state != SHARK_STATE_BITE) {
        s->state = SHARK_STATE_BITE;
        s->stateTime = 0.0f;
        s->speed = 1.65f;
        StartTimer(&s->biteRecoverTimer);

        d->drawColor = (Color){ 255, 255, 255, 0 };
        d->health = 0;
        DonSetState(d, DONOGAN_STATE_DEATH);
        s->victimHidden = true;
        StartTimer(&s->victimHideTimer);
    }

    switch (s->state)
    {
        default:
        case SHARK_STATE_WANDER:
        {
            s->speed = Lerp(s->speed, s->wanderSpeed, 0.04f);

            if (canHuntDon) {
                s->state = SHARK_STATE_STALK;
                s->stateTime = 0.0f;
                break;
            }

            if (Vector3Distance(s->pos, s->goal) <= s->arriveRadius || HasTimerElapsed(&s->planTimer)) {
                s->goal = Shark_PickWanderGoal(s);
                ResetTimer(&s->planTimer);
                StartTimer(&s->planTimer);
            }

            Shark_FacePoint(s, s->goal, dt);
            Shark_MoveToward(s, s->goal, dt);
            Shark_ApplySwimBones(s, s->animTime, 0.85f);
        } break;

        case SHARK_STATE_STALK:
        {
            if (!canHuntDon) {
                s->state = SHARK_STATE_WANDER;
                s->stateTime = 0.0f;
                s->goal = Shark_PickWanderGoal(s);
                break;
            }

            Vector3 target = d->pos;
            target.y = Clamp(d->pos.y, minY + 0.5f, maxY - 0.2f);

            s->speed = Lerp(s->speed, s->stalkSpeed, 0.05f);
            Shark_FacePoint(s, target, dt);
            Shark_MoveToward(s, target, dt);
            Shark_ApplySwimBones(s, s->animTime, 1.15f);
        } break;

        case SHARK_STATE_BITE:
        {
            Vector3 target = d->pos;
            target.y = Clamp(target.y, minY + 0.2f, maxY - 0.1f);
            Shark_FacePoint(s, target, dt);
            Shark_MoveToward(s, target, dt);
            Shark_ApplyBiteBones(s, s->stateTime);

            if (HasTimerElapsed(&s->biteRecoverTimer)) {
                s->state = SHARK_STATE_RECOVER;
                s->stateTime = 0.0f;
                s->goal = Shark_PickWanderGoal(s);
                ResetTimer(&s->biteRecoverTimer);
            }
        } break;

        case SHARK_STATE_RECOVER:
        {
            s->speed = Lerp(s->speed, s->wanderSpeed, 0.03f);
            Shark_FacePoint(s, s->goal, dt);
            Shark_MoveToward(s, s->goal, dt);
            Shark_ApplySwimBones(s, s->animTime, 1.0f);

            if (Vector3Distance(s->pos, s->goal) <= s->arriveRadius || s->stateTime > 2.4f) {
                s->state = canHuntDon ? SHARK_STATE_STALK : SHARK_STATE_WANDER;
                s->stateTime = 0.0f;
                if (!canHuntDon) s->goal = Shark_PickWanderGoal(s);
            }
        } break;
    }

    // Final safety clamp after movement.
    seabedHere = GetTerrainHeightFromMeshXZ(s->pos.x, s->pos.z);
    if (seabedHere > -9000.0f) {
        minY = seabedHere + s->bottomClearance;
        maxY = surfaceY - s->surfaceClearance;
        if (maxY <= minY) maxY = minY + 0.25f;
        s->pos.y = Clamp(s->pos.y, minY, maxY);

        // If this XZ is too shallow, push back toward home rather than hanging out in shoals.
        if ((surfaceY - seabedHere) < s->minWaterDepth) {
            s->goal = s->home;
            s->state = SHARK_STATE_WANDER;
            s->stateTime = 0.0f;
        }
    }

    UpdateModelAnimation(s->model, s->proc, 0);
}

#endif // SHARK_H
