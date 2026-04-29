#ifndef CORN_H
#define CORN_H

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdbool.h>
#include <string.h>
#include "texture.h"
#include "frustum.h"
#include "collision.h"

#ifndef CORN_MAX_INSTANCES
#define CORN_MAX_INSTANCES 8192
#endif

#define CORN_DRAW_DISTANCE 4396.0f
#define CORN_GROUND_Y_OFFSET 3.0f   // tweak if corn sinks/floats
#define CORN_DEFAULT_SCALE  4.0f

typedef struct CornPlant {
    Vector3 pos;
    float yaw;
    float scale;
} CornPlant;

static Model gCornModel = { 0 };
static Texture2D gCornTexture = { 0 };
static Material gCornMaterial = { 0 };
static bool gCornReady = false;

static CornPlant gCorn[CORN_MAX_INSTANCES];
static Matrix gCornTransforms[CORN_MAX_INSTANCES];
static int gCornCount = 0;

static inline void Corn_Clear(void)
{
    gCornCount = 0;
    memset(gCorn, 0, sizeof(gCorn));
}

static inline void Corn_Init(Shader instancingShader)
{
    Corn_Clear();

    gCornModel = LoadModel("models/corn.obj");
    gCornTexture = LoadMyTexture("textures/corn.png");

    gCornMaterial = LoadMaterialDefault();
    gCornMaterial.shader = instancingShader;
    gCornMaterial.maps[MATERIAL_MAP_DIFFUSE].texture = gCornTexture;
    gCornMaterial.maps[MATERIAL_MAP_DIFFUSE].color = WHITE;

    if (gCornModel.materialCount > 0) {
        gCornModel.materials[0] = gCornMaterial;
    }

    gCornReady = true;
}

static inline void Corn_AddPlant(Vector3 pos, float yaw, float scale)
{
    if (gCornCount >= CORN_MAX_INSTANCES) return;

    float ground = GetTerrainHeightFromMeshXZ(pos.x, pos.z);
    if (ground > -9000.0f) {
        pos.y = ground + CORN_GROUND_Y_OFFSET;
    }

    gCorn[gCornCount].pos = pos;
    gCorn[gCornCount].yaw = yaw;
    gCorn[gCornCount].scale = scale;
    gCornCount++;
}

// center = middle of field.
// sizeX/sizeZ = world dimensions.
// spacing = distance between corn stalks.
// jitter = random offset, try 0.25f to 1.0f.
static inline void Corn_AddFieldBox(
    Vector3 center,
    float sizeX,
    float sizeZ,
    float spacing,
    float jitter,
    float scaleMin,
    float scaleMax
)
{
    if (spacing <= 0.01f) spacing = 1.0f;
    if (scaleMin <= 0.0f) scaleMin = CORN_DEFAULT_SCALE;
    if (scaleMax < scaleMin) scaleMax = scaleMin;

    int cols = (int)(sizeX / spacing);
    int rows = (int)(sizeZ / spacing);

    float startX = center.x - sizeX * 0.5f;
    float startZ = center.z - sizeZ * 0.5f;

    for (int z = 0; z < rows; z++)
    {
        for (int x = 0; x < cols; x++)
        {
            if (gCornCount >= CORN_MAX_INSTANCES) return;

            float fx = startX + x * spacing;
            float fz = startZ + z * spacing;

            if (jitter > 0.0f)
            {
                fx += ((float)GetRandomValue(-1000, 1000) / 1000.0f) * jitter;
                fz += ((float)GetRandomValue(-1000, 1000) / 1000.0f) * jitter;
            }

            float yaw = ((float)GetRandomValue(0, 359)) * DEG2RAD;

            float scale = scaleMin;
            if (scaleMax > scaleMin)
            {
                float t = (float)GetRandomValue(0, 1000) / 1000.0f;
                scale = scaleMin + (scaleMax - scaleMin) * t;
            }

            Corn_AddPlant((Vector3) { fx, center.y, fz }, yaw, scale);
        }
    }
}

static inline void DrawCornFields(Vector3 donPos, Frustum frustum, bool drawDebug)
{
    if (!gCornReady) return;
    if (gCornModel.meshCount <= 0) return;

    int visibleCount = 0;
    float maxDistSq = CORN_DRAW_DISTANCE * CORN_DRAW_DISTANCE;

    for (int i = 0; i < gCornCount; i++)
    {
        CornPlant* c = &gCorn[i];

        float dx = c->pos.x - donPos.x;
        float dz = c->pos.z - donPos.z;
        float distSq = dx * dx + dz * dz;

        if (distSq > maxDistSq) { continue; }
        if (!IsPointInFrustum(c->pos, frustum)) { continue; }

        Matrix S = MatrixScale(c->scale/2.0, c->scale, c->scale/2.0);
        Matrix R = MatrixRotateY(c->yaw);
        Matrix T = MatrixTranslate(c->pos.x, c->pos.y, c->pos.z);

        gCornTransforms[visibleCount++] = MatrixMultiply(MatrixMultiply(S, R), T);

        if (drawDebug) {
            DrawSphere(c->pos, 0.2f, YELLOW);
        }

        if (visibleCount >= CORN_MAX_INSTANCES) break;
    }

    if (visibleCount <= 0) return;

    BeginShaderMode(gCornMaterial.shader);
    DrawMeshInstanced(
        gCornModel.meshes[0],
        gCornMaterial,
        gCornTransforms,
        visibleCount
    );
    EndShaderMode();
}

static inline void Corn_Unload(void)
{
    if (gCornModel.meshCount > 0) UnloadModel(gCornModel);
    if (gCornTexture.id > 0) UnloadTexture(gCornTexture);
    gCornReady = false;
}

#endif // CORN_H