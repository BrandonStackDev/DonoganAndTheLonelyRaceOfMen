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
#include "texture.h"
#include "donogan.h"

typedef enum {
    MODEL_HOME_NONE = -1,
    MODEL_HOME_CABIN, //001
    MODEL_HOME_BRICK, //002
    MODEL_HOME_NICE, //003
    MODEL_HOME_TENT, //teepee, 004
    MODEL_HOME_TEMPLE, //temple
    MODEL_HOME_CASTLE, //castle, cool texture!
    MODEL_HOME_WINDMILL, //windmill, will have spinning rotor
    MODEL_BARN,         //barn that holds da wrench
    MODEL_TREE_HOUSE,
    MODEL_HOME_COTTAGE,
    MODEL_HOME_COZY,
    MODEL_HOME_OLE_HOOSE,
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
    SCENE_HOME_TEMPLE_01,
    SCENE_HOME_CASTLE_01,
    SCENE_HOME_WINDMILL_01,
    SCENE_HOME_WINDMILL_02,
    SCENE_HOME_WINDMILL_03,
    SCENE_HOME_WINDMILL_04,
    SCENE_HOME_WINDMILL_05,
    SCENE_HOME_WINDMILL_06,
    SCENE_HOME_WINDMILL_07,
    SCENE_HOME_WINDMILL_08,
    SCENE_HOME_WINDMILL_09,
    SCENE_HOME_WINDMILL_10,
    SCENE_HOME_WINDMILL_11,
    SCENE_HOME_BARN_01,
    SCENE_TREE_HOUSE_01,
    SCENE_TREE_HOUSE_02,
    SCENE_HOME_COTTAGE_01,
    SCENE_HOME_COTTAGE_02,
    SCENE_HOME_COTTAGE_03,
    SCENE_HOME_COZY_01,
    SCENE_HOME_COZY_02,
    SCENE_HOME_OLE_01,
    SCENE_TOTAL_COUNT
} Scene_Type;

typedef struct {
    Scene_Type type;
    Model_Home_Type modelType;
    Vector3 pos;
    float yaw, scale;//for now just yaw rotations
    //below this line, only use in preview.c
    BoundingBox origBox, box;
    bool active;
} Scene;

Scene Scenes[SCENE_TOTAL_COUNT];
Model HomeModels[MODEL_HOME_TOTAL_COUNT];

#define WATER_WHEEL_COUNT 1
#define WATER_WHEEL_BUCKET_COUNT 6

typedef struct WaterWheel {
    Vector3 pos;
    float yaw;
    float scale;
    float spin;
    float spinSpeed;

    Model rotor;
    Texture2D rotorTex;

    Model bigRing;
    Model smallRing;
    Model axle;
    Model bucket;
    Model mount;

    BoundingBox box;
} WaterWheel;

WaterWheel gWaterWheels[WATER_WHEEL_COUNT];

static inline BoundingBox RotateScaleTranslateBoundingBoxY(BoundingBox orig, Vector3 pos, float scale, float yaw)
{
    // scale local box first
    Vector3 localMin = {
        orig.min.x * scale,
        orig.min.y * scale,
        orig.min.z * scale
    };
    Vector3 localMax = {
        orig.max.x * scale,
        orig.max.y * scale,
        orig.max.z * scale
    };

    float c = cosf(yaw);
    float s = sinf(yaw);

    Vector3 mn = { FLT_MAX,  FLT_MAX,  FLT_MAX };
    Vector3 mx = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (int ix = 0; ix < 2; ++ix)
        for (int iy = 0; iy < 2; ++iy)
            for (int iz = 0; iz < 2; ++iz)
            {
                Vector3 p = {
                    ix ? localMax.x : localMin.x,
                    iy ? localMax.y : localMin.y,
                    iz ? localMax.z : localMin.z
                };

                // rotate around Y in local space
                Vector3 r = {
                    p.x * c - p.z * s,
                    p.y,
                    p.x * s + p.z * c
                };

                // translate to world
                r = Vector3Add(r, pos);

                if (r.x < mn.x) mn.x = r.x;
                if (r.y < mn.y) mn.y = r.y;
                if (r.z < mn.z) mn.z = r.z;
                if (r.x > mx.x) mx.x = r.x;
                if (r.y > mx.y) mx.y = r.y;
                if (r.z > mx.z) mx.z = r.z;
            }

    return (BoundingBox) { mn, mx };
}

void InitHomes() {
    // Load the models
    Model home001 = LoadModel("models/home_001.obj");
    home001.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/home_001.png");
    HomeModels[MODEL_HOME_CABIN] = home001;
    Model home002 = LoadModel("models/home_002.obj");
    home002.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/home_002.png");
    HomeModels[MODEL_HOME_BRICK] = home002;
    Model home003 = LoadModel("models/home_003.obj");
    home003.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/home_003.png");
    HomeModels[MODEL_HOME_NICE] = home003;
    Model home004 = LoadModel("models/home_004.obj");
    home004.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/home_004.png");
    HomeModels[MODEL_HOME_TENT] = home004;
    Model home005 = LoadModel("models/home_005.obj");
    home005.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/home_005.png");
    HomeModels[MODEL_HOME_TEMPLE] = home005;
    Model home006 = LoadModel("models/home_006.obj");
    home006.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/home_006.png");
    HomeModels[MODEL_HOME_CASTLE] = home006;
    Model home007 = LoadModel("models/home_007.obj");
    home007.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/home_007.png");
    HomeModels[MODEL_HOME_WINDMILL] = home007;
    Model home008 = LoadModel("models/barn.obj");
    home008.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/barn.png");
    HomeModels[MODEL_BARN] = home008;
    Model home009 = LoadModel("models/treehouse.obj");
    home009.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/treehouse.png");
    HomeModels[MODEL_TREE_HOUSE] = home009;
    Model home010 = LoadModel("models/cottage.obj");
    home010.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/cottage.png");
    HomeModels[MODEL_HOME_COTTAGE] = home010;
    Model home011 = LoadModel("models/cozy.obj");
    home011.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/cozy.png");
    HomeModels[MODEL_HOME_COZY] = home011;
    Model home012 = LoadModel("models/oleHoose.obj");
    home012.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/oleHoose.png");
    HomeModels[MODEL_HOME_OLE_HOOSE] = home012;

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
    Scenes[SCENE_HOME_TEMPLE_01] = (Scene){ ////greek temple
        .type = SCENE_HOME_TEMPLE_01,
        .modelType = MODEL_HOME_TEMPLE,
        .pos = (Vector3){ -2214.00f, 350.00f, 3463.00f },
        .yaw = PI/2.0f,          // adjust if you want a different facing
        .scale = 128.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_CASTLE_01] = (Scene){ ////castle!
        .type = SCENE_HOME_CASTLE_01,
        .modelType = MODEL_HOME_CASTLE,
        .pos = (Vector3){ 2360.00f, 572.00f, 750.00f },
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 256.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_WINDMILL_01] = (Scene){ ////windmill! by castle
        .type = SCENE_HOME_WINDMILL_01,
        .modelType = MODEL_HOME_WINDMILL,
        .pos = (Vector3){ 2662.00f, 502.40f, 1475.00f }, //started at 472 , y from measured => min +24 - +30 max
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 32.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_WINDMILL_02] = (Scene){ ////windmill! by donogans
        .type = SCENE_HOME_WINDMILL_02,
        .modelType = MODEL_HOME_WINDMILL,
        .pos = (Vector3){ 2296.00f, 367.00f, 4754.00f }, //started at 472 , y from measured => min +24 - +30 max
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 32.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_WINDMILL_03] = (Scene){ ////windmill! --far side of castle first one
        .type = SCENE_HOME_WINDMILL_03,
        .modelType = MODEL_HOME_WINDMILL,
        .pos = (Vector3){ 2055.00f, 487.00f, 195.00f }, //started at 472 , y from measured => min +24 - +30 max
        .yaw = PI / 2.0f,          // adjust if you want a different facing
        .scale = 32.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_WINDMILL_04] = (Scene){ ////windmill! --middle one
        .type = SCENE_HOME_WINDMILL_04,
        .modelType = MODEL_HOME_WINDMILL,
        .pos = (Vector3){ 1933.00f, 432.00f, -533.00f }, //started at 472 , y from measured => min +24 - +30 max
        .yaw = PI / 2.0f,          // adjust if you want a different facing
        .scale = 32.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_WINDMILL_05] = (Scene){ ////windmill! middle 2 castle far side
        .type = SCENE_HOME_WINDMILL_05,
        .modelType = MODEL_HOME_WINDMILL,
        .pos = (Vector3){ 1814.00f, 492.40f, -1185.00f }, //started at 472 , y from measured => min +24 - +30 max
        .yaw = PI / 2.0f,          // adjust if you want a different facing
        .scale = 32.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_WINDMILL_06] = (Scene){ ////windmill! --far side castle second to last
        .type = SCENE_HOME_WINDMILL_06,
        .modelType = MODEL_HOME_WINDMILL,
        .pos = (Vector3){ 1785.00f, 534.00f, -1723.00f }, //started at 472 , y from measured => min +24 - +30 max
        .yaw = PI / 2.0f,          // adjust if you want a different facing
        .scale = 32.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_WINDMILL_07] = (Scene){ ////windmill! --far side castle furthest and turned
        .type = SCENE_HOME_WINDMILL_07,
        .modelType = MODEL_HOME_WINDMILL,
        .pos = (Vector3){ 1942.00f, 410.00f, -2710.00f }, //started at 472 , y from measured => min +24 - +30 max
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 32.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_WINDMILL_08] = (Scene){ ////windmill! --far side of TOL further away close to beach
        .type = SCENE_HOME_WINDMILL_08,
        .modelType = MODEL_HOME_WINDMILL,
        .pos = (Vector3){ -2666.00f, 435.00f, -2406.00f }, //started at 472 , y from measured => min +24 - +30 max
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 32.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_WINDMILL_09] = (Scene){ ////windmill! --yeti mt 1
        .type = SCENE_HOME_WINDMILL_09,
        .modelType = MODEL_HOME_WINDMILL,
        .pos = (Vector3){ -712.00f, 816.00f, 2876.00f }, //started at 472 , y from measured => min +24 - +30 max
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 32.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_WINDMILL_10] = (Scene){ ////windmill! //windmill by the barn
        .type = SCENE_HOME_WINDMILL_10,
        .modelType = MODEL_HOME_WINDMILL,
        .pos = (Vector3){ 922.00f, 353.00f, 1998.00f }, //started at 472 , y from measured => min +24 - +30 max
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 32.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_WINDMILL_11] = (Scene){ ////windmill! -- windmill past TOL first one
        .type = SCENE_HOME_WINDMILL_11,
        .modelType = MODEL_HOME_WINDMILL,
        .pos = (Vector3){ -1696.00f, 465.0f, -1714.00f }, //started at 472 , y from measured => min +24 - +30 max
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 32.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_BARN_01] = (Scene){ ////barn
        .type = SCENE_HOME_BARN_01,
        .modelType = MODEL_BARN,
        .pos = (Vector3){ 875.34, 374.00, 1353.11 },
        .yaw = PI / 2.0f, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 46.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_TREE_HOUSE_01] = (Scene){ ////by donogans
        .type = SCENE_TREE_HOUSE_01,
        .modelType = MODEL_TREE_HOUSE,
        .pos = (Vector3){ 2725.00, 436.00, 4117.71 },
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 8.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_TREE_HOUSE_02] = (Scene){ ////treehouse by windmill at the top of the map
        .type = SCENE_TREE_HOUSE_02,
        .modelType = MODEL_TREE_HOUSE,
        .pos = (Vector3){ -2417.00, 412.00, -2570.00},
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 8.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_COTTAGE_01] = (Scene){ ////cottage near middle of map
        .type = SCENE_HOME_COTTAGE_01,
        .modelType = MODEL_HOME_COTTAGE,
        .pos = (Vector3){ -1214.70, 400, 603.48},
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 24.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_COTTAGE_02] = (Scene){ ////cottage near middle of map
        .type = SCENE_HOME_COTTAGE_02,
        .modelType = MODEL_HOME_COTTAGE,
        .pos = (Vector3){ -600.53, 810.34, 2838.54 },
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 30.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_COTTAGE_03] = (Scene){ ////cottage near middle of map
        .type = SCENE_HOME_COTTAGE_03,
        .modelType = MODEL_HOME_COTTAGE,
        .pos = (Vector3){ 2500.96, 347, 2418.06},
        .yaw = PI, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 30.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_COZY_01] = (Scene){ ////abby's?
        .type = SCENE_HOME_COZY_01,
        .modelType = MODEL_HOME_COZY,
        .pos = (Vector3){ 2267.08, 345, -3420.73 },
        .yaw = PI, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 24.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_COZY_02] = (Scene){ ////abby's?
        .type = SCENE_HOME_COZY_02,
        .modelType = MODEL_HOME_COZY,
        .pos = (Vector3){ 2622.03, 345, 2466.80 },
        .yaw = PI / 2.0f, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 24.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_OLE_01] = (Scene){ ////near store 1
        .type = SCENE_HOME_OLE_01,
        .modelType = MODEL_HOME_OLE_HOOSE,
        .pos = (Vector3){ 2550, 354, 2280 },
        .yaw = PI, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 32.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    for (int i = 0; i < SCENE_TOTAL_COUNT; i++)
    {
        BoundingBox original = GetModelBoundingBox(HomeModels[Scenes[i].modelType]);
        Scenes[i].origBox = RotateScaleTranslateBoundingBoxY(original, (Vector3) {0,0,0}, Scenes[i].scale, Scenes[i].yaw);
        Scenes[i].box = UpdateBoundingBox(Scenes[i].origBox, Scenes[i].pos);
    }
}

static inline void WaterWheel_Init(void)
{
    WaterWheel* w = &gWaterWheels[0];
    memset(w, 0, sizeof(*w));

    w->pos = (Vector3){ -1660, 322, -1400 }; //
    w->yaw = 0.0f;
    w->scale = 16.0f;
    w->spin = 0.0f;
    w->spinSpeed = 70.0f; // degrees/sec

    w->rotor = LoadModel("models/ww_rotor.obj");
    w->rotorTex = LoadMyTexture("textures/ww_rotor.png");
    w->rotor.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = w->rotorTex;

    // low segment torus = blocky wooden wheel
    w->bigRing = LoadModelFromMesh(GenMeshTorus(3.2f, 0.18f, 12, 4));
    w->smallRing = LoadModelFromMesh(GenMeshTorus(0.85f, 0.14f, 10, 4));

    w->bigRing.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = BROWN;
    w->smallRing.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = BROWN;

    // long axle through middle, toward back mount
    w->axle = LoadModelFromMesh(GenMeshCylinder(0.22f, 3.2f, 8));
    w->axle.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = BROWN;

    // trough / paddle bucket: simple box for now
    w->bucket = LoadModelFromMesh(GenMeshCube(1.15f, 0.32f, 0.42f));
    w->bucket.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = BROWN;

    // non-spinning back machine/mount
    w->mount = LoadModelFromMesh(GenMeshCube(1.2f, 2.2f, 1.2f));
    w->mount.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = DARKBROWN;

    w->box = (BoundingBox){
        (Vector3) {
 w->pos.x - 5, w->pos.y - 5, w->pos.z - 5
},
(Vector3) {
w->pos.x + 5, w->pos.y + 5, w->pos.z + 5
}
    };
}

static inline void WaterWheel_Update(float dt)
{
    for (int i = 0; i < WATER_WHEEL_COUNT; i++)
    {
        WaterWheel* w = &gWaterWheels[i];
        w->spin += w->spinSpeed * dt;
        if (w->spin > 360.0f) w->spin -= 360.0f;
    }
}

//static inline Matrix WaterWheel_BaseMatrix(WaterWheel* w)
//{
//    Matrix S = MatrixScale(w->scale, w->scale, w->scale);
//    Matrix R = MatrixRotateY(w->yaw);
//    Matrix T = MatrixTranslate(w->pos.x, w->pos.y, w->pos.z);
//    return MatrixMultiply(MatrixMultiply(S, R), T);
//}
static inline Matrix WaterWheel_BaseMatrix(WaterWheel* w)
{
    Matrix S = MatrixScale(w->scale, w->scale, w->scale);

    // yaw placement + fixed 90 degree local turn for the whole wheel assembly
    Matrix R = MatrixRotateY(w->yaw + -90.0f * DEG2RAD);

    Matrix T = MatrixTranslate(w->pos.x, w->pos.y, w->pos.z);

    return MatrixMultiply(MatrixMultiply(S, R), T);
}
static inline Matrix WaterWheel_BaseMatrixRotor(WaterWheel* w)
{
    Matrix S = MatrixScale(w->scale*2, w->scale*2, w->scale*2);

    // yaw placement + fixed 90 degree local turn for the whole wheel assembly
    Matrix R = MatrixRotateY(w->yaw + -90.0f * DEG2RAD);

    Matrix T = MatrixTranslate(w->pos.x, w->pos.y, w->pos.z);

    return MatrixMultiply(MatrixMultiply(S, R), T);
}
static inline void WaterWheel_DrawOne(WaterWheel* w)
{
    Matrix base = WaterWheel_BaseMatrix(w);
    Matrix baseRotor = WaterWheel_BaseMatrixRotor(w);

    // Spin around local X axis.
    // If it spins wrong, change this to MatrixRotateZ(w->spin*DEG2RAD)
    Matrix spin = MatrixRotateX(w->spin * DEG2RAD);

    // side offsets
    Matrix leftSide = MatrixTranslate(-0.55f, 0.0f, 0.0f);
    Matrix rightSide = MatrixTranslate(0.55f, 0.0f, 0.0f);

    // torus default orientation may need a 90 degree turn
    Matrix torusFace = MatrixRotateY(90.0f * DEG2RAD);

    // spinning rotor center
    Matrix rotorM = MatrixMultiply(spin, baseRotor);
    //DrawModel(w->rotor, (Vector3) { 0 }, w->scale * 4, WHITE);*/
    // Better explicit matrix draw for custom mesh pieces
    DrawMesh(w->rotor.meshes[0], w->rotor.materials[0], rotorM);

    // big rings, both sides
    DrawMesh(w->bigRing.meshes[0], w->bigRing.materials[0],
        MatrixMultiply(MatrixMultiply(torusFace, MatrixMultiply(leftSide, spin)), base));

    DrawMesh(w->bigRing.meshes[0], w->bigRing.materials[0],
        MatrixMultiply(MatrixMultiply(torusFace, MatrixMultiply(rightSide, spin)), base));

    // small rings, both sides
    DrawMesh(w->smallRing.meshes[0], w->smallRing.materials[0],
        MatrixMultiply(MatrixMultiply(torusFace, MatrixMultiply(leftSide, spin)), base));

    DrawMesh(w->smallRing.meshes[0], w->smallRing.materials[0],
        MatrixMultiply(MatrixMultiply(torusFace, MatrixMultiply(rightSide, spin)), base));

    // 6 paddle/trough boxes around wheel
    for (int k = 0; k < WATER_WHEEL_BUCKET_COUNT; k++)
    {
        float a = ((float)k / (float)WATER_WHEEL_BUCKET_COUNT) * 2.0f * PI;

        float r = 1.96f;

        // Start at top of wheel, then orbit that point around local X.
        Matrix orbit = MatrixRotateX(a);
        Matrix pushOut = MatrixTranslate(0.0f, r, 0.0f);

        // Optional: rotate bucket itself so it follows the wheel angle.
        Matrix bucketFace = MatrixRotateX(a);

        Matrix bucketLocal = MatrixMultiply(bucketFace, pushOut);
        bucketLocal = MatrixMultiply(bucketLocal, orbit);

        Matrix bucketM = MatrixMultiply(MatrixMultiply(bucketLocal, spin), base);

        DrawMesh(w->bucket.meshes[0], w->bucket.materials[0], bucketM);
    }

    // axle spins too
    Matrix axleFace = MatrixRotateZ(90.0f * DEG2RAD);
    DrawMesh(w->axle.meshes[0], w->axle.materials[0],
        MatrixMultiply(MatrixMultiply(axleFace, spin), base));

    // back mount DOES NOT spin
    /*Matrix mountLocal = MatrixTranslate(0.0f, 0.0f, -2.15f);
    DrawMesh(w->mount.meshes[0], w->mount.materials[0],
        MatrixMultiply(mountLocal, base));*/
        // back mount DOES NOT spin
    // Behind rotor is local X after your wheel yaw fix.
    // If it goes in front, flip -2.15f to +2.15f.
    Matrix mountLocal = MatrixTranslate(-3.4f, 0.0f, 0.0f);

    DrawMesh(w->mount.meshes[0], w->mount.materials[0],MatrixMultiply(mountLocal, base));
}

static void WaterWheel_Draw(Donogan* d)
{
    for (int i = 0; i < WATER_WHEEL_COUNT; i++)
    {
        if (Vector3Distance(d->pos, gWaterWheels[i].pos) > 888) { continue; }
        WaterWheel_DrawOne(&gWaterWheels[i]);
    }
}
#endif // JC_H
