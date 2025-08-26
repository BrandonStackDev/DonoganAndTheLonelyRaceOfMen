#ifndef INTERACT_H
#define INTERACT_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h> 
#include <stdbool.h>

typedef enum {
    POI_TYPE_NONE = -1,
    POI_TYPE_TRUCK,
    POI_TYPE_TOTAL_COUNT
} POI_Type;

// Type Definitions
typedef struct {
    POI_Type type;
    Vector3* pos;
} POI;

POI InteractivePoints[POI_TYPE_TOTAL_COUNT];
#endif // INTERACT_H
