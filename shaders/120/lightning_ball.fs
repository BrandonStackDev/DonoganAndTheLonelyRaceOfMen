#version 120
uniform vec3  uColor;        // electric tint, e.g. (0.4,0.7,1.0)
uniform float uIntensity;    // overall brightness multiplier
uniform float uTime;
uniform float uNoiseScale;
uniform vec3  uViewPos;      // camera pos for rim glow

varying vec3 vN;
varying vec3 vW;

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
    vec3 n = normalize(vN);
    vec3 V = normalize(uViewPos - vW);

    // two moving “bands” around the sphere (thin bright arcs)
    float a1 = sin(dot(n, normalize(vec3( 0.9, 0.2, 0.1))) * 30.0 + uTime*10.0);
    float a2 = sin(dot(n, normalize(vec3(-0.3, 0.8, 0.5))) * 37.0 - uTime*12.0);
    float bands = max(0.0, 1.0 - abs(a1)) + max(0.0, 1.0 - abs(a2));

    // spark speckle
    float nval   = noise(vW * (uNoiseScale*1.2) + vec3(uTime*3.1));
    float sparks = smoothstep(0.75, 1.0, nval);

    // rim glow facing the camera
    float rim = pow(1.0 - max(dot(n, V), 0.0), 3.0);

    float emissive = clamp(0.35 + 1.7*bands + 1.2*sparks + 1.3*rim, 0.0, 4.0);
    vec3 col = uColor * emissive * uIntensity;

    gl_FragColor = vec4(col, clamp(emissive*0.7, 0.2, 1.0)); // some alpha for additive glow
}
