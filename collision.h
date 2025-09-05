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
    int cx = (int)floorf(x / CHUNK_WORLD_SIZE) + half;
    int cy = (int)floorf(z / CHUNK_WORLD_SIZE) + half;
    
    if (onLoad && cx > -1 && cy > -1 && cx < CHUNK_COUNT && cy < CHUNK_COUNT)
    {
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
    }

    //TraceLog(LOG_WARNING, "Not found in any triangle: (%f , %f)", x, z);
    TraceLog(LOG_WARNING, "Not found in any triangle: (%.3f , %.3f) (%d %d)", (double)x, (double)z, cx, cy);
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

// -----------------------------------------------------------------------------
// AABB vs Mesh triangles (ground+wall resolution)
// -----------------------------------------------------------------------------
typedef struct MeshBoxHit {
    bool  hit;         // any contact at all
    bool  hitGround;   // stood on ground (mostly flat tri)
    float groundY;     // y to place Donogan at (if hitGround)
    Vector3 normal;    // normal of the last contacting triangle
    Vector3 push;      // gentle horizontal push for walls
} MeshBoxHit;

// simple AABB overlap (inclusive)
static inline bool AabbOverlap(BoundingBox a, BoundingBox b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
        (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
        (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

// overlap amount along one axis (returns 0 if no overlap)
static inline float AxisOverlap(float aMin, float aMax, float bMin, float bMax) {
    float left = bMax - aMin; // push left -> right
    float right = aMax - bMin; // push right -> left
    float o = (left < right) ? left : right;
    return (o > 0.0f) ? o : 0.0f;
}

// classify “flat enough” using cosine of max slope angle.
// Example: cos(50°) ≈ 0.642 => anything with n.y >= 0.64 is ground-like.
#define DEFAULT_GROUND_SLOPE_COS 0.64f

// Epsilon tolerances
#define GROUND_EPS_BELOW   0.02f   // how far below feet we still accept
#define GROUND_MAX_STEP    1.0f    // optional: maximum step-up allowed
#define WALL_PUSH_SCALE    0.6f    // scale for gentle push
#define HORIZ_EPS          1e-4f

// NOTE: `meshWorldOffset` should be the world-space base (e.g., scene/chunk position).
//       `meshScale` uniformly scales the mesh vertices (homes use this).
//       `groundSlopeCos`: e.g., DEFAULT_GROUND_SLOPE_COS (or cosf(DEG2RAD*maxSlopeDeg))
// Rotate around Y using precomputed sin/cos
static inline Vector3 RotY(Vector3 v, float s, float c) {
    return (Vector3) { v.x* c + v.z * s, v.y, -v.x * s + v.z * c };
}

// Same return struct as your original
static inline MeshBoxHit CollideAABBWithMeshTriangles(
    BoundingBox box,
    const Mesh* mesh,
    Vector3 meshWorldOffset,  // world position of model
    float meshScale,          // Scenes[i].scale
    float yawRadians,         // Scenes[i].yaw
    float groundSlopeCos,     // keep for API parity
    bool  wallsOnly           // true => ignore ground classification
) {
    MeshBoxHit out = (MeshBoxHit){ 0 };
    out.groundY = -10000.0f;
    if (!mesh || mesh->vertexCount < 3 || mesh->triangleCount < 1) return out;

    float* verts = (float*)mesh->vertices;
    // NOTE: If your imported model uses 32-bit indices, change this cast:
    unsigned short* tris = (unsigned short*)mesh->indices;

    const float s = sinf(yawRadians), c = cosf(yawRadians);

    // For a representative XZ point when checking ground, use the box center
    Vector3 boxCenter = {
        0.5f * (box.min.x + box.max.x),
        0.5f * (box.min.y + box.max.y),
        0.5f * (box.min.z + box.max.z)
    };

    float bestGroundY = -10000.0f;
    Vector3 wallPushAccum = (Vector3){ 0 };

    int triCount = mesh->triangleCount;
    for (int i = 0; i < triCount; i++) {
        int i0, i1, i2;
        if (tris) { i0 = tris[i * 3 + 0]; i1 = tris[i * 3 + 1]; i2 = tris[i * 3 + 2]; }
        else { i0 = i * 3 + 0;       i1 = i * 3 + 1;       i2 = i * 3 + 2; }
        if (i0 >= mesh->vertexCount || i1 >= mesh->vertexCount || i2 >= mesh->vertexCount) continue;

        // IMPORTANT: scale + rotate + translate (NO MAP_SCALE here for models)
        Vector3 a = RotY((Vector3) { verts[i0 * 3 + 0] * meshScale, verts[i0 * 3 + 1] * meshScale, verts[i0 * 3 + 2] * meshScale }, s, c);
        Vector3 b = RotY((Vector3) { verts[i1 * 3 + 0] * meshScale, verts[i1 * 3 + 1] * meshScale, verts[i1 * 3 + 2] * meshScale }, s, c);
        Vector3 c3 = RotY((Vector3) { verts[i2 * 3 + 0] * meshScale, verts[i2 * 3 + 1] * meshScale, verts[i2 * 3 + 2] * meshScale }, s, c);
        a = Vector3Add(a, meshWorldOffset);
        b = Vector3Add(b, meshWorldOffset);
        c3 = Vector3Add(c3, meshWorldOffset);

        // --- the rest is identical to your existing routine ---
        // build tri AABB, quick reject vs box; compute normal n = normalize(cross(b-a, c-a));
        // if (!wallsOnly && n.y >= groundSlopeCos) evaluate ground plane at (boxCenter.xz) and keep highest <= feet+eps;
        // else accumulate gentle horizontal wall push using your AxisOverlap/horizontal normal approach.
        // (Reuse your existing ground + wall code paths here to keep behavior consistent.)
    }

    if (bestGroundY > -1000.0f && !wallsOnly) { out.hitGround = true; out.groundY = bestGroundY; }
    if (Vector3Length(wallPushAccum) > 0.0f) { out.hit = true; out.push = Vector3Scale(Vector3Normalize(wallPushAccum), WALL_PUSH_SCALE); }
    return out;
}

//static inline MeshBoxHit CollideAABBWithMeshTriangles(
//    BoundingBox box,
//    const Mesh* mesh,
//    Vector3 meshWorldOffset,
//    float meshScale,
//    float groundSlopeCos
//) {
//    MeshBoxHit out = (MeshBoxHit){ 0 };
//    out.groundY = -10000.0f;
//
//    if (!mesh || mesh->vertexCount < 3 || mesh->triangleCount < 1) {
//        return out;
//    }
//
//    float* verts = (float*)mesh->vertices;
//    unsigned short* tris = (unsigned short*)mesh->indices;
//
//    // We'll accumulate the *highest* valid ground Y and a single gentle wall push
//    float bestGroundY = -10000.0f;
//    Vector3 bestGroundN = (Vector3){ 0,1,0 };
//    Vector3 wallPushAccum = (Vector3){ 0 };
//
//    // For a representative XZ point when checking ground, use the box center
//    Vector3 boxCenter = {
//        0.5f * (box.min.x + box.max.x),
//        0.5f * (box.min.y + box.max.y),
//        0.5f * (box.min.z + box.max.z)
//    };
//
//    // Combined scale
//    const float S = MAP_SCALE * meshScale;
//
//    int triCount = mesh->triangleCount;
//    for (int i = 0; i < triCount; i++) {
//        int i0, i1, i2;
//        if (tris) {
//            i0 = tris[i * 3 + 0];
//            i1 = tris[i * 3 + 1];
//            i2 = tris[i * 3 + 2];
//        }
//        else {
//            i0 = i * 3 + 0; i1 = i * 3 + 1; i2 = i * 3 + 2;
//        }
//        if (i0 >= mesh->vertexCount || i1 >= mesh->vertexCount || i2 >= mesh->vertexCount) continue;
//
//        // Apply scale S and then world offset
//        Vector3 a = { S * verts[i0 * 3 + 0] + meshWorldOffset.x,
//                      S * verts[i0 * 3 + 1] + meshWorldOffset.y,
//                      S * verts[i0 * 3 + 2] + meshWorldOffset.z };
//        Vector3 b = { S * verts[i1 * 3 + 0] + meshWorldOffset.x,
//                      S * verts[i1 * 3 + 1] + meshWorldOffset.y,
//                      S * verts[i1 * 3 + 2] + meshWorldOffset.z };
//        Vector3 c = { S * verts[i2 * 3 + 0] + meshWorldOffset.x,
//                      S * verts[i2 * 3 + 1] + meshWorldOffset.y,
//                      S * verts[i2 * 3 + 2] + meshWorldOffset.z };
//
//        // quick tri-AABB coarse test via triangle’s AABB
//        BoundingBox triBox;
//        triBox.min = (Vector3){ fminf(a.x, fminf(b.x, c.x)),
//                                fminf(a.y, fminf(b.y, c.y)),
//                                fminf(a.z, fminf(b.z, c.z)) };
//        triBox.max = (Vector3){ fmaxf(a.x, fmaxf(b.x, c.x)),
//                                fmaxf(a.y, fmaxf(b.y, c.y)),
//                                fmaxf(a.z, fmaxf(b.z, c.z)) };
//        if (!AabbOverlap(box, triBox)) continue;
//
//        // --- the rest of your existing normal/ground/wall logic stays the same ---
//        Vector3 e1 = Vector3Subtract(b, a);
//        Vector3 e2 = Vector3Subtract(c, a);
//        Vector3 n = Vector3Normalize(Vector3CrossProduct(e1, e2));
//        if (n.y < 0.0f) n = Vector3Negate(n);
//
//        if (n.y >= groundSlopeCos) {
//            float y = GetHeightOnTriangle((Vector3) { boxCenter.x, 0, boxCenter.z }, a, b, c);
//            if (y > -9999.0f) {
//                float feetY = box.min.y;
//                float deltaUp = y - feetY;
//                if (deltaUp >= -GROUND_EPS_BELOW && deltaUp <= GROUND_MAX_STEP) {
//                    if (y > bestGroundY) { bestGroundY = y; bestGroundN = n; }
//                    out.hit = true;
//                }
//            }
//        }
//        else {
//            if ((box.min.y <= triBox.max.y) && (box.max.y >= triBox.min.y)) {
//                float ox = AxisOverlap(box.min.x, box.max.x, triBox.min.x, triBox.max.x);
//                float oz = AxisOverlap(box.min.z, box.max.z, triBox.min.z, triBox.max.z);
//                if (ox > 0.0f && oz > 0.0f) {
//                    Vector3 push = (Vector3){ 0 };
//                    if (ox < oz) {
//                        float sign = (boxCenter.x < 0.5f * (triBox.min.x + triBox.max.x)) ? -1.0f : +1.0f;
//                        push.x = sign * ox * WALL_PUSH_SCALE;
//                    }
//                    else {
//                        float sign = (boxCenter.z < 0.5f * (triBox.min.z + triBox.max.z)) ? -1.0f : +1.0f;
//                        push.z = sign * oz * WALL_PUSH_SCALE;
//                    }
//                    Vector3 hn = (Vector3){ n.x, 0.0f, n.z };
//                    float hlen = Vector3Length(hn);
//                    if (hlen > HORIZ_EPS) {
//                        hn = Vector3Scale(hn, (ox < oz ? ox : oz) * (WALL_PUSH_SCALE * 0.5f) / hlen);
//                        push = Vector3Add(push, hn);
//                    }
//                    wallPushAccum = Vector3Add(wallPushAccum, push);
//                    out.hit = true;
//                    out.normal = n;
//                }
//            }
//        }
//    }
//
//    if (bestGroundY > -9999.0f) {
//        out.hitGround = true;
//        out.groundY = bestGroundY;
//        out.normal = bestGroundN;
//        out.push = (Vector3){ 0 };
//    }
//    else if (Vector3LengthSqr(wallPushAccum) > 0.0f) {
//        out.push = wallPushAccum;
//    }
//    return out;
//}

void DebugLogMeshBoxHit(
    const char* tag, int sceneIndex,
    BoundingBox box, Vector3 donPos,
    MeshBoxHit hit,
    Vector3 meshWorldOffset, float meshScale
) {
    TraceLog(LOG_INFO,
        "[%s] scene=%d  donPos=(%.2f,%.2f,%.2f)  "
        "boxMin=(%.2f,%.2f,%.2f) boxMax=(%.2f,%.2f,%.2f)  "
        "hit=%d ground=%d groundY=%.3f  "
        "normal=(%.3f,%.3f,%.3f)  push=(%.3f,%.3f,%.3f)  "
        "meshOfs=(%.2f,%.2f,%.2f) scale=%.2f",
        tag, sceneIndex,
        donPos.x, donPos.y, donPos.z,
        box.min.x, box.min.y, box.min.z,
        box.max.x, box.max.y, box.max.z,
        hit.hit ? 1 : 0, hit.hitGround ? 1 : 0, hit.groundY,
        hit.normal.x, hit.normal.y, hit.normal.z,
        hit.push.x, hit.push.y, hit.push.z,
        meshWorldOffset.x, meshWorldOffset.y, meshWorldOffset.z, meshScale
    );
}


#endif // COLLISION_H
