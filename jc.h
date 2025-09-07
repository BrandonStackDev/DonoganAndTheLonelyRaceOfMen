#ifndef JC_H
#define JC_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h> 
#include <stdbool.h>

//me
#include "core.h"

typedef enum {
    MODEL_HOME_NONE = -1,
    MODEL_HOME_CABIN, //001
    MODEL_HOME_BRICK, //002
    MODEL_HOME_NICE, //003
    MODEL_HOME_TENT, //teepee, 004
    MODEL_HOME_TOTAL_COUNT
} Model_Home_Type;

typedef enum {
   SCENE_NONE = -1,
    SCENE_HOME_CABIN_01,
    SCENE_HOME_CABIN_02,
    SCENE_HOME_BRICK_01,
    SCENE_HOME_NICE_01,
    SCENE_HOME_NICE_02,
    SCENE_HOME_TENT_01,
    SCENE_HOME_TENT_02,
    SCENE_HOME_TENT_03,
    SCENE_HOME_TENT_04,
    SCENE_TOTAL_COUNT
} Scene_Type;

typedef struct {
    Scene_Type type;
    Model_Home_Type modelType;
    Vector3 pos;
    float yaw, scale;//for now just yaw rotations
    //below this line, only use in preview.c
    BoundingBox origBox, box;
} Scene;

Scene Scenes[SCENE_TOTAL_COUNT];
Model HomeModels[MODEL_HOME_TOTAL_COUNT];

void InitHomes() {
    // Load the models
    Model home001 = LoadModel("models/home_001.obj");
    home001.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("textures/home_001.png");
    HomeModels[MODEL_HOME_CABIN] = home001;
    Model home002 = LoadModel("models/home_002.obj");
    home002.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("textures/home_002.png");
    HomeModels[MODEL_HOME_BRICK] = home002;
    Model home003 = LoadModel("models/home_003.obj");
    home003.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("textures/home_003.png");
    HomeModels[MODEL_HOME_NICE] = home003;
    Model home004 = LoadModel("models/home_004.obj");
    home004.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("textures/home_004.png");
    HomeModels[MODEL_HOME_TENT] = home004;

    // Populate the scene list (the important part)
    Scenes[SCENE_HOME_CABIN_01] = (Scene){ //where the truck is
        .type = SCENE_HOME_CABIN_01,
        .modelType = MODEL_HOME_CABIN,
        .pos = (Vector3){ 1273.47f, 327.12f, 1256.42f },
        .yaw = 0.0f,          // adjust if you want a different facing
        .scale = 16.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_CABIN_02] = (Scene){ //where donogan starts
        .type = SCENE_HOME_CABIN_02,
        .modelType = MODEL_HOME_CABIN,
        .pos = (Vector3){ 2973.70f, 319.00f, 4042.42f },
        .yaw = PI/2.0f,          // adjust if you want a different facing
        .scale = 16.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_BRICK_01] = (Scene){ //curved inside near donogans home
        .type = SCENE_HOME_BRICK_01,
        .modelType = MODEL_HOME_BRICK,
        .pos = (Vector3){ 2689.00f, 342.00f, 4555.00f },
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 64.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_NICE_01] = (Scene){ //where donogan starts, same island, nice home!
        .type = SCENE_HOME_NICE_01,
        .modelType = MODEL_HOME_NICE,
        .pos = (Vector3){ 1866.00f, 374.00f, 4151.00f },
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 42.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_NICE_02] = (Scene){ //where donogan starts, same island, nice home!
        .type = SCENE_HOME_NICE_02,
        .modelType = MODEL_HOME_NICE,
        .pos = (Vector3){ -642.00f, 576.00f, 3721.00f },
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 42.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_TENT_01] = (Scene){ //Indian settlement!
        .type = SCENE_HOME_TENT_01,
        .modelType = MODEL_HOME_TENT,
        .pos = (Vector3){ -3886.00f, 345.00f, 1206.00f },
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 24.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_TENT_02] = (Scene){ ////Indian settlement!
        .type = SCENE_HOME_TENT_02,
        .modelType = MODEL_HOME_TENT,
        .pos = (Vector3){ -3850.00f, 342.00f, 960.00f },
        .yaw = PI,          // adjust if you want a different facing
        .scale = 24.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_TENT_03] = (Scene){ ////Indian settlement!
        .type = SCENE_HOME_TENT_03,
        .modelType = MODEL_HOME_TENT,
        .pos = (Vector3){ -3721.00f, 346.00f, 1109.00f },
        .yaw = -PI / 2.0f,          // adjust if you want a different facing
        .scale = 24.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_TENT_04] = (Scene){ ////Indian settlement!
        .type = SCENE_HOME_TENT_04,
        .modelType = MODEL_HOME_TENT,
        .pos = (Vector3){ -3753.00f, 350.00f, 1355.00f },
        .yaw = PI,          // adjust if you want a different facing
        .scale = 24.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    for (int i = 0; i < SCENE_TOTAL_COUNT; i++)
    {
        Scenes[i].origBox = ScaleBoundingBox(GetModelBoundingBox(HomeModels[Scenes[i].modelType]), Scenes[i].scale);
        Scenes[i].box = UpdateBoundingBox(Scenes[i].origBox, Scenes[i].pos);
    }
}


#endif // JC_H
