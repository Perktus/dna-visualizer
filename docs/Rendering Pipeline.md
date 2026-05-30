# Rendering pipeline

The renderer uses OpenGL 3.3 Core Profile with a two-pass pipeline: an off-screen Phong-lit scene pass, then a screen-space post-process pass.

## Pipeline diagram

```
┌──────────────────┐
│   Scene pass     │  bind FBO, depth test ON
│   phong.vert/frag│  draw strandA, strandB, bonds, highlight sphere
└────────┬─────────┘
         │ color + depth → FBO textures
         ▼
┌──────────────────┐
│  Post-process    │  bind default FB, depth test OFF
│ screen.vert      │  fullscreen quad samples uScreenTex
│ post.frag        │  Sobel OR FXAA OR raw, then optional vignette
└────────┬─────────┘
         ▼
┌──────────────────┐
│  ImGui overlay   │  drawn on default framebuffer
└──────────────────┘
```

## Scene pass

**Shaders:** `assets/shaders/phong.vert`, `assets/shaders/phong.frag`

### Vertex stage

Per-vertex attributes from `Mesh`:

| Location | Attribute | Type |
|----------|-----------|------|
| 0 | position | `vec3` |
| 1 | normal | `vec3` |
| 2 | uv | `vec2` |
| 3 | color | `vec3` (nucleotide tint) |

The vertex shader transforms positions with `uModel`, `uView`, and `uProjection`, and passes world-space fragment position, normal, UV, and vertex color to the fragment stage.

### Fragment stage (Phong)

Lighting model:

- **Ambient** — `0.1 × uLightColor`
- **Diffuse** — Lambert term `max(N·L, 0)`
- **Specular** — Blinn-Phong-style reflection with shininess `uShininess`

Final color:

```
result = (ambient + diffuse) × (texColor × vColor × uColor) + specular
```

| Uniform | Purpose |
|---------|---------|
| `uModel`, `uView`, `uProjection` | MVP transforms |
| `uLightPos`, `uLightColor` | Point light (world space) |
| `uViewPos` | Camera position for specular |
| `uShininess` | Specular exponent (UI slider 1–128) |
| `uDiffuseTex` | Bound to unit 0 (`sphere.jpg`) |
| `uColor` | Per-draw tint multiplier |

### Draw order

1. **strandA** and **strandB** — white `uColor`; vertex colors carry nucleotide hues
2. **bonds** — slightly desaturated gray `(0.85, 0.85, 0.9)`
3. **Highlight sphere** (after mutation) — pulsing yellow overlay at the mutated index, scaled 1.15× the nucleotide radius

The helix is centered vertically by translating the model matrix by `-m_helixCenterY` on the Y axis, then rotated around Y by `m_rotationAngle` when animation is enabled.

## Framebuffer object

`Framebuffer` owns:

- **Color attachment** — `GL_TEXTURE_2D`, RGBA8, linear filtering
- **Depth attachment** — renderbuffer object (`GL_DEPTH24_STENCIL8`)

On window resize, `Framebuffer::resize()` destroys and recreates attachments at the new dimensions. `Application::render()` calls resize every frame so the FBO always matches the drawable size.

A static fullscreen quad (VAO + VBO) is shared across instances for the post-process pass.

## Post-process pass

**Shaders:** `assets/shaders/screen.vert`, `assets/shaders/post.frag`

The fragment shader selects one primary effect:

| Priority | Effect | Behavior |
|----------|--------|----------|
| 1 | **Sobel** | 3×3 luminance kernel → edge magnitude as grayscale output |
| 2 | **FXAA** | Fast Approximate Anti-Aliasing (3.11-style) on scene color |
| 3 | **None** | Passthrough sample of `uScreenTex` |

Sobel and FXAA are mutually exclusive in the shader branch (`if / else if`). When Sobel is enabled, FXAA is skipped even if its checkbox is on — the UI shows a hint about this.

**Vignette** runs after the primary effect when enabled:

```
factor = 1.0 - strength × dot(p, p) × 3.2
color *= clamp(factor, 0, 1)
```

where `p = uv - 0.5`.

### Uniforms

| Uniform | Type | Notes |
|---------|------|-------|
| `uScreenTex` | sampler2D | FBO color attachment |
| `uTexelSize` | vec2 | `(1/width, 1/height)` |
| `uSobelEnabled` | int | 0 or 1 |
| `uFxaaEnabled` | int | 0 or 1 |
| `uVignetteEnabled` | int | 0 or 1 |
| `uVignetteStrength` | float | 0.0–1.0 from UI |

## Camera

`Camera` implements an **arcball orbit** around the origin:

- **Rotate** — left mouse drag adjusts yaw and pitch (pitch clamped to ±89°)
- **Zoom** — scroll wheel adjusts distance (clamped 1–50 units)

View matrix: `glm::lookAt(eye, origin, up)` where `eye` is computed from spherical coordinates.

Projection: perspective with 45° FOV, dynamic aspect ratio, near 0.1, far 100.

## OpenGL state summary

| Pass | Depth test | Clear color |
|------|------------|-------------|
| Scene (FBO) | Enabled | `(0.05, 0.05, 0.1, 1)` |
| Post (default FB) | Disabled | same |
| ImGui | Managed by backend | — |
