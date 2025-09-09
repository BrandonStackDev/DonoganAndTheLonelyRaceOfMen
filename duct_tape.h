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



#define gEnvBoundingBoxCount 9

EnvBoundingBox gEnvBoundingBoxes[gEnvBoundingBoxCount] = { 0 };

void GoGoGadgetDuctTape()
{
    gEnvBoundingBoxes[0] = (EnvBoundingBox){EBBT_WALL,(BoundingBox) {(Vector3) {2960.0f, 320.0f, 4026.0f},(Vector3) {2987.0f, 340.0f, 4027.0f}}};//don home side wall one
    gEnvBoundingBoxes[1] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 2960.0f, 320.0f, 4058.0f },(Vector3) { 2987.0f, 340.0f, 4059.0f } } };//son home side wall
    gEnvBoundingBoxes[2] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 1281.0f, 327.0f, 1270.0f },(Vector3) { 1288.0f, 340.0f, 1272.0f } } };//truck short
    gEnvBoundingBoxes[3] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 1259.0f, 327.0f, 1270.0f },(Vector3) { 1266.0f, 340.0f, 1272.0f } } };//truck short
    gEnvBoundingBoxes[4] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 1257.8f, 327.0f, 1243.0f },(Vector3) { 1261.0f, 340.0f, 1270.0f } } };//truck side
    gEnvBoundingBoxes[5] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 1286.0f, 327.0f, 1243.0f },(Vector3) { 1289.2f, 340.0f, 1270.0f } } };//truck other side
    gEnvBoundingBoxes[6] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 1259.0f, 327.0f, 1240.0f },(Vector3) { 1288.0f, 340.0f, 1244.0f } } };//truck back wall
    gEnvBoundingBoxes[7] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { -2331.0f, 312, 3378.0f },(Vector3) { -2198.0f, 330, 3550.0f } } };//temple wall
    gEnvBoundingBoxes[8] = (EnvBoundingBox){ EBBT_GROUND,(BoundingBox) { (Vector3) { -2331.0f, 330, 3378.0f },(Vector3) { -2198.0f, 333, 3550.0f } } };//temple ground
}

#endif // DUCT_TAPE_H
