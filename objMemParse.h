#ifndef OBJMEMPARSE_H
#define OBJMEMPARSE_H

#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

typedef struct ObjVec3 {
    float x, y, z;
} ObjVec3;

typedef struct ObjVec2 {
    float x, y;
} ObjVec2;

typedef struct ObjFaceVert {
    int v;
    int vt;
    int vn;
} ObjFaceVert;

typedef struct FloatArray {
    float* data;
    int count;
    int cap;
} FloatArray;

typedef struct Vec3Array {
    ObjVec3* data;
    int count;
    int cap;
} Vec3Array;

typedef struct Vec2Array {
    ObjVec2* data;
    int count;
    int cap;
} Vec2Array;

static void FloatArrayPush(FloatArray* a, float v)
{
    if (a->count >= a->cap) {
        int newCap = (a->cap > 0) ? a->cap * 2 : 1024;
        float* p = (float*)realloc(a->data, sizeof(float) * newCap);
        if (!p) return;
        a->data = p;
        a->cap = newCap;
    }
    a->data[a->count++] = v;
}

static void Vec3ArrayPush(Vec3Array* a, ObjVec3 v)
{
    if (a->count >= a->cap) {
        int newCap = (a->cap > 0) ? a->cap * 2 : 1024;
        ObjVec3* p = (ObjVec3*)realloc(a->data, sizeof(ObjVec3) * newCap);
        if (!p) return;
        a->data = p;
        a->cap = newCap;
    }
    a->data[a->count++] = v;
}

static void Vec2ArrayPush(Vec2Array* a, ObjVec2 v)
{
    if (a->count >= a->cap) {
        int newCap = (a->cap > 0) ? a->cap * 2 : 1024;
        ObjVec2* p = (ObjVec2*)realloc(a->data, sizeof(ObjVec2) * newCap);
        if (!p) return;
        a->data = p;
        a->cap = newCap;
    }
    a->data[a->count++] = v;
}

static void SkipSpaces(const char** p)
{
    while (**p == ' ' || **p == '\t') (*p)++;
}

static void SkipToNextLine(const char** p)
{
    while (**p && **p != '\n') (*p)++;
    if (**p == '\n') (*p)++;
}

static int ParseInt(const char** p, int* out)
{
    char* endPtr = NULL;
    long v = strtol(*p, &endPtr, 10);
    if (endPtr == *p) return 0;
    *out = (int)v;
    *p = endPtr;
    return 1;
}

static int ParseFloat(const char** p, float* out)
{
    char* endPtr = NULL;
    float v = strtof(*p, &endPtr);
    if (endPtr == *p) return 0;
    *out = v;
    *p = endPtr;
    return 1;
}

static int FixObjIndex(int idx, int count)
{
    // OBJ indices are 1-based. Negative indices are relative to the end.
    if (idx > 0) return idx - 1;
    if (idx < 0) return count + idx;
    return -1;
}

static int ParseFaceVertex(const char** p, ObjFaceVert* fv)
{
    fv->v = -1;
    fv->vt = -1;
    fv->vn = -1;

    int raw = 0;
    if (!ParseInt(p, &raw)) return 0;
    fv->v = raw;

    if (**p == '/') {
        (*p)++;

        if (**p != '/' && **p != ' ' && **p != '\t' && **p != '\r' && **p != '\n' && **p != '\0') {
            if (!ParseInt(p, &raw)) return 0;
            fv->vt = raw;
        }

        if (**p == '/') {
            (*p)++;
            if (**p != ' ' && **p != '\t' && **p != '\r' && **p != '\n' && **p != '\0') {
                if (!ParseInt(p, &raw)) return 0;
                fv->vn = raw;
            }
        }
    }

    return 1;
}

static void EmitFaceVertex(
    const ObjFaceVert* fv,
    const Vec3Array* positions,
    const Vec2Array* texcoords,
    const Vec3Array* normals,
    FloatArray* outVerts,
    FloatArray* outTex,
    FloatArray* outNorm)
{
    int vi = FixObjIndex(fv->v, positions->count);
    int vti = FixObjIndex(fv->vt, texcoords->count);
    int vni = FixObjIndex(fv->vn, normals->count);

    ObjVec3 p = { 0, 0, 0 };
    ObjVec2 t = { 0, 0 };
    ObjVec3 n = { 0, 0, 0 };

    if (vi >= 0 && vi < positions->count) p = positions->data[vi];
    if (vti >= 0 && vti < texcoords->count) t = texcoords->data[vti];
    if (vni >= 0 && vni < normals->count) n = normals->data[vni];

    FloatArrayPush(outVerts, p.x);
    FloatArrayPush(outVerts, p.y);
    FloatArrayPush(outVerts, p.z);

    FloatArrayPush(outTex, t.x);
    FloatArrayPush(outTex, t.y);

    FloatArrayPush(outNorm, n.x);
    FloatArrayPush(outNorm, n.y);
    FloatArrayPush(outNorm, n.z);
}

static void EmitTriangle(
    const ObjFaceVert* a,
    const ObjFaceVert* b,
    const ObjFaceVert* c,
    const Vec3Array* positions,
    const Vec2Array* texcoords,
    const Vec3Array* normals,
    FloatArray* outVerts,
    FloatArray* outTex,
    FloatArray* outNorm)
{
    EmitFaceVertex(a, positions, texcoords, normals, outVerts, outTex, outNorm);
    EmitFaceVertex(b, positions, texcoords, normals, outVerts, outTex, outNorm);
    EmitFaceVertex(c, positions, texcoords, normals, outVerts, outTex, outNorm);
}

static Mesh MakeEmptyMesh(void)
{
    Mesh m = { 0 };
    return m;
}

// Caller owns returned mesh CPU arrays and should later:
// UploadMesh(&mesh, false);  // main thread
// Model model = LoadModelFromMesh(mesh);
// ...
// UnloadModel(model); OR free arrays manually if abandoned before upload.
static Mesh LoadObjMeshFromMemory(const unsigned char* objBytes, unsigned int objLen)
{
    if (!objBytes || objLen == 0) return MakeEmptyMesh();

    char* text = (char*)malloc(objLen + 1);
    if (!text) return MakeEmptyMesh();

    memcpy(text, objBytes, objLen);
    text[objLen] = '\0';

    Vec3Array positions = { 0 };
    Vec2Array texcoords = { 0 };
    Vec3Array normals = { 0 };

    FloatArray outVerts = { 0 };
    FloatArray outTex = { 0 };
    FloatArray outNorm = { 0 };

    const char* p = text;

    while (*p)
    {
        SkipSpaces(&p);

        if (*p == '#') {
            SkipToNextLine(&p);
            continue;
        }

        if (*p == '\r' || *p == '\n') {
            SkipToNextLine(&p);
            continue;
        }

        if (p[0] == 'v' && isspace((unsigned char)p[1]))
        {
            p++;
            ObjVec3 v = { 0 };
            SkipSpaces(&p);
            if (!ParseFloat(&p, &v.x)) { SkipToNextLine(&p); continue; }
            SkipSpaces(&p);
            if (!ParseFloat(&p, &v.y)) { SkipToNextLine(&p); continue; }
            SkipSpaces(&p);
            if (!ParseFloat(&p, &v.z)) { SkipToNextLine(&p); continue; }
            Vec3ArrayPush(&positions, v);
            SkipToNextLine(&p);
            continue;
        }

        if (p[0] == 'v' && p[1] == 't' && isspace((unsigned char)p[2]))
        {
            p += 2;
            ObjVec2 vt = { 0 };
            SkipSpaces(&p);
            if (!ParseFloat(&p, &vt.x)) { SkipToNextLine(&p); continue; }
            SkipSpaces(&p);
            if (!ParseFloat(&p, &vt.y)) { SkipToNextLine(&p); continue; }
            Vec2ArrayPush(&texcoords, vt);
            SkipToNextLine(&p);
            continue;
        }

        if (p[0] == 'v' && p[1] == 'n' && isspace((unsigned char)p[2]))
        {
            p += 2;
            ObjVec3 vn = { 0 };
            SkipSpaces(&p);
            if (!ParseFloat(&p, &vn.x)) { SkipToNextLine(&p); continue; }
            SkipSpaces(&p);
            if (!ParseFloat(&p, &vn.y)) { SkipToNextLine(&p); continue; }
            SkipSpaces(&p);
            if (!ParseFloat(&p, &vn.z)) { SkipToNextLine(&p); continue; }
            Vec3ArrayPush(&normals, vn);
            SkipToNextLine(&p);
            continue;
        }

        if (p[0] == 'f' && isspace((unsigned char)p[1]))
        {
            p++;

            ObjFaceVert face[64];
            int faceCount = 0;

            while (*p && *p != '\r' && *p != '\n')
            {
                SkipSpaces(&p);
                if (!*p || *p == '\r' || *p == '\n') break;
                if (faceCount >= 64) break;

                if (!ParseFaceVertex(&p, &face[faceCount])) break;
                faceCount++;
                SkipSpaces(&p);
            }

            if (faceCount >= 3)
            {
                for (int i = 1; i < faceCount - 1; i++)
                {
                    EmitTriangle(
                        &face[0], &face[i], &face[i + 1],
                        &positions, &texcoords, &normals,
                        &outVerts, &outTex, &outNorm
                    );
                }
            }

            SkipToNextLine(&p);
            continue;
        }

        // ignore: o, g, s, usemtl, mtllib, etc.
        SkipToNextLine(&p);
    }

    free(text);
    free(positions.data);
    free(texcoords.data);
    free(normals.data);

    Mesh mesh = { 0 };

    if (outVerts.count == 0) {
        free(outVerts.data);
        free(outTex.data);
        free(outNorm.data);
        return mesh;
    }

    mesh.vertexCount = outVerts.count / 3;
    mesh.triangleCount = mesh.vertexCount / 3;

    mesh.vertices = outVerts.data;
    mesh.texcoords = outTex.data;
    mesh.normals = outNorm.data;

    // no index buffer; fully expanded triangles
    mesh.indices = NULL;

    return mesh;
}
#endif // OBJMEMPARSE_H