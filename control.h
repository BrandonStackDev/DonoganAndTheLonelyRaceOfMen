#ifndef CONTROL_H
#define CONTROL_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include <inttypes.h>
#include <string.h>

///////////////////////CONTROLLER STUFF////////////////////////////////////////////
// Controller state with the exact field names preview.c uses
typedef struct {
    float lx;
    float ly;
    float rx;
    float ry;
    float normLX;
    float normLY;
    float normRX;
    float normRY;
    uint8_t buttons1;
    uint8_t buttons2;
    uint8_t buttons3;
    int dpad;
    int dpad_up;
    int dpad_down;
    int dpad_left;
    int dpad_right;
    int btnSquare;
    int btnCross;
    int btnCircle;
    int btnTriangle;
    int btnL1;
    int btnR1;
    int btnL2;
    int btnR2;
    int btnL3;
    int btnR3;
    int btnStart;
    int btnSelect;
} ControllerData;

// You read this name elsewhere; make it real.
static bool contInvertY = false;

// Poll raylib and fill the state above. Returns true if a pad is present.
static bool ReadControllerWindows(int index, ControllerData* out)
{
    memset(out, 0, sizeof(*out));
    if (!IsGamepadAvailable(index)) return false;

    // Axes
    out->lx = GetGamepadAxisMovement(index, GAMEPAD_AXIS_LEFT_X);
    out->ly = GetGamepadAxisMovement(index, GAMEPAD_AXIS_LEFT_Y);
    out->rx = GetGamepadAxisMovement(index, GAMEPAD_AXIS_RIGHT_X);
    out->ry = GetGamepadAxisMovement(index, GAMEPAD_AXIS_RIGHT_Y);

    // Simple deadzone to produce the *norm* fields your code uses
    const float DZ = 0.15f;
    out->normLX = (fabsf(out->lx) < DZ) ? 0 : out->lx;
    out->normLY = (fabsf(out->ly) < DZ) ? 0 : out->ly;
    out->normRX = (fabsf(out->rx) < DZ) ? 0 : out->rx;
    out->normRY = (fabsf(out->ry) < DZ) ? 0 : out->ry;

    // Face buttons: map to PS naming used in the file
    out->btnSquare = IsGamepadButtonDown(index, GAMEPAD_BUTTON_RIGHT_FACE_LEFT);
    out->btnCross = IsGamepadButtonDown(index, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    out->btnCircle = IsGamepadButtonDown(index, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT);
    out->btnTriangle = IsGamepadButtonDown(index, GAMEPAD_BUTTON_RIGHT_FACE_UP);

    // Bumpers
    out->btnL1 = IsGamepadButtonDown(index, GAMEPAD_BUTTON_LEFT_TRIGGER_1);
    out->btnR1 = IsGamepadButtonDown(index, GAMEPAD_BUTTON_RIGHT_TRIGGER_1);

    // Treat analog triggers as booleans too (your code checks btnL2/btnR2)
    float lt = GetGamepadAxisMovement(index, GAMEPAD_AXIS_LEFT_TRIGGER);
    float rt = GetGamepadAxisMovement(index, GAMEPAD_AXIS_RIGHT_TRIGGER);
    out->btnL2 = lt > 0.5f;
    out->btnR2 = rt > 0.5f;

    // Stick clicks
    out->btnL3 = IsGamepadButtonDown(index, GAMEPAD_BUTTON_LEFT_THUMB);
    out->btnR3 = IsGamepadButtonDown(index, GAMEPAD_BUTTON_RIGHT_THUMB);

    // D-pad
    out->dpad_up = IsGamepadButtonDown(index, GAMEPAD_BUTTON_LEFT_FACE_UP);
    out->dpad_down = IsGamepadButtonDown(index, GAMEPAD_BUTTON_LEFT_FACE_DOWN);
    out->dpad_left = IsGamepadButtonDown(index, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
    out->dpad_right = IsGamepadButtonDown(index, GAMEPAD_BUTTON_LEFT_FACE_RIGHT);

    out->btnStart = IsGamepadButtonDown(index, GAMEPAD_BUTTON_MIDDLE_RIGHT);//start button
    out->btnSelect = IsGamepadButtonDown(index, GAMEPAD_BUTTON_MIDDLE_LEFT);//start button

    return true;
}
void PrintMatrix(Matrix m)
{
    printf("Matrix:\n");
    printf("[ %8.3f %8.3f %8.3f %8.3f ]\n", m.m0, m.m4, m.m8, m.m12);
    printf("[ %8.3f %8.3f %8.3f %8.3f ]\n", m.m1, m.m5, m.m9, m.m13);
    printf("[ %8.3f %8.3f %8.3f %8.3f ]\n", m.m2, m.m6, m.m10, m.m14);
    printf("[ %8.3f %8.3f %8.3f %8.3f ]\n", m.m3, m.m7, m.m11, m.m15);
}

Vector3 RotateY(Vector3 v, float angle) {
    float cs = cosf(angle);
    float sn = sinf(angle);
    return (Vector3) {
        v.x* cs - v.z * sn,
            v.y,
            v.x* sn + v.z * cs
    };
}

Vector3 RotateX(Vector3 v, float angle) {
    float cs = cosf(angle);
    float sn = sinf(angle);
    return (Vector3) {
        v.x,
            v.y* cs - v.z * sn,
            v.y* sn + v.z * cs
    };
}

Vector3 RotateZ(Vector3 v, float angle) {
    float cs = cosf(angle);
    float sn = sinf(angle);
    return (Vector3) {
        v.x* cs - v.y * sn,
            v.x* sn + v.y * cs,
            v.z
    };
}

// Set axis deadzones
const float leftStickDeadzoneX = 0.1f;
const float leftStickDeadzoneY = 0.1f;
const float rightStickDeadzoneX = 0.1f;
const float rightStickDeadzoneY = 0.1f;
const float leftTriggerDeadzone = -0.9f;
const float rightTriggerDeadzone = -0.9f;
//controller input constants for truck
float bounceCollector = 0; //gets set
const float acceleration = 0.0178f;
const float deceleration = 0.046f;
const float steeringSpeed = 1.5f;
const float maxSpeed = 1.54321;
const float maxSpeedReverse = -0.75f;
float steerInput = 0; //gets set
float verticalVelocity = 0; //gets set

static void ApplyKeyboardToControllerData(ControllerData* out)
{
    if (!out) return;

    // Left stick: movement
    float lx = 0.0f;
    float ly = 0.0f;

    if (IsKeyDown(KEY_A)) lx -= 1.0f;
    if (IsKeyDown(KEY_D)) lx += 1.0f;
    if (IsKeyDown(KEY_W)) ly -= 1.0f;
    if (IsKeyDown(KEY_S)) ly += 1.0f;

    // Right stick: camera / aim
    float rx = 0.0f;
    float ry = 0.0f;

    if (IsKeyDown(KEY_LEFT))  rx -= 1.0f;
    if (IsKeyDown(KEY_RIGHT)) rx += 1.0f;
    if (IsKeyDown(KEY_UP))    ry -= 1.0f;
    if (IsKeyDown(KEY_DOWN))  ry += 1.0f;

    // Normalize diagonal keyboard movement so WASD is not faster diagonally.
    float lLen = sqrtf(lx * lx + ly * ly);
    if (lLen > 1.0f) {
        lx /= lLen;
        ly /= lLen;
    }

    float rLen = sqrtf(rx * rx + ry * ry);
    if (rLen > 1.0f) {
        rx /= rLen;
        ry /= rLen;
    }

    // Keyboard overwrites matching controller axes only when used.
    if (lLen > 0.0f) {
        out->lx = lx;
        out->ly = ly;
        out->normLX = lx;
        out->normLY = ly;
    }

    if (rLen > 0.0f) {
        out->rx = rx;
        out->ry = ry;
        out->normRX = rx;
        out->normRY = ry;
    }

    // Face buttons, PS-style
    if (IsKeyDown(KEY_J))          out->btnSquare = 1;   // Square
    if (IsKeyDown(KEY_SPACE))      out->btnCross = 1;    // Cross / X / confirm / jump
    if (IsKeyDown(KEY_K))          out->btnCircle = 1;   // Circle / roll / cancel-ish
    if (IsKeyDown(KEY_E))          out->btnTriangle = 1; // Triangle / interact / exit talk

    // Shoulder / trigger buttons
    if (IsKeyDown(KEY_Q))          out->btnL1 = 1;
    if (IsKeyDown(KEY_R))          out->btnR1 = 1;
    if (IsKeyDown(KEY_LEFT_SHIFT)) out->btnL2 = 1;
    if (IsKeyDown(KEY_F))          out->btnR2 = 1;

    // Stick clicks
    if (IsKeyDown(KEY_LEFT_CONTROL)) out->btnL3 = 1;
    if (IsKeyDown(KEY_C))            out->btnR3 = 1;

    // D-pad: IJKL, since arrow keys are right stick.
    if (IsKeyDown(KEY_I)) out->dpad_up = 1;
    if (IsKeyDown(KEY_M)) out->dpad_down = 1;
    if (IsKeyDown(KEY_U)) out->dpad_left = 1;
    if (IsKeyDown(KEY_O)) out->dpad_right = 1;

    // Menu buttons
    if (IsKeyDown(KEY_ENTER)) out->btnStart = 1;
    if (IsKeyDown(KEY_TAB))   out->btnSelect = 1;

    // Optional packed dpad value, if you ever use out->dpad directly.
    out->dpad = 0;
    if (out->dpad_up)    out->dpad |= 1;
    if (out->dpad_down)  out->dpad |= 2;
    if (out->dpad_left)  out->dpad |= 4;
    if (out->dpad_right) out->dpad |= 8;
}

////////////////////////////////////////////////////////////////////////////////

#endif // CONTROL_H
