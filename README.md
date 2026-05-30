<div align="center">
  <a href="https://github.com/Perktus">
    <img src="https://i.imgur.com/di9bARH.png" alt="Logo" width="700" height="376">
  </a>

  <h3>Interactive 3D DNA helix visualizer</h3>
  <p>C++17 · OpenGL 3.3 · procedural geometry · post-processing</p>
</div>

## Overview

DNA Visualizer is a computer-graphics coursework project that renders a double-stranded DNA helix in real time. The codebase is split into small modules (`core`, `dna`, `renderer`, `ui`) and uses modern OpenGL: Phong shading, off-screen rendering via an FBO, and a post-processing stack.

**Highlights**

- Procedural helix — nucleotide spheres, backbone tubes, base-pair bridges
- Per-base coloring (A / T / G / C) with a Watson–Crick complementary strand
- Arcball camera and ImGui panels for scene control
- `MutationEngine` — point substitutions with on-screen highlight and history log
- Post-processing — Sobel edge detection, FXAA, vignette

## Documentation

Detailed guides live in [`docs/`](docs/Overview.md):

| Document | Topics |
|----------|--------|
| [Overview](docs/Overview.md) | Index and quick reference |
| [Architecture](docs/Architecture.md) | Modules, main loop, data flow |
| [Rendering Pipeline](docs/Rendering%20Pipeline.md) | FBO, Phong shaders, post-process |
| [DNA Model](docs/DNA%20Model.md) | Sequences, helix params, mutations |
| [Module Reference](docs/Module%20Reference.md) | Public classes and APIs |
| [Build & Development](docs/Build%20and%20Development.md) | Dependencies, conventions, extending |

## Tech stack

| Layer | Libraries |
|-------|-----------|
| Window & input | [GLFW](https://www.glfw.org/) |
| OpenGL loader | [GLAD](https://github.com/Dav1dde/glad) |
| Math | [GLM](https://github.com/g-truc/glm) |
| UI | [Dear ImGui](https://github.com/ocornut/imgui) |
| Images | [STB](https://github.com/nothings/stb) |

Dependencies are fetched automatically via CMake `FetchContent` on first configure.

## Getting started

### Prerequisites

- **CMake** 3.20+
- C++17 compiler (MSVC 2019+, GCC 9+, Clang 10+)
- GPU with **OpenGL 3.3 Core** support
- Internet access on **first configure**

### Build

**Windows**

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

**Linux / macOS**

```bash
cmake -S . -B build
cmake --build build
```

Release: add `--config Release` on Windows, or `-DCMAKE_BUILD_TYPE=Release` on Unix.

### Run

Start the executable from the **repository root** — asset paths are relative to the working directory:

```powershell
# Windows
.\build\Debug\dna_visualizer.exe
```

```bash
# Linux / macOS
./build/dna_visualizer
```

## Controls

| Input | Action |
|-------|--------|
| **Left mouse + drag** | Orbit camera around the helix |
| **Mouse wheel** | Zoom in / out |
| **Esc** | Quit |
| **ImGui panels** | Sequence, mutations, lighting, animation, post-processing |

### ImGui panels

| Panel | Purpose |
|-------|---------|
| **DNA Sequence** | View coding strand, regenerate helix, point mutations |
| **Render Settings** | Animation, Phong lighting, filters, mutation log |
| **Stats** | FPS and frame time |

## Rendering pipeline

```
┌─────────────┐     ┌──────────────┐     ┌──────────────┐     ┌─────────┐
│  3D scene   │ ──► │  Framebuffer │ ──► │ Post-process │ ──► │  ImGui  │ ──► Display
│ Phong + tex │     │ color + depth│     │ Sobel/FXAA/  │     │ overlay │
└─────────────┘     └──────────────┘     │   vignette   │     └─────────┘
                                          └──────────────┘
```

1. **Scene pass** — helix meshes with Phong shading and a diffuse texture.
2. **Off-screen buffer** — color and depth stored in an FBO.
3. **Post-process** — fullscreen quad; Sobel *or* FXAA, then optional vignette.
4. **UI** — ImGui drawn on top of the final image.

See [Rendering Pipeline](docs/Rendering%20Pipeline.md) for shader and uniform details.

## Nucleotide colors

| Base | Color | Complement |
|------|-------|------------|
| **A** | Red | T |
| **T** | Blue | A |
| **G** | Green | C |
| **C** | Yellow | G |

Strand **A** shows the coding sequence; strand **B** is built from its Watson–Crick complement.

## Post-processing

| Effect | Description |
|--------|-------------|
| **Sobel** | Edge map from scene luminance (replaces color) |
| **FXAA** | Fast approximate anti-aliasing (off while Sobel is active) |
| **Vignette** | Darkens corners; strength adjustable in UI |

## Project structure

```
dna-visualizer/
├── assets/
│   ├── shaders/          # Phong, post-process, fullscreen quad
│   └── textures/
├── docs/                 # Architecture, rendering, DNA model, API reference
├── include/
│   ├── core/             # Application, Window, InputManager, Timer
│   ├── dna/              # DNASequence, HelixGeometry, MutationEngine
│   ├── renderer/         # Shader, Mesh, Camera, Texture, Framebuffer
│   └── ui/               # UILayer, UIState
├── src/
├── CMakeLists.txt
└── README.md
```
