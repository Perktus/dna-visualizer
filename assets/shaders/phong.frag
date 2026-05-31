#version 330 core
// Model Phonga (per-fragment): ambient + światło kierunkowe + punktowe

in vec3 vFragPos;
in vec3 vNormal;
in vec2 vUV;
in vec3 vColor;

out vec4 FragColor;

uniform vec3  uColor;
uniform float uShininess;
uniform sampler2D uDiffuseTex;
uniform int   uUseTexture;

uniform vec3 uAmbientColor;
uniform float uAmbientStrength;

uniform vec3 uDirLightDir;
uniform vec3 uDirLightColor;

uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uViewPos;

vec3 calcDiffuseSpecular(vec3 N, vec3 L, vec3 lightColor, vec3 albedo) {
    vec3 V = normalize(uViewPos - vFragPos);
    vec3 R = reflect(-L, N);

    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * lightColor * albedo;
    float spec = pow(max(dot(R, V), 0.0), uShininess);
    vec3 specular = spec * lightColor * 0.5;

    return diffuse + specular;
}

void main() {
    vec3 N = normalize(vNormal);

    vec3 albedo = vColor * uColor;
    if (uUseTexture == 1)
        albedo *= texture(uDiffuseTex, vUV).rgb;

    vec3 result = uAmbientStrength * uAmbientColor * albedo;

    vec3 dirL = normalize(-uDirLightDir);
    result += calcDiffuseSpecular(N, dirL, uDirLightColor, albedo);

    vec3 pointL = normalize(uLightPos - vFragPos);
    result += calcDiffuseSpecular(N, pointL, uLightColor, albedo);

    FragColor = vec4(result, 1.0);
}
