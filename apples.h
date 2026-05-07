#ifndef APPLES_H
#define APPLES_H

#include "raylib.h"
#include "core.h"
#include "texture.h"
#include "models.h"

#define MAX_APPLES_TOTAL 128


typedef struct {
    bool spawned;
    Vector3 pos;
    float yaw, pitch, roll, scale;
    BoundingBox origBox, box;
    bool  falling;
    bool  fallen;      // eligible for triangle pickup when true
    Vector3 vel;       // simple gravity
} Apple;

Model apple;
Texture appleTex;
Apple apples[MAX_APPLES_TOTAL];
int gAppleIndex = 0;

#define BLOOM_SHADE_COUNT 32
#define BLOOM_PER_SHADE 8192
#define BLOOM_RADIUS 1024.0f
#define BLOOM_HOLD_TIME 2.2f
#define BLOOM_MANA_COST 30

typedef struct BloomSystem {
    bool ready;
    bool bloomed;
    float hold;
    int manaPaid;

    Model sphere;
    Material mats[BLOOM_SHADE_COUNT];

    Matrix* xforms[BLOOM_SHADE_COUNT];
    int count[BLOOM_SHADE_COUNT];

    bool generating;
    int genShade;
    Vector3 genTolPos;
} BloomSystem;

static BloomSystem gBloom = { 0 };

static Color gBloomColors[BLOOM_SHADE_COUNT] = {
    // greens, 0-19
    {  35, 100,  35,255 }, {  45, 120,  40,255 },
    {  55, 135,  45,255 }, {  65, 150,  50,255 },
    {  75, 165,  55,255 }, {  85, 180,  60,255 },
    {  95, 195,  70,255 }, { 105, 210,  75,255 },
    {  70, 130,  80,255 }, {  85, 150,  90,255 },
    { 100, 170, 100,255 }, { 115, 190, 110,255 },
    { 130, 210, 120,255 }, { 145, 225, 130,255 },
    {  90, 145,  35,255 }, { 110, 165,  45,255 },
    { 130, 185,  55,255 }, { 150, 205,  65,255 },
    { 170, 225,  80,255 }, { 190, 240,  95,255 },

    // blues, 20-22
    {  70, 150, 190,255 }, {  90, 180, 220,255 },
    { 130, 210, 240,255 },

    // reds / purples, 23-28
    { 180,  40,  70,255 }, { 210,  55,  90,255 },
    { 150,  45, 130,255 }, { 175,  70, 170,255 },
    { 120,  60, 190,255 }, { 155,  90, 220,255 },

    // pinks, 29-31
    { 235, 100, 165,255 }, { 255, 135, 190,255 },
    { 255, 175, 220,255 },
};

static inline bool Bloom_IsDeadTree(Model_Type t)
{
    return t == MODEL_TREE_DEAD_01 ||
        t == MODEL_TREE_DEAD_02 ||
        t == MODEL_TREE_DEAD_03;
}

void InitApples()
{
    //apples
    apple = LoadModel("models/apple.obj");
    appleTex = LoadMyTexture("textures/apple.png");
    apple.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = appleTex;
    for (int i = 0; i < MAX_APPLES_TOTAL; ++i) {
        apples[i].spawned = false;
        apples[i].falling = false;
        apples[i].fallen = false;
        apples[i].vel = (Vector3){ 0 };
    }
}

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
    a->scale = 0.42f;
    a->yaw = a->pitch = a->roll = 0;

    a->origBox = GetModelBoundingBox(apple);
    a->box = UpdateBoundingBox(a->origBox, a->pos);
    return true;
}

void UpdateApples(float dt) {
    for (int i = 0; i < MAX_APPLES_TOTAL; i++) {
        Apple* a = &apples[i];
        if (!a->spawned) { continue; }

        if (a->falling && !a->fallen) {
            a->vel.y -= 9.8f * dt * 0.35f;        // light gravity
            a->pos = Vector3Add(a->pos, Vector3Scale(a->vel, dt * 60.0f));
            a->box = UpdateBoundingBox(a->origBox, a->pos);

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


static inline void DrawApples(Donogan* d) {
    for (int i = 0; i < MAX_APPLES_TOTAL; ++i) {
        if (!apples[i].spawned) { continue; }
        if (Vector3DistanceSqr(d->pos,apples[i].pos) > 200 * 200) { continue; }
        //TraceLog(LOG_INFO, "drawing spawned apple...%d", i);
        DrawModel(apple, apples[i].pos, apples[i].scale, WHITE);
        // optional debug
        // DrawBoundingBox(apples[i].box, RED);
    }
}

static inline bool FindPropVertexInBand(const StaticGameObject* g, float minAbove, float maxAbove, Vector3* outPos)
{
    if (!g || !outPos) return false;
    if (g->type < 0 || g->type >= MODEL_TOTAL_COUNT) return false;

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

    Vector3 cand[256];
    int candCount = 0;

    const float* v = mesh->vertices;
    for (int i = 0; i < mesh->vertexCount; i++)
    {
        Vector3 p = { v[i * 3 + 0], v[i * 3 + 1], v[i * 3 + 2] };
        Vector3 w = Vector3Transform(p, M);

        float dy = (w.y - g->pos.y) / g->scale;
        if (dy >= minAbove && dy <= maxAbove)
        {
            if (candCount < 256) cand[candCount++] = w;
        }
    }

    if (candCount <= 0) return false;
    *outPos = cand[GetRandomValue(0, candCount - 1)];
    return true;
}

void InitBloomSystem(Shader shader)
{
    memset(&gBloom, 0, sizeof(gBloom));

    Mesh mesh = GenMeshSphere(1.0f, 3, 5);
    gBloom.sphere = LoadModelFromMesh(mesh);

    for (int i = 0; i < BLOOM_SHADE_COUNT; i++)
    {
        gBloom.xforms[i] = MemAlloc(sizeof(Matrix) * BLOOM_PER_SHADE);
        gBloom.count[i] = 0;

        gBloom.mats[i] = LoadMaterialDefault();
        gBloom.mats[i].shader = shader;
        gBloom.mats[i].maps[MATERIAL_MAP_DIFFUSE].color = gBloomColors[i];
    }

    gBloom.ready = true;
}

void UnloadBloomSystem(void)
{
    for (int i = 0; i < BLOOM_SHADE_COUNT; i++)
    {
        if (gBloom.xforms[i]) MemFree(gBloom.xforms[i]);
        gBloom.xforms[i] = NULL;
    }

    if (gBloom.sphere.meshCount > 0) UnloadModel(gBloom.sphere);

    memset(&gBloom, 0, sizeof(gBloom));
}

static inline void Bloom_AddInstance(int shade, Vector3 p, float scale)
{
    if (shade < 0 || shade >= BLOOM_SHADE_COUNT) return;
    if (gBloom.count[shade] >= BLOOM_PER_SHADE) return;

    Matrix S = MatrixScale(scale, scale, scale);
    Matrix T = MatrixTranslate(p.x, p.y, p.z);
    gBloom.xforms[shade][gBloom.count[shade]++] = MatrixMultiply(S, T);
}

void UpdateTreeOfLifeBloomGeneration(void)
{
    if (!gBloom.ready || !gBloom.generating) return;

    int shade = gBloom.genShade;

    bool isGreen = shade < 20;
    bool isBlue = shade >= 20 && shade <= 22;
    bool isFlower = shade >= 23;

    //int perTree = isGreen ? 28 : isBlue ? 5 : 8;
    int perTree = isGreen ? 20 : isBlue ? 4 : 6;
    float minBand = isGreen ? 2.8f : 3.5f;
    float maxBand = isGreen ? 11.5f : 12.5f;

    for (int i = 0; i < numCloseProps; i++)
    {
        StaticGameObject* g = CloseProps[i];
        if (!g) continue;
        if (!Bloom_IsDeadTree(g->type)) continue;
        if (Vector3DistanceSqr(g->pos, gBloom.genTolPos) > BLOOM_RADIUS * BLOOM_RADIUS) continue;

        float treeScale = g->scale;

        // 16-ish = normal tree.
        // bigger trees get BIGGER bloom balls.
        float sizeMul = Clamp(treeScale / 16.0f, 0.75f, 3.5f);
        float countMul = Clamp(treeScale / 16.0f, 0.35f, 1.25f);

        int treePerTree = (int)((float)perTree * countMul);
        if (treePerTree < 4) treePerTree = 4;

        int madeForThisTree = 0;

        for (int tries = 0; tries < treePerTree * 4 && madeForThisTree < treePerTree; tries++)
        {
            Vector3 p;
            if (!FindPropVertexInBand(g, minBand, maxBand, &p)) continue;

            float treeScale = g->scale;

            float sizeMul = Clamp(treeScale / 10.0f, 1.0f, 7.5f);
            float spreadXZ = Clamp(treeScale * 0.22f, 1.2f, 6.5f);
            float spreadY = Clamp(treeScale * 0.12f, 0.8f, 4.0f);

            float a = GetRandomValue(0, 359) * DEG2RAD;
            float rr = sqrtf(GetRandomValue(0, 10000) / 10000.0f) * spreadXZ;

            p.x += cosf(a) * rr;
            p.z += sinf(a) * rr;
            p.y += GetRandomValue(0, 10000) / 10000.0f * spreadY;

            float scale = isGreen
                ? GetRandomValue(65, 140) * 0.022f
                : GetRandomValue(35, 80) * 0.022f;

            scale *= sizeMul;

            Bloom_AddInstance(shade, p, scale);
            madeForThisTree++;
        }
    }

    gBloom.genShade++;

    if (gBloom.genShade >= BLOOM_SHADE_COUNT)
    {
        gBloom.generating = false;
        gBloom.bloomed = true;
        TraceLog(LOG_INFO, "Tree of Life bloom generation finished.");
    }
}

void GenerateTreeOfLifeBloom(Vector3 tolPos)
{
    if (!gBloom.ready || gBloom.bloomed) return;

    for (int i = 0; i < BLOOM_SHADE_COUNT; i++) gBloom.count[i] = 0;

    for (int i = 0; i < numCloseProps; i++)
    {
        StaticGameObject* g = CloseProps[i];
        if (!g || !Bloom_IsDeadTree(g->type)) continue;

        float d = Vector3DistanceSqr(g->pos, tolPos);
        if (d > BLOOM_RADIUS * BLOOM_RADIUS) continue;

        // Leaves: 8 green shades
        // per tree limits, but much denser overall
        const int LEAVES_PER_SHADE_PER_TREE = 220;
        const int FLOWERS_PER_SHADE_PER_TREE = 70;

        // Leaves: higher on the tree now
        for (int shade = 0; shade < 8; shade++)
        {
            int madeForThisTree = 0;

            for (int tries = 0; tries < LEAVES_PER_SHADE_PER_TREE * 4 &&
                madeForThisTree < LEAVES_PER_SHADE_PER_TREE; tries++)
            {
                Vector3 p;
                if (!FindPropVertexInBand(g, 1.25f, 8.5f, &p)) continue;

                p.x += GetRandomValue(-35, 35) * 0.01f;
                p.y += GetRandomValue(-10, 50) * 0.01f;
                p.z += GetRandomValue(-35, 35) * 0.01f;

                Bloom_AddInstance(shade, p, GetRandomValue(24, 46) * 0.01f);
                madeForThisTree++;
            }
        }

        // Flowers: even higher, mostly crown / branch tips
        for (int shade = 8; shade < BLOOM_SHADE_COUNT; shade++)
        {
            int madeForThisTree = 0;

            for (int tries = 0; tries < FLOWERS_PER_SHADE_PER_TREE * 4 &&
                madeForThisTree < FLOWERS_PER_SHADE_PER_TREE; tries++)
            {
                Vector3 p;
                if (!FindPropVertexInBand(g, 2.0f, 9.0f, &p)) continue;

                p.x += GetRandomValue(-28, 28) * 0.01f;
                p.y += GetRandomValue(-5, 40) * 0.01f;
                p.z += GetRandomValue(-28, 28) * 0.01f;

                Bloom_AddInstance(shade, p, GetRandomValue(17, 30) * 0.01f);
                madeForThisTree++;
            }
        }
    }

    gBloom.bloomed = true;
    TraceLog(LOG_INFO, "Tree of Life bloom generated.");
}

void DrawTreeOfLifeBloom()
{
    if (!gBloom.ready) return;
    if (!gBloom.bloomed && !gBloom.generating) return;

    for (int shade = 0; shade < BLOOM_SHADE_COUNT; shade++)
    {
        if (gBloom.count[shade] <= 0) continue;

        DrawMeshInstanced(
            gBloom.sphere.meshes[0],
            gBloom.mats[shade],
            gBloom.xforms[shade],
            gBloom.count[shade]
        );
    }
}

void StartTreeOfLifeBloomGeneration(Vector3 tolPos)
{
    if (!gBloom.ready || gBloom.bloomed || gBloom.generating) return;

    for (int i = 0; i < BLOOM_SHADE_COUNT; i++)
    {
        gBloom.count[i] = 0;
    }

    gBloom.genTolPos = tolPos;
    gBloom.genShade = 0;
    gBloom.generating = true;

    TraceLog(LOG_INFO, "Tree of Life bloom generation started.");
}

void UpdateTreeOfLifeBloomSpell(Donogan* d, ControllerData* pad, float dt)
{
    if (!d || !pad) return;
    if (!gBloom.ready || gBloom.bloomed) return;
    if (!missions[MISSION_FIND_TOL].complete) return;

    Vector3 tolPos = *InteractivePoints[POI_TYPE_TREE_OF_LIFE].pos;

    if (Vector3DistanceSqr(d->pos, tolPos) > BLOOM_RADIUS) //BLOOM_RADIUS == 32^2, so 32 units away
    {
        gBloom.hold = 0.0f;
        gBloom.manaPaid = 0;
        return;
    }

    if (!pad->btnSquare)
    {
        gBloom.hold = 0.0f;
        gBloom.manaPaid = 0;
        return;
    }

    gBloom.hold += dt;

    int targetPaid = (int)((gBloom.hold / BLOOM_HOLD_TIME) * BLOOM_MANA_COST);
    if (targetPaid > BLOOM_MANA_COST) targetPaid = BLOOM_MANA_COST;

    int chunk = targetPaid - gBloom.manaPaid;
    if (chunk > 0)
    {
        if (d->mana < chunk)
        {
            gBloom.hold = 0.0f;
            gBloom.manaPaid = 0;
            return;
        }

        d->mana -= chunk;
        gBloom.manaPaid += chunk;
    }

    if (gBloom.hold >= BLOOM_HOLD_TIME && gBloom.manaPaid >= BLOOM_MANA_COST)
    {
        StartTreeOfLifeBloomGeneration(tolPos);
    }
}

#endif