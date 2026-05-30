#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uScreenTex;
uniform int uSobelEnabled;
uniform int uVignetteEnabled;
uniform float uVignetteStrength;
uniform vec2 uTexelSize;

float luminance(vec3 color) {
    return dot(color, vec3(0.299, 0.587, 0.114));
}

vec3 applySobel(vec2 uv) {
    float tl = luminance(texture(uScreenTex, uv + vec2(-uTexelSize.x, uTexelSize.y)).rgb);
    float t = luminance(texture(uScreenTex, uv + vec2(0.0, uTexelSize.y)).rgb);
    float tr = luminance(texture(uScreenTex, uv + vec2( uTexelSize.x, uTexelSize.y)).rgb);
    float l = luminance(texture(uScreenTex, uv + vec2(-uTexelSize.x, 0.0)).rgb);
    float r = luminance(texture(uScreenTex, uv + vec2( uTexelSize.x, 0.0)).rgb);
    float bl = luminance(texture(uScreenTex, uv + vec2(-uTexelSize.x, -uTexelSize.y)).rgb);
    float b = luminance(texture(uScreenTex, uv + vec2(0.0, -uTexelSize.y)).rgb);
    float br = luminance(texture(uScreenTex, uv + vec2( uTexelSize.x, -uTexelSize.y)).rgb);

    float gx = -tl - 2.0 * l - bl + tr + 2.0 * r + br;
    float gy = -tl - 2.0 * t - tr + bl + 2.0 * b + br;
    float edge = sqrt(gx * gx + gy * gy);
    return vec3(edge);
}

vec3 applyVignette(vec3 color, vec2 uv, float strength) {
    vec2 p = uv - 0.5;
    float factor = 1.0 - strength * dot(p, p) * 3.2;
    return color * clamp(factor, 0.0, 1.0);
}

void main() {
    vec3 color = (uSobelEnabled != 0) ? applySobel(vUV) : texture(uScreenTex, vUV).rgb;

    if (uVignetteEnabled != 0)
        color = applyVignette(color, vUV, uVignetteStrength);

    FragColor = vec4(color, 1.0);
}
