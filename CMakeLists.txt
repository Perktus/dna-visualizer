# ── CMake ──────────────────────────────────────────────
cmake_minimum_required(VERSION 3.20)
project(dna_visualizer VERSION 1.0 LANGUAGES CXX)

# ── C++ Standard ───────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Fetch Content ──────────────────────────────────────
include(FetchContent)

# ── GLFW ──────────────────────────────────────────────
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# ── GLM ───────────────────────────────────────────────
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# ── GLAD ──────────────────────────────────────────────
FetchContent_Declare(glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        glad2
    SOURCE_SUBDIR  cmake
)
FetchContent_MakeAvailable(glad)

glad_add_library(glad_gl REPRODUCIBLE API gl:core=3.3)

# ── Executable ────────────────────────────────────────
add_executable(dna_visualizer
    src/core/main.cpp
    src/renderer/Shader.cpp
    src/renderer/Mesh.cpp
    src/renderer/Camera.cpp
)

# ── MSVC ──────────────────────────────────────────────
if(MSVC)
    target_link_options(dna_visualizer PRIVATE /NODEFAULTLIB:LIBCMT)
endif()

# ── Link Libraries ─────────────────────────────────────
target_link_libraries(dna_visualizer PRIVATE
    glfw
    glm
    glad_gl
)

# ── Include Directories ─────────────────────────────────
target_include_directories(dna_visualizer PRIVATE
    include/
)