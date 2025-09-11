#ifndef TRUCK_H
#define TRUCK_H

// Includes
#include "raylib.h"
#include "raymath.h"
//me
#include "timer.h"
#include "core.h"

#define MAX_TURN_ANGLE 0.26f //radians
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

    TruckBoxFront = (BoundingBox){ (Vector3) { 0,0,0 }, (Vector3) { 1,1,1 } };
    TruckBoxBack = (BoundingBox){ (Vector3) { 0,0,0 }, (Vector3) { 1,1,1 } };
    TruckBoxLeft = (BoundingBox){ (Vector3) { 0,0,0 }, (Vector3) { 1,1,1 } };
    TruckBoxRight = (BoundingBox){ (Vector3) { 0,0,0 }, (Vector3) { 1,1,1 } };
    TruckBoxTires[0] = (BoundingBox){ (Vector3) { 0,0,0 }, (Vector3) { 1,1,1 } };
    TruckBoxTires[1] = (BoundingBox){ (Vector3) { 0,0,0 }, (Vector3) { 1,1,1 } };
    TruckBoxTires[2] = (BoundingBox){ (Vector3) { 0,0,0 }, (Vector3) { 1,1,1 } };
    TruckBoxTires[3] = (BoundingBox){ (Vector3) { 0,0,0 }, (Vector3) { 1,1,1 } };

    truckInteractTimer = CreateTimer(1.0f);
    StartTimer(&truckInteractTimer);
}

void UpdateTruckBoxes()
{
    TruckBoxFront = UpdateBoundingBox(TruckBoxFront, truckPosition);
    TruckBoxBack = UpdateBoundingBox(TruckBoxBack, truckPosition);
    TruckBoxLeft = UpdateBoundingBox(TruckBoxLeft, truckPosition);
    TruckBoxRight = UpdateBoundingBox(TruckBoxRight, truckPosition);
    TruckBoxTires[0] = UpdateBoundingBox(TruckBoxTires[0], truckPosition);
    TruckBoxTires[1] = UpdateBoundingBox(TruckBoxTires[1], truckPosition);
    TruckBoxTires[2] = UpdateBoundingBox(TruckBoxTires[2], truckPosition);
    TruckBoxTires[3] = UpdateBoundingBox(TruckBoxTires[3], truckPosition);
}

#endif // TRUCK_H
