#ifndef MODELS_H
#define MODELS_H

#include "raylib.h"
#include <stdlib.h>

#define MAX_PROPS_ALLOWED 2048
#define MAX_PROPS_UPPER_BOUND (MAX_PROPS_ALLOWED * 2)  // safety cap

// Enum for all models used in the game
typedef enum {
    MODEL_NONE = -1,
    MODEL_TREE,
    MODEL_TREE_2,
    MODEL_TREE_3,
    MODEL_TREE_4,
    MODEL_TREE_DEAD_01,
    MODEL_TREE_DEAD_02,
    MODEL_TREE_DEAD_03,
    MODEL_TREE_PINE,
    MODEL_ROCK,
    MODEL_ROCK2,
    MODEL_ROCK3,
    MODEL_ROCK4,
    MODEL_ROCK5,
    MODEL_GRASS,
    MODEL_GRASS_LARGE,
    MODEL_GRASS_THICK,
    MODEL_TOTAL_COUNT  // Always keep last to get number of models
} Model_Type;

typedef enum {
    BIOME_NONE = -1,
    BIOME_FOREST,
    BIOME_FOREST_DEAD,
    BIOME_FOREST_PINE,
    BIOME_FOREST_ECCLECTIC,
    BIOME_GRASSLAND_SIMPLE,
    BIOME_GRASSLAND_FULL,
    BIOME_MOUNTAIN_1,
    BIOME_MOUNTAIN_2,
    BIOME_TOTAL_COUNT
} Biome_Type;

#define MAX_BERRIES_PER_TREE 12
#define MAX_APPLES_TOTAL 128

typedef struct {
    Model_Type type;
    Vector3 pos;
    float yaw, pitch, roll, scale;
    //below this line, only use in preview.c
    BoundingBox origBox, box;
    BoundingBox origOuterBox, outerBox;

    //berries for tree_02
    bool hasBerries;
    bool  berriesSpawned;
    int   berryCount;                // 0..MAX_BERRIES_PER_TREE
    Vector3 berryPos[MAX_BERRIES_PER_TREE];
    float   berryScale[MAX_BERRIES_PER_TREE];
} StaticGameObject;

typedef struct {
    bool spawned;
    Vector3 pos;
    float yaw, pitch, roll, scale;
    //below this line, only use in preview.c
    BoundingBox origBox, box;
    bool  falling;
    bool  fallen;      // eligible for triangle pickup when true
    Vector3 vel;       // simple gravity
} Apple;

Model apple;
Texture appleTex;
Apple apples[MAX_APPLES_TOTAL];
int gAppleIndex = 0;

// Optional: Array of model names, useful for debugging or file loading
static const char *ModelNames[MODEL_TOTAL_COUNT] = {
    "tree",
    "tree2",
    "tree3",
    "tree4",
    "tree_dead",
    "tree_dead2",
    "tree_dead3",
    "tree_pine",
    "rock",
    "rock2",
    "rock3",
    "rock4",
    "rock5",
    "grass",
    "grass_large",
    "grass_thick",
};

static const char *ModelPaths[MODEL_TOTAL_COUNT] = {
    "models/tree_bg.glb",
    "models/tree_02.obj",
    "models/tree_03.obj",
    "models/tree_04.obj",
    "models/tree_dead_01.obj",
    "models/tree_dead_02.obj",
    "models/tree_dead_03.obj",
    "models/tree_pine_01.obj",
    "models/rock1.glb",
    "models/rock2.obj",
    "models/rock3.obj",
    "models/rock4.obj",
    "models/rock5.obj",
    "models/grass.obj",
    "models/grass_large.obj",
    "models/grass_thick.obj",
};

static const char *ModelPathsFull[MODEL_TOTAL_COUNT] = {
    "models/tree.glb",
    "models/tree_02.obj",
    "models/tree_03.obj",
    "models/tree_04.obj",
    "models/tree_dead_01.obj",
    "models/tree_dead_02.obj",
    "models/tree_dead_03.obj",
    "models/tree_pine_01.obj",
    "models/rock1.glb",
    "models/rock2.obj",
    "models/rock3.obj",
    "models/rock4.obj",
    "models/rock5.obj",
    "models/grass.obj",
    "models/grass_large.obj",
    "models/grass_thick.obj",
};

static const char *ModelPathsFullTextures[MODEL_TOTAL_COUNT] = {
    "textures/tree_skin_small.png",
    "textures/tree_02.png",
    "textures/tree_03.png",
    "textures/tree_04.png",
    "textures/tree_dead_01.png",
    "textures/tree_dead_02.png",
    "textures/tree_dead_03.png",
    "textures/tree_pine_01.png",
    "textures/rock1.png",
    "textures/rock1.png",
    "textures/rock1.png",
    "textures/rock2.png",
    "textures/rock2.png",
    "textures/grass.png",
    "textures/grass.png",
    "textures/grass_yellow.png",
};

Model StaticObjectModels[MODEL_TOTAL_COUNT];
Model HighFiStaticObjectModels[MODEL_TOTAL_COUNT];
Texture HighFiStaticObjectModelTextures[MODEL_TOTAL_COUNT];
Material HighFiStaticObjectMaterials[MODEL_TOTAL_COUNT];
Matrix HighFiTransforms[MODEL_TOTAL_COUNT][MAX_PROPS_UPPER_BOUND];//meant to be set per draw loop, and then completely overwritten, dynamic so over estimate and test

// Optional: Utility function (only if you want it in header)
static inline const char *GetModelName(Model_Type model) {
    if (model >= 0 && model < MODEL_TOTAL_COUNT)
        return ModelNames[model];
    return "none";
}

BoundingBox UpdateModelBoundingBox(BoundingBox box, Vector3 pos)
{
    // Calculate the half-extents from the current box
    Vector3 halfSize = {
        (box.max.x - box.min.x) / 2.0f,
        (box.max.y - box.min.y) / 2.0f,
        (box.max.z - box.min.z) / 2.0f
    };

    // Create new min and max based on the new center
    BoundingBox movedBox = {
        .min = {
            pos.x - halfSize.x,
            pos.y - halfSize.y,
            pos.z - halfSize.z
        },
        .max = {
            pos.x + halfSize.x,
            pos.y + halfSize.y,
            pos.z + halfSize.z
        }
    };

    return movedBox;
}
//////////////////////////////////////////////DEFINE MODELS FROM PERLIN COLOR NOISE//////////////////////////////////////////////////////
#define TREE_MATCH_DISTANCE_SQ 200  // ±14 RGB range
#define ROCK_MATCH_DISTANCE_SQ 300


int ColorDistanceSquared(Color a, Color b) {
    return (a.r - b.r)*(a.r - b.r) + (a.g - b.g)*(a.g - b.g) + (a.b - b.b)*(a.b - b.b);
}

Model_Type GetRandomModelForBiome(Biome_Type biome) {
    switch (biome) {
        case BIOME_FOREST: {
            const Model_Type props[] = { MODEL_GRASS, MODEL_GRASS_THICK, MODEL_GRASS_THICK,MODEL_GRASS_LARGE, MODEL_TREE,  MODEL_TREE_2, MODEL_ROCK2, MODEL_ROCK3, MODEL_GRASS, MODEL_GRASS_THICK, MODEL_GRASS_THICK,MODEL_GRASS_LARGE, MODEL_GRASS_THICK,MODEL_GRASS_LARGE, MODEL_GRASS_THICK,MODEL_GRASS_LARGE, MODEL_GRASS_THICK,MODEL_GRASS_LARGE, MODEL_GRASS_THICK,MODEL_GRASS_LARGE };
            return props[rand() % 20];
        }
        case BIOME_FOREST_DEAD: {
            const Model_Type props[] = { MODEL_GRASS, MODEL_GRASS_LARGE, MODEL_TREE_DEAD_01, MODEL_TREE_DEAD_02, MODEL_TREE_DEAD_03, MODEL_ROCK, MODEL_ROCK4, MODEL_GRASS, MODEL_GRASS_THICK, MODEL_GRASS_THICK,MODEL_GRASS_LARGE, MODEL_GRASS_THICK, MODEL_GRASS_THICK, MODEL_GRASS_THICK };
            return props[rand() % 14];
        }
        case BIOME_FOREST_PINE: {
            const Model_Type props[] = { MODEL_GRASS, MODEL_TREE_PINE, MODEL_GRASS, MODEL_TREE_PINE, MODEL_ROCK5 , MODEL_GRASS_THICK , MODEL_GRASS_THICK , MODEL_GRASS_THICK, MODEL_GRASS_THICK, MODEL_GRASS_THICK, MODEL_GRASS_THICK, MODEL_GRASS_THICK, MODEL_GRASS_THICK, MODEL_GRASS_THICK };
            return props[rand() % 14];
        }
        case BIOME_FOREST_ECCLECTIC: {
            const Model_Type props[] = { MODEL_TREE,    MODEL_TREE_2,    MODEL_TREE_3,    MODEL_TREE_4,    MODEL_TREE_DEAD_01,    MODEL_TREE_DEAD_02,    MODEL_TREE_DEAD_03,    MODEL_TREE_PINE,    MODEL_ROCK,    MODEL_ROCK2,    MODEL_ROCK3,    MODEL_ROCK4,    MODEL_ROCK5,    MODEL_GRASS,    MODEL_GRASS_LARGE,    MODEL_GRASS_THICK, MODEL_GRASS,    MODEL_GRASS_LARGE,    MODEL_GRASS_THICK , MODEL_GRASS,    MODEL_GRASS_LARGE,    MODEL_GRASS_THICK,  MODEL_GRASS_LARGE,    MODEL_GRASS_THICK,  MODEL_GRASS_LARGE,    MODEL_GRASS_THICK,  MODEL_GRASS_LARGE,    MODEL_GRASS_THICK };
            return props[rand() % 28];
        }
        case BIOME_GRASSLAND_SIMPLE: {
            const Model_Type props[] = { MODEL_GRASS, MODEL_GRASS_THICK, MODEL_GRASS_LARGE, MODEL_GRASS, MODEL_GRASS_THICK, MODEL_GRASS_LARGE, MODEL_ROCK, MODEL_GRASS, MODEL_GRASS_THICK, MODEL_GRASS_LARGE, MODEL_GRASS, MODEL_GRASS_THICK, MODEL_GRASS_THICK, MODEL_GRASS_LARGE, MODEL_GRASS, MODEL_GRASS_THICK, MODEL_GRASS_LARGE, MODEL_GRASS_THICK, MODEL_GRASS_LARGE, MODEL_GRASS, MODEL_GRASS_THICK, MODEL_GRASS_LARGE, MODEL_GRASS_THICK, MODEL_GRASS_LARGE, MODEL_GRASS, MODEL_GRASS_THICK, MODEL_GRASS_LARGE };
            return props[rand() % 27];
        }
        case BIOME_GRASSLAND_FULL: {
            const Model_Type props[] = { MODEL_GRASS, MODEL_GRASS_THICK, MODEL_GRASS_LARGE, MODEL_GRASS_THICK, MODEL_GRASS_LARGE, MODEL_GRASS_THICK, MODEL_GRASS_LARGE, MODEL_GRASS_LARGE, MODEL_GRASS_LARGE, MODEL_GRASS_LARGE, MODEL_GRASS_LARGE, MODEL_GRASS_LARGE };
            return props[rand() % 12];
        }
        case BIOME_MOUNTAIN_1: {
            const Model_Type props[] = { MODEL_GRASS, MODEL_GRASS, MODEL_GRASS, MODEL_ROCK2, MODEL_ROCK3 ,MODEL_ROCK4 ,MODEL_ROCK5 ,MODEL_ROCK, MODEL_GRASS , MODEL_GRASS , MODEL_GRASS , MODEL_GRASS , MODEL_GRASS , MODEL_GRASS , MODEL_GRASS , MODEL_GRASS , MODEL_GRASS , MODEL_GRASS , MODEL_GRASS , MODEL_GRASS , MODEL_GRASS , MODEL_GRASS };
            return props[rand() % 22];
        }
        case BIOME_MOUNTAIN_2: {
            const Model_Type props[] = { MODEL_GRASS, MODEL_GRASS, MODEL_GRASS_THICK, MODEL_ROCK, MODEL_TREE_DEAD_01, MODEL_TREE_3, MODEL_GRASS, MODEL_GRASS, MODEL_GRASS, MODEL_GRASS, MODEL_GRASS, MODEL_GRASS, MODEL_GRASS, MODEL_GRASS, MODEL_GRASS, MODEL_GRASS, MODEL_GRASS, MODEL_GRASS };
            return props[rand() % 18];
        }
        default: return MODEL_NONE;
    }
}

Biome_Type GetBiomeFromColor(Color c) {
    if (ColorDistanceSquared(c, (Color){120,200,120 }) < 8000) {return BIOME_GRASSLAND_SIMPLE;}
    if (ColorDistanceSquared(c, (Color){130,210,130}) < 7000) {return BIOME_GRASSLAND_FULL;}
    if (ColorDistanceSquared(c, (Color){ 200,139, 34 }) < 6000) {return BIOME_FOREST;}
    if (ColorDistanceSquared(c, (Color){80,100, 200 }) < 6000) {return BIOME_FOREST_DEAD;}
    if (ColorDistanceSquared(c, (Color){10, 90, 40}) < 6000) {return BIOME_FOREST_PINE;}
    if (ColorDistanceSquared(c, (Color){ 40,250, 40 }) < 2600) {return BIOME_FOREST_ECCLECTIC;}//I like this one but it shows up too much, I lowered it
    if (ColorDistanceSquared(c, (Color){150,150,150 }) < 6000) {return BIOME_MOUNTAIN_1;}
    if (ColorDistanceSquared(c, (Color){120,120,120 }) < 6000) {return BIOME_MOUNTAIN_2;}
    
    return BIOME_NONE;
}

Model_Type GetModelTypeFromColor(Color c, float heightEst) {
    //todo: if height estimate is above something, probably snow. heightEst
    Biome_Type biome = GetBiomeFromColor(c);
    return GetRandomModelForBiome(biome);
}


void InitStaticGameProps(Shader shader, Shader grass_s) {
    //apples
    apple = LoadModel("models/apple.obj");
    appleTex = LoadTexture("textures/apple.png");
    apple.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = appleTex;
    for (int i = 0; i < MAX_APPLES_TOTAL; ++i) {
        apples[i].spawned = false;
        apples[i].falling = false;
        apples[i].fallen = false;
        apples[i].vel = (Vector3){ 0 };
    }
    //static props
    for (int i = 0; i < MODEL_TOTAL_COUNT; i++) {
        // Load base model and texture
        StaticObjectModels[i] = LoadModel(ModelPaths[i]);
        HighFiStaticObjectModels[i] = LoadModel(ModelPathsFull[i]);
        HighFiStaticObjectModelTextures[i] = LoadTexture(ModelPathsFullTextures[i]);

        // Deep copy material
        Material mat = LoadMaterialDefault();
        mat.shader = (i== MODEL_GRASS || i == MODEL_GRASS_LARGE || i == MODEL_GRASS_THICK) ? grass_s: shader;
        mat.maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
        mat.maps[MATERIAL_MAP_DIFFUSE].texture = HighFiStaticObjectModelTextures[i];
        HighFiStaticObjectMaterials[i] = mat;

        // Assign clean material to model
        HighFiStaticObjectModels[i].materials[0] = HighFiStaticObjectMaterials[i];
    }
}

// models.h — below InitStaticGameProps() or near other inlines


static inline bool FindAppleVertexInBand(const StaticGameObject* g, float minAbove, float maxAbove, Vector3* outPos) {
    if (!g || g->type != MODEL_TREE) return false;
    Model m = HighFiStaticObjectModels[g->type];
    if (m.meshCount <= 0) return false;
    Mesh* mesh = &m.meshes[0];
    if (!mesh || mesh->vertexCount <= 0 || !mesh->vertices) return false;

    Matrix S = MatrixScale(g->scale, g->scale, g->scale);
    Matrix Rx = MatrixRotateX(g->pitch);
    Matrix Ry = MatrixRotateY(g->yaw);
    Matrix Rz = MatrixRotateZ(g->roll);
    Matrix R = MatrixMultiply(MatrixMultiply(Rx, Ry), Rz);
    Matrix SR = MatrixMultiply(S, R);
    Matrix T = MatrixTranslate(g->pos.x, g->pos.y, g->pos.z);
    Matrix M = MatrixMultiply(SR, T);

    Vector3 cand[256]; int candCount = 0;
    const int vc = mesh->vertexCount; const float* v = mesh->vertices;

    for (int i = 0; i < vc; i++) {
        Vector3 p = (Vector3){ v[i * 3 + 0], v[i * 3 + 1], v[i * 3 + 2] };
        Vector3 w = Vector3Transform(p, M);
        float dy = (w.y - g->pos.y) / g->scale; // normalized by scale
        if (dy >= minAbove && dy <= maxAbove) {
            if (candCount < 256) cand[candCount++] = w;
        }
    }
    if (candCount == 0) return false;
    *outPos = cand[GetRandomValue(0, candCount - 1)];
    return true;
}

// Pick a slot for a new apple:
// 1) Prefer any free slot (spawned == false) scanning from gAppleIndex
// 2) If all taken, return gAppleIndex as the replacement slot (round-robin)
//    and advance gAppleIndex either way.
static inline int AcquireAppleSlot(void) {
    int start = gAppleIndex % MAX_APPLES_TOTAL;

    // Try to find a free slot first
    for (int n = 0; n < MAX_APPLES_TOTAL; ++n) {
        int idx = (start + n) % MAX_APPLES_TOTAL;
        if (!apples[idx].spawned) {
            gAppleIndex = (idx + 1) % MAX_APPLES_TOTAL; // next search starts after this
            return idx;
        }
    }

    // No free slot—replace in round-robin
    int idx = start;
    gAppleIndex = (start + 1) % MAX_APPLES_TOTAL;
    return idx;
}

// models.h
static inline bool SpawnAppleOnTree(StaticGameObject* g, float minBand, float maxBand) {
    if (!g || g->type != MODEL_TREE) return false;

    Vector3 p;
    if (!FindAppleVertexInBand(g, minBand, maxBand, &p)) return false;

    int globalIdx = AcquireAppleSlot();
    Apple* a = &apples[globalIdx];

    a->spawned = true;
    a->falling = false;
    a->fallen = false;
    a->vel = (Vector3){ 0 };
    a->pos = p;
    a->scale = 1;
    a->yaw = a->pitch = a->roll = 0;

    a->origBox = GetModelBoundingBox(apple);
    a->box = UpdateModelBoundingBox(a->origBox, a->pos);
    return true;
}

static inline void UpdateApples(float dt) {
    for (int i = 0; i < MAX_APPLES_TOTAL; ++i) {
        Apple* a = &apples[i];
        if (!a->spawned) { continue; }

        if (a->falling && !a->fallen) {
            a->vel.y -= 9.8f * dt * 0.35f;        // light gravity
            a->pos = Vector3Add(a->pos, Vector3Scale(a->vel, dt * 60.0f));
            a->box = UpdateModelBoundingBox(a->origBox, a->pos);

            float ground = GetTerrainHeightFromMeshXZ(a->pos.x, a->pos.z);
            if (a->pos.y <= ground + 0.25f) {
                a->pos.y = ground + 0.25f;
                a->fallen = true;
                a->falling = false;
                a->vel = (Vector3){ 0 };
            }
        }
    }
}


// models.h
// models.h
static inline void SpawnBerriesForProp(StaticGameObject* g) {
    if (!g || g->berriesSpawned) return;
    if (g->type != MODEL_TREE_2) return;

    Model m = HighFiStaticObjectModels[g->type];
    if (m.meshCount <= 0) return;

    Mesh* mesh = &m.meshes[0];
    if (!mesh || mesh->vertexCount <= 0 || !mesh->vertices) return;

    // ---- MATCH THE RENDER PATH EXACTLY ----
    // Angles in your draw are used as RADIANS, not degrees.
    Matrix S = MatrixScale(g->scale, g->scale, g->scale);
    Matrix Rx = MatrixRotateX(g->pitch);
    Matrix Ry = MatrixRotateY(g->yaw);
    Matrix Rz = MatrixRotateZ(g->roll);
    Matrix R = MatrixMultiply(MatrixMultiply(Rx, Ry), Rz);            // Rx * Ry * Rz
    Matrix SR = MatrixMultiply(S, R);                                   // S * R
    Matrix T = MatrixTranslate(g->pos.x, g->pos.y, g->pos.z);
    Matrix M = MatrixMultiply(SR, T);                                  // (S*R) * T

    const float minAbove = 2.7f;
    Vector3 cand[256];
    int candCount = 0;

    const int vc = mesh->vertexCount;
    const float* v = mesh->vertices; // xyz interleaved

    for (int i = 0; i < vc; i++) {
        Vector3 p = (Vector3){ v[i * 3 + 0], v[i * 3 + 1], v[i * 3 + 2] };
        Vector3 w = Vector3Transform(p, M);                             // local->world

        if (w.y >= g->pos.y + (minAbove * g->scale)) {
            if (candCount < (int)(sizeof(cand) / sizeof(cand[0]))) {
                cand[candCount] = w;
                candCount++;
            }
        }
    }
    if (candCount == 0)
    {
        TraceLog(LOG_WARNING, "berry spawn failed? no candidate vertices?");
        return;
    }

    g->berriesSpawned = true;

    int want = 4 + GetRandomValue(0, 8); // 3..5
    if (want > MAX_BERRIES_PER_TREE) { want = MAX_BERRIES_PER_TREE; }
    if (want > candCount) { want = candCount; }
    g->berryCount = want;

    // Pick a random subset if needed
    if (candCount <= want) {
        for (int i = 0; i < want; ++i) {
            g->berryPos[i] = cand[i];
            g->berryScale[i] = 0.10f * g->scale + (GetRandomValue(0, 8) * 0.0025f);
        }
    }
    else {
        int idx[256]; for (int i = 0; i < candCount; ++i) idx[i] = i;
        for (int i = 0; i < want; ++i) {
            int j = i + GetRandomValue(0, candCount - 1 - i);
            int t = idx[i]; idx[i] = idx[j]; idx[j] = t;
            g->berryPos[i] = cand[idx[i]];
            g->berryScale[i] = 0.10f * g->scale + (GetRandomValue(0, 8) * 0.0025f);
        }
    }
}


// Draw simple spheres (you can swap to a mesh later)
static inline void DrawBerriesForProp(const StaticGameObject* g) {
    if (!g || !g->hasBerries || !g->berriesSpawned || g->berryCount <= 0) return;

    for (int i = 0; i < g->berryCount; ++i) {
        DrawSphereEx(g->berryPos[i], g->berryScale[i], 8, 8, (Color) { 200, 30, 60, 255 });
        // Optional little stem:
        // DrawCylinderEx(Vector3Add(g->berryPos[i], (Vector3){0,-g->berryScale[i]*0.6f,0}),
        //                Vector3Add(g->berryPos[i], (Vector3){0,-g->berryScale[i]*1.2f,0}),
        //                g->berryScale[i]*0.15f, g->berryScale[i]*0.05f, 6, (Color){80,50,20,255});
    }
}

static inline void DrawApples(void) {
    for (int i = 0; i < MAX_APPLES_TOTAL; ++i) {
        if (!apples[i].spawned) { continue; }
        //TraceLog(LOG_INFO, "drawing spawned apple...%d", i);
        DrawModel(apple, apples[i].pos, apples[i].scale, WHITE);
        // optional debug
        // DrawBoundingBox(apples[i].box, RED);
    }
}


#endif // MODELS_H
