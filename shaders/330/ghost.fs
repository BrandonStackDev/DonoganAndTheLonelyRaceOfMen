#version 330

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vWorldPos;

uniform sampler2D texture0;  // Raylib diffuse/albedo
uniform vec4 colDiffuse;     // Raylib material tint
uniform float u_time;        // Only custom uniform

out vec4 finalColor;

float luma(vec3 c) { return dot(c, vec3(0.2126, 0.7152, 0.0722)); }

// Simple UV swirl around the center
vec2 swirl(vec2 uv, float amount)
{
    vec2 c = uv - 0.5;
    float r = length(c);
    float a = atan(c.y, c.x);
    a += amount * r; // stronger twist further from center
    return vec2(cos(a), sin(a)) * r + 0.5;
}

void main()
{
    // Base texture
    vec4 base = texture(texture0, vTexCoord) * colDiffuse;

    // --- Eye detection: bright yellowish pixels in the base map ---
    // (High R & G, relatively lower B, and overall bright)
    float isYellowish = step(0.6, base.r) * step(0.6, base.g) * step(base.b, 0.5);
    float isBright    = step(0.6, luma(base.rgb));
    float eyeMask     = clamp(isYellowish * isBright, 0.0, 1.0);

    // --- Global transparency “breathing” (uniform across model) ---
    float alphaPulse = 0.5 + 0.5 * sin(u_time * 0.7);
    float ghostAlpha = mix(0.25, 0.85, alphaPulse);

    // --- Eerie blue swirl over the cloth ---
    vec2 suv = swirl(vTexCoord, 6.0 * sin(u_time * 0.25));
    float swirlBand = 0.5 + 0.5 * sin((suv.x + suv.y) * 18.0 + u_time * 1.5);

    vec3 blueLo = vec3(0.20, 0.40, 0.80);
    vec3 blueHi = vec3(0.40, 0.80, 1.00);
    vec3 blueTint = mix(blueLo, blueHi, swirlBand);

    // Mix base cloth toward blue tint (constant strength)
    float tintAmt = 0.55;
    vec3 clothColor = mix(base.rgb, blueTint, tintAmt);

    // Subtle fake rim lift without camera vector (soft edges)
    float rim = pow(clamp(1.0 - abs(vNormal.y), 0.0, 1.0), 2.0);
    clothColor += rim * 0.05;

    // --- Eyes: neon yellow-green, with a little animated “glow” ---
    vec3 eyeColor = vec3(0.9, 1.0, 0.3);
    float eyeGlow = 0.6 + 0.4 * sin(u_time * 3.0);

    vec3 color = mix(clothColor, eyeColor * (1.0 + eyeGlow), eyeMask);
    float alpha = mix(ghostAlpha, 0.95, eyeMask); // eyes stay mostly solid

    //finalColor = vec4(color, alpha);
    finalColor = vec4(color, 1.0);
}
