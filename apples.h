#ifndef APPLES_H
#define APPLES_H

#include "raylib.h"
#include <stdlib.h>
#include "core.h"

#define MAX_APPLES_TOTAL 128


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


void InitApples()
{
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


static inline void DrawApples(void) {
    for (int i = 0; i < MAX_APPLES_TOTAL; ++i) {
        if (!apples[i].spawned) { continue; }
        //TraceLog(LOG_INFO, "drawing spawned apple...%d", i);
        DrawModel(apple, apples[i].pos, apples[i].scale, WHITE);
        // optional debug
        // DrawBoundingBox(apples[i].box, RED);
    }
}

#endif