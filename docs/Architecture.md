# Architecture

DNA Visualizer is a single-process desktop application written in C++17. All code lives in the `dna` namespace and is organized into four logical layers.

## Layer overview

```
┌─────────────────────────────────────────────────────────────┐
│                        Application                          │
│  (orchestrates update/render, owns all subsystems)          │
└──────────┬──────────────┬──────────────┬────────────────────┘
           │              │              │
     ┌─────▼─────┐  ┌─────▼─────┐  ┌─────▼─────┐  ┌──────────┐
     │   core    │  │    dna    │  │ renderer  │  │    ui    │
     │ Window    │  │ Sequence  │  │ Shader    │  │ UILayer  │
     │ Input     │  │ Helix     │  │ Mesh      │  │ UIState  │
     │ Timer     │  │ Mutation  │  │ Camera    │  └──────────┘
     └───────────┘  └───────────┘  │ FBO       │
                                   └───────────┘
```

| Layer | Path | Role |
|-------|------|------|
| **core** | `include/core/`, `src/core/` | GLFW window, input polling, frame timing |
| **dna** | `include/dna/`, `src/dna/` | Biological data model and procedural helix meshes |
| **renderer** | `include/renderer/`, `src/renderer/` | OpenGL resources: shaders, meshes, textures, camera, FBO |
| **ui** | `include/ui/`, `src/ui/` | ImGui panels and shared `UIState` |

## Application lifecycle

`Application` is the central coordinator. Construction initializes:

1. A random 32 bp `DNASequence` and corresponding `HelixMeshes`
2. Phong and post-process `Shader` programs
3. Diffuse `Texture` (`assets/textures/sphere.jpg`)
4. Off-screen `Framebuffer` sized to the window
5. `UILayer` bound to the GLFW window handle

The main loop in `Application::run()`:

```
while window open:
    timer.tick()
    poll GLFW events
    ui.beginFrame()
    update()          // camera, animation, mutations, UI triggers
    render()          // FBO scene pass → post-process → ImGui
    ui.endFrame()
    swap buffers
    flush input state
```

## Data flow

### Sequence → geometry

When the sequence changes (mutation or regenerate), `Application::rebuildHelix()` calls `HelixGeometry::build()`, which produces three meshes:

- **strandA** — coding strand with per-nucleotide vertex colors
- **strandB** — complementary strand, offset 180° in phase
- **bonds** — cylindrical bridges between base pairs

Meshes are uploaded to GPU once per rebuild; draw calls reuse the same VAOs until the next rebuild.

### UI → simulation

ImGui does not call DNA APIs directly. Panels mutate a shared `UIState` struct; `Application::update()` reads boolean triggers (`triggerMutation`, `triggerRegenerate`) and applies changes on the next frame. This keeps rendering logic decoupled from immediate-mode UI.

### Rendering → display

The 3D scene is never drawn directly to the default framebuffer. `render()` binds the FBO, draws the helix, unbinds, then runs a fullscreen post-process pass that samples the FBO color texture. ImGui is rendered last, on top of the final image.

## Key design choices

- **Modular headers** — each subsystem exposes a small public API under `include/`.
- **Move-only GPU types** — `Mesh`, `Shader`, and `Framebuffer` delete copy operations to prevent accidental double-free of GL handles.
- **FetchContent dependencies** — GLFW, GLM, GLAD, STB, and ImGui are pulled at configure time; no manual dependency install beyond CMake and a compiler.
- **Relative asset paths** — shaders and textures load from `assets/` relative to the process working directory, not the executable location.

## File map

```
src/core/
  main.cpp           Entry point
  Application.cpp    Main loop, scene + post-process orchestration
  Window.cpp         GLFW setup, callbacks, resize handling
  InputManager.cpp   Mouse drag, scroll, key state
  Timer.cpp          Delta time for animation

src/dna/
  DNASequence.cpp    Nucleotide storage, complement, random generation
  HelixGeometry.cpp  Procedural mesh generation
  MutationEngine.cpp Mutation history and sequence edits

src/renderer/
  Shader.cpp         Compile/link GLSL, uniform setters
  Mesh.cpp           VAO/VBO upload, sphere primitive
  Camera.cpp         Arcball orbit + perspective projection
  Texture.cpp        STB image loading
  Framebuffer.cpp    FBO, color texture, depth RBO, fullscreen quad
  StbMacro.cpp       STB_IMPLEMENTATION for stb_image

src/ui/
  UILayer.cpp        ImGui init, three panels, frame hooks
```
