#ifndef BG_H
#define BG_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h> 
#include <stdbool.h>
//me
#include "timer.h"

#define MAX_BG_PER_TYPE_AT_ONCE 16

typedef enum {
    BG_NONE = -1,//probably do not use
    BG_GHOST,
    BG_TYPE_COUNT
} BadGuyType;

typedef struct {
    bool isInUse;
    BadGuyType type;
    Model model;
    Texture tex;
    Shader shader;
    BoundingBox origBox, origBodyBox, origHeadBox;
} BadGuyBorrowModel; //for borrowing models for bad guy instances

BadGuyBorrowModel* bgModelBorrower;

typedef struct {
    bool active;
    bool dead;
    BadGuyType type;
    Vector3 spawnPoint;
    float spawnRadius;
    Timer respawnTimer;
    int gbm_index; //global borrowed model index
    int state;
    int curAnim;
    Vector3 pos;
    float yaw, pitch, roll;
    BoundingBox box, bodyBox, headBox;
} BadGuy; //instance of a bad guy, will borrow its model

int total_bg_models_all_types;

void InitBadGuyModels()
{
    total_bg_models_all_types = MAX_BG_PER_TYPE_AT_ONCE * BG_TYPE_COUNT;
    BadGuyBorrowModel* bgModelBorrower = (BadGuyBorrowModel*)malloc(sizeof(BadGuyBorrowModel) * total_bg_models_all_types);
    for (int bg_t = 0; bg_t < BG_TYPE_COUNT; bg_t++)
    {
        for (int i = 0; i < MAX_BG_PER_TYPE_AT_ONCE; i++)
        {
            int index = i+(bg_t * MAX_BG_PER_TYPE_AT_ONCE);
            bgModelBorrower[index].type = bg_t;
            if (bg_t == BG_GHOST)
            {

            }
        }
    }
}

#endif // BG_H
