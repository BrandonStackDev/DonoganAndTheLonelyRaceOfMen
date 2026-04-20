#version 120

attribute vec3 vertexPosition;
attribute vec2 vertexTexCoord;
attribute vec3 vertexNormal;
attribute vec4 vertexColor;

uniform mat4 mvp;
uniform mat4 matModel;
uniform float uTime;
uniform float uVariant;

varying vec3 fragWorldPos;
varying vec3 fragNormal;
varying float fragPulse;

void main()
{
    vec3 pos = vertexPosition;
    vec3 n = normalize(vertexNormal);

    float t = uTime * (1.2 + uVariant * 0.23);

    float wave1 = sin((pos.x * 5.0) + t * 2.4 + uVariant * 3.1);
    float wave2 = cos((pos.z * 4.0) - t * 1.9 + uVariant * 2.2);
    float wave3 = sin((pos.y * 6.0) + t * 2.8 + uVariant * 1.7);

    float ripple = (wave1 + wave2 + wave3) * 0.06;

    // squishy upward bulge so it feels wet
    pos += n * ripple;
    pos.y += sin(t + pos.x * 3.0 + pos.z * 2.0) * 0.035;

    vec4 worldPos = matModel * vec4(pos, 1.0);
    fragWorldPos = worldPos.xyz;
    fragNormal = normalize((matModel * vec4(n, 0.0)).xyz);
    fragPulse = 0.5 + 0.5 * sin(t * 2.0 + uVariant * 5.0);

    gl_Position = mvp * vec4(pos, 1.0);
}