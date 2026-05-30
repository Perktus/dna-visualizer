<div align="center">
  <a href="https://github.com/Perktus">
    <img src="https://i.imgur.com/di9bARH.png" alt="Logo" width="700" height="376">
  </a>
</div>

## Overview

DNA Visualizer was developed as a computer-graphics coursework project. It combines a modular C++17 codebase with modern OpenGL practices: shader-based Phong lighting, off-screen rendering, and a post-processing stack for edge detection and screen-space effects.

**Highlights**

- Procedural helix geometry (nucleotide spheres, backbone tubes, base-pair bridges)
- Per-base coloring (A / T / G / C) with a complementary second strand
- Interactive camera (arcball) and ImGui-driven scene parameters
- `MutationEngine` with substitution and mutation history
- Framebuffer pipeline: Sobel edges, FXAA anti-aliasing, vignette

## TechStack

| Layer | Libraries |
|-------|-----------|
| Window & input | [GLFW](https://www.glfw.org/) |
| OpenGL loader | [GLAD](https://github.com/Dav1dde/glad) |
| Math | [GLM](https://github.com/g-truc/glm) |
| UI | [IMGUI](https://github.com/ocornut/imgui) |
| Images | [STB](https://github.com/nothings/stb) |

## Getting started

### Prerequisites

- **CMake** 3.20 or newer  
- C++17 compiler (MSVC 2019+, GCC 9+, Clang 10+)  
- GPU with **OpenGL 3.3** support  
- Internet access on **first configure** (dependencies are fetched automatically)

### Build

**Windows**

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

**Linux / MacOS**

```bash
cmake -S . -B build
cmake --build build
```

For a release build, use `--config Release` on Windows or `-DCMAKE_BUILD_TYPE=Release` on Unix.

### Run

Launch the executable from the **repository root** so relative paths to `assets/` resolve correctly:

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
| **Render Settings** | Animation, Phong lighting, image filters, mutation log |
| **Stats** | FPS and frame time |

## Rendering pipeline

```
┌─────────────┐     ┌──────────────┐     ┌─────────────┐
│  3D scene   │ ──► │  Framebuffer │ ──► │ Post-process │ ──► │ ImGui │ ──► Display
│ Phong+tex   │     │  (color+depth)│     │ Sobel/FXAA/  │     │ overlay│
└─────────────┘     └──────────────┘     │   vignette   │     └─────────┘
                                          └─────────────┘
```

1. **Scene pass** - helix meshes rendered with Phong fragment shading and a diffuse texture.  
2. **Off-screen buffer** - color (and depth) stored in an FBO.  
3. **Post-process** - fullscreen quad; optional Sobel *or* FXAA, then optional vignette.  
4. **UI** - ImGui drawn on top of the final image.

## Nucleotide colors

| Base | Color |
|------|--------|
| **A** | Red |
| **T** | Blue |
| **G** | Green |
| **C** | Yellow |

Strand **A** displays the coding sequence; strand **B** is built from its Watson–Crick complement.

## Post-processing

| Effect | Description |
|--------|-------------|
| **Sobel** | Edge map from scene luminance (replaces color pass) |
| **FXAA** | Fast approximate anti-aliasing on the scene (disabled while Sobel is on) |
| **Vignette** | Darkens image corners; strength adjustable |

## Project structure

```
dna-visualizer/
├── assets/
│   ├── shaders/              # Phong, post-process, fullscreen quad
│   └── textures/
├── include/
│   ├── core/                 # Application, Window, InputManager, Timer
│   ├── dna/                  # DNASequence, HelixGeometry, MutationEngine
│   ├── renderer/             # Shader, Mesh, Camera, Texture, Framebuffer
│   └── ui/                   # UILayer, UIState
├── src/
├── CMakeLists.txt
└── README.md
```
