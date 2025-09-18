#version 120
uniform float uTime;
varying float vR;
varying float vY;

void main() {
    float flicker = 0.75 + 0.25 * sin(uTime * 15.0);
    float radial = 1.0 - smoothstep(0.45, 0.75, vR);
    float top    = 1.0 - smoothstep(0.60, 1.00, vY);
    float a = clamp(0.9 * radial * (0.6 + 0.4 * top) * (0.7 + 0.3 * sin(uTime * 6.7)), 0.0, 1.0);

    vec3 hot  = vec3(1.0, 0.95, 0.80);
    vec3 mid  = vec3(1.0, 0.55, 0.10);
    vec3 cool = vec3(0.90, 0.08, 0.02);
    float rampX = clamp(0.2*(1.0 - radial) + 0.8*(1.0 - vY), 0.0, 1.0);
    vec3 c1 = mix(hot, mid, smoothstep(0.15, 0.55, rampX));
    vec3 col = mix(c1,  cool, smoothstep(0.55, 1.00, rampX));
    //vec3 eyeColor = vec3(0.9, 1.0, 0.3);
    gl_FragColor = vec4(col, a) * flicker;
    //gl_FragColor = vec4(eyeColor, 1);
}
