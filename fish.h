#ifndef FISH_H
#define FISH_H

// Includes
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h> 
#include <stdbool.h>

typedef struct {
    Vector3 pos, vel;
    float yawDeg, scale;
} Fish;

typedef struct {
    Vector3 center;
    int schoolCount;
    float schoolRadius;
    Fish* fish;
    Vector3 fishTarget;
} School;

void UpdateSchool(Fish* f, int n, Vector3 target, float dt) {
    const float maxSpeed = 3.5f, maxTurn = 90.0f; // deg/s
    const float neighbor = 2.0f, sep = 0.8f;

    for (int i = 0; i < n; i++) {
        Vector3 v = f[i].vel;

        // Cohesion/Alignment (sample a few neighbors)
        Vector3 avgPos = { 0 }, avgVel = { 0 }; int c = 0;
        for (int k = 0; k < 6; k++) { // sample 6 random others
            int j = (i + 1 + (i * 13 + k * 7) % n) % n;
            Vector3 d = Vector3Subtract(f[j].pos, f[i].pos);
            float d2 = Vector3LengthSqr(d);
            if (d2 < neighbor * neighbor) { avgPos = Vector3Add(avgPos, f[j].pos); avgVel = Vector3Add(avgVel, f[j].vel); c++; }
            if (d2 < sep * sep) { v = Vector3Subtract(v, Vector3Scale(Vector3Normalize(d), 1.2f)); }
        }
        if (c > 0) {
            avgPos = Vector3Scale(avgPos, 1.0f / c);
            avgVel = Vector3Scale(avgVel, 1.0f / c);
            v = Vector3Add(v, Vector3Scale(Vector3Normalize(Vector3Subtract(avgPos, f[i].pos)), 0.6f)); // cohesion
            v = Vector3Add(v, Vector3Scale(Vector3Normalize(avgVel), 0.4f));                              // alignment
        }

        // Goal + jitter
        v = Vector3Add(v, Vector3Scale(Vector3Normalize(Vector3Subtract(target, f[i].pos)), 0.8f));
        v = Vector3Add(v, (Vector3) { (GetRandomValue(-5, 5) * 0.01f), (GetRandomValue(-2, 2) * 0.01f), (GetRandomValue(-5, 5) * 0.01f) });

        // Clamp speed
        float sp = Vector3Length(v);
        if (sp > maxSpeed) v = Vector3Scale(Vector3Normalize(v), maxSpeed);

        // Update yaw smoothly (turn-rate limited)
        float yawNow = f[i].yawDeg;
        float yawWant = RAD2DEG * atan2f(v.x, v.z);        // +Z forward
        float dy = fmodf(yawWant - yawNow + 540.0f, 360.0f) - 180.0f;
        float step = Clamp(dy, -maxTurn * dt, maxTurn * dt);
        f[i].yawDeg = yawNow + step;

        // Move
        f[i].vel = (Vector3){ sinf(DEG2RAD * f[i].yawDeg) * sp, v.y, cosf(DEG2RAD * f[i].yawDeg) * sp };
        f[i].pos = Vector3Add(f[i].pos, Vector3Scale(f[i].vel, dt));
    }
}


#endif // FISH_H
