#ifndef PORTAL_H
#define PORTAL_H

#include "raylib.h"
#include "raymath.h"
#include "donogan.h"

#define PORTAL_RADIUS 5
#define PORTAL_RADIUS_SQ PORTAL_RADIUS * PORTAL_RADIUS
#define NUM_PORTALS 1


typedef struct {
    Vector3 pos, warp;
} Portal;

Portal portals[NUM_PORTALS];
Shader portalShader;
int pTimeLoc;
int pViewLoc;
int pColorLoc;
int pIntLoc;
int pDispLoc;
int pNoiseLoc;
Model portalModel;

static void InitPortals()
{
    //shader stuff
    portalShader = LoadShader("shaders/120/portal.vs", "shaders/120/portal.fs");
    portalShader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(portalShader, "mvp");
    portalShader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(portalShader, "model");
    int pTimeLoc = GetShaderLocation(portalShader, "uTime");
    int pViewLoc = GetShaderLocation(portalShader, "uViewPos");
    int pColorLoc = GetShaderLocation(portalShader, "uColor");
    int pIntLoc = GetShaderLocation(portalShader, "uIntensity");
    int pDispLoc = GetShaderLocation(portalShader, "uDispAmp");
    int pNoiseLoc = GetShaderLocation(portalShader, "uNoiseScale");
    portalModel = LoadModelFromMesh(GenMeshSphere(PORTAL_RADIUS, 64, 64));
    portalModel.materials[0].shader = portalShader;
    //init all portals
    portals[0] = (Portal){ (Vector3) { -419.69, 620, -1198.05 },(Vector3) { -660.72, 800, -918.63 } }; //pos then warp
}

static void DetectPortals(Donogan* d)
{
    for (int i = 0; i < NUM_PORTALS; i++)
    {
        if (Vector3DistanceSqr(d->pos,portals[i].pos) < PORTAL_RADIUS_SQ) 
        {
            d->pos = portals[i].warp; //warp
        }
    }
}

static void DrawPortals(Donogan* d)
{
    float time = GetTime();
    SetShaderValue(portalShader, GetShaderLocation(portalShader, "uTime"), &time, SHADER_UNIFORM_FLOAT);
    SetShaderValue(portalShader, GetShaderLocation(portalShader, "uViewPos"), &(d->pos), SHADER_UNIFORM_VEC3);
    Vector3 color = (Vector3){ 0.0439f, 0.0122f, 0.0494f }; // darkpurple
    SetShaderValue(portalShader, GetShaderLocation(portalShader, "uColor"), &color, SHADER_UNIFORM_VEC3);
    float intensity = 1.5f, dispAmp = 0.5f, nscale = 2.0f;
    SetShaderValue(portalShader, GetShaderLocation(portalShader, "uIntensity"), &intensity, SHADER_UNIFORM_FLOAT);
    SetShaderValue(portalShader, GetShaderLocation(portalShader, "uDispAmp"), &dispAmp, SHADER_UNIFORM_FLOAT);
    SetShaderValue(portalShader, GetShaderLocation(portalShader, "uNoiseScale"), &nscale, SHADER_UNIFORM_FLOAT);

    BeginBlendMode(BLEND_ADDITIVE); // make it bloom with itself
    for (int i = 0; i < NUM_PORTALS; i++) {
        //culling
        if (Vector3DistanceSqr(d->pos, portals[i].pos) > 200 * 200) { continue; }
        //draw
        Matrix m = MatrixMultiply(
            MatrixScale(1.12, 1.12, 1.12),
            MatrixTranslate(portals[i].pos.x, portals[i].pos.y, portals[i].pos.z)
        );
        DrawMesh(portalModel.meshes[0], portalModel.materials[0], m);
    }
    EndBlendMode();
}

#endif //PORTAL_H