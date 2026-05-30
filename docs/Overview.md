# DNA Visualizer — Documentation

Technical documentation for the DNA Visualizer coursework project. For a quick start, see the [main README](../README.md).

## Contents

| Document | Description |
|----------|-------------|
| [Architecture](Architecture.md) | High-level design, module boundaries, and main loop |
| [Rendering pipeline](Rendering%20Pipeline.md) | FBO, Phong shading, post-processing (Sobel, FXAA, vignette) |
| [DNA model](DNA%20Model.md) | Sequences, helix geometry, Watson–Crick pairing, mutations |
| [Module reference](Module%20Reference.md) | Classes, responsibilities, and key APIs |
| [Build & development](Build%20and%20Development.md) | Build steps, dependencies, conventions, extending the project |

## Quick reference

```
Application loop
  poll events → update (input, animation, mutations) → render scene to FBO
  → post-process → ImGui overlay → swap buffers
```

**Entry point:** `src/core/main.cpp` → `dna::Application::run()`

**Asset paths:** Shaders and textures are loaded relative to the working directory (`assets/`). Always launch the executable from the repository root.
