#ifndef JC_H
#define JC_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>

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
    MODEL_HOME_OLE_STONE,
    MODEL_CINDER,
    MODEL_CINDER_CAVE,
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
    SCENE_HOME_STONE_01,
    SCENE_HOME_OLE_02,
    SCENE_HOME_BARN_02,
    SCENE_HOME_COTTAGE_04,
    SCENE_HOME_COZY_03,
    SCENE_CINDER,
    SCENE_CINDER_CAVE,
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

    Texture2D wood1Tex;
    Texture2D wood2Tex;

    Model bigRing;
    Model smallRing;
    Model axle;
    Model bucket;
    Model mount;

    BoundingBox box; // rough whole-wheel box

    BoundingBox bucketOrigBox;
    BoundingBox bucketBoxes[WATER_WHEEL_BUCKET_COUNT];

    BoundingBox mountOrigBox;
    BoundingBox mountBox;
} WaterWheel;

WaterWheel gWaterWheels[WATER_WHEEL_COUNT];

Matrix WaterWheel_BucketMatrix(WaterWheel* w, int k);
Matrix WaterWheel_MountMatrix(WaterWheel* w);

//order matters
typedef enum {
    BCOL_FLOOR = 0,
    BCOL_CEILING,
    BCOL_WALL
} BuildingColliderType;

typedef struct {
    BuildingColliderType type;
    // model-local vertices from the .collide.txt file
    Vector3 local[8];
} BuildingColliderRaw;

#define MAX_BUILDING_COLLIDERS_PER_MODEL 512
#define BUILDING_SEAM_MATCH_EPS_LOCAL 0.007f
#define BUILDING_WALL_FLOOR_RELIEF_LOCAL 0.200f

typedef struct {
    BuildingColliderRaw cols[MAX_BUILDING_COLLIDERS_PER_MODEL];
    int count;
    bool loaded;
} BuildingColliderSet;

BuildingColliderSet HomeCollisionSets[MODEL_HOME_TOTAL_COUNT];

//me
#include "core.h"
#include "texture.h"
#include "donogan.h"


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

static inline Vector3 WaterWheel_TransformPoint(Vector3 p, Matrix m)
{
    return (Vector3) {
        p.x* m.m0 + p.y * m.m4 + p.z * m.m8 + m.m12,
            p.x* m.m1 + p.y * m.m5 + p.z * m.m9 + m.m13,
            p.x* m.m2 + p.y * m.m6 + p.z * m.m10 + m.m14
    };
}

static inline BoundingBox WaterWheel_TransformBoundingBox(BoundingBox b, Matrix m)
{
    Vector3 mn = { FLT_MAX,  FLT_MAX,  FLT_MAX };
    Vector3 mx = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (int ix = 0; ix < 2; ix++)
        for (int iy = 0; iy < 2; iy++)
            for (int iz = 0; iz < 2; iz++)
            {
                Vector3 p = {
                    ix ? b.max.x : b.min.x,
                    iy ? b.max.y : b.min.y,
                    iz ? b.max.z : b.min.z
                };

                Vector3 q = WaterWheel_TransformPoint(p, m);

                if (q.x < mn.x) mn.x = q.x;
                if (q.y < mn.y) mn.y = q.y;
                if (q.z < mn.z) mn.z = q.z;

                if (q.x > mx.x) mx.x = q.x;
                if (q.y > mx.y) mx.y = q.y;
                if (q.z > mx.z) mx.z = q.z;
            }

    return (BoundingBox) { mn, mx };
}

static inline BuildingColliderType BuildingColliderTypeFromString(const char* s)
{
    if (strcmp(s, "floor") == 0) return BCOL_FLOOR;
    if (strcmp(s, "ceiling") == 0) return BCOL_CEILING;
    return BCOL_WALL;
}

static inline const char* BuildingColliderTypeName(BuildingColliderType t)
{
    switch (t)
    {
    case BCOL_FLOOR:   return "floor";
    case BCOL_CEILING: return "ceiling";
    case BCOL_WALL:    return "wall";
    default:           return "unknown";
    }
}
static inline void BuildingCollision_RelieveWallFloorSeams(BuildingColliderSet* set)
{
    //this made things worse
}

static inline bool LoadBuildingCollisionFile(Model_Home_Type modelType, const char* path)
{
    if (modelType < 0 || modelType >= MODEL_HOME_TOTAL_COUNT) return false;

    BuildingColliderSet* set = &HomeCollisionSets[modelType];
    set->count = 0;
    set->loaded = false;

    FILE* f = fopen(path, "r");
    if (!f)
    {
        TraceLog(LOG_WARNING, "No building collision file: %s", path);
        return false;
    }

    char line[2048];

    while (fgets(line, sizeof(line), f))
    {
        if (line[0] == '#') continue;
        if (strncmp(line, "collider", 8) != 0) continue;

        if (set->count >= MAX_BUILDING_COLLIDERS_PER_MODEL)
        {
            TraceLog(LOG_WARNING, "Too many building colliders in %s", path);
            break;
        }

        char typeName[64] = { 0 };
        float v[24] = { 0 };

        int n = sscanf(line,
            "collider %63s "
            "%ff %ff %ff %ff %ff %ff %ff %ff %ff %ff %ff %ff "
            "%ff %ff %ff %ff %ff %ff %ff %ff %ff %ff %ff %ff",
            typeName,
            &v[0], &v[1], &v[2],
            &v[3], &v[4], &v[5],
            &v[6], &v[7], &v[8],
            &v[9], &v[10], &v[11],
            &v[12], &v[13], &v[14],
            &v[15], &v[16], &v[17],
            &v[18], &v[19], &v[20],
            &v[21], &v[22], &v[23]);

        // Some sscanf implementations do not like the literal f suffix.
        // Try again without requiring the f.
        if (n != 25)
        {
            n = sscanf(line,
                "collider %63s "
                "%f %f %f %f %f %f %f %f %f %f %f %f "
                "%f %f %f %f %f %f %f %f %f %f %f %f",
                typeName,
                &v[0], &v[1], &v[2],
                &v[3], &v[4], &v[5],
                &v[6], &v[7], &v[8],
                &v[9], &v[10], &v[11],
                &v[12], &v[13], &v[14],
                &v[15], &v[16], &v[17],
                &v[18], &v[19], &v[20],
                &v[21], &v[22], &v[23]);
        }

        if (n != 25)
        {
            TraceLog(LOG_WARNING, "Bad collider line in %s: %s", path, line);
            continue;
        }

        BuildingColliderRaw* c = &set->cols[set->count++];
        c->type = BuildingColliderTypeFromString(typeName);

        for (int i = 0; i < 8; i++)
        {
            c->local[i] = (Vector3){
                v[i * 3 + 0],
                v[i * 3 + 1],
                v[i * 3 + 2]
            };
        }
    }

    fclose(f);

    set->loaded = (set->count > 0);

    if (set->loaded)
    {
        BuildingCollision_RelieveWallFloorSeams(set);
    }

    TraceLog(LOG_WARNING,
        "Loaded building collision %s: modelType=%d count=%d",
        path,
        modelType,
        set->count);

    return set->loaded;
}

static inline void InitHomeCollisionSets(void)
{
    for (int i = 0; i < MODEL_HOME_TOTAL_COUNT; i++)
    {
        HomeCollisionSets[i] = (BuildingColliderSet){ 0 };
    }

    // First test: home_001 / MODEL_HOME_CABIN.
    LoadBuildingCollisionFile(MODEL_HOME_CABIN,"collide_homes/home_001.collide.txt");
    LoadBuildingCollisionFile(MODEL_BARN, "collide_homes/barn.collide.txt");
    LoadBuildingCollisionFile(MODEL_HOME_WINDMILL, "collide_homes/home_007.collide.txt");
    LoadBuildingCollisionFile(MODEL_HOME_NICE, "collide_homes/home_003.collide.txt");
    LoadBuildingCollisionFile(MODEL_CINDER, "collide_homes/cinder.collide.txt");
    LoadBuildingCollisionFile(MODEL_CINDER_CAVE, "collide_homes/cave.collide.txt");
    LoadBuildingCollisionFile(MODEL_HOME_COTTAGE, "collide_homes/cottage.collide.txt");
    //LoadBuildingCollisionFile(MODEL_HOME_COZY, "collide_homes/cozy.collide.txt"); //this sucks, just use the old one
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
    Model home013 = LoadModel("models/old-stone.obj");
    home013.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/old-stone.png");
    HomeModels[MODEL_HOME_OLE_STONE] = home013;
    Model home014 = LoadModel("models/cinder.obj");
    home014.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/cinder.png");
    home014.materials[0].maps[MATERIAL_MAP_METALNESS].texture = LoadMyTexture("textures/spire_metal.png");
    home014.materials[0].maps[MATERIAL_MAP_NORMAL].texture = LoadMyTexture("textures/spire_normal.png");
    home014.materials[0].maps[MATERIAL_MAP_EMISSION].texture = LoadMyTexture("textures/spire_emissive.png");
    HomeModels[MODEL_CINDER] = home014;
    Model home015 = LoadModel("models/cave.obj");
    home015.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadMyTexture("textures/cave.png");
    home015.materials[0].maps[MATERIAL_MAP_METALNESS].texture = LoadMyTexture("textures/cave_metal.png");
    home015.materials[0].maps[MATERIAL_MAP_NORMAL].texture = LoadMyTexture("textures/cave_normal.png");
    home015.materials[0].maps[MATERIAL_MAP_EMISSION].texture = LoadMyTexture("textures/cave_emissive.png");
    HomeModels[MODEL_CINDER_CAVE] = home015;

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
    //new
    Scenes[SCENE_HOME_STONE_01] = (Scene){ //
        .type = SCENE_HOME_STONE_01,
        .modelType = MODEL_HOME_OLE_STONE,
        .pos = (Vector3){ -2565.52, 406.59, -2467.37 },
        .yaw = PI, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 32.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_OLE_02] = (Scene){ ////near store 1
        .type = SCENE_HOME_OLE_02,
        .modelType = MODEL_HOME_OLE_HOOSE,
        .pos = (Vector3){ -2402.42, 460, -2105.23 },
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 32.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_BARN_02] = (Scene){ ////barn
        .type = SCENE_HOME_BARN_02,
        .modelType = MODEL_BARN,
        .pos = (Vector3){ -2467.08, 482, -1811.71 },
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 46.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_COTTAGE_04] = (Scene){ ////cottage near middle of map
        .type = SCENE_HOME_COTTAGE_04,
        .modelType = MODEL_HOME_COTTAGE,
        .pos = (Vector3){ -2098.71, 473, -2207.61 },
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 30.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_HOME_COZY_03] = (Scene){ ////abby's?
        .type = SCENE_HOME_COZY_03,
        .modelType = MODEL_HOME_COZY,
        .pos = (Vector3){ -2120.97, 451, -2376.71 },
        .yaw = 0, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 24.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_CINDER] = (Scene){ //cinder spire
        .type = SCENE_CINDER,
        .modelType = MODEL_CINDER,
        .pos = (Vector3){ -416, 716, -1160 },
        .yaw = PI, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 64.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    Scenes[SCENE_CINDER_CAVE] = (Scene){ //cinder spire
        .type = SCENE_CINDER_CAVE,
        .modelType = MODEL_CINDER_CAVE,
        .pos = (Vector3){ -416, 607, -1160 },
        .yaw = PI / 2.0f, // PI / 2.0f,          // adjust if you want a different facing
        .scale = 82.0f,
        .origBox = (BoundingBox){0},
        .box = (BoundingBox){0}
    };
    
    for (int i = 0; i < SCENE_TOTAL_COUNT; i++)
    {
        BoundingBox original = GetModelBoundingBox(HomeModels[Scenes[i].modelType]);
        Scenes[i].origBox = RotateScaleTranslateBoundingBoxY(original, (Vector3) {0,0,0}, Scenes[i].scale, Scenes[i].yaw);
        if (Scenes[i].modelType != MODEL_CINDER_CAVE) { Scenes[i].origBox.max.y += Scenes[i].scale * 0.2; }
        Scenes[i].box = UpdateBoundingBox(Scenes[i].origBox, Scenes[i].pos);
    }
    InitHomeCollisionSets();
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

    w->wood1Tex = LoadMyTexture("textures/wood1.png");
    w->wood2Tex = LoadMyTexture("textures/wood2.png");

    // low segment torus = blocky wooden wheel
    w->bigRing = LoadModelFromMesh(GenMeshTorus(3.2f, 0.18f, 12, 4));
    w->smallRing = LoadModelFromMesh(GenMeshTorus(0.85f, 0.14f, 10, 4));

    w->bigRing.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = w->wood1Tex;
    w->smallRing.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = w->wood2Tex;

    // long axle through middle
    w->axle = LoadModelFromMesh(GenMeshCylinder(0.22f, 3.2f, 8));
    w->axle.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = w->wood1Tex;

    // trough / paddle bucket
    w->bucket = LoadModelFromMesh(GenMeshCube(1.15f, 0.32f, 0.42f));
    w->bucket.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = w->wood2Tex;

    // non-spinning back machine/mount/base square
    w->mount = LoadModelFromMesh(GenMeshCube(1.2f, 2.2f, 1.2f));
    w->mount.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = w->wood1Tex;

    w->bucketOrigBox = GetModelBoundingBox(w->bucket);
    w->mountOrigBox = GetModelBoundingBox(w->mount);

    w->box = (BoundingBox){
        (Vector3) {
 w->pos.x - 5, w->pos.y - 5, w->pos.z - 5
},
(Vector3) {
w->pos.x + 5, w->pos.y + 5, w->pos.z + 5
}
    };
}

//static inline void WaterWheel_Update(float dt)
//{
//    for (int i = 0; i < WATER_WHEEL_COUNT; i++)
//    {
//        WaterWheel* w = &gWaterWheels[i];
//        w->spin += w->spinSpeed * dt;
//        if (w->spin > 360.0f) w->spin -= 360.0f;
//    }
//}

static inline void WaterWheel_CollideDonny(Donogan* d)
{
    if (!d) return;

    for (int i = 0; i < WATER_WHEEL_COUNT; i++)
    {
        WaterWheel* w = &gWaterWheels[i];

        if (Vector3DistanceSqr(d->pos, w->pos) > 120.0f * 120.0f) continue;

        // Base/mount is a wall-ish block. Push X/Z only.
        if (CheckCollisionBoxes(d->box, w->mountBox))
        {
            BoundingBox a = d->box;
            BoundingBox b = w->mountBox;

            Vector3 ac = { (a.min.x + a.max.x) * 0.5f, (a.min.y + a.max.y) * 0.5f, (a.min.z + a.max.z) * 0.5f };
            Vector3 bc = { (b.min.x + b.max.x) * 0.5f, (b.min.y + b.max.y) * 0.5f, (b.min.z + b.max.z) * 0.5f };
            Vector3 ah = { (a.max.x - a.min.x) * 0.5f, (a.max.y - a.min.y) * 0.5f, (a.max.z - a.min.z) * 0.5f };
            Vector3 bh = { (b.max.x - b.min.x) * 0.5f, (b.max.y - b.min.y) * 0.5f, (b.max.z - b.min.z) * 0.5f };

            Vector3 diff = Vector3Subtract(ac, bc);
            float penX = (ah.x + bh.x) - fabsf(diff.x);
            float penZ = (ah.z + bh.z) - fabsf(diff.z);

            if (penX < penZ) d->pos.x += (diff.x >= 0.0f) ? penX + 0.03f : -penX - 0.03f;
            else             d->pos.z += (diff.z >= 0.0f) ? penZ + 0.03f : -penZ - 0.03f;

            d->box = UpdateBoundingBox(d->origBB, d->pos);
            d->innerBox = UpdateBoundingBox(d->origInnerBB, d->pos);
            d->outerBox = UpdateBoundingBox(d->origOuterBB, d->pos);
        }

        // Troughs are moving ground.
        for (int k = 0; k < WATER_WHEEL_BUCKET_COUNT; k++)
        {
            BoundingBox b = w->bucketBoxes[k];

            // Only use a thin landing slab at the top of the trough.
            BoundingBox top = b;
            top.min.y = b.max.y - 1.1f;
            top.max.y = b.max.y + 0.35f;

            if (!CheckCollisionBoxes(d->box, top)) continue;

            float feetY = DonFeetWorldY(d);

            // Only land if Donny is coming from above-ish, not hitting the underside.
            if (feetY <= b.max.y + 0.6f && feetY >= b.max.y - 2.2f && d->velY <= 1.0f)
            {
                d->groundY = b.max.y;
                d->groundNormal = (Vector3){ 0, 1, 0 };
                d->onGround = true;
                d->velY = 0.0f;

                // Snap his feet to the top.
                d->pos.y = d->groundY - d->firstBB.min.y * d->scale;

                d->box = UpdateBoundingBox(d->origBB, d->pos);
                d->innerBox = UpdateBoundingBox(d->origInnerBB, d->pos);
                d->outerBox = UpdateBoundingBox(d->origOuterBB, d->pos);
            }
        }
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

static inline void WaterWheel_Update(float dt)
{
    for (int i = 0; i < WATER_WHEEL_COUNT; i++)
    {
        WaterWheel* w = &gWaterWheels[i];

        w->spin += w->spinSpeed * dt;
        if (w->spin > 360.0f) w->spin -= 360.0f;
        if (w->spin < 0.0f)   w->spin += 360.0f;

        for (int k = 0; k < WATER_WHEEL_BUCKET_COUNT; k++)
        {
            Matrix bucketM = WaterWheel_BucketMatrix(w, k);
            w->bucketBoxes[k] = WaterWheel_TransformBoundingBox(w->bucketOrigBox, bucketM);

            // make the collision box a tiny bit friendlier on top
            w->bucketBoxes[k].max.y += 0.15f;
        }

        Matrix mountM = WaterWheel_MountMatrix(w);
        w->mountBox = WaterWheel_TransformBoundingBox(w->mountOrigBox, mountM);

        w->box = (BoundingBox){
            (Vector3) {w->pos.x - 70.0f, w->pos.y - 70.0f, w->pos.z - 70.0f},
            (Vector3) {w->pos.x + 70.0f, w->pos.y + 70.0f, w->pos.z + 70.0f}
        };
    }
}

static inline Matrix WaterWheel_BaseMatrixRotor(WaterWheel* w)
{
    Matrix S = MatrixScale(w->scale*2, w->scale*2, w->scale*2);

    // yaw placement + fixed 90 degree local turn for the whole wheel assembly
    Matrix R = MatrixRotateY(w->yaw + -90.0f * DEG2RAD);

    Matrix T = MatrixTranslate(w->pos.x, w->pos.y, w->pos.z);

    return MatrixMultiply(MatrixMultiply(S, R), T);
}
static inline Matrix WaterWheel_BucketMatrix(WaterWheel* w, int k)
{
    Matrix base = WaterWheel_BaseMatrix(w);

    float a = ((float)k / (float)WATER_WHEEL_BUCKET_COUNT) * 2.0f * PI;
    float spinRad = w->spin * DEG2RAD;
    float total = a + spinRad;

    float r = 1.96f;

    // Orbit position around local X, but DO NOT rotate the bucket with total.
    float localY = cosf(total) * r;
    float localZ = sinf(total) * r;

    Matrix bucketHang = MatrixTranslate(0.0f, localY, localZ);

    // Optional fixed art correction only. This is NOT spin.
    // If your trough model faces wrong, tweak this constant.
    Matrix bucketFace = MatrixIdentity();
    // Matrix bucketFace = MatrixRotateY(90.0f * DEG2RAD);

    return MatrixMultiply(MatrixMultiply(bucketFace, bucketHang), base);
}

static inline Matrix WaterWheel_MountMatrix(WaterWheel* w)
{
    Matrix base = WaterWheel_BaseMatrix(w);
    Matrix mountLocal = MatrixTranslate(-3.4f, 0.0f, 0.0f);
    return MatrixMultiply(mountLocal, base);
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
    // 6 suspended troughs around wheel.
    // They orbit with the wheel, but stay upright.
    for (int k = 0; k < WATER_WHEEL_BUCKET_COUNT; k++)
    {
        Matrix bucketM = WaterWheel_BucketMatrix(w, k);
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

    DrawMesh(w->mount.meshes[0], w->mount.materials[0], WaterWheel_MountMatrix(w));
}

static void WaterWheel_Draw(Donogan* d)
{
    for (int i = 0; i < WATER_WHEEL_COUNT; i++)
    {
        if (Vector3DistanceSqr(d->pos, gWaterWheels[i].pos) > 888*888) { continue; }
        WaterWheel_DrawOne(&gWaterWheels[i]);
    }
}

#endif // JC_H
