#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uScreenTex;
uniform int uSobelEnabled;
uniform vec2 uTexelSize;

float luminance(vec3 color) {
    return dot(color, vec3(0.299, 0.587, 0.114));
}

void main() {
    vec3 scene = texture(uScreenTex, vUV).rgb;

    if (uSobelEnabled == 0) {
        FragColor = vec4(scene, 1.0);
        return;
    }

    float tl = luminance(texture(uScreenTex, vUV + vec2(-uTexelSize.x,  uTexelSize.y)).rgb);
    float t  = luminance(texture(uScreenTex, vUV + vec2(0.0,            uTexelSize.y)).rgb);
    float tr = luminance(texture(uScreenTex, vUV + vec2( uTexelSize.x,  uTexelSize.y)).rgb);
    float l  = luminance(texture(uScreenTex, vUV + vec2(-uTexelSize.x,  0.0)).rgb);
    float r  = luminance(texture(uScreenTex, vUV + vec2( uTexelSize.x,  0.0)).rgb);
    float bl = luminance(texture(uScreenTex, vUV + vec2(-uTexelSize.x, -uTexelSize.y)).rgb);
    float b  = luminance(texture(uScreenTex, vUV + vec2(0.0,           -uTexelSize.y)).rgb);
    float br = luminance(texture(uScreenTex, vUV + vec2( uTexelSize.x, -uTexelSize.y)).rgb);

    float gx = -tl - 2.0 * l - bl + tr + 2.0 * r + br;
    float gy = -tl - 2.0 * t - tr + bl + 2.0 * b + br;
    float edge = sqrt(gx * gx + gy * gy);

    FragColor = vec4(vec3(edge), 1.0);
}
