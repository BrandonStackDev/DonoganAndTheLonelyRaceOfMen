#version 120
uniform vec3  uColor;       // e.g. 0.45,0.75,1.0
uniform float uIntensity;   // ~1.5..2.0
uniform float uTime;
uniform float uNoiseScale;  // ~3.0
uniform vec3  uViewPos;     // camera position

varying vec3 vN;
varying vec3 vW;

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
    vec3 n = normalize(vN);
    vec3 V = normalize(uViewPos - vW);

    // animated thin bands
    float a1 = sin(dot(n, normalize(vec3( 0.9, 0.2, 0.1))) * 30.0 + uTime*10.0);
    float a2 = sin(dot(n, normalize(vec3(-0.3, 0.8, 0.5))) * 37.0 - uTime*12.0);
    float bands = max(0.0, 1.0 - abs(a1)) + max(0.0, 1.0 - abs(a2));

    // spark speckle
    float nval   = pnoise(vW * (uNoiseScale*1.2) + vec3(uTime*3.1, uTime*2.2, uTime*1.3));
    float sparks = smoothstep(0.75, 1.0, nval);

    // rim glow facing camera
    float rim = pow(max(1.0 - max(dot(n, V), 0.0), 0.0), 3.0);

    float emissive = clamp(0.35 + 1.7*bands + 1.2*sparks + 1.3*rim, 0.0, 4.0);

    // How "white-hot" are we?
    float hot = smoothstep(1.0, 2.5, emissive);   // 0..1 as it gets very bright

    // Push hottest zones toward blue
    vec3 blue = vec3(0.35, 0.70, 1.00);
    vec3 tint = mix(uColor, blue, hot);

    // Soft highlight compression (prevents pure white blowout)
    float eComp = emissive / (1.0 + emissive);    // simple Reinhard-like tone map

    // Base alpha, then drop it where it’s hottest (more transparent at white-hot)
    float alpha = clamp(eComp * 0.9, 0.12, 0.9);
    alpha *= (1.0 - 0.6 * hot);                   // up to 60% reduction in hot zones

    vec3 col = tint * eComp * uIntensity;

    gl_FragColor = vec4(col, alpha);
}
