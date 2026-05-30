#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uScreenTex;
uniform int uSobelEnabled;
uniform int uFxaaEnabled;
uniform int uVignetteEnabled;
uniform float uVignetteStrength;
uniform vec2 uTexelSize;

const float FXAA_REDUCE_MIN = 1.0 / 128.0;
const float FXAA_REDUCE_MUL = 1.0 / 8.0;
const float FXAA_SPAN_MAX = 8.0;

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

// FXAA 3.11 — edge-directed blur on the scene color buffer
vec3 applyFxaa(vec2 uv) {
    vec3 rgbNW = texture(uScreenTex, uv + vec2(-uTexelSize.x, uTexelSize.y)).rgb;
    vec3 rgbNE = texture(uScreenTex, uv + vec2( uTexelSize.x, uTexelSize.y)).rgb;
    vec3 rgbSW = texture(uScreenTex, uv + vec2(-uTexelSize.x, -uTexelSize.y)).rgb;
    vec3 rgbSE = texture(uScreenTex, uv + vec2( uTexelSize.x, -uTexelSize.y)).rgb;
    vec3 rgbM = texture(uScreenTex, uv).rgb;

    float lumaNW = luminance(rgbNW);
    float lumaNE = luminance(rgbNE);
    float lumaSW = luminance(rgbSW);
    float lumaSE = luminance(rgbSE);
    float lumaM = luminance(rgbM);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    float lumaRange = lumaMax - lumaMin;

    if (lumaRange < max(0.0312, lumaMax * 0.125))
        return rgbM;

    float lumaNS = lumaNW + lumaNE + lumaSW + lumaSE;
    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max(lumaNS * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = clamp(dir * rcpDirMin, vec2(-FXAA_SPAN_MAX), vec2(FXAA_SPAN_MAX)) * uTexelSize;

    vec3 rgbA = 0.5 * (
        texture(uScreenTex, uv + dir * (1.0 / 3.0 - 0.5)).rgb +
        texture(uScreenTex, uv + dir * (2.0 / 3.0 - 0.5)).rgb
    );
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        texture(uScreenTex, uv + dir * -0.5).rgb +
        texture(uScreenTex, uv + dir *  0.5).rgb
    );

    float lumaB = luminance(rgbB);
    if (lumaB < lumaMin || lumaB > lumaMax)
        return rgbA;
    return rgbB;
}

vec3 applyVignette(vec3 color, vec2 uv, float strength) {
    vec2 p = uv - 0.5;
    float factor = 1.0 - strength * dot(p, p) * 3.2;
    return color * clamp(factor, 0.0, 1.0);
}

void main() {
    vec3 color;

    if (uSobelEnabled != 0) {
        color = applySobel(vUV);
    } else if (uFxaaEnabled != 0) {
        color = applyFxaa(vUV);
    } else {
        color = texture(uScreenTex, vUV).rgb;
    }

    if (uVignetteEnabled != 0)
        color = applyVignette(color, vUV, uVignetteStrength);

    FragColor = vec4(color, 1.0);
}
