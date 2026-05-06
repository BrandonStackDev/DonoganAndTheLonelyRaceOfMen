#ifndef COLLISION_H
#define COLLISION_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
//me
#include "core.h"
#include "jc.h"

#define BUILDING_FLOOR_STAND_PAD 0.12f

#define BUILDING_WALL_SKIN_WORLD            0.2f
#define BUILDING_FLOOR_SKIN_WORLD           0.2f
#define BUILDING_CEILING_SKIN_WORLD         0.2f
#define BUILDING_FLOOR_XZ_SKIN_WORLD        0.2f
#define BUILDING_FLOOR_CATCH_ABOVE_WORLD    0.012f
#define BUILDING_FLOOR_CATCH_BELOW_WORLD    3.0f
#define BUILDING_FLOOR_CATCH_BELOW_WORLD_BIG 12.0f
#define BUILDING_CEILING_CATCH_WORLD        0.2f
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
    //TraceLog(LOG_WARNING, "Not found in any triangle: (%.3f , %.3f) (%d %d)", (double)x, (double)z, cx, cy);
    return -10000.0f; // Not found in any triangle
}

// Returns the up-facing terrain normal under (x, z).
// If nothing is found, logs a warning and returns (0,1,0) as a safe default.
Vector3 GetTerrainNormalFromMeshXZ(float x, float z)
{
    int half = CHUNK_COUNT / 2;
    int cx = (int)floorf(x / CHUNK_WORLD_SIZE) + half;
    int cy = (int)floorf(z / CHUNK_WORLD_SIZE) + half;

    if (onLoad && cx > -1 && cy > -1 && cx < CHUNK_COUNT && cy < CHUNK_COUNT)
    {
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
    bool  hitWall;
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
#define GROUND_MAX_STEP    4.2f    // optional: maximum step-up allowed
#define WALL_PUSH_SCALE    0.6f    // scale for gentle push
#define HORIZ_EPS          1e-4f

// NOTE: `meshWorldOffset` should be the world-space base (e.g., scene/chunk position).
//       `meshScale` uniformly scales the mesh vertices (homes use this).
//       `groundSlopeCos`: e.g., DEFAULT_GROUND_SLOPE_COS (or cosf(DEG2RAD*maxSlopeDeg))
// Rotate around Y using precomputed sin/cos
static inline Vector3 RotY(Vector3 v, float s, float c) {
    return (Vector3) { v.x* c + v.z * s, v.y, -v.x * s + v.z * c };
}

typedef struct {
    BuildingColliderType type;
    Vector3 v[8];

    Vector3 center;
    Vector3 axis[3];
    float half[3];
} BuildingColliderWorld;

static inline Vector3 Building_RotateY(Vector3 p, float yaw)
{
    float c = cosf(yaw);
    float s = sinf(yaw);

    // Match existing home mesh collision/draw convention.
    return (Vector3) {
        p.x* c + p.z * s,
            p.y,
            -p.x * s + p.z * c
    };
}

static inline Vector3 Building_LocalToWorldPoint(Vector3 p, Vector3 scenePos, float sceneScale, float sceneYaw)
{
    p = Vector3Scale(p, sceneScale);
    p = Building_RotateY(p, sceneYaw);
    return Vector3Add(p, scenePos);
}

static inline float SafeLen(Vector3 v)
{
    float l = Vector3Length(v);
    return (l < 0.0001f) ? 0.0001f : l;
}

static inline BuildingColliderWorld Building_MakeWorldCollider(
    const BuildingColliderRaw* raw,
    Vector3 scenePos,
    float sceneScale,
    float sceneYaw)
{
    BuildingColliderWorld out = { 0 };
    out.type = raw->type;

    for (int i = 0; i < 8; i++)
    {
        out.v[i] = Building_LocalToWorldPoint(raw->local[i], scenePos, sceneScale, sceneYaw);
    }

    // Center from all 8 verts.
    out.center = (Vector3){ 0 };
    for (int i = 0; i < 8; i++)
    {
        out.center = Vector3Add(out.center, out.v[i]);
    }
    out.center = Vector3Scale(out.center, 1.0f / 8.0f);

    // These are authored rectangular-ish prisms.
    // Use 0->1, 0->3, 0->4 as local box axes.
    Vector3 e0 = Vector3Subtract(out.v[1], out.v[0]);
    Vector3 e1 = Vector3Subtract(out.v[3], out.v[0]);
    Vector3 e2 = Vector3Subtract(out.v[4], out.v[0]);

    out.half[0] = SafeLen(e0) * 0.5f;
    out.half[1] = SafeLen(e1) * 0.5f;
    out.half[2] = SafeLen(e2) * 0.5f;

    out.axis[0] = Vector3Normalize(e0);
    out.axis[1] = Vector3Normalize(e1);
    out.axis[2] = Vector3Normalize(e2);

    return out;
}

static inline BoundingBox Building_WorldColliderAABB(const BuildingColliderWorld* c)
{
    Vector3 mn = { FLT_MAX, FLT_MAX, FLT_MAX };
    Vector3 mx = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (int i = 0; i < 8; i++)
    {
        Vector3 p = c->v[i];

        if (p.x < mn.x) mn.x = p.x;
        if (p.y < mn.y) mn.y = p.y;
        if (p.z < mn.z) mn.z = p.z;

        if (p.x > mx.x) mx.x = p.x;
        if (p.y > mx.y) mx.y = p.y;
        if (p.z > mx.z) mx.z = p.z;
    }

    return (BoundingBox) { mn, mx };
}

typedef struct BuildingBoxHit {
    bool hit;
    bool hitFloor;
    bool hitCeiling;
    bool hitWall;

    float groundY;
    Vector3 push;
    Vector3 normal;
    float maxWallY;
} BuildingBoxHit;

static inline void ProjectAABBOnAxis(BoundingBox b, Vector3 axis, float* outMin, float* outMax)
{
    Vector3 corners[8] = {
        { b.min.x, b.min.y, b.min.z },
        { b.max.x, b.min.y, b.min.z },
        { b.min.x, b.max.y, b.min.z },
        { b.max.x, b.max.y, b.min.z },
        { b.min.x, b.min.y, b.max.z },
        { b.max.x, b.min.y, b.max.z },
        { b.min.x, b.max.y, b.max.z },
        { b.max.x, b.max.y, b.max.z }
    };

    float mn = Vector3DotProduct(corners[0], axis);
    float mx = mn;

    for (int i = 1; i < 8; i++)
    {
        float d = Vector3DotProduct(corners[i], axis);
        if (d < mn) mn = d;
        if (d > mx) mx = d;
    }

    *outMin = mn;
    *outMax = mx;
}

static inline void ProjectColliderOnAxis(const BuildingColliderWorld* c, Vector3 axis, float* outMin, float* outMax)
{
    float mn = Vector3DotProduct(c->v[0], axis);
    float mx = mn;

    for (int i = 1; i < 8; i++)
    {
        float d = Vector3DotProduct(c->v[i], axis);
        if (d < mn) mn = d;
        if (d > mx) mx = d;
    }

    *outMin = mn;
    *outMax = mx;
}

static inline Vector3 BoxCenter(BoundingBox b)
{
    return (Vector3) {
        (b.min.x + b.max.x) * 0.5f,
            (b.min.y + b.max.y) * 0.5f,
            (b.min.z + b.max.z) * 0.5f
    };
}

static inline bool AABBVsBuildingOBB(
    BoundingBox aabb,
    const BuildingColliderWorld* c,
    Vector3* outPush,
    Vector3* outNormal)
{
    // Broad phase.
    BoundingBox cb = Building_WorldColliderAABB(c);
    if (!CheckCollisionBoxes(aabb, cb)) return false;

    Vector3 axes[6] = {
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 0, 0, 1 },
        c->axis[0],
        c->axis[1],
        c->axis[2]
    };

    float bestOverlap = FLT_MAX;
    Vector3 bestAxis = { 0 };

    Vector3 ac = BoxCenter(aabb);
    Vector3 toA = Vector3Subtract(ac, c->center);

    for (int i = 0; i < 6; i++)
    {
        Vector3 axis = axes[i];

        if (Vector3LengthSqr(axis) < 0.00001f) continue;
        axis = Vector3Normalize(axis);

        float amin, amax, bmin, bmax;
        ProjectAABBOnAxis(aabb, axis, &amin, &amax);
        ProjectColliderOnAxis(c, axis, &bmin, &bmax);

        float overlap = fminf(amax, bmax) - fmaxf(amin, bmin);

        if (overlap <= 0.0f)
        {
            return false;
        }

        if (overlap < bestOverlap)
        {
            bestOverlap = overlap;
            bestAxis = axis;
        }
    }

    // Make push direction point from collider toward Don.
    if (Vector3DotProduct(bestAxis, toA) < 0.0f)
    {
        bestAxis = Vector3Negate(bestAxis);
    }

    if (outNormal) *outNormal = bestAxis;
    if (outPush) *outPush = Vector3Scale(bestAxis, bestOverlap + 0.02f);

    return true;
}

static inline bool AABBVsBuildingOBBEx(
    BoundingBox aabb,
    const BuildingColliderWorld* c,
    float skin,
    Vector3* outPush,
    Vector3* outNormal)
{
    // Broad phase with inflated collider AABB.
    BoundingBox cb = Building_WorldColliderAABB(c);

    cb.min.x -= skin;
    cb.max.x += skin;
    if (c->type != BCOL_WALL)
    {
        cb.min.y -= skin;
        cb.max.y += skin;
    }
    cb.min.z -= skin;
    cb.max.z += skin;

    if (!CheckCollisionBoxes(aabb, cb)) return false;
    //if (c->type == BCOL_WALL)
    //{
    //    float wallTop = BuildingColliderMaxY(c);
    //    const float WALL_TOP_IGNORE_PAD = 0.15f;

    //    // If Don's feet are clearly above the top of this wall prism,
    //    // this buried/low wall should not block him.
    //    if (aabb.min.y > wallTop + WALL_TOP_IGNORE_PAD)
    //    {
    //        return false;
    //    }
    //}

    Vector3 axes[6] = {
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 0, 0, 1 },
        c->axis[0],
        c->axis[1],
        c->axis[2]
    };

    float bestOverlap = FLT_MAX;
    Vector3 bestAxis = { 0 };

    Vector3 ac = BoxCenter(aabb);
    Vector3 toA = Vector3Subtract(ac, c->center);

    for (int i = 0; i < 6; i++)
    {
        Vector3 axis = axes[i];

        if (Vector3LengthSqr(axis) < 0.00001f) continue;
        axis = Vector3Normalize(axis);

        float amin, amax, bmin, bmax;

        ProjectAABBOnAxis(aabb, axis, &amin, &amax);
        ProjectColliderOnAxis(c, axis, &bmin, &bmax);

        // Runtime forgiveness. The file can be paper-thin;
        // collision behaves as if it has usable thickness.
        float useSkin = skin;

        // For walls, do not inflate vertically.
        // This prevents wall colliders from catching Don at floor/ledge seams.
        if (c->type == BCOL_WALL && fabsf(axis.y) > 0.4f)
        {
            useSkin = 0.0f;
        }

        bmin -= useSkin;
        bmax += useSkin;

        float overlap = fminf(amax, bmax) - fmaxf(amin, bmin);

        if (overlap <= 0.0f)
        {
            return false;
        }

        if (overlap < bestOverlap)
        {
            bestOverlap = overlap;
            bestAxis = axis;
        }
    }

    if (Vector3DotProduct(bestAxis, toA) < 0.0f)
    {
        bestAxis = Vector3Negate(bestAxis);
    }

    if (outNormal) *outNormal = bestAxis;
    if (outPush) *outPush = Vector3Scale(bestAxis, bestOverlap + 0.02f);

    return true;
}

static inline float BuildingColliderMaxY(const BuildingColliderWorld* c)
{
    float y = c->v[0].y;
    for (int i = 1; i < 8; i++) if (c->v[i].y > y) y = c->v[i].y;
    return y;
}

static inline float BuildingColliderFloorYAtXZ(const BuildingColliderWorld* c, float x, float z)
{
    // Try the top/selected quad as two triangles.
    // Your authored prism stores 0-3 as one side and 4-7 as the other side.
    // For floors, one of those sides is the actual sloped surface.
    Vector3 p = { x, 0.0f, z };

    float yA = GetHeightOnTriangle(p, c->v[0], c->v[1], c->v[2]);
    if (yA > -9999.0f) return yA;

    float yB = GetHeightOnTriangle(p, c->v[0], c->v[2], c->v[3]);
    if (yB > -9999.0f) return yB;

    float yC = GetHeightOnTriangle(p, c->v[4], c->v[5], c->v[6]);
    if (yC > -9999.0f) return yC;

    float yD = GetHeightOnTriangle(p, c->v[4], c->v[6], c->v[7]);
    if (yD > -9999.0f) return yD;

    return -10000.0f;
}

static inline float BuildingColliderMinY(const BuildingColliderWorld* c)
{
    float y = c->v[0].y;
    for (int i = 1; i < 8; i++) if (c->v[i].y < y) y = c->v[i].y;
    return y;
}

static inline bool Building_AABBOverlapsXZ(BoundingBox a, BoundingBox b, float skin)
{
    return (a.min.x <= b.max.x + skin && a.max.x >= b.min.x - skin) &&
        (a.min.z <= b.max.z + skin && a.max.z >= b.min.z - skin);
}

static inline BuildingBoxHit CollideDonAABBWithBuildingFloorCeilingSensor(
    BoundingBox donBox,
    const BuildingColliderWorld* c,
    float donVelY)
{
    BuildingBoxHit out = { 0 };
    out.groundY = -10000.0f;

    BoundingBox cb = Building_WorldColliderAABB(c);

    const float XZ_SKIN = BUILDING_FLOOR_XZ_SKIN_WORLD;
    const float FLOOR_GRACE_ABOVE = BUILDING_FLOOR_CATCH_ABOVE_WORLD;
    const float FLOOR_GRACE_BELOW = donVelY<=-12.04? BUILDING_FLOOR_CATCH_BELOW_WORLD_BIG:BUILDING_FLOOR_CATCH_BELOW_WORLD;
    const float CEILING_GRACE = BUILDING_CEILING_CATCH_WORLD;

    if (!Building_AABBOverlapsXZ(donBox, cb, XZ_SKIN))
    {
        return out;
    }

    float feetY = donBox.min.y;
    float headY = donBox.max.y;

    if (c->type == BCOL_FLOOR)
    {
        Vector3 center = BoxCenter(donBox);
        float y = BuildingColliderFloorYAtXZ(c, center.x, center.z);
        y += BUILDING_FLOOR_STAND_PAD;

        if (y < -9999.0f)
        {
            y = BuildingColliderMaxY(c); // fallback only
        }

        // Don can be slightly below because frames/tiny slabs/tall speeds.
        // Don can be above by a few units so we can catch landing/support.
        if (feetY >= y - FLOOR_GRACE_BELOW && feetY <= y + FLOOR_GRACE_ABOVE)
        {
            out.hit = true;
            out.hitFloor = true;
            out.groundY = y;
            out.normal = (Vector3){ 0, 1, 0 };
        }
    }
    else if (c->type == BCOL_CEILING)
    {
        float y = BuildingColliderMinY(c);

        // Only treat it as a ceiling if Don's head is below/near the ceiling plane.
        // This prevents the duplicate ceiling made by F from pushing Don down
        // when he is standing on top of the same surface.
        if (headY <= y + CEILING_GRACE && headY >= y - CEILING_GRACE)
        {
            out.hit = true;
            out.hitCeiling = true;
            out.push = (Vector3){ 0.0f, -0.25f, 0.0f };
            out.normal = (Vector3){ 0, -1, 0 };
        }
    }

    return out;
}

static inline BuildingBoxHit CollideDonAABBWithBuildingCollider(
    BoundingBox donBox,
    const BuildingColliderWorld* c,
    float donVelY)
{
    BuildingBoxHit out = { 0 };
    out.groundY = -10000.0f;

    if (c->type == BCOL_FLOOR || c->type == BCOL_CEILING)
    {
        return CollideDonAABBWithBuildingFloorCeilingSensor(donBox, c, donVelY);
    }

    Vector3 push = { 0 };
    Vector3 normal = { 0 };

    float skin = 0.0f;

    if (c->type == BCOL_WALL)
    {
        skin = BUILDING_WALL_SKIN_WORLD;
    }
    else if (c->type == BCOL_FLOOR) //these arent needed but just incase we make changes and thats nolonger the case, keeping it
    {
        skin = BUILDING_FLOOR_SKIN_WORLD;
    }
    else if (c->type == BCOL_CEILING)
    {
        skin = BUILDING_CEILING_SKIN_WORLD;
    }

    if (!AABBVsBuildingOBBEx(donBox, c, skin, &push, &normal))
    {
        return out;
    }

    out.hit = true;
    out.push = push;
    out.normal = normal;

    if (c->type == BCOL_FLOOR)
    {
        out.hitFloor = true;
        out.groundY = BuildingColliderMaxY(c);
    }
    else if (c->type == BCOL_CEILING)
    {
        out.hitCeiling = true;
    }
    else
    {
        out.hitWall = true;
        out.maxWallY = -99999;
        for (int i = 0; i < 8; i++)
        {
            if (c->v[i].y > out.maxWallY) { out.maxWallY = c->v[i].y; }
        }
    }

    return out;
}
static inline bool Building_AABBPairOverlapsXZ(BoundingBox prevBox, BoundingBox currBox, BoundingBox b, float skin)
{
    float aMinX = fminf(prevBox.min.x, currBox.min.x);
    float aMaxX = fmaxf(prevBox.max.x, currBox.max.x);
    float aMinZ = fminf(prevBox.min.z, currBox.min.z);
    float aMaxZ = fmaxf(prevBox.max.z, currBox.max.z);

    return (aMinX <= b.max.x + skin && aMaxX >= b.min.x - skin) &&
        (aMinZ <= b.max.z + skin && aMaxZ >= b.min.z - skin);
}

static inline BuildingBoxHit CollideDonAABBWithSceneBuildingCollidersSweptY(
    BoundingBox prevBox,
    BoundingBox currBox,
    const Scene* scene,
    float donVelY)
{
    BuildingBoxHit best = { 0 };
    best.groundY = -10000.0f;

    if (!scene) return best;
    if (scene->modelType < 0 || scene->modelType >= MODEL_HOME_TOTAL_COUNT) return best;

    BuildingColliderSet* set = &HomeCollisionSets[scene->modelType];
    if (!set->loaded || set->count <= 0) return best;

    const float XZ_SKIN = BUILDING_FLOOR_XZ_SKIN_WORLD;
    const float FLOOR_GRACE_ABOVE = BUILDING_FLOOR_CATCH_ABOVE_WORLD;
    const float FLOOR_GRACE_BELOW = donVelY <= -12.04 ? BUILDING_FLOOR_CATCH_BELOW_WORLD_BIG : BUILDING_FLOOR_CATCH_BELOW_WORLD;
    const float CEILING_GRACE = BUILDING_CEILING_CATCH_WORLD;

    float prevFeetY = prevBox.min.y;
    float currFeetY = currBox.min.y;

    float prevHeadY = prevBox.max.y;
    float currHeadY = currBox.max.y;

    for (int i = 0; i < set->count; i++)
    {
        BuildingColliderWorld wc = Building_MakeWorldCollider(
            &set->cols[i],
            scene->pos,
            scene->scale,
            scene->yaw
        );

        BoundingBox cb = Building_WorldColliderAABB(&wc);

        if (!Building_AABBPairOverlapsXZ(prevBox, currBox, cb, XZ_SKIN))
        {
            continue;
        }

        if (wc.type == BCOL_FLOOR)
        {
            Vector3 center = BoxCenter(currBox);
            float y = BuildingColliderFloorYAtXZ(&wc, center.x, center.z);
            y += BUILDING_FLOOR_STAND_PAD;
            if (y < -9999.0f)
            {
                y = BuildingColliderMaxY(&wc); // fallback only
            }

            // Falling/settling crossed the floor plane this frame.
            bool crossedDown =
                (prevFeetY >= y - FLOOR_GRACE_ABOVE) &&
                (currFeetY <= y + FLOOR_GRACE_BELOW);

            if (crossedDown)
            {
                best.hit = true;
                best.hitFloor = true;

                if (y > best.groundY)
                {
                    best.groundY = y;
                    best.normal = (Vector3){ 0, 1, 0 };
                }
            }
        }
        else if (wc.type == BCOL_CEILING)
        {
            float y = BuildingColliderMinY(&wc);

            // Jumping upward crossed the ceiling plane this frame.
            bool crossedUp =
                (prevHeadY <= y + CEILING_GRACE) &&
                (currHeadY >= y - CEILING_GRACE);

            if (crossedUp)
            {
                best.hit = true;
                best.hitCeiling = true;
                best.push = (Vector3){ 0.0f, -0.35f, 0.0f };
                best.normal = (Vector3){ 0, -1, 0 };
            }
        }
    }

    return best;
}
static inline BuildingBoxHit CollideDonAABBWithSceneBuildingColliders(
    BoundingBox donBox,
    const Scene* scene,
    float donVelY)
{
    BuildingBoxHit best = { 0 };
    best.groundY = -10000.0f;

    if (!scene) return best;
    if (scene->modelType < 0 || scene->modelType >= MODEL_HOME_TOTAL_COUNT) return best;

    BuildingColliderSet* set = &HomeCollisionSets[scene->modelType];

    if (!set->loaded || set->count <= 0)
    {
        return best;
    }

    for (int i = 0; i < set->count; i++)
    {
        BuildingColliderWorld wc = Building_MakeWorldCollider(
            &set->cols[i],
            scene->pos,
            scene->scale,
            scene->yaw
        );

        BuildingBoxHit h = CollideDonAABBWithBuildingCollider(donBox, &wc, donVelY);
        if (!h.hit) continue;

        best.hit = true;

        if (h.hitFloor)
        {
            best.hitFloor = true;
            if (h.groundY > best.groundY)
            {
                best.groundY = h.groundY;
            }
        }
        else if (h.hitCeiling)
        {
            best.hitCeiling = true;

            // For ceiling, push only vertically downward for now.
            if (fabsf(h.push.y) > fabsf(best.push.y))
            {
                best.push = h.push;
                best.normal = h.normal;
            }
        }
        else if (h.hitWall)
        {
            best.hitWall = true;

            // Prefer horizontal wall push.
            h.push.y = 0.0f;

            if (Vector3LengthSqr(h.push) > Vector3LengthSqr(best.push))
            {
                best.push = h.push;
                best.normal = h.normal;
            }
        }
    }

    return best;
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
    Vector3 bestGroundN = (Vector3){ 0,1,0 };
    Vector3 wallPushAccum = (Vector3){ 0 };
    float feetY = box.min.y;
    bool lookForFloor = false; float minAcceptableY = 0;

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

        // quick tri-AABB coarse test via triangle’s AABB
        const float SKIN = 0.05f; // 5 cm
        BoundingBox triBox;
        triBox.min = (Vector3){ fminf(a.x, fminf(b.x, c3.x)),
                                fminf(a.y, fminf(b.y, c3.y)),
                                fminf(a.z, fminf(b.z, c3.z)) };
        triBox.max = (Vector3){ fmaxf(a.x, fmaxf(b.x, c3.x)),
                                fmaxf(a.y, fmaxf(b.y, c3.y)),
                                fmaxf(a.z, fmaxf(b.z, c3.z)) };
        triBox.min.x -= SKIN; triBox.max.x += SKIN;
        triBox.min.z -= SKIN; triBox.max.z += SKIN;
        if (!AabbOverlap(box, triBox)) { continue; }

        // --- the rest of your existing normal/ground/wall logic stays the same ---
        Vector3 e1 = Vector3Subtract(b, a);
        Vector3 e2 = Vector3Subtract(c3, a);
        Vector3 n = Vector3Normalize(Vector3CrossProduct(e1, e2));
        if (n.y < 0.0f)  { n = Vector3Negate(n); }

        if (n.y >= groundSlopeCos) {
            float y = GetHeightOnTriangle((Vector3) { boxCenter.x, 0, boxCenter.z }, a, b, c3);
            if (y > -9999.0f) {
                float deltaUp = y - feetY;
                if (deltaUp >= -GROUND_EPS_BELOW && deltaUp <= GROUND_MAX_STEP)
                {
                    if (y > bestGroundY) { bestGroundY = y; bestGroundN = n; }
                    out.hit = true;
                }
            }
        }
        else 
        {
            float triYSpan = triBox.max.y - box.min.y;

            // Small vertical lip / floor edge: do not treat as wall.
            // Let ground/ceiling logic handle it.
            if (triYSpan < 0.6 && triYSpan > 0)
            {
                lookForFloor = true;
                if (triBox.max.y + 0.02 > minAcceptableY) { minAcceptableY = triBox.max.y + 0.02; }
                TraceLog(LOG_WARNING, "lookForFloor!");
            }
            if ((box.min.y <= triBox.max.y) && (box.max.y >= triBox.min.y)) {
                float ox = AxisOverlap(box.min.x, box.max.x, triBox.min.x, triBox.max.x);
                float oz = AxisOverlap(box.min.z, box.max.z, triBox.min.z, triBox.max.z);
                if (ox > 0.0f && oz > 0.0f) {
                    Vector3 push = (Vector3){ 0 };
                    if (ox < oz) {
                        float sign = (boxCenter.x < 0.5f * (triBox.min.x + triBox.max.x)) ? -1.0f : +1.0f;
                        push.x = sign * ox * WALL_PUSH_SCALE;
                    }
                    else {
                        float sign = (boxCenter.z < 0.5f * (triBox.min.z + triBox.max.z)) ? -1.0f : +1.0f;
                        push.z = sign * oz * WALL_PUSH_SCALE;
                    }
                    // dir from triangle to the player box center
                    Vector3 triCenter = (Vector3){
                        (a.x + b.x + c3.x) * (1.0f / 3.0f),
                        (a.y + b.y + c3.y) * (1.0f / 3.0f),
                        (a.z + b.z + c3.z) * (1.0f / 3.0f)
                    };
                    Vector3 toBox = Vector3Subtract(boxCenter, triCenter);

                    // 1) ensure our axis MTV "push" points away from the tri
                    if (Vector3DotProduct(push, toBox) < 0.0f) { push = Vector3Negate(push); }

                    // 2) add a bit of horizontal normal, also guaranteed to face the box
                    Vector3 hn = (Vector3){ n.x, 0.0f, n.z };
                    float hlen = Vector3Length(hn);
                    if (hlen > HORIZ_EPS) {
                        hn = Vector3Scale(hn, 1.0f / hlen);
                        if (Vector3DotProduct(hn, toBox) < 0.0f) hn = Vector3Negate(hn);
                        float amt = (ox < oz ? ox : oz) * (WALL_PUSH_SCALE * 0.5f);
                        push = Vector3Add(push, Vector3Scale(hn, amt));
                    }
                    wallPushAccum = Vector3Add(wallPushAccum, push);
                    out.hit = true;
                    out.normal = n;
                }
            }
        }
    }

    out.hitGround = false;
    out.groundY = -10000.0f;
    out.normal = (Vector3){ 0,0,0 };
    out.push = (Vector3){ 0,0,0 };

    const bool hasGround = (bestGroundY > -9988.0f);
    bool hasWall = (Vector3LengthSqr(wallPushAccum) > 0.0f);

    if (hasGround) {
        out.hit = true;
        out.hitGround = true;
        out.groundY = bestGroundY;
        if (!hasWall) { out.normal = bestGroundN; }
        if (lookForFloor && minAcceptableY > out.groundY) { out.groundY = minAcceptableY; }
    }

    if (hasWall && !(lookForFloor && hasGround)) {
        out.hit = true;
        out.hitWall = true;

        wallPushAccum.y = 0.0f;
        out.push = Vector3Scale(wallPushAccum, WALL_PUSH_SCALE);

        if (!hasGround) out.normal = bestGroundN;
    }

    return out;
}

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
