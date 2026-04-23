#pragma once

#include <core/InputManager.h>
#include <string>
#include <memory>

struct GLFWwindow;

namespace dna {

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const;
    void pollEvents();
    void swapBuffers();

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    float getAspect() const { return static_cast<float>(m_width) / static_cast<float>(m_height); }
    InputManager& getInput() { return m_input; }

private:
    GLFWwindow* m_window{ nullptr };
    int m_width { 0 };
    int m_height{ 0 };
    InputManager m_input;

    static void cbMouseButton (GLFWwindow*, int button, int action, int);
    static void cbMouseMove (GLFWwindow*, double x, double y);
    static void cbScroll (GLFWwindow*, double, double yOffset);
    static void cbKey (GLFWwindow*, int key, int, int action, int);
    static void cbResize (GLFWwindow*, int width, int height);
};

} // namespace dna