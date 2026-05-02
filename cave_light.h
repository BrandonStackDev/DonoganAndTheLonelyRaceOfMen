#ifndef CAVE_LIGHT_H
#define CAVE_LIGHT_H

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdbool.h>
#include <stdio.h>

#define CAVE_TORCH_MAX 16
#define CAVE_SHADER_LIGHT_MAX 16

typedef struct CaveTorch {
    Vector3 pos;
    bool lit;
    bool startsLit;
    float radius;
    float strength;
} CaveTorch;

typedef struct CaveLightSystem {
    Shader pbrShader;

    int locViewPos;
    int locAmbientColor;
    int locAmbient;
    int locNumOfLights;

    int locTiling;
    int locOffset;

    int locUseTexAlbedo;
    int locUseTexNormal;
    int locUseTexMRA;
    int locUseTexEmissive;

    int locAlbedoColor;
    int locEmissiveColor;
    int locNormalValue;
    int locMetallicValue;
    int locRoughnessValue;
    int locAoValue;
    int locEmissivePower;

    int locLightEnabled[CAVE_SHADER_LIGHT_MAX];
    int locLightType[CAVE_SHADER_LIGHT_MAX];
    int locLightPosition[CAVE_SHADER_LIGHT_MAX];
    int locLightTarget[CAVE_SHADER_LIGHT_MAX];
    int locLightColor[CAVE_SHADER_LIGHT_MAX];
    int locLightIntensity[CAVE_SHADER_LIGHT_MAX];

    Model torchPole;
    Model torchHead;

    CaveTorch torches[CAVE_TORCH_MAX];
    int torchCount;

    bool ready;
} CaveLightSystem;

static CaveLightSystem gCaveLight = { 0 };

static inline void Cave_SetShaderLocations(Shader* s)
{
    s->locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(*s, "mvp");
    s->locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(*s, "matModel");
    s->locs[SHADER_LOC_MATRIX_NORMAL] = GetShaderLocation(*s, "matNormal");
    s->locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(*s, "viewPos");

    // These names match your pbr.fs
    s->locs[SHADER_LOC_MAP_DIFFUSE] = GetShaderLocation(*s, "albedoMap");
    s->locs[SHADER_LOC_MAP_METALNESS] = GetShaderLocation(*s, "mraMap");
    s->locs[SHADER_LOC_MAP_NORMAL] = GetShaderLocation(*s, "normalMap");
    s->locs[SHADER_LOC_MAP_EMISSION] = GetShaderLocation(*s, "emissiveMap");
}

static inline void CaveLight_AddTorch(Vector3 pos, bool lit)
{
    if (gCaveLight.torchCount >= CAVE_TORCH_MAX) return;

    CaveTorch* t = &gCaveLight.torches[gCaveLight.torchCount++];

    t->pos = pos;
    t->lit = lit;
    t->startsLit = lit;
    t->radius = 55;
    t->strength = 8;
}

static inline void CaveLight_Init(void)
{
    gCaveLight = (CaveLightSystem){ 0 };

    gCaveLight.pbrShader = LoadShader(
        "shaders/100/pbr.vs",
        "shaders/100/pbr.fs"
    );

    Cave_SetShaderLocations(&gCaveLight.pbrShader);

    gCaveLight.locViewPos = GetShaderLocation(gCaveLight.pbrShader, "viewPos");
    gCaveLight.locAmbientColor = GetShaderLocation(gCaveLight.pbrShader, "ambientColor");
    gCaveLight.locAmbient = GetShaderLocation(gCaveLight.pbrShader, "ambient");
    gCaveLight.locNumOfLights = GetShaderLocation(gCaveLight.pbrShader, "numOfLights");

    gCaveLight.locTiling = GetShaderLocation(gCaveLight.pbrShader, "tiling");
    gCaveLight.locOffset = GetShaderLocation(gCaveLight.pbrShader, "offset");

    gCaveLight.locUseTexAlbedo = GetShaderLocation(gCaveLight.pbrShader, "useTexAlbedo");
    gCaveLight.locUseTexNormal = GetShaderLocation(gCaveLight.pbrShader, "useTexNormal");
    gCaveLight.locUseTexMRA = GetShaderLocation(gCaveLight.pbrShader, "useTexMRA");
    gCaveLight.locUseTexEmissive = GetShaderLocation(gCaveLight.pbrShader, "useTexEmissive");

    gCaveLight.locAlbedoColor = GetShaderLocation(gCaveLight.pbrShader, "albedoColor");
    gCaveLight.locEmissiveColor = GetShaderLocation(gCaveLight.pbrShader, "emissiveColor");
    gCaveLight.locNormalValue = GetShaderLocation(gCaveLight.pbrShader, "normalValue");
    gCaveLight.locMetallicValue = GetShaderLocation(gCaveLight.pbrShader, "metallicValue");
    gCaveLight.locRoughnessValue = GetShaderLocation(gCaveLight.pbrShader, "roughnessValue");
    gCaveLight.locAoValue = GetShaderLocation(gCaveLight.pbrShader, "aoValue");
    gCaveLight.locEmissivePower = GetShaderLocation(gCaveLight.pbrShader, "emissivePower");

    for (int i = 0; i < CAVE_SHADER_LIGHT_MAX; i++)
    {
        char name[64];

        snprintf(name, sizeof(name), "lights[%d].enabled", i);
        gCaveLight.locLightEnabled[i] = GetShaderLocation(gCaveLight.pbrShader, name);

        snprintf(name, sizeof(name), "lights[%d].type", i);
        gCaveLight.locLightType[i] = GetShaderLocation(gCaveLight.pbrShader, name);

        snprintf(name, sizeof(name), "lights[%d].position", i);
        gCaveLight.locLightPosition[i] = GetShaderLocation(gCaveLight.pbrShader, name);

        snprintf(name, sizeof(name), "lights[%d].target", i);
        gCaveLight.locLightTarget[i] = GetShaderLocation(gCaveLight.pbrShader, name);

        snprintf(name, sizeof(name), "lights[%d].color", i);
        gCaveLight.locLightColor[i] = GetShaderLocation(gCaveLight.pbrShader, name);

        snprintf(name, sizeof(name), "lights[%d].intensity", i);
        gCaveLight.locLightIntensity[i] = GetShaderLocation(gCaveLight.pbrShader, name);
    }

    // Torch geometry: raylib cylinders.
    gCaveLight.torchPole = LoadModelFromMesh(GenMeshCylinder(0.13f, 3.0f, 8));
    gCaveLight.torchHead = LoadModelFromMesh(GenMeshCylinder(0.35f, 0.55f, 10));

    gCaveLight.torchPole.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = (Color){ 75, 45, 25, 255 };
    gCaveLight.torchHead.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = (Color){ 45, 35, 30, 255 };

    // TEMP POSITIONS.
    // Replace these when you grab real cave positions.
    CaveLight_AddTorch((Vector3) { -437.22, 660.94, -1124.01 }, true);
    CaveLight_AddTorch((Vector3) { -388.66, 660.96, -1124.75 }, false);
    CaveLight_AddTorch((Vector3) { -418.29, 660.93, -1132.54 }, false);
    CaveLight_AddTorch((Vector3) { -463.38, 612.97, -1151.04 }, false);
    CaveLight_AddTorch((Vector3) { -463.41, 612.97, -1211.26 }, true);
    CaveLight_AddTorch((Vector3) { -378.39, 613.15, -1212.42 }, false);
    CaveLight_AddTorch((Vector3) { -378.38, 613.15, -1119.28 }, false);
    CaveLight_AddTorch((Vector3) { -376.59, 613.16, -1157.61 }, false);
    CaveLight_AddTorch((Vector3) { -462.88, 612.97, -1118.36 }, false);
    CaveLight_AddTorch((Vector3) { -423.84, 613.06, -1117.81 }, false);
    //bottom
    CaveLight_AddTorch((Vector3) { -458.61, 566.95, -1123.05 }, false);
    CaveLight_AddTorch((Vector3) { -372.36, 568.64, -1133.93 }, false);
    CaveLight_AddTorch((Vector3) { -451.36, 566.97, -1158.41 }, false);
    CaveLight_AddTorch((Vector3) { -381.59, 567.12, -1212.85 }, false);
    CaveLight_AddTorch((Vector3) { -449.49, 566.97, -1203.50 }, false);
    CaveLight_AddTorch((Vector3) { -442.59, 566.99, -1181.10 }, false);

    gCaveLight.ready = true;
}

static inline void CaveLight_ApplyToCinderModels(Model* cinder, Model* cave)
{
    if (!gCaveLight.ready) return;

    if (cinder)
    {
        for (int i = 0; i < cinder->materialCount; i++)
        {
            cinder->materials[i].shader = gCaveLight.pbrShader;
        }
    }

    if (cave)
    {
        for (int i = 0; i < cave->materialCount; i++)
        {
            cave->materials[i].shader = gCaveLight.pbrShader;
        }
    }
}

static inline void CaveLight_UpdateShader(Vector3 viewPos, bool caveMode)
{
    if (!gCaveLight.ready) return;

    // Basic PBR material controls.
    Vector2 tiling = { 1.0f, 1.0f };
    Vector2 offset = { 0.0f, 0.0f };

    int useTexAlbedo = 1;
    int useTexNormal = 0;    // keep OFF first; OBJ tangents may be bad/missing
    int useTexMRA = 1;
    int useTexEmissive = 1;

    Vector4 albedoColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    Vector4 emissiveColor = { 1.0f, 0.45f, 0.12f, 1.0f };

    float normalValue = 1.0f;
    float metallicValue = 0.0f;
    float roughnessValue = 0.55f;
    float aoValue = 1.0f;
    float emissivePower = 1.25f;

    Vector3 ambientColor = (Vector3) { 0.0004f, 0.00025f, 0.0009f };

    float ambient = 0.0000002;

    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locViewPos, &viewPos, SHADER_UNIFORM_VEC3);

    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locTiling, &tiling, SHADER_UNIFORM_VEC2);
    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locOffset, &offset, SHADER_UNIFORM_VEC2);

    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locUseTexAlbedo, &useTexAlbedo, SHADER_UNIFORM_INT);
    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locUseTexNormal, &useTexNormal, SHADER_UNIFORM_INT);
    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locUseTexMRA, &useTexMRA, SHADER_UNIFORM_INT);
    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locUseTexEmissive, &useTexEmissive, SHADER_UNIFORM_INT);

    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locAlbedoColor, &albedoColor, SHADER_UNIFORM_VEC4);
    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locEmissiveColor, &emissiveColor, SHADER_UNIFORM_VEC4);
    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locNormalValue, &normalValue, SHADER_UNIFORM_FLOAT);
    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locMetallicValue, &metallicValue, SHADER_UNIFORM_FLOAT);
    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locRoughnessValue, &roughnessValue, SHADER_UNIFORM_FLOAT);
    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locAoValue, &aoValue, SHADER_UNIFORM_FLOAT);
    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locEmissivePower, &emissivePower, SHADER_UNIFORM_FLOAT);

    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locAmbientColor, &ambientColor, SHADER_UNIFORM_VEC3);
    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locAmbient, &ambient, SHADER_UNIFORM_FLOAT);

    int pushed = 0;

    for (int i = 0; i < gCaveLight.torchCount && pushed < CAVE_SHADER_LIGHT_MAX; i++)
    {
        CaveTorch* t = &gCaveLight.torches[i];
        if (!caveMode || !t->lit) continue;

        int enabled = 1;
        int type = 1; // point light

        Vector3 lightPos = t->pos;
        lightPos.y += 3.45f;

        Vector3 target = { 0.0f, 0.0f, 0.0f };
        Vector4 color = { 1.0f, 0.48f, 0.16f, 1.0f };
        float intensity = t->strength;

        SetShaderValue(gCaveLight.pbrShader, gCaveLight.locLightEnabled[pushed], &enabled, SHADER_UNIFORM_INT);
        SetShaderValue(gCaveLight.pbrShader, gCaveLight.locLightType[pushed], &type, SHADER_UNIFORM_INT);
        SetShaderValue(gCaveLight.pbrShader, gCaveLight.locLightPosition[pushed], &lightPos, SHADER_UNIFORM_VEC3);
        SetShaderValue(gCaveLight.pbrShader, gCaveLight.locLightTarget[pushed], &target, SHADER_UNIFORM_VEC3);
        SetShaderValue(gCaveLight.pbrShader, gCaveLight.locLightColor[pushed], &color, SHADER_UNIFORM_VEC4);
        SetShaderValue(gCaveLight.pbrShader, gCaveLight.locLightIntensity[pushed], &intensity, SHADER_UNIFORM_FLOAT);

        pushed++;
    }

    for (int i = pushed; i < CAVE_SHADER_LIGHT_MAX; i++)
    {
        int enabled = 0;
        int type = 1;
        Vector3 pos = { 0.0f, 0.0f, 0.0f };
        Vector3 target = { 0.0f, 0.0f, 0.0f };
        Vector4 color = { 0.0f, 0.0f, 0.0f, 1.0f };
        float intensity = 0.0f;

        SetShaderValue(gCaveLight.pbrShader, gCaveLight.locLightEnabled[i], &enabled, SHADER_UNIFORM_INT);
        SetShaderValue(gCaveLight.pbrShader, gCaveLight.locLightType[i], &type, SHADER_UNIFORM_INT);
        SetShaderValue(gCaveLight.pbrShader, gCaveLight.locLightPosition[i], &pos, SHADER_UNIFORM_VEC3);
        SetShaderValue(gCaveLight.pbrShader, gCaveLight.locLightTarget[i], &target, SHADER_UNIFORM_VEC3);
        SetShaderValue(gCaveLight.pbrShader, gCaveLight.locLightColor[i], &color, SHADER_UNIFORM_VEC4);
        SetShaderValue(gCaveLight.pbrShader, gCaveLight.locLightIntensity[i], &intensity, SHADER_UNIFORM_FLOAT);
    }

    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locNumOfLights, &pushed, SHADER_UNIFORM_INT);
}

static inline void CaveLight_LightNearbyTorch(Vector3 donPos)
{
    for (int i = 0; i < gCaveLight.torchCount; i++)
    {
        CaveTorch* t = &gCaveLight.torches[i];

        if (t->lit) continue;

        if (Vector3DistanceSqr(donPos, t->pos) < 9.0f * 9.0f)
        {
            t->lit = true;
        }
    }
}

static inline void CaveLight_DrawTorches(Model fireModel, Shader fireShader, int fireVariantLoc, bool caveMode)
{
    if (!gCaveLight.ready) return;
    if (!caveMode) return;

    for (int i = 0; i < gCaveLight.torchCount; i++)
    {
        CaveTorch* t = &gCaveLight.torches[i];

        Vector3 polePos = t->pos;
        polePos.y += 1.5f;

        DrawModel(gCaveLight.torchPole, polePos, 1.0f, DARKBLUE);

        Vector3 headPos = t->pos;
        headPos.y += 3.48f;

        DrawModel(gCaveLight.torchHead, headPos, 1.0f, DARKPURPLE);

        if (t->lit)
        {
            Vector3 flamePos = headPos;
            flamePos.y += 0.8f;

            BeginBlendMode(BLEND_ADDITIVE);

            float v = (float)GetTime() + (float)i * 1.37f;
            SetShaderValue(fireShader, fireVariantLoc, &v, SHADER_UNIFORM_FLOAT);

            DrawModelEx(
                fireModel,
                flamePos,
                (Vector3) {
                0, 1, 0
            },
                0.0f,
                (Vector3) {
                0.55f, 0.75f, 0.55f
            },
                WHITE
            );

            v += 2.15f;
            SetShaderValue(fireShader, fireVariantLoc, &v, SHADER_UNIFORM_FLOAT);

            DrawModelEx(
                fireModel,
                flamePos,
                (Vector3) {
                0, 1, 0
            },
                0.0f,
                (Vector3) {
                0.85f, 0.55f, 0.85f
            },
                YELLOW
            );

            EndBlendMode();
        }
    }
}

static inline void CaveLight_Shutdown(void)
{
    if (!gCaveLight.ready) return;

    UnloadModel(gCaveLight.torchPole);
    UnloadModel(gCaveLight.torchHead);
    UnloadShader(gCaveLight.pbrShader);

    gCaveLight = (CaveLightSystem){ 0 };
}

#endif