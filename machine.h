#ifndef MACHINE_H
#define MACHINE_H

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "texture.h"   // for LoadMyTexture()
#include "truck.h"     // for truckPosition / truckAngle if you already expose them
#include "donogan.h"   // for Donogan if you want to pass don directly
#include "interact.h"
#include "frustum.h"
#include "jc.h"
#include <stdbool.h>
#include <string.h>

// ============================================================
// CONFIG
// ============================================================

#define MACHINE_DRAW_DISTANCE       50.0f
#define MACHINE_INTERACT_DISTANCE   10.5f
#define MACHINE_COUNT_WINDMILL      11
#define MACHINE_COUNT_TOTAL         (MACHINE_COUNT_WINDMILL + 1)

#define TRUCK_MACHINE_INDEX         (MACHINE_COUNT_TOTAL - 1)

// Lift animation
#define MACHINE_LIFT_MAX_HEIGHT     5.20f
#define MACHINE_LIFT_MIN_HEIGHT     0.18f
#define MACHINE_LIFT_SPEED          2.25f

// Visual offsets
#define MACHINE_MODEL_SCALE         3.0f
#define MACHINE_LIFT_MODEL_SCALE    5.0f

// Cylinder: from ground up to underside of lift bed
#define MACHINE_LIFT_CYL_RADIUS     0.28f

// Optional: box padding for culling/collision-ish checks
#define MACHINE_BOX_PAD_XZ          1.6f
#define MACHINE_BOX_PAD_Y           2.8f

// ============================================================
// TYPES
// ============================================================

typedef enum MachineType
{
    MILL_MACHINE = 0,
    TRUCK_MACHINE = 1
} MachineType;

typedef struct Machine
{
    MachineType type;
    int index;
    Scene_Type scene_type;

    Vector3 pos;
    float yaw;
    float scale;

    bool active;           // pressed / operating / used
    bool visible;
    bool canInteract;

    BoundingBox localBox;
    BoundingBox worldBox;
} Machine;

// ============================================================
// GLOBALS
// ============================================================

static Model gMachineModel = { 0 };
static Texture2D gMachineTexture = { 0 };
static bool gMachineModelReady = false;

static Model gLiftBedModel = { 0 };
static Texture2D gLiftBedTexture = { 0 };
static bool gLiftBedReady = false;
//917.38, 323.76, 2004.90

//typedef struct {
//    Scene_Type stype;
//    MachineType mtype;
//    int id;
//} MachineHomeTie;



// runtime machine list
static Machine gMachines[MACHINE_COUNT_TOTAL];
static int gMachineCount = 0;

// truck lift animation state
static float gTruckLiftHeight = MACHINE_LIFT_MAX_HEIGHT;   // starts up
static float gTruckLiftTargetHeight = MACHINE_LIFT_MAX_HEIGHT;
static bool gTruckLiftLowering = false;
static bool gTruckLiftAtFloor = false;

// local offset from truck position to lift-bed center.
// tweak these after first test if needed.
static Vector3 gTruckLiftOffset = { 0.0f, 0.0f, -1.35f };

// If your truck model needs a different yaw relation, tweak this.
static float gTruckLiftYawOffset = 0.0f;

// ============================================================
// INTERNAL HELPERS
// ============================================================

static inline Vector3 MachineRotateYawOffset(Vector3 local, float yaw)
{
    float c = cosf(yaw);
    float s = sinf(yaw);
    return (Vector3) {
        local.x* c - local.z * s,
            local.y,
            local.x* s + local.z * c
    };
}

static inline BoundingBox Machine_MakeBoxAroundPoint(Vector3 p, float padXZ, float padY)
{
    BoundingBox b = { 0 };
    b.min = (Vector3){ p.x - padXZ, p.y - 0.25f, p.z - padXZ };
    b.max = (Vector3){ p.x + padXZ, p.y + padY,  p.z + padXZ };
    return b;
}

static inline void Machine_RebuildWorldBox(Machine* m)
{
    // simple padded world box around machine position.
    // good enough for draw cull + interaction space.
    m->worldBox = Machine_MakeBoxAroundPoint(m->pos, MACHINE_BOX_PAD_XZ, MACHINE_BOX_PAD_Y);
}

static inline void Machine_ClearAll(void)
{
    memset(gMachines, 0, sizeof(gMachines));
    gMachineCount = 0;
}

static inline void Machine_Add(MachineType type, int index, Vector3 pos, float yaw, float scale, Scene_Type scene_type)
{
    if (gMachineCount >= MACHINE_COUNT_TOTAL) return;

    Machine* m = &gMachines[gMachineCount];
    memset(m, 0, sizeof(*m));

    m->type = type;
    m->index = index;
    m->scene_type = scene_type;
    m->pos = pos;
    m->yaw = yaw;
    m->scale = scale;
    m->active = false;
    m->visible = true;
    m->canInteract = false;
    m->localBox = (BoundingBox){
        .min = (Vector3){ -1.0f, 0.0f, -1.0f },
        .max = (Vector3){  1.0f, 2.5f,  1.0f }
    };
    Machine_RebuildWorldBox(m);

    gMachineCount++;
}

static inline Vector3 Machine_GetTruckLiftWorldPos(Vector3 truckPos, float truckYaw)
{
    Vector3 off = MachineRotateYawOffset(gTruckLiftOffset, truckYaw + gTruckLiftYawOffset);
    return Vector3Add(truckPos, off);
}

// ============================================================
// INIT / SHUTDOWN
// ============================================================

static inline void Machine_Init(void)
{
    Machine_ClearAll();

    // ------------------------
    // models / textures
    // ------------------------
    gMachineModel = LoadModel("models/machine.obj");
    gMachineTexture = LoadMyTexture("textures/machine.png");
    if (gMachineModel.materialCount > 0)
    {
        gMachineModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = gMachineTexture;
    }
    gMachineModelReady = true;

    gLiftBedModel = LoadModel("models/lift.obj");
    gLiftBedTexture = LoadMyTexture("textures/lift.png");
    if (gLiftBedModel.materialCount > 0)
    {
        gLiftBedModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = gLiftBedTexture;
    }
    gLiftBedReady = true;

    // ------------------------
    // machine placements
    // ------------------------

    // 11 windmill machines for now.
    // Fill these with real positions when ready.
    // I left them easy to edit.
    //windmills
    ////far side of castle
    //        don_pos = 2060.37, 463.83, 200.41     SCENE_HOME_WINDMILL_03
    //        don_pos = 1941.82, 406.55, -526.68    SCENE_HOME_WINDMILL_04
    //        don_pos = 1821.41, 476.18, -1176.68   SCENE_HOME_WINDMILL_05
    //        don_pos = 1795.21, 505.97, -1717.80   SCENE_HOME_WINDMILL_06
    //        don_pos = 1938.34, 390.98, -2698.09   SCENE_HOME_WINDMILL_07
    //
    //        //top far side, past TOL ring
    //        don_pos = -1700.79, 436.68, -1707.10  SCENE_HOME_WINDMILL_11 
    //        don_pos = -2670.76, 406.59, -2397.02  SCENE_HOME_WINDMILL_08
    //        don_pos = 2657.69, 472.62, 1484.53 //castle
    //        don_pos = 2289.40, 338.82, 4761.52 //donogans
    //        don_pos = -717.63, 789.85, 2883.16 //yeti mt 1
    Machine_Add(MILL_MACHINE, 0, (Vector3) { 917.38, 326.02, 2004.90 }, 0.0f, 1.0f, SCENE_HOME_WINDMILL_10); //barn
    Machine_Add(MILL_MACHINE, 1, (Vector3) { 2060.37, 462.50, 200.41 }, PI / 2.0f, 1.0f, SCENE_HOME_WINDMILL_03);//far side castle
    Machine_Add(MILL_MACHINE, 2, (Vector3) { 1944.82, 407.00, -526.68 }, PI / 2.0f, 1.0f, SCENE_HOME_WINDMILL_04);
    Machine_Add(MILL_MACHINE, 3, (Vector3) { 1821.41, 478.00, -1176.68 }, PI / 2.0f, 1.0f, SCENE_HOME_WINDMILL_05);
    Machine_Add(MILL_MACHINE, 4, (Vector3) { 1795.21, 509.00, -1717.80 }, PI / 2.0f, 1.0f, SCENE_HOME_WINDMILL_06);
    Machine_Add(MILL_MACHINE, 5, (Vector3) { 1938.34, 393.08, -2698.09 }, 0.0f, 1.0f, SCENE_HOME_WINDMILL_07);
    Machine_Add(MILL_MACHINE, 6, (Vector3) { -1700.79, 439.00, -1707.10 }, 0.0f, 1.0f, SCENE_HOME_WINDMILL_11);//past TOL
    Machine_Add(MILL_MACHINE, 7, (Vector3) { -2670.76, 409.77, -2397.02 }, 0.0f, 1.0f, SCENE_HOME_WINDMILL_08);//past past TOL
    Machine_Add(MILL_MACHINE, 8, (Vector3) { 2657.69, 475.00, 1484.53 }, 0.0f, 1.0f, SCENE_HOME_WINDMILL_01); //castle
    Machine_Add(MILL_MACHINE, 9, (Vector3) { 2289.40, 341.00, 4761.52 }, 0.0f, 1.0f, SCENE_HOME_WINDMILL_02);  //by donogans
    Machine_Add(MILL_MACHINE, 10, (Vector3) { -717.63, 792.00, 2883.16 }, 0.0f, 1.0f, SCENE_HOME_WINDMILL_09); //--yeti mt

    // Truck machine from your placement notes
    Machine_Add(TRUCK_MACHINE, TRUCK_MACHINE_INDEX, (Vector3) {1263.17f, 330.30f, 1250.56f}, 0.0f, 1.0f, -1); //dummy for truck

    // lift state
    gTruckLiftHeight = MACHINE_LIFT_MAX_HEIGHT;
    gTruckLiftTargetHeight = MACHINE_LIFT_MAX_HEIGHT;
    gTruckLiftLowering = false;
    gTruckLiftAtFloor = false;
}

static inline void Machine_Unload(void)
{
    if (gMachineModelReady)
    {
        UnloadModel(gMachineModel);
        gMachineModelReady = false;
    }

    if (gLiftBedReady)
    {
        UnloadModel(gLiftBedModel);
        gLiftBedReady = false;
    }

    if (gMachineTexture.id) { UnloadTexture(gMachineTexture); gMachineTexture.id = 0; }
    if (gLiftBedTexture.id) { UnloadTexture(gLiftBedTexture); gLiftBedTexture.id = 0; }
}

// ============================================================
// UPDATE
// ============================================================

static inline void Machine_Update(float dt, Donogan * d, Vector3 * truckPos)
{
    if (gTruckLiftLowering)
    {
        float dty = MACHINE_LIFT_SPEED * dt;
        gTruckLiftHeight -= dty;
        (*truckPos).y -= dty;
        if (gTruckLiftHeight <= MACHINE_LIFT_MIN_HEIGHT)
        {
            gTruckLiftHeight = MACHINE_LIFT_MIN_HEIGHT;
            gTruckLiftLowering = false;
            gTruckLiftAtFloor = true;
            d->unlockedTruck = true;
            missions[MISSION_GET_TRUCK].complete = true;
            d->xp += 120;
        }
    }

    // keep boxes fresh if you later animate machine positions
    for (int i = 0; i < gMachineCount; i++)
    {
        if (gMachines[i].type == MILL_MACHINE && gMachines[i].active) 
        { 
            Scenes[gMachines[i].scene_type].active = true; 
        }
    }
}

static inline int Machine_FindInteractable(Vector3 playerPos, float maxDistance)
{
    int best = -1;
    float bestD = maxDistance;

    for (int i = 0; i < gMachineCount; i++)
    {
        Machine* m = &gMachines[i];
        float d = Vector3Distance(playerPos, m->pos);
        m->canInteract = (d <= maxDistance);

        if (d < bestD)
        {
            bestD = d;
            best = i;
        }
    }

    return best;
}

// Call this on triangle just-pressed.
// Returns machine index if something triggered, otherwise -1.
static inline int Machine_TryInteract(Donogan * d, Vector3 playerPos, bool hasWrench)
{
    if (!hasWrench) return -1;

    int idx = Machine_FindInteractable(playerPos, MACHINE_INTERACT_DISTANCE);
    if (idx < 0) return -1;

    Machine* m = &gMachines[idx];
    if (m->active) { return -1; }
    m->active = true;

    if (m->type == TRUCK_MACHINE)
    {
        // start truck lift lowering
        gTruckLiftLowering = true;
        gTruckLiftAtFloor = false;
        gTruckLiftTargetHeight = MACHINE_LIFT_MIN_HEIGHT;
    }
    else if (m->type == MILL_MACHINE)
    {
        // you got that windmill behavior
        Scenes[m->scene_type].active = true;
        d->xp += 80;
    }

    return idx;
}

// ============================================================
// DRAW HELPERS
// ============================================================

static inline bool Machine_ShouldDraw(Vector3 camPos, Vector3 objectPos, float maxDistance)
{
    return (Vector3Distance(camPos, objectPos) <= maxDistance);
}

static inline void Machine_DrawSingle(const Machine* m)
{
    if (!gMachineModelReady) return;
    DrawModelEx(
        gMachineModel,
        m->pos,
        (Vector3) {
        0, 1, 0
    },
        m->yaw* RAD2DEG,
        (Vector3) {
        m->scale* MACHINE_MODEL_SCALE, m->scale* MACHINE_MODEL_SCALE, m->scale* MACHINE_MODEL_SCALE
    },
        WHITE
    );

    // tiny helper marker if active
    if (m->active)
    {
        DrawCylinderEx(
            Vector3Add(m->pos, (Vector3) { 0, 2.8f, 0 }),
            Vector3Add(m->pos, (Vector3) { 0, 3.8f, 0 }),
            0.16f, 0.16f, 8, SKYBLUE
        );
    }
}

// Draw all machines.
// frustumFn can be NULL if you want distance-only.
static inline void Machine_DrawAll(Vector3 camPos, Frustum frustum)
{
    for (int i = 0; i < gMachineCount; i++)
    {
        Machine* m = &gMachines[i];

        if (!Machine_ShouldDraw(camPos, m->pos, MACHINE_DRAW_DISTANCE)) { continue; }
        if (!IsBoxInFrustum(m->worldBox, frustum)) { continue; }

        Machine_DrawSingle(m);
    }
}

// ============================================================
// TRUCK LIFT DRAW
// ============================================================

// Draws the lift bed model plus a hydraulic cylinder.
// truckYaw should be in radians.
static inline void Machine_DrawTruckLift()
{
    if (!gLiftBedReady) return;

    Vector3 liftBasePos = Machine_GetTruckLiftWorldPos((Vector3){ 1281.42, 327.53, 1251.01 }, 0);
    Vector3 liftPos = liftBasePos;
    liftPos.y += gTruckLiftHeight;

    // draw lift bed model
    DrawModelEx(
        gLiftBedModel,
        liftPos,
        (Vector3) {
        0, 1, 0
    },
        0 * RAD2DEG, //truckYaw initial
        (Vector3) {
        MACHINE_LIFT_MODEL_SCALE, MACHINE_LIFT_MODEL_SCALE, MACHINE_LIFT_MODEL_SCALE
    },
        WHITE
    );

    // draw support / hydraulic cylinder from near-floor to under lift bed
    Vector3 cylStart = liftBasePos;
    cylStart.y += 0.12f;

    Vector3 cylEnd = liftPos;
    cylEnd.y -= 0.35f;

    DrawCylinderEx(
        cylStart,
        cylEnd,
        MACHINE_LIFT_CYL_RADIUS,
        MACHINE_LIFT_CYL_RADIUS * 0.82f,
        10,
        GRAY
    );

    // optional cap sphere-ish look
    DrawSphere(cylStart, MACHINE_LIFT_CYL_RADIUS * 0.92f, DARKGRAY);
    DrawSphere(cylEnd, MACHINE_LIFT_CYL_RADIUS * 0.76f, DARKGRAY);
}

// ============================================================
// OPTIONAL DEBUG / QUERY HELPERS
// ============================================================

static inline bool Machine_IsTruckLiftAtFloor(void)
{
    return gTruckLiftAtFloor;
}

static inline bool Machine_IsTruckLiftLowering(void)
{
    return gTruckLiftLowering;
}

static inline float Machine_GetTruckLiftHeight(void)
{
    return gTruckLiftHeight;
}

static inline Machine* Machine_GetByIndex(int idx)
{
    if (idx < 0 || idx >= gMachineCount) return NULL;
    return &gMachines[idx];
}

#endif // MACHINE_H