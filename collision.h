#ifndef COLLISION_H
#define COLLISION_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h> 
#include <stdbool.h>
//me
#include "core.h"

////////////////////////////////////////////////////////////////////////////////
// Barycentric interpolation to get Y at point (x, z) on triangle
float GetHeightOnTriangle(Vector3 p, Vector3 a, Vector3 b, Vector3 c)
{
    // Convert to 2D XZ plane
    float px = p.x, pz = p.z;

    float ax = a.x, az = a.z;
    float bx = b.x, bz = b.z;
    float cx = c.x, cz = c.z;

    // Compute vectors
    float v0x = bx - ax;
    float v0z = bz - az;
    float v1x = cx - ax;
    float v1z = cz - az;
    float v2x = px - ax;
    float v2z = pz - az;

    // Compute dot products
    float d00 = v0x * v0x + v0z * v0z;
    float d01 = v0x * v1x + v0z * v1z;
    float d11 = v1x * v1x + v1z * v1z;
    float d20 = v2x * v0x + v2z * v0z;
    float d21 = v2x * v1x + v2z * v1z;

    // Compute barycentric coordinates
    float denom = d00 * d11 - d01 * d01;
    if (denom == 0.0f)
    {
        //TraceLog(LOG_INFO, "denom == 0");
        return -10000.0f;
    }

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    // If point is outside triangle
    if (u < 0 || v < 0 || w < 0)
    {
        //TraceLog(LOG_INFO, "Outside of plane (%.2f,%.2f,%.2f)", u, v, w);
        return -10000.0f;
    }

    // Interpolate Y
    return u * a.y + v * b.y + w * c.y;
}

float GetTerrainHeightFromMeshXZ(float x, float z)
{
    int half = CHUNK_COUNT / 2;
    int cx = (int)floor(x / CHUNK_WORLD_SIZE) + half;
    int cy = (int)floor(z / CHUNK_WORLD_SIZE) + half;

    Chunk chunk = chunks[cx][cy];
    Mesh mesh = chunk.model.meshes[0];

    int S = (int)sqrtf(mesh.triangleCount / 2); float cell = (float)CHUNK_WORLD_SIZE / S;
    int gx = (int)floorf((x - chunk.position.x) / cell), gz = (int)floorf((z - chunk.position.z) / cell);
    if (gx < 0) gx = 0; else if (gx >= S) gx = S - 1; if (gz < 0) gz = 0; else if (gz >= S) gz = S - 1;
    int firstTri = (gz * S + gx) * 2, lastTri = firstTri + 2;

    float* verts = (float*)mesh.vertices;
    unsigned short* tris = (unsigned short*)mesh.indices;
    //TraceLog(LOG_INFO, "chunk pos (%f, %f, %f)", chunk.position.x, chunk.position.y, chunk.position.z);
    //TraceLog(LOG_INFO, "chunk cen (%f, %f, %f)", chunk.center.x, chunk.center.y, chunk.center.z);
    if (!verts || mesh.vertexCount < 3 || mesh.triangleCount < 1)
    {
        TraceLog(LOG_WARNING, "Something wrong with collision: (%f x %f)", x, z);
        if (!verts) { TraceLog(LOG_WARNING, "!verts"); }
        if (mesh.vertexCount < 3) { TraceLog(LOG_WARNING, "mesh.vertexCount < 3"); }
        if (mesh.triangleCount < 1) { TraceLog(LOG_WARNING, "mesh.triangleCount < 1"); }
        return -10000.0f;
    }

    Vector3 p = { x, 0.0f, z };

    for (int i = firstTri; i < lastTri; i++)
    {
        int i0, i1, i2;

        if (tris) {
            i0 = tris[i * 3 + 0];
            i1 = tris[i * 3 + 1];
            i2 = tris[i * 3 + 2];
        }
        else {
            i0 = i * 3 + 0;
            i1 = i * 3 + 1;
            i2 = i * 3 + 2;
        }

        if (i0 >= mesh.vertexCount || i1 >= mesh.vertexCount || i2 >= mesh.vertexCount) { continue; }

        Vector3 a = {
            (MAP_SCALE * verts[i0 * 3 + 0] + chunk.position.x),
            (MAP_SCALE * verts[i0 * 3 + 1] + chunk.position.y),
            (MAP_SCALE * verts[i0 * 3 + 2] + chunk.position.z)
        };
        Vector3 b = {
            (MAP_SCALE * verts[i1 * 3 + 0] + chunk.position.x),
            (MAP_SCALE * verts[i1 * 3 + 1] + chunk.position.y),
            (MAP_SCALE * verts[i1 * 3 + 2] + chunk.position.z)
        };
        Vector3 c = {
            (MAP_SCALE * verts[i2 * 3 + 0] + chunk.position.x),
            (MAP_SCALE * verts[i2 * 3 + 1] + chunk.position.y),
            (MAP_SCALE * verts[i2 * 3 + 2] + chunk.position.z)
        };
        //TraceLog(LOG_INFO, "Tri %d verts: a=(%.2f,%.2f,%.2f)", i, a.x, a.y, a.z);
        //TraceLog(LOG_INFO, "Tri %d verts: b=(%.2f,%.2f,%.2f)", i, b.x, b.y, b.z);
        //TraceLog(LOG_INFO, "Tri %d verts: c=(%.2f,%.2f,%.2f)", i, c.x, c.y, c.z);
        float y = GetHeightOnTriangle((Vector3) { x, 0, z }, a, b, c);
        if (y > -9999.0f) return y;
    }

    TraceLog(LOG_WARNING, "Not found in any triangle: (%f x %f)", x, z);
    return -10000.0f; // Not found in any triangle
}

// Returns the up-facing terrain normal under (x, z).
// If nothing is found, logs a warning and returns (0,1,0) as a safe default.
Vector3 GetTerrainNormalFromMeshXZ(float x, float z)
{
    int half = CHUNK_COUNT / 2;
    int cx = (int)floorf(x / CHUNK_WORLD_SIZE) + half;
    int cy = (int)floorf(z / CHUNK_WORLD_SIZE) + half;

    Chunk chunk = chunks[cx][cy];
    Mesh mesh = chunk.model.meshes[0];

    int S = (int)sqrtf(mesh.triangleCount / 2);
    float cell = (float)CHUNK_WORLD_SIZE / S;

    int gx = (int)floorf((x - chunk.position.x) / cell);
    int gz = (int)floorf((z - chunk.position.z) / cell);
    if (gx < 0) gx = 0; else if (gx >= S) gx = S - 1;
    if (gz < 0) gz = 0; else if (gz >= S) gz = S - 1;

    int firstTri = (gz * S + gx) * 2;
    int lastTri = firstTri + 2;

    float* verts = (float*)mesh.vertices;
    unsigned short* tris = (unsigned short*)mesh.indices;

    if (!verts || mesh.vertexCount < 3 || mesh.triangleCount < 1)
    {
        TraceLog(LOG_WARNING, "Normal query bad mesh at (%f, %f)", x, z);
        if (!verts) { TraceLog(LOG_WARNING, "!verts"); }
        if (mesh.vertexCount < 3) { TraceLog(LOG_WARNING, "mesh.vertexCount < 3"); }
        if (mesh.triangleCount < 1) { TraceLog(LOG_WARNING, "mesh.triangleCount < 1"); }
        return (Vector3) { 0, 1, 0 };
    }

    Vector3 p = (Vector3){ x, 0.0f, z };

    for (int i = firstTri; i < lastTri; i++)
    {
        int i0, i1, i2;
        if (tris) {
            i0 = tris[i * 3 + 0];
            i1 = tris[i * 3 + 1];
            i2 = tris[i * 3 + 2];
        }
        else {
            i0 = i * 3 + 0;
            i1 = i * 3 + 1;
            i2 = i * 3 + 2;
        }

        if (i0 >= mesh.vertexCount || i1 >= mesh.vertexCount || i2 >= mesh.vertexCount) continue;

        Vector3 a = {
            (MAP_SCALE * verts[i0 * 3 + 0] + chunk.position.x),
            (MAP_SCALE * verts[i0 * 3 + 1] + chunk.position.y),
            (MAP_SCALE * verts[i0 * 3 + 2] + chunk.position.z)
        };
        Vector3 b = {
            (MAP_SCALE * verts[i1 * 3 + 0] + chunk.position.x),
            (MAP_SCALE * verts[i1 * 3 + 1] + chunk.position.y),
            (MAP_SCALE * verts[i1 * 3 + 2] + chunk.position.z)
        };
        Vector3 c = {
            (MAP_SCALE * verts[i2 * 3 + 0] + chunk.position.x),
            (MAP_SCALE * verts[i2 * 3 + 1] + chunk.position.y),
            (MAP_SCALE * verts[i2 * 3 + 2] + chunk.position.z)
        };

        // Use your existing point-in-triangle/height test to choose the correct tri
        float y = GetHeightOnTriangle(p, a, b, c);
        if (y > -9999.0f)
        {
            Vector3 e1 = Vector3Subtract(b, a);
            Vector3 e2 = Vector3Subtract(c, a);
            Vector3 n = Vector3Normalize(Vector3CrossProduct(e1, e2));
            if (n.y < 0.0f) n = Vector3Negate(n); // ensure "up"
            return n;
        }
    }

    TraceLog(LOG_WARNING, "Normal not found in any triangle: (%f x %f)", x, z);
    return (Vector3) { 0, 1, 0 }; // safe default
}


#endif // COLLISION_H
