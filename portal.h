#ifndef PORTAL_H
#define PORTAL_H

#include "raylib.h"
#include "raymath.h"
#include "donogan.h"

#define NUM_PORTALS 7

typedef struct {
    Vector3 pos, warp;
    float radius;
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
    portalModel = LoadModelFromMesh(GenMeshSphere(1, 64, 64));
    portalModel.materials[0].shader = portalShader;
    //init all portals
    portals[0] = (Portal){ (Vector3) { -419.69, 620, -1198.05 },(Vector3) { -660.72, 800, -918.63 }, 5.1 }; //pos then warp, cinderSpire
    portals[1] = (Portal){ (Vector3) { 2587.34, 479, 831.95 },(Vector3) { 2396, 600, 550 }, 4 }; //pos then warp, castle corner
    portals[2] = (Portal){ (Vector3) { -2560.12, 400, -2462.20 },(Vector3) { -2659.82, 400, -2499.97 }, 3 }; //ole stoney, stuckers (hard brittish accent)
    portals[3] = (Portal){ (Vector3) { -2120.68, 444, -2374.63 },(Vector3) { -2246.58, 500, -2237.34 }, 3 }; //cozy cottage, to corn field
    portals[4] = (Portal){ (Vector3) { -1107, 70, -7410 },(Vector3) { 0,1996,0 }, 32 }; //at the north in the ocean
    portals[5] = (Portal){ (Vector3) { -2465.81, 485, -1798.69 },(Vector3) { -2440, 560, -1825 }, 4 }; //wrenville barn
    portals[6] = (Portal){ (Vector3) { -441.94, 558, -1158.35 },(Vector3) { -660.72, 800, -918.63 }, 4 }; //bottom of cinderSpire
}

static void DetectPortals(Donogan* d)
{
    for (int i = 0; i < NUM_PORTALS; i++)
    {
        if (Vector3DistanceSqr(d->pos, portals[i].pos) < (portals[i].radius * portals[i].radius))
        {
            d->pos = portals[i].warp; //warp
            d->inWater = false;
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
        if (Vector3DistanceSqr(d->pos, portals[i].pos) > 500 * 600) { continue; }
        //draw
        float r = portals[i].radius + 0.12;
        Matrix m = MatrixMultiply(
            MatrixScale(r,r,r),
            MatrixTranslate(portals[i].pos.x, portals[i].pos.y, portals[i].pos.z)
        );
        DrawMesh(portalModel.meshes[0], portalModel.materials[0], m);
    }
    EndBlendMode();
}

#endif //PORTAL_H