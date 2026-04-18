#ifndef FRUSTUM_H
#define FRUSTUM_H

//structs
typedef struct Plane {
    Vector3 normal;
    float d;
} Plane;

typedef struct Frustum {
    Plane planes[6]; // left, right, top, bottom, near, far
} Frustum;

static Plane NormalizePlane(Plane p) {
    float len = Vector3Length(p.normal);
    return (Plane) {
        .normal = Vector3Scale(p.normal, 1.0f / len),
            .d = p.d / len
    };
}

Frustum ExtractFrustum(Matrix mat)
{
    Frustum f;

    // LEFT
    f.planes[0] = NormalizePlane((Plane) {
        .normal = (Vector3){ mat.m3 + mat.m0, mat.m7 + mat.m4, mat.m11 + mat.m8 },
            .d = mat.m15 + mat.m12
    });

    // RIGHT
    f.planes[1] = NormalizePlane((Plane) {
        .normal = (Vector3){ mat.m3 - mat.m0, mat.m7 - mat.m4, mat.m11 - mat.m8 },
            .d = mat.m15 - mat.m12
    });

    // BOTTOM
    f.planes[2] = NormalizePlane((Plane) {
        .normal = (Vector3){ mat.m3 + mat.m1, mat.m7 + mat.m5, mat.m11 + mat.m9 },
            .d = mat.m15 + mat.m13
    });

    // TOP
    f.planes[3] = NormalizePlane((Plane) {
        .normal = (Vector3){ mat.m3 - mat.m1, mat.m7 - mat.m5, mat.m11 - mat.m9 },
            .d = mat.m15 - mat.m13
    });

    // NEAR
    f.planes[4] = NormalizePlane((Plane) {
        .normal = (Vector3){ mat.m3 + mat.m2, mat.m7 + mat.m6, mat.m11 + mat.m10 },
            .d = mat.m15 + mat.m14
    });

    // FAR
    f.planes[5] = NormalizePlane((Plane) {
        .normal = (Vector3){ mat.m3 - mat.m2, mat.m7 - mat.m6, mat.m11 - mat.m10 },
            .d = mat.m15 - mat.m14
    });

    return f;
}

bool IsBoxInFrustum(BoundingBox box, Frustum frustum)
{
    for (int i = 0; i < 6; i++)
    {
        Plane plane = frustum.planes[i];

        // Find the corner of the AABB that is most *opposite* to the normal
        Vector3 positive = {
            (plane.normal.x >= 0) ? box.max.x : box.min.x,
            (plane.normal.y >= 0) ? box.max.y : box.min.y,
            (plane.normal.z >= 0) ? box.max.z : box.min.z
        };

        // If that corner is outside, the box is not visible
        float distance = Vector3DotProduct(plane.normal, positive) + plane.d;
        if (distance < 0) { return false; }
    }

    return true;
}

// Returns true if point p lies inside (or on) all 6 frustum planes.
// Assumes the same plane convention as your AABB test: n·x + d >= 0 is inside.
static inline bool IsPointInFrustum(Vector3 p, Frustum frustum)
{
    for (int i = 0; i < 6; ++i)
    {
        Plane plane = frustum.planes[i];
        float distance = Vector3DotProduct(plane.normal, p) + plane.d;
        if (distance < 0.0f) return false; // outside this plane -> outside frustum
    }
    return true;
}


#endif //FRUSTUM_H