#version 120

uniform float uTime;
uniform float uVariant;

varying float vR;
varying float vY;
varying float vAng;

void main() {
    // base fades
    float radial = 1.0 - smoothstep(0.35, 0.80, vR);   // soft edge
    float top    = 1.0 - smoothstep(0.55, 1.00, vY);   // fade near top

    // tongues (angular stripes that move over time)
    float tongues = 0.5 + 0.5*sin(vAng*5.0 + uTime*6.0 + uVariant*11.0);
    float lick    = 0.4 + 0.6* smoothstep(0.15, 0.95, vY)
                          * (0.5 + 0.5*sin(vY*14.0 + uTime*10.0 + uVariant*7.0));

    // overall alpha
    float a = 0.95 * radial * (0.6 + 0.4*top) * (0.55 + 0.45*tongues*lick);

    // flicker (layer-dependent so passes don't sync)
    float flicker = 0.75 + 0.25 * sin(uTime*15.0 + uVariant*13.0);
    a *= flicker;

    // fire ramp with small hue shift per layer
    vec3 hot  = vec3(1.0, 0.96, 0.82);
    vec3 mid  = vec3(1.0, 0.58, 0.12);
    vec3 cool = vec3(0.92, 0.10, 0.03);

    float rampShift = 0.10 * sin(uVariant*3.0 + uTime*1.5);
    float rampX = clamp(0.20*(1.0 - radial) + 0.80*(1.0 - vY) + rampShift, 0.0, 1.0);
    vec3 c1 = mix(hot, mid, smoothstep(0.15, 0.55, rampX));
    vec3 col = mix(c1,  cool, smoothstep(0.55, 1.00, rampX));

    // slightly boost core brightness
    col *= (0.7 + 0.3*(1.0 - smoothstep(0.18, 0.70, vR)));

    gl_FragColor = vec4(col, a);
}
