#version 120

attribute vec3 vertexPosition;
attribute vec3 vertexNormal;

uniform mat4 mvp;
uniform mat4 model;   // kept for same locs (not used here on purpose)
uniform float uTime;

varying float vR;
varying float vY;

void main() {
    vec3 p = vertexPosition;

    // Shape into a taller, slightly thinner “flame”
    p.x *= 0.72;
    p.z *= 0.72;
    p.y *= 1.85;

    // Gentle wobble so it “dances”
    float s = sin(uTime*4.0 + vertexPosition.y*3.0) * 0.08;
    p.x += s;
    p.z += s * 0.6;

    // Varyings for the fragment shader (soft edge / top fade)
    vR = length(p.xz);
    vY = clamp((p.y + 0.6) / 2.4, 0.0, 1.0);

    // IMPORTANT:
    // Use mvp only — DrawModel(fireModel, P, ...) already bakes the model transform into mvp.
    gl_Position = mvp * vec4(p, 1.0);

    // If (and only if) you draw at the origin and rely on the 'model' uniform to position,
    // swap the line above for:
    // gl_Position = mvp * model * vec4(p, 1.0);
}
