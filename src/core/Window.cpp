#include <core/Window.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace dna {

// ── Konstruktor / Destruktor ──────────────────────────

Window::Window(int width, int height, const std::string& title)
    : m_width(width), m_height(height)
{
    if (!glfwInit())
        throw std::runtime_error("[GLFW] init failed");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window)
        throw std::runtime_error("[GLFW] window creation failed");

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // vsync

    // Inicjalizacja GLAD
    if (!gladLoadGL(glfwGetProcAddress))
        throw std::runtime_error("[GLAD] failed to load OpenGL");

    // Przekazujemy wskaźnik na Window do callbacków
    glfwSetWindowUserPointer(m_window, this);

    // Rejestracja callbacków
    glfwSetMouseButtonCallback    (m_window, cbMouseButton);
    glfwSetCursorPosCallback      (m_window, cbMouseMove);
    glfwSetScrollCallback         (m_window, cbScroll);
    glfwSetKeyCallback            (m_window, cbKey);
    glfwSetFramebufferSizeCallback(m_window, cbResize);

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
}

Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

// ── Public ────────────────────────────────────────────

bool Window::shouldClose() const { return glfwWindowShouldClose(m_window); }
void Window::pollEvents()        { glfwPollEvents(); }
void Window::swapBuffers()       { glfwSwapBuffers(m_window); }

// ── Callbacki ─────────────────────────────────────────

void Window::cbMouseButton(GLFWwindow* w, int button, int action, int) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
    self->m_input.onMouseButton(button, action);
}

void Window::cbMouseMove(GLFWwindow* w, double x, double y) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
    self->m_input.onMouseMove(x, y);
}

void Window::cbScroll(GLFWwindow* w, double, double yOffset) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
    self->m_input.onScroll(yOffset);
}

void Window::cbKey(GLFWwindow* w, int key, int, int action, int) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
    self->m_input.onKey(key, action);

    // ESC zamyka okno
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(w, true);
}

void Window::cbResize(GLFWwindow* w, int width, int height) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
    self->m_width  = width;
    self->m_height = height;
    glViewport(0, 0, width, height);
}

} // namespace dna