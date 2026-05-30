# DNA model

The biological layer models a double-stranded DNA helix as a coding sequence plus its Watson–Crick complement, rendered as colored spheres connected by backbone tubes and hydrogen-bond bridges.

## Nucleotides

```cpp
enum class Nucleotide { A, T, G, C };
```

| Base | Complement | RGB color (approx.) |
|------|------------|---------------------|
| **A** (Adenine) | T | Red `(0.95, 0.26, 0.21)` |
| **T** (Thymine) | A | Blue `(0.13, 0.59, 0.95)` |
| **G** (Guanine) | C | Green `(0.30, 0.69, 0.31)` |
| **C** (Cytosine) | G | Yellow `(1.00, 0.76, 0.03)` |

Helper functions:

- `complement(Nucleotide)` — Watson–Crick pairing rule
- `fromChar` / `toChar` — parse and serialize single characters (`A`, `T`, `G`, `C`; case-insensitive input)
- `nucleotideColor(Nucleotide)` — returns the GLM color used as vertex attribute

## DNASequence

Stores one coding strand as `std::vector<Nucleotide>`.

| Method | Description |
|--------|-------------|
| `DNASequence(string)` | Parse a sequence from characters |
| `random(length)` | Generate a uniform random sequence |
| `complementary()` | Return a new sequence with each base replaced by its complement |
| `mutate(index, base)` | Point substitution |
| `insert(index, base)` | Insert a base (shifts indices) |
| `remove(index)` | Delete a base |
| `toString()` | Serialize to `ATGC...` string |

Invalid characters in the constructor throw `std::invalid_argument`. Out-of-range indices throw `std::out_of_range`.

## Helix geometry

Procedural geometry is defined by `HelixParams`:

| Parameter | Default | Meaning |
|-----------|---------|---------|
| `radius` | 1.5 | Helix radius (world units) |
| `rise` | 0.34 | Vertical spacing per base pair |
| `twist` | 36.0° | Rotation per base pair |
| `tubeRadius` | 0.15 | Backbone cylinder radius |
| `sphereRadius` | 0.25 | Nucleotide sphere radius |
| `tubeSides` | 8 | Cylinder tessellation |
| `sphereStacks` | 8 | Sphere latitude subdivisions |

### Nucleotide position

Each base index `i` is placed on a helix:

```
angle = radians(twist × i) + radians(phaseOffset)

x = radius × cos(angle)
y = rise × i
z = radius × sin(angle)
```

- **Strand A** uses `phaseOffset = 0°`
- **Strand B** (complement) uses `phaseOffset = 180°` — opposite side of the helix

### Mesh construction

`HelixGeometry::build()` returns `HelixMeshes` with three meshes:

1. **strandA** — For each nucleotide: UV-sphere at `nucleotidePos(i, 0°)` with vertex color from the coding base. Between consecutive nucleotides: cylindrical tube along the segment.
2. **strandB** — Same for the complementary sequence at 180° phase.
3. **bonds** — For each index, a thinner tube (`tubeRadius × 0.6`) connects the A and B positions at that height.

All geometry is rebuilt when the sequence changes. There is no incremental mesh update.

### Vertical centering

The application computes:

```
helixCenterY = (length - 1) × rise × 0.5
```

and applies `translate(0, -helixCenterY, 0)` so the helix is centered around the world origin for camera orbit.

## MutationEngine

Wraps a reference to `DNASequence` and records every edit in `m_history`.

| Operation | `MutationType` | Effect on sequence |
|-----------|----------------|-------------------|
| `substitute(index, base)` | Substitution | Replace base at index |
| `insert(index, base)` | Insertion | Insert before index |
| `deleteBase(index)` | Deletion | Remove base at index |

Each call appends a `MutationEvent`:

```cpp
struct MutationEvent {
    MutationType type;
    int index;
    Nucleotide oldBase;
    Nucleotide newBase;
};
```

### UI integration

The ImGui panel exposes **point substitution** only:

1. User picks index and target base (A/T/G/C)
2. Sets `UIState::triggerMutation = true`
3. `Application::update()` calls `MutationEngine::substitute()`, rebuilds the helix, and writes a text log like `A5G` (old base + index + new base) for each historical event

After a mutation, a highlight sphere pulses at the affected nucleotide for 2 seconds.

**Regenerate** creates a new random sequence of the chosen length (8–64 bp), clears mutation history, and rebuilds geometry.

## Biological simplifications

This is a visualization model, not a biophysical simulation:

- Base-pair geometry is symmetric bridges, not accurate hydrogen-bond angles
- No major/minor groove distinction
- Twist and rise are uniform constants, not sequence-dependent
- Insertion and deletion APIs exist in code but are not wired to the UI
