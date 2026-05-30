# Module reference

Concise API reference for the public headers under `include/`.

---

## core/

### `Window`

GLFW window wrapper with embedded `InputManager`.

| Method | Description |
|--------|-------------|
| `shouldClose()` | True when user requested close |
| `pollEvents()` | Process GLFW events |
| `swapBuffers()` | Present frame |
| `getWidth()`, `getHeight()`, `getAspect()` | Drawable size |
| `getInput()` | Reference to input manager |
| `getHandle()` | Raw `GLFWwindow*` for ImGui |

Static callbacks forward mouse, scroll, key, and resize events to `InputManager` and update internal dimensions.

### `InputManager`

Frame-based input state (polled once per frame, flushed after swap).

| Method | Description |
|--------|-------------|
| `isMouseHeld()` | Left button currently down |
| `getMouseDelta()` | Pixel delta since last frame |
| `getScrollDelta()` | Scroll wheel delta |
| `isKeyPressed(key)` | Edge-triggered key press |
| `flush()` | Reset per-frame deltas |

### `Timer`

| Method | Description |
|--------|-------------|
| `tick()` | Update internal clock |
| `getDeltaTime()` | Seconds since last `tick()` |

### `Application`

Top-level application class. Not intended for extension outside `src/core/Application.cpp`.

---

## dna/

### `DNASequence`

See [DNA model](DNA%20Model.md).

### `HelixGeometry`

| Method | Returns |
|--------|---------|
| `build(seq, params?)` | `HelixMeshes` — three ready-to-draw meshes |
| `nucleotidePos(index, params, phaseOffset)` | World-space center of nucleotide `index` |

### `MutationEngine`

| Method | Returns |
|--------|---------|
| `substitute(index, newBase)` | `MutationEvent` |
| `insert(index, base)` | `MutationEvent` |
| `deleteBase(index)` | `MutationEvent` |
| `history()` | Const reference to event log |
| `clearHistory()` | Remove all recorded events |
| `lastMutation()` | Pointer to last event, or `nullptr` |

---

## renderer/

### `Shader`

Loads and compiles vertex + fragment shader files from disk.

| Method | Description |
|--------|-------------|
| `bind()` / `unbind()` | Activate program |
| `setInt`, `setFloat`, `setVec2`, `setVec3`, `setMat4` | Uniform upload helpers |

### `Mesh`

GPU mesh with interleaved vertex layout: position, normal, UV, color.

| Method | Description |
|--------|-------------|
| `Mesh(vertices, indices)` | Upload to VAO/VBO/EBO |
| `draw()` | `glDrawElements(GL_TRIANGLES, ...)` |
| `createSphere(r, stacks, slices)` | Unit-scale sphere primitive |

Move-only; copying is deleted.

### `Camera`

Arcball camera. See [Rendering pipeline](Rendering%20Pipeline.md#camera).

| Method | Description |
|--------|-------------|
| `rotate(deltaX, deltaY)` | Orbit |
| `zoom(delta)` | Adjust distance |
| `getView()`, `getProjection()` | GLM matrices |
| `getPosition()` | Eye position in world space |
| `setAspect(ratio)` | Update projection on resize |

### `Texture`

Loads an image via STB and creates a `GL_TEXTURE_2D`.

| Method | Description |
|--------|-------------|
| `bind(unit)` | Bind to texture unit (default 0) |

### `Framebuffer`

Off-screen render target.

| Method | Description |
|--------|-------------|
| `resize(w, h)` | Recreate attachments |
| `bind()` / `unbind()` | FBO / default framebuffer |
| `colorTexture()` | Color attachment ID for sampling |
| `width()`, `height()` | Current dimensions |
| `drawFullscreenQuad()` | Static helper for post-process |

---

## ui/

### `UIState`

Plain struct shared between ImGui panels and `Application::update()`. Fields include animation toggles, light parameters, post-process flags, mutation controls, and `mutationLog`.

Trigger flags (`triggerMutation`, `triggerRegenerate`) are one-shot: the application clears them after handling.

### `UILayer`

ImGui lifecycle bound to a GLFW window.

| Method | Description |
|--------|-------------|
| `beginFrame()` | New ImGui frame |
| `render(state, sequence)` | Draw DNA Sequence, Render Settings, and Stats panels |
| `endFrame()` | Render draw lists via OpenGL3 backend |

---

## Shader files

| File | Role |
|------|------|
| `phong.vert` / `phong.frag` | Lit 3D scene |
| `screen.vert` / `post.frag` | Fullscreen post-process |
| `basic.vert` / `basic.frag` | Legacy/simple shader (not used by Application) |
