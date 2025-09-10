#version 120
attribute vec3 vertexPosition;
attribute vec3 vertexNormal;

uniform mat4 mvp;
uniform mat4 model;
uniform float uTime;
uniform float uDispAmp;      // 0..1, surface jitter amount
uniform float uNoiseScale;   // e.g. 3.0

varying vec3 vN;   // world normal
varying vec3 vW;   // world pos

float hash(vec3 p){ return fract(sin(dot(p, vec3(127.1,311.7, 74.7))) * 43758.5453); }
float noise(vec3 x){
    vec3 i = floor(x), f = fract(x);
    f = f*f*(3.0-2.0*f);
    float n =
        mix(mix(mix(hash(i+vec3(0,0,0)), hash(i+vec3(1,0,0)), f.x),
                mix(hash(i+vec3(0,1,0)), hash(i+vec3(1,1,0)), f.x), f.y),
            mix(mix(hash(i+vec3(0,0,1)), hash(i+vec3(1,0,1)), f.x),
                mix(hash(i+vec3(0,1,1)), hash(i+vec3(1,1,1)), f.x), f.y), f.z);
    return n;
}

void main(){
    vec4 wp = model * vec4(vertexPosition, 1.0);
    vec3 wn = normalize((model * vec4(vertexNormal, 0.0)).xyz);

    float n = noise(wp.xyz * uNoiseScale + vec3(uTime*2.0, uTime*1.3, uTime*1.7));
    float disp = (n - 0.5) * 2.0 * uDispAmp; // -amp..+amp
    wp.xyz += wn * disp;

    vN = wn;
    vW = wp.xyz;
    gl_Position = mvp * wp;
}
