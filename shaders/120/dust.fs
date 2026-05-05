#version 120

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D texture0;
uniform float uTime;
uniform float uDuration;
uniform vec4 uColor;

float puff(vec2 uv, vec2 center, float radius, float edge)
{
    float d = length(uv - center);
    return 1.0 - smoothstep(radius - edge, radius, d);
}

void main()
{
    vec2 uv01 = fragTexCoord;
    vec2 uv = uv01 * 2.0 - 1.0;

    float progress = clamp(uTime / max(uDuration, 0.001), 0.0, 1.0);

    vec4 tex = texture2D(texture0, uv01);

    float grow = mix(0.75, 1.35, progress);
    uv /= grow;

    float cloud = 0.0;
    cloud += puff(uv, vec2( 0.00,  0.00), 0.72, 0.40);
    cloud += puff(uv, vec2(-0.38,  0.10), 0.48, 0.28);
    cloud += puff(uv, vec2( 0.36,  0.04), 0.46, 0.28);
    cloud += puff(uv, vec2(-0.08, -0.34), 0.44, 0.30);
    cloud += puff(uv, vec2( 0.18,  0.34), 0.38, 0.24);

    cloud = clamp(cloud, 0.0, 1.0);

    float fadeIn = smoothstep(0.0, 0.12, progress);
    float fadeOut = 1.0 - smoothstep(0.45, 1.0, progress);
    float alpha = tex.a * cloud * fadeIn * fadeOut * uColor.a * fragColor.a;

    if (alpha < 0.01) discard;

    vec3 rgb = tex.rgb * uColor.rgb * fragColor.rgb;

    gl_FragColor = vec4(rgb, alpha);
}