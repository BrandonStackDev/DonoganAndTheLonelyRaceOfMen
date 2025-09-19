#ifndef TRUCK_H
#define TRUCK_H

// Includes
#include "raylib.h"
#include "raymath.h"
//c
#include <float.h>          // FLT_MAX

//me
#include "timer.h"
#include "core.h"
#include "game.h"

#define MAX_TURN_ANGLE 0.26f //radians

#define TRUCK_BOX_HALF        1.0f   // half-size for the 4 body boxes  (-1..+1 local)
#define TIRE_BOX_HALF         1.0f   // half-size for each tire box     (-1..+1 local)
#define TRUCK_Y_OFFSET_DRAW   1.62f  // same baseline you use when drawing

typedef enum {
    GROUND,
    AIRBORNE,
    LANDING
} Truck_Air_State;

Timer truckInteractTimer;
bool displayTruckPoints = false;
bool displayTruckForward = false;
Vector3 truckPosition = { 1279.45f, 333.71f, 1249.00f };
Vector3 truckBedPosition = { 0.0f, 1.362f, 0.0f };
Vector3 truckForward = { 0.0f, 0.0f, 1.0f };  // Forward is along +Z
Vector3 rearAxleOffset = { 0, 0, -1.5f }; // adjust Z as needed
Vector3 truckOrigin = { 0 };
Vector3 front = { 0 };
Vector3 back = { 0 };
#define truckFrontDim 3.4f
#define truckBackDim -4.4f
#define truckLength truckFrontDim - truckBackDim
float truckWidth = 3.6f;
float truckYOffset = 1.2f;
float tireYPos[4] = { 0,0,0,0 }; //fl, fr, bl, br
float tireYOffset[4] = { 0,0,0,0 };
float tireSpinDelta[4] = { 0,0,0,0 };
float tireSpinPos[4] = { 0,0,0,0 };
float tireTurnDelta[4] = { 0,0,0,0 };
float tireTurnPos[4] = { 0,0,0,0 };
float truckSpeed = 0.0f;
float truckAngle = 0.0f; // Yaw angle
float truckPitch = 0.0f;
float truckPitchYOffset = 0.0f;
float truckRoll = 0.0f;
float friction = 0.96f;//I dont want this to fight too much with rolling down hills
const float spinRate = 720.0f; // degrees per unit of speed, tweak as needed
Truck_Air_State truckAirState = GROUND;
//sliding truck when moving fast and turning too hard
bool isTruckSliding = false;
bool truckSlidePeek = false;
Vector3 truckSlideForward = { 0.0f, 0.0f, 0.0f }; //well set this bycorrectly rotating forward
float truckSlideSpeed = 0;
float rotSlide = 0;
//tricks
int points = 0;
float truckTrickYaw = 0;
float truckTrickPitch = 0;
float truckTrickRoll = 0;
bool doing360 = false;
bool doingFlip = false;
bool doingRoll = false;
bool doingBonkers = false;
bool bonkersPeeked = false;
Vector3 bonkersStartOffsets[4] = {
    {  1.6f, 0.0f,  3.36f }, // Front-right
    { -1.58f, 0.0f,  3.36f }, // Front-left - stubby
    {  1.6f, 0.0f, -2.64f }, // Rear-right
    { -1.6f, 0.0f, -2.64f }  // Rear-left
};
Vector3 bonkersPeekOffsets[4] = {
    {  6.0f, -3.0f,  6.0f }, // Front-right
    { -6.0f, -3.0f,  6.0f }, // Front-left - stubby
    {  6.0f, -3.0f, -6.0f }, // Rear-right
    { -6.0f, -3.0f, -6.0f }  // Rear-left
};
//models and stuff
Model truck;
Material truckMaterial;
Model tire;
Material tireMaterial;
Model tires[4];
Vector3 tireOffsets[4];
//collision
BoundingBox TruckBoxFront;
BoundingBox TruckBoxBack;
BoundingBox TruckBoxLeft;
BoundingBox TruckBoxRight;
BoundingBox TruckBoxTires[4];

// Compose the truck body quaternion exactly like your draw code:
// q = qYaw * (qPitch * qRoll)
static inline Quaternion Truck_BodyQuat(void)
{
    float finalTruckYaw = truckAngle + truckTrickYaw;
    float finalTruckPitch = truckPitch - truckTrickPitch;
    float finalTruckRoll = truckRoll + truckTrickRoll;

    Quaternion qYaw = QuaternionFromAxisAngle((Vector3) { 0, 1, 0 }, finalTruckYaw);
    Quaternion qPitch = QuaternionFromAxisAngle((Vector3) { 1, 0, 0 }, finalTruckPitch);
    Quaternion qRoll = QuaternionFromAxisAngle((Vector3) { 0, 0, 1 }, finalTruckRoll);
    return QuaternionMultiply(qYaw, QuaternionMultiply(qPitch, qRoll));
}

// Rotate a local point and add the world base origin
static inline Vector3 Truck_LocalToWorld(Vector3 pLocal, Quaternion q, Vector3 worldBase)
{
    Vector3 r = Vector3RotateByQuaternion(pLocal, q);
    return Vector3Add(worldBase, r);
}

// Build an AABB by rotating the 8 corners of a local cube around localCenter
static inline BoundingBox Truck_AabbFromLocalCube(Vector3 localCenter, float half,
    Quaternion q, Vector3 worldBase)
{
    Vector3 minv = (Vector3){ FLT_MAX,  FLT_MAX,  FLT_MAX };
    Vector3 maxv = (Vector3){ -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (int sx = -1; sx <= 1; sx += 2)
        for (int sy = -1; sy <= 1; sy += 2)
            for (int sz = -1; sz <= 1; sz += 2)
            {
                Vector3 p = (Vector3){
                    localCenter.x + sx * half,
                    localCenter.y + sy * half,
                    localCenter.z + sz * half
                };
                Vector3 w = Truck_LocalToWorld(p, q, worldBase);
                if (w.x < minv.x) minv.x = w.x; if (w.y < minv.y) minv.y = w.y; if (w.z < minv.z) minv.z = w.z;
                if (w.x > maxv.x) maxv.x = w.x; if (w.y > maxv.y) maxv.y = w.y; if (w.z > maxv.z) maxv.z = w.z;
            }
    return (BoundingBox) { minv, maxv };
}

// Centers for the four body boxes in TRUCK LOCAL space
static inline float TruckFrontZ_Local(void) { return truckFrontDim - rearAxleOffset.z; }
static inline float TruckBackZ_Local(void) { return truckBackDim - rearAxleOffset.z; }
static inline float TruckMidZ_Local(void) { return 0.5f * (TruckFrontZ_Local() + TruckBackZ_Local()); }
static inline float TruckSideX_Local(bool right) { return (right ? +1.0f : -1.0f) * (truckWidth * 0.5f); }


void InitTruck()
{
    // Load  //todo: move this and most of the truck stuff into truck.h
    truck = LoadModel("models/truck.obj");
    truck.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("textures/truck.png");
    truckMaterial = LoadMaterialDefault();
    truckMaterial.shader = LoadShader(0, 0);
    truckMaterial.maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
    truckMaterial.maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("textures/truck.png");
    TraceLog(LOG_INFO, "CWD: %s", GetWorkingDirectory());
    TraceLog(LOG_INFO, "Has truck.obj? %d  Has texture? %d",
        FileExists("models/truck.obj"), FileExists("textures/truck.png"));

    if (truck.meshCount == 0) {
        TraceLog(LOG_ERROR, "Truck failed to load: meshCount==0");
        // bail out or skip drawing the truck
    }
    // Load tire
    tire = LoadModel("models/tire.obj");
    tire.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("textures/tire.png");
    tireMaterial = LoadMaterialDefault();
    tireMaterial.shader = LoadShader(0, 0);
    tireMaterial.maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
    tireMaterial.maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("textures/tire.png");
    tires[0] = tire;
    tires[1] = tire;
    tires[2] = tire;
    tires[3] = tire;
    // Set tire offsets relative to truck
    tireOffsets[0] = (Vector3) { 1.6f, 0.0f,  3.36f }; // Front-right
    tireOffsets[1] = (Vector3) { -1.58f, 0.0f, 3.36f }; // Front-left - stubby
    tireOffsets[2] = (Vector3) { 1.6f, 0.0f, -2.64f }; // Rear-right
    tireOffsets[3] = (Vector3) { -1.6f, 0.0f, -2.64f };  // Rear-left

    TruckBoxFront = (BoundingBox){ (Vector3) { -1,-1,-1 }, (Vector3) { 1,1,1 } };
    TruckBoxBack = (BoundingBox){ (Vector3) { -1,-1,-1 }, (Vector3) { 1,1,1 } };
    TruckBoxLeft = (BoundingBox){ (Vector3) { -1,-1,-1 }, (Vector3) { 1,1,1 } };
    TruckBoxRight = (BoundingBox){ (Vector3) { -1,-1,-1 }, (Vector3) { 1,1,1 } };
    TruckBoxTires[0] = (BoundingBox){ (Vector3) { -1,-1,-1 }, (Vector3) { 1,1,1 } };
    TruckBoxTires[1] = (BoundingBox){ (Vector3) { -1,-1,-1 }, (Vector3) { 1,1,1 } };
    TruckBoxTires[2] = (BoundingBox){ (Vector3) { -1,-1,-1 }, (Vector3) { 1,1,1 } };
    TruckBoxTires[3] = (BoundingBox){ (Vector3) { -1,-1,-1 }, (Vector3) { 1,1,1 } };

    truckInteractTimer = CreateTimer(1.0f);
    StartTimer(&truckInteractTimer);
}

void UpdateTruckBoxes()
{
    // Match the same base translation you use for drawing (baseline Y lift)
    Vector3 worldBase = (Vector3){ truckOrigin.x, truckOrigin.y + TRUCK_Y_OFFSET_DRAW, truckOrigin.z };
    Quaternion q = Truck_BodyQuat();

    // Choose the local Y for the body boxes (rough center of the body in your local space)
    const float boxY = 1.0f; // tweak if you want them higher/lower on the chassis

    // Body box local centers
    Vector3 cFront = (Vector3){ 0.0f, boxY, TruckFrontZ_Local() };
    Vector3 cBack = (Vector3){ 0.0f, boxY, TruckBackZ_Local() };
    Vector3 cLeft = (Vector3){ TruckSideX_Local(false), boxY, TruckMidZ_Local() };
    Vector3 cRight = (Vector3){ TruckSideX_Local(true),  boxY, TruckMidZ_Local() };

    // Build world AABBs for the 4 sides (all square, -1..+1 local scaled by TRUCK_BOX_HALF)
    TruckBoxFront = Truck_AabbFromLocalCube(cFront, TRUCK_BOX_HALF, q, worldBase);
    TruckBoxBack = Truck_AabbFromLocalCube(cBack, TRUCK_BOX_HALF, q, worldBase);
    TruckBoxLeft = Truck_AabbFromLocalCube(cLeft, TRUCK_BOX_HALF, q, worldBase);
    TruckBoxRight = Truck_AabbFromLocalCube(cRight, TRUCK_BOX_HALF, q, worldBase);

    // Tires: centers come from tireOffsets[] in truck local space,
    // plus the per-tire suspension drop on Y (subtract tireYOffset[i] after rotation)
    for (int i = 0; i < 4; ++i)
    {
        Vector3 lc = tireOffsets[i];
        Vector3 wc = Truck_LocalToWorld(lc, q, worldBase);
        wc.y -= tireYOffset[i]; // same suspension deflection you apply when drawing tires
        TruckBoxTires[i] = (BoundingBox){
            (Vector3) {
 wc.x - TIRE_BOX_HALF, wc.y - TIRE_BOX_HALF, wc.z - TIRE_BOX_HALF
},
(Vector3) {
wc.x + TIRE_BOX_HALF, wc.y + TIRE_BOX_HALF, wc.z + TIRE_BOX_HALF
}
        };
    }
}

// --->>> SUMMON (state + helpers)
static bool  truckSummonActive = false;
static float truckSummonStopRadius = 4.25f;   // stop this far from Donny
static float truckSummonMaxSpeed = 2.12f;    // cap speed while summoning
static float truckSummonAccel = 12.0f;    // accel/decel rate (u/s^2)
static float truckSummonSteerRate = 2.0f;     // rad/s yaw rate cap

static inline float WrapAngle(float a) {
    while (a > PI) a -= PI * 2.0f;
    while (a < -PI) a += PI * 2.0f;
    return a;
}

static inline Vector3 YOnly(Vector3 v) { return (Vector3) { v.x, 0.0f, v.z }; }

static void TruckSummonStart(void) {
    truckSummonActive = true;
    if(!IsSoundPlaying(carHorn)) { PlaySoundVol(carHorn);}
}

static void TruckSummonCancel(void) {
    truckSummonActive = false;
}


#endif // TRUCK_H
