#version 100
precision mediump float;

uniform sampler2D texture0;
uniform float time;

varying vec2 fragTexCoord;

void main() {
    vec2 uv = fragTexCoord;
    vec4 baseColor = texture2D(texture0, uv);

    float scanline = sin(uv.y * 800.0 + time * 10.0) * 0.04;
    float noise = (sin(uv.x * 1000.0 + time) * cos(uv.y * 1000.0 + time)) * 0.02;
    float dist = distance(uv, vec2(0.5, 0.5));
    float vignette = clamp(1.2 - dist * 1.2, 0.0, 1.0);

    vec3 color = baseColor.rgb - scanline - noise;
    color *= vignette;

    gl_FragColor = vec4(color, baseColor.a);
}