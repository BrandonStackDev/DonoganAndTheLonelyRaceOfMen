#ifndef CAVE_LIGHT_H
#define CAVE_LIGHT_H

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdbool.h>
#include <stdio.h>

#define CAVE_TORCH_MAX 32
#define CAVE_SHADER_LIGHT_MAX 8

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
    int locAmbient;
    int locLightCount;
    int locLightPos[CAVE_SHADER_LIGHT_MAX];
    int locLightColor[CAVE_SHADER_LIGHT_MAX];
    int locLightRadius[CAVE_SHADER_LIGHT_MAX];
    int locLightStrength[CAVE_SHADER_LIGHT_MAX];

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

    // Texture samplers.
    // These names must match your pbr.fs.
    s->locs[SHADER_LOC_MAP_ALBEDO] = GetShaderLocation(*s, "albedoMap");
    s->locs[SHADER_LOC_MAP_METALNESS] = GetShaderLocation(*s, "metalnessMap");
    s->locs[SHADER_LOC_MAP_NORMAL] = GetShaderLocation(*s, "normalMap");
    s->locs[SHADER_LOC_MAP_EMISSION] = GetShaderLocation(*s, "emissionMap");
}

static inline void CaveLight_AddTorch(Vector3 pos, bool lit)
{
    if (gCaveLight.torchCount >= CAVE_TORCH_MAX) return;

    CaveTorch* t = &gCaveLight.torches[gCaveLight.torchCount++];

    t->pos = pos;
    t->lit = lit;
    t->startsLit = lit;
    t->radius = 85.0f;
    t->strength = 3.5f;
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
    gCaveLight.locAmbient = GetShaderLocation(gCaveLight.pbrShader, "ambientColor");
    gCaveLight.locLightCount = GetShaderLocation(gCaveLight.pbrShader, "lightCount");

    for (int i = 0; i < CAVE_SHADER_LIGHT_MAX; i++)
    {
        char name[64];

        TextFormat(""); // keeps raylib linked; harmless

        snprintf(name, sizeof(name), "lights[%d].pos", i);
        gCaveLight.locLightPos[i] = GetShaderLocation(gCaveLight.pbrShader, name);

        snprintf(name, sizeof(name), "lights[%d].color", i);
        gCaveLight.locLightColor[i] = GetShaderLocation(gCaveLight.pbrShader, name);

        snprintf(name, sizeof(name), "lights[%d].radius", i);
        gCaveLight.locLightRadius[i] = GetShaderLocation(gCaveLight.pbrShader, name);

        snprintf(name, sizeof(name), "lights[%d].strength", i);
        gCaveLight.locLightStrength[i] = GetShaderLocation(gCaveLight.pbrShader, name);
    }

    // Torch geometry: raylib cylinders.
    gCaveLight.torchPole = LoadModelFromMesh(GenMeshCylinder(0.13f, 3.0f, 8));
    gCaveLight.torchHead = LoadModelFromMesh(GenMeshCylinder(0.35f, 0.55f, 10));

    gCaveLight.torchPole.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = (Color){ 75, 45, 25, 255 };
    gCaveLight.torchHead.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = (Color){ 45, 35, 30, 255 };

    // TEMP POSITIONS.
    // Replace these when you grab real cave positions.
    CaveLight_AddTorch((Vector3) { -437.22, 660.94, -1124.01 }, true);
    CaveLight_AddTorch((Vector3) { -388.66, 660.96, -1124.75 }, true);
    CaveLight_AddTorch((Vector3) { -418.29, 660.93, -1132.54 }, true);
    CaveLight_AddTorch((Vector3) { -463.38, 612.97, -1151.04 }, true);
    CaveLight_AddTorch((Vector3) { -463.41, 612.97, -1211.26 }, true);
    CaveLight_AddTorch((Vector3) { -378.39, 613.15, -1212.42 }, false);
    CaveLight_AddTorch((Vector3) { -378.38, 613.15, -1119.28 }, true);
    CaveLight_AddTorch((Vector3) { -376.59, 613.16, -1157.61 }, false);
    CaveLight_AddTorch((Vector3) { -462.88, 612.97, -1118.36 }, false);
    CaveLight_AddTorch((Vector3) { -423.84, 613.06, -1117.81 }, true);

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

    Vector3 ambient = caveMode
        ? (Vector3) { 0.006f, 0.005f, 0.009f }   // almost black
    : (Vector3) { 0.25f, 0.25f, 0.25f };

    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locViewPos, &viewPos, SHADER_UNIFORM_VEC3);
    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locAmbient, &ambient, SHADER_UNIFORM_VEC3);

    int pushed = 0;

    if (caveMode)
    {
        for (int i = 0; i < gCaveLight.torchCount && pushed < CAVE_SHADER_LIGHT_MAX; i++)
        {
            CaveTorch* t = &gCaveLight.torches[i];
            if (!t->lit) continue;

            Vector3 lightPos = t->pos;
            lightPos.y += 2.2f;

            Vector3 color = { 1.0f, 0.48f, 0.16f };
            float radius = t->radius;
            float strength = t->strength;

            SetShaderValue(gCaveLight.pbrShader, gCaveLight.locLightPos[pushed], &lightPos, SHADER_UNIFORM_VEC3);
            SetShaderValue(gCaveLight.pbrShader, gCaveLight.locLightColor[pushed], &color, SHADER_UNIFORM_VEC3);
            SetShaderValue(gCaveLight.pbrShader, gCaveLight.locLightRadius[pushed], &radius, SHADER_UNIFORM_FLOAT);
            SetShaderValue(gCaveLight.pbrShader, gCaveLight.locLightStrength[pushed], &strength, SHADER_UNIFORM_FLOAT);

            pushed++;
        }
    }

    SetShaderValue(gCaveLight.pbrShader, gCaveLight.locLightCount, &pushed, SHADER_UNIFORM_INT);
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

        DrawModel(gCaveLight.torchPole, polePos, 1.0f, WHITE);

        Vector3 headPos = t->pos;
        headPos.y += 3.08f;

        DrawModel(gCaveLight.torchHead, headPos, 1.0f, WHITE);

        if (t->lit)
        {
            Vector3 flamePos = headPos;
            flamePos.y += 0.35f;

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
                WHITE
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