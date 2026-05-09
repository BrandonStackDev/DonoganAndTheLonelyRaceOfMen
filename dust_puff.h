#ifndef DUST_PUFF_H
#define DUST_PUFF_H

#include "raylib.h"
#include "raymath.h"
#include "timer.h"

#define DUST_PUFF_MAX 16

typedef struct DustPuff {
    bool active;
    Vector3 pos;
    Timer timer;
    float age;
    float duration;
    float size;
    Color color;
} DustPuff;

static DustPuff gDustPuffs[DUST_PUFF_MAX];

static Shader gDustShader = { 0 };
static Texture2D gDustTexture = { 0 };

static int gDustTimeLoc = -1;
static int gDustDurationLoc = -1;
static int gDustColorLoc = -1;

static inline void DustPuff_Init(void)
{
    gDustShader = LoadShader("shaders/120/dust.vs", "shaders/120/dust.fs");
    gDustTimeLoc = GetShaderLocation(gDustShader, "uTime");
    gDustDurationLoc = GetShaderLocation(gDustShader, "uDuration");
    gDustColorLoc = GetShaderLocation(gDustShader, "uColor");
    gDustTexture = LoadTexture("textures/dust.png");

    for (int i = 0; i < DUST_PUFF_MAX; i++)
    {
        gDustPuffs[i] = (DustPuff){ 0 };
        gDustPuffs[i].timer = CreateTimer(1.5);
    }
}

static inline void DustPuff_Cleanup(void)
{
    if (gDustTexture.id != 0) UnloadTexture(gDustTexture);
    if (gDustShader.id != 0) UnloadShader(gDustShader);

    gDustTexture = (Texture2D){ 0 };
    gDustShader = (Shader){ 0 };
}

static inline void DustPuff_SpawnEx(Vector3 pos, float duration, float size, Color color)
{
    int slot = -1;

    for (int i = 0; i < DUST_PUFF_MAX; i++)
    {
        if (!gDustPuffs[i].active)
        {
            slot = i;
            break;
        }
    }

    // If full, overwrite oldest-ish first slot.
    if (slot < 0) slot = 0;

    DustPuff* p = &gDustPuffs[slot];

    p->active = true;
    p->pos = pos;
    p->duration = duration;
    p->size = size;
    p->color = color;
    p->age = 0;
    p->timer = CreateTimer(duration);
    StartTimer(&p->timer);
}

static inline void DustPuff_Spawn(Vector3 pos)
{
    DustPuff_SpawnEx(pos, 0.67, 2.4, (Color) { 200, 200, 200, 255 });
}

static inline void DustPuff_UpdateDraw(Camera3D camera)
{
    if (gDustShader.id == 0 || gDustTexture.id == 0) return;

    float dt = GetFrameTime();

    for (int i = 0; i < DUST_PUFF_MAX; i++)
    {
        DustPuff* p = &gDustPuffs[i];
        if (!p->active) continue;

        if (HasTimerElapsed(&p->timer))
        {
            p->active = false;
            continue;
        }

        p->age += dt;

        float color[4] = {
            p->color.r / 255.0f,
            p->color.g / 255.0f,
            p->color.b / 255.0f,
            p->color.a / 255.0f
        };

        BeginShaderMode(gDustShader);

        SetShaderValue(gDustShader, gDustTimeLoc, &p->age, SHADER_UNIFORM_FLOAT);
        SetShaderValue(gDustShader, gDustDurationLoc, &p->duration, SHADER_UNIFORM_FLOAT);
        SetShaderValue(gDustShader, gDustColorLoc, color, SHADER_UNIFORM_VEC4);

        DrawBillboard(camera, gDustTexture, p->pos, p->size, WHITE);

        EndShaderMode();
    }
}

#endif