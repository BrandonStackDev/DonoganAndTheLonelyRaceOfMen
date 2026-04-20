#version 120

varying vec3 fragWorldPos;
varying vec3 fragNormal;
varying float fragPulse;

void main()
{
    vec3 N = normalize(fragNormal);
    vec3 L = normalize(vec3(0.2, 1.0, 0.35));

    float ndl = max(dot(N, L), 0.0);
    float fres = pow(1.0 - max(dot(N, vec3(0.0, 0.0, 1.0)), 0.0), 2.0);

    vec3 darkCol = vec3(0.45, 0.03, 0.03);
    vec3 midCol  = vec3(0.78, 0.06, 0.06);
    vec3 hiCol   = vec3(1.00, 0.16, 0.16);

    vec3 col = mix(darkCol, midCol, ndl * 0.8 + 0.2);
    col = mix(col, hiCol, fres * 0.35 + fragPulse * 0.15);

    float alpha = 0.68 + fragPulse * 0.24;

    gl_FragColor = vec4(col, alpha);
}