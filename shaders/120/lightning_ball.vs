#version 120
attribute vec3 vertexPosition;
attribute vec3 vertexNormal;

uniform mat4 mvp;     // raylib updates this per-draw
uniform mat4 model;   // raylib updates this per-draw

uniform float uTime;
uniform float uDispAmp;     // 0.. ~0.2
uniform float uNoiseScale;  // ~3.0

varying vec3 vN;   // world normal
varying vec3 vW;   // world position

float hash3(vec3 p) { return fract(sin(dot(p, vec3(127.1,311.7,74.7))) * 43758.5453123); }
float pnoise(vec3 x) {
    vec3 i = floor(x), f = fract(x);
    f = f*f*(3.0-2.0*f);
    float n =
      mix(mix(mix(hash3(i+vec3(0,0,0)), hash3(i+vec3(1,0,0)), f.x),
              mix(hash3(i+vec3(0,1,0)), hash3(i+vec3(1,1,0)), f.x), f.y),
          mix(mix(hash3(i+vec3(0,0,1)), hash3(i+vec3(1,0,1)), f.x),
              mix(hash3(i+vec3(0,1,1)), hash3(i+vec3(1,1,1)), f.x), f.y), f.z);
    return n;
}

void main() {
    // Work in object space for displacement used in gl_Position
    vec3 lp = vertexPosition;
    vec3 ln = normalize(vertexNormal);

    // Sample noise in world space so it crawls nicely on the surface
    vec3 wposForNoise = (model * vec4(lp, 1.0)).xyz;
    float n = pnoise(wposForNoise * uNoiseScale + vec3(uTime*2.0, uTime*1.3, uTime*1.7));
    float disp = (n - 0.5) * 2.0 * uDispAmp;
    lp += ln * disp;

    // Build varyings in world space
    vec4 wp = model * vec4(lp, 1.0);
    vW = wp.xyz;
    vN = normalize((model * vec4(ln, 0.0)).xyz);

    // Project ONCE: mvp already includes model
    gl_Position = mvp * vec4(lp, 1.0);
}
