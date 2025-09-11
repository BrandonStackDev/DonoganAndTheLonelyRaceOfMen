#version 100

// Input vertex attributes
attribute vec3 vertexPosition;
attribute vec2 vertexTexCoord;
attribute vec3 vertexNormal;
attribute vec4 vertexColor;

attribute mat4 instanceTransform;

// Input uniform values
uniform mat4 mvp;
uniform mat4 matNormal;

uniform float uTime;        // seconds

// Output vertex attributes (to fragment shader)
varying vec3 fragPosition;
varying vec2 fragTexCoord;
varying vec4 fragColor;
varying vec3 fragNormal;


// NOTE: Add your custom variables here

void main()
{
// ----- hardcoded wind params (edit these 2 if desired) -----
    const vec2  WIND_DIR = normalize(vec2(0.6, 0.8)); // constant XZ direction
    const float AMP      = 0.13;                      // sway amplitude at tip (meters)
    const float FREQ     = 0.20;                      // Hz (cycles per second)
    // ----------------------------------------------------------

    // 0 at base, 1 at tip (most grass uses V for height)
    float tip  = clamp(vertexTexCoord.y, 0.0, 1.0);
    float bend = tip * tip;                           // more motion at the tip, less at base

    // simple sine over time
    float swayPhase = 6.2831853 * FREQ * uTime;       // 2π * FREQ * t
    float sway      = sin(swayPhase);

    // lateral offset in object space along WIND_DIR
    vec3 offset = vec3(WIND_DIR.x, 0.0, WIND_DIR.y) * (AMP * bend * sway);
    vec3 pos    = vertexPosition + offset;

    //stuff
    // --- bend top down if above threshold (branchless) ---
    const float THRESH_Y  = 2.21;   // start bending above this height (object space)
    const float STR_Y     = 0.3;   // how strongly to bend Y (bigger = more downward)
    const float STR_XZ    = 0.2;  // how strongly to pull X/Z back toward the stem
    
    float over = max(0.0, vertexPosition.y - THRESH_Y);   // amount above threshold
    
    // Push Y back down, proportional to how far above threshold we are
    pos.y -= over * STR_Y;
    
    // Pull X/Z slightly toward the stem (reduces lateral sway near the very top)
    float tXZ = clamp(over * STR_XZ, 0.0, 1.0);
    pos.x = mix(pos.x, vertexPosition.x, tXZ);
    pos.z = mix(pos.z, vertexPosition.z, tXZ);


    // instance transform
    vec4 worldPos = vec4(pos, 1.0);

    // Compute MVP for current instance
    mat4 mvpi = mvp*instanceTransform;

    // Send vertex attributes to fragment shader
    fragPosition = vec3(mvpi*vec4(vertexPosition, 1.0));
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    fragNormal = normalize(vec3(matNormal*vec4(vertexNormal, 1.0)));

    // Calculate final vertex position
    //gl_Position = mvpi*vec4(vertexPosition, 1.0); //old

    gl_Position  = mvpi * worldPos;
}
