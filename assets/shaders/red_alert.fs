#version 100
precision mediump float;

uniform sampler2D texture0;
uniform float time;

varying vec2 fragTexCoord;

void main() {
    vec4 baseColor = texture2D(texture0, fragTexCoord);
    float pulse = pow(sin(time * 5.0) * 0.5 + 0.5, 2.0);
    vec3 alertColor = mix(baseColor.rgb, vec3(1.0, 0.1, 0.1), pulse * 0.4);
    
    gl_FragColor = vec4(alertColor, baseColor.a);
}