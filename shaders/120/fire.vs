#version 120

attribute vec3 vertexPosition;
attribute vec3 vertexNormal;

uniform mat4 mvp;
uniform mat4 model;   // kept for same loc name (unused here since you draw at P)
uniform float uTime;
uniform float uVariant; // per-layer phase/variation

varying float vR;     // radial distance after shaping
varying float vY;     // normalized height 0..1
varying float vAng;   // angular position for tongues

void main() {
    vec3 p = vertexPosition;

    // base shape: narrower XZ, taller Y
    p.x *= 0.68;
    p.z *= 0.68;
    p.y *= 2.05;

    // cylindrical coords (before swirl)
    float r = length(p.xz);
    float ang = atan(p.z, p.x);

    // swirl and lateral wobble (layer-dependent)
    float phase = uVariant * 3.1;
    float swirl = 0.55 + 0.35*sin(uTime*1.7 + phase);
    ang += swirl * 0.6 * sin(p.y*2.2 + uTime*2.4 + phase);

    // reconstruct xz from (r, ang)
    p.x = r * cos(ang);
    p.z = r * sin(ang);

    // “dancing” offset
    float wob = 0.07 * (sin(uTime*5.0 + p.y*3.2 + phase)
                      + 0.5*sin(uTime*9.0 + p.y*6.1 + phase*1.2));
    p.x += wob;
    p.z += wob * 0.6;

    // tiny normal shimmer
    float n = 0.5 + 0.5*sin(uTime*10.0 + vertexPosition.x*4.5 + phase);
    p += vertexNormal * (0.04 * n);

    // varyings
    vR   = length(p.xz);
    vY   = clamp((p.y + 0.6) / 2.6, 0.0, 1.0);
    vAng = ang;

    // IMPORTANT: use mvp only (you draw at world P with DrawModel/DrawModelEx)
    gl_Position = mvp * vec4(p, 1.0);
}
