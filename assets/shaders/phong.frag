#version 330 core

in vec3 vFragPos;
in vec3 vNormal;
in vec2 vUV;

out vec4 FragColor;

uniform vec3  uColor;
uniform float uShininess;
uniform sampler2D uDiffuseTex;

uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uViewPos;

void main() {
    vec3 N = normalize(vNormal);
    vec3 L = normalize(uLightPos - vFragPos);   
    vec3 V = normalize(uViewPos - vFragPos);   
    vec3 R = reflect(-L, N);                    

    vec3 texColor = texture(uDiffuseTex, vUV).rgb;

    vec3 ambient = 0.1 * uLightColor;

    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * uLightColor;

    float spec = pow(max(dot(R, V), 0.0), uShininess);
    vec3 specular = spec * uLightColor * 0.5;

    vec3 result = (ambient + diffuse) * (texColor * uColor) + specular;
    FragColor = vec4(result, 1.0);
}