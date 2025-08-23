#ifndef TRUCK_H
#define TRUCK_H

// Includes
#include "raylib.h"
#include "raymath.h"

#define MAX_TURN_ANGLE 0.25f //radians
typedef enum {
    GROUND,
    AIRBORNE,
    LANDING
} Truck_Air_State;


#endif // TRUCK_H
