# Build & development

## Requirements

| Tool | Version |
|------|---------|
| CMake | 3.20+ |
| C++ compiler | C++17 (MSVC 2019+, GCC 9+, Clang 10+) |
| GPU | OpenGL 3.3 Core |
| Network | Required on first CMake configure (FetchContent) |

## Build commands

**Windows (multi-config generator):**

```powershell
cmake -S . -B build
cmake --build build --config Debug
.\build\Debug\dna_visualizer.exe
```

**Linux / macOS:**

```bash
cmake -S . -B build
cmake --build build
./build/dna_visualizer
```

Release builds:

- Windows: `--config Release`
- Unix: `-DCMAKE_BUILD_TYPE=Release` at configure time

## Dependencies (automatic)

CMake `FetchContent` downloads and builds:

| Package | Tag / version | Used for |
|---------|---------------|----------|
| GLFW | 3.4 | Window, context, input |
| GLM | 1.0.1 | Math (matrices, vectors) |
| GLAD | glad2 branch | OpenGL 3.3 loader |
| STB | master | Image loading (`stb_image`) |
| ImGui | v1.90.4 | UI (static library target) |

ImGui is compiled as a project-local static library with GLFW + OpenGL3 backends.

## Running from the correct directory

Asset paths are **relative to the working directory**, not the executable path:

```
assets/shaders/phong.vert
assets/textures/sphere.jpg
```

Always run from the repository root. If shaders fail to load, verify the current working directory contains an `assets/` folder.

## Project layout conventions

- Public headers: `include/<module>/`
- Implementations: `src/<module>/`
- Include style: `#include <core/Window.h>` (module path, no `src/`)
- Namespace: everything in `dna::`
- GPU resource classes: move-only, RAII destructors release GL names

## MSVC note

`CMakeLists.txt` links with `/NODEFAULTLIB:LIBCMT` on MSVC to avoid static runtime conflicts with fetched dependencies.

## Common development tasks

### Change helix appearance

Edit defaults in `HelixParams` (`include/dna/HelixGeometry.h`) or pass custom params from `HelixGeometry::build()` call sites.

### Add a post-process effect

1. Implement the effect in `assets/shaders/post.frag`
2. Add toggles to `UIState` and ImGui in `UILayer.cpp`
3. Pass new uniforms in `Application::renderPostProcess()`

### Wire insertion/deletion mutations to UI

`MutationEngine::insert()` and `deleteBase()` are implemented but not exposed in ImGui. Follow the substitution pattern in `Application::update()`:

- Add UI controls and a trigger flag in `UIState`
- Call the engine method, rebuild helix, update the log format

### Add new geometry

Use `Mesh` with custom `Vertex` arrays, or extend `HelixGeometry` with additional builder functions. Rebuild is triggered via `Application::rebuildHelix()`.

## Debugging tips

- **Black screen** — check shader compile logs (printed to stderr on failure), verify FBO completeness after resize
- **Missing texture** — confirm `assets/textures/sphere.jpg` exists and STB loaded the file
- **Wrong colors on strand B** — strand B uses the complementary sequence; colors come from complement bases, not mirrored A colors
- **ImGui not responding** — ensure `beginFrame()` / `endFrame()` wrap `render()` and GLFW callbacks are not blocked

## Controls (runtime)

| Input | Action |
|-------|--------|
| Left mouse drag | Orbit camera |
| Scroll wheel | Zoom |
| Esc | Quit |
| ImGui panels | Sequence, lighting, post-processing, stats |

See the [main README](../README.md) for panel descriptions.
