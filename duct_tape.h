#ifndef DUCT_TAPE_H
#define DUCT_TAPE_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h> 
#include <stdbool.h>

// Type Definitions
typedef enum 
{
    EBBT_GROUND,
    EBBT_WALL,
} EnvBoundingBoxType;

typedef struct
{
    EnvBoundingBoxType type;     // ground/wall/etc.
    BoundingBox box;             // Raylib AABB (min/max)
} EnvBoundingBox;



#define gEnvBoundingBoxCount 2

EnvBoundingBox gEnvBoundingBoxes[gEnvBoundingBoxCount] = { 0 };

void GoGoGadgetDuctTape()
{
    gEnvBoundingBoxes[0] = (EnvBoundingBox){EBBT_WALL,(BoundingBox) {(Vector3) {2960.0f, 320.0f, 4026.0f},(Vector3) {2987.0f, 330.0f, 4027.0f}}};
    gEnvBoundingBoxes[1] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 2960.0f, 320.0f, 4058.0f },(Vector3) { 2987.0f, 330.0f, 4059.0f } } };
}

#endif // DUCT_TAPE_H
