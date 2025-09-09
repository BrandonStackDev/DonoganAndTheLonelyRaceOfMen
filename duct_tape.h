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



#define gEnvBoundingBoxCount 18

EnvBoundingBox gEnvBoundingBoxes[gEnvBoundingBoxCount] = { 0 };

void GoGoGadgetDuctTape()
{
    gEnvBoundingBoxes[0] = (EnvBoundingBox){EBBT_WALL,(BoundingBox) {(Vector3) {2960.0f, 320.0f, 4026.0f},(Vector3) {2987.0f, 340.0f, 4027.0f}}};//don home side wall one
    gEnvBoundingBoxes[1] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 2960.0f, 320.0f, 4058.0f },(Vector3) { 2987.0f, 340.0f, 4059.0f } } };//don home side wall
    gEnvBoundingBoxes[2] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 1281.0f, 327.0f, 1270.0f },(Vector3) { 1288.0f, 340.0f, 1272.0f } } };//truck short
    gEnvBoundingBoxes[3] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 1259.0f, 327.0f, 1270.0f },(Vector3) { 1266.0f, 340.0f, 1272.0f } } };//truck short
    gEnvBoundingBoxes[4] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 1257.8f, 327.0f, 1243.0f },(Vector3) { 1261.0f, 340.0f, 1270.0f } } };//truck side
    gEnvBoundingBoxes[5] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 1286.0f, 327.0f, 1243.0f },(Vector3) { 1289.2f, 340.0f, 1270.0f } } };//truck other side
    gEnvBoundingBoxes[6] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 1259.0f, 327.0f, 1240.0f },(Vector3) { 1288.0f, 340.0f, 1244.0f } } };//truck back wall
    gEnvBoundingBoxes[7] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { -2331.0f, 312, 3378.0f },(Vector3) { -2198.0f, 330, 3550.0f } } };//temple wall
    gEnvBoundingBoxes[8] = (EnvBoundingBox){ EBBT_GROUND,(BoundingBox) { (Vector3) { -2331.0f, 330, 3378.0f },(Vector3) { -2198.0f, 333, 3550.0f } } };//temple ground
    gEnvBoundingBoxes[9] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 1900.0f, 360, 4119.0f },(Vector3) { 1909.0f, 370, 4181.0f } } };//nice 01 wall 1
    gEnvBoundingBoxes[10] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 1823.0f, 360, 4176.0f },(Vector3) { 1842.0f, 368.6f, 4183.0f } } };//nice 01 wall 2
    gEnvBoundingBoxes[11] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { -685, 560, 3723.0f },(Vector3) { -662.0f, 570, 3752.0f } } };//nice 02 wall 1
    gEnvBoundingBoxes[12] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { -605.0f, 558, 3690.0f },(Vector3) { -599.0f, 572, 3752.0f } } };//nice 02 wall 2
    gEnvBoundingBoxes[13] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 2626, 333, 4490 },(Vector3) { 2749, 350, 4493 } } };//brick book temple wall
    gEnvBoundingBoxes[14] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 2708, 320, 4614 },(Vector3) { 2751, 350, 4620.5f } } };//brick book temple wall small 01
    gEnvBoundingBoxes[15] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 2627, 320, 4614 },(Vector3) { 2670, 350, 4620.5f } } };//brick book temple wall small 02
    gEnvBoundingBoxes[16] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 2389, 520, 551 },(Vector3) { 2404, 540, 922 } } };//castle wall 1
    gEnvBoundingBoxes[17] = (EnvBoundingBox){ EBBT_WALL,(BoundingBox) { (Vector3) { 2140, 520, 540 },(Vector3) { 2390, 540, 552 } } };//castle wall 2
}

#endif // DUCT_TAPE_H
