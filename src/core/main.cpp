#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>

#include <renderer/Shader.h>
#include <renderer/Mesh.h>
#include <renderer/Camera.h>

// ── Globals ───────────────────────────────────────────
static dna::Camera* g_camera  = nullptr;
static bool         g_mouseHeld = false;
static double       g_lastX     = 0.0;
static double       g_lastY     = 0.0;

// ── Callbacks ─────────────────────────────────────────

void onMouseButton(GLFWwindow*, int button, int action, int) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        g_mouseHeld = (action == GLFW_PRESS);
        glfwGetCursorPos(glfwGetCurrentContext(), &g_lastX, &g_lastY);
    }
}

void onMouseMove(GLFWwindow*, double x, double y) {
    if (!g_mouseHeld || !g_camera) return;

    float dx = static_cast<float>(x - g_lastX);
    float dy = static_cast<float>(y - g_lastY);
    g_lastX = x;
    g_lastY = y;

    g_camera->rotate(dx, -dy);
}

void onScroll(GLFWwindow*, double, double yOffset) {
    if (g_camera) g_camera->zoom(static_cast<float>(yOffset));
}

void onResize(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
    if (g_camera)
        g_camera->setAspect(static_cast<float>(width) / static_cast<float>(height));
}

void onKey(GLFWwindow* window, int key, int, int action, int) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// ── Main ──────────────────────────────────────────────

int main() {
    // Inicjalizacja GLFW
    if (!glfwInit()) {
        std::cerr << "[GLFW] init failed\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "DNA Visualizer", nullptr, nullptr);
    if (!window) {
        std::cerr << "[GLFW] window creation failed\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    // Inicjalizacja GLAD
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "[GLAD] failed to load OpenGL\n";
        return -1;
    }

    glViewport(0, 0, 1280, 720);
    glEnable(GL_DEPTH_TEST);

    // Rejestracja callbacków
    glfwSetMouseButtonCallback(window, onMouseButton);
    glfwSetCursorPosCallback  (window, onMouseMove);
    glfwSetScrollCallback     (window, onScroll);
    glfwSetFramebufferSizeCallback(window, onResize);
    glfwSetKeyCallback        (window, onKey);

    // Kamera
    dna::Camera camera(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
    g_camera = &camera;

    // Shader
    dna::Shader shader("assets/shaders/basic.vert",
                       "assets/shaders/basic.frag");

    // Sfera
    dna::Mesh sphere = dna::Mesh::createSphere(1.0f, 32, 32);

    // Macierz modelu — sfera w centrum sceny
    glm::mat4 model = glm::mat4(1.0f);

    // ── Pętla główna ──────────────────────────────────
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.bind();
        shader.setMat4("uModel",      model);
        shader.setMat4("uView",       camera.getView());
        shader.setMat4("uProjection", camera.getProjection());
        shader.setVec3("uColor",      glm::vec3(0.2f, 0.8f, 0.4f));

        sphere.draw();

        shader.unbind();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}