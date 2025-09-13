#version 330

// Raylib attribute names
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;

// Raylib-provided matrices
uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;

// Our only custom uniform
uniform float u_time;

// Varyings
out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vWorldPos;

void main()
{
    // World position before displacement (for stable waves)
    vec4 worldPos = matModel * vec4(vertexPosition, 1.0);

    // --- Cloth ripple (model-space displacement along the normal) ---
    // Tunables baked in:
    float freq1 = 1.8;
    float freq2 = 1.1;
    float speed = 1.2;
    float amp   = 0.06;  // model-space units; scales with your model

    float w1 = sin(worldPos.x * freq1 + u_time * speed);
    float w2 = sin(worldPos.z * freq2 + u_time * (speed * 1.35));
    float ripple = 0.5 * (w1 + w2);

    vec3 displaced = vertexPosition + vertexNormal * (ripple * amp);

    vec4 worldDisplaced = matModel * vec4(displaced, 1.0);

    // Pass-through
    vTexCoord = vertexTexCoord;
    vWorldPos = worldDisplaced.xyz;
    vNormal   = normalize((matNormal * vec4(vertexNormal, 0.0)).xyz);

    gl_Position = mvp * worldDisplaced;
}
