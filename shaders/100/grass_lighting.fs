#version 100

precision mediump float;

// Input vertex attributes (from vertex shader)
varying vec3 fragPosition;
varying vec2 fragTexCoord;
varying vec4 fragColor;
varying vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// NOTE: Add your custom variables here

#define     MAX_LIGHTS              4
#define     LIGHT_DIRECTIONAL       0
#define     LIGHT_POINT             1

struct Light {
    int enabled;
    int type;
    vec3 position;
    vec3 target;
    vec4 color;
};

// Input lighting values
uniform Light lights[MAX_LIGHTS];
uniform vec4 ambient;
uniform vec3 viewPos;

void main() {
    // base texture * material color
    vec4 base = texture2D(texture0, fragTexCoord) * colDiffuse;

    // --- tiny, hardcoded styling knobs (edit these if you want) ---
    const float DARKEN = 0.82;                     // 1.0 = unchanged; <1.0 darkens
    const vec3  TINT_LOW  = vec3(0.12, 0.30, 0.14); // deeper green (near base)
    const vec3  TINT_HIGH = vec3(0.18, 0.42, 0.18); // lighter green (near tip)
    const float BLEND = 0.22;                      // 0..1 mix toward tint
    // --------------------------------------------------------------

    // Height-based tint mix using V (keeps it simple and nicely blended)
    float h = clamp(fragTexCoord.y, 0.0, 1.0);
    vec3 tint = mix(TINT_LOW, TINT_HIGH, h);

    // darken + blend
    vec3 rgb = base.rgb * DARKEN;
    rgb = mix(rgb, tint, BLEND);

    gl_FragColor = vec4(rgb, base.a);
}

