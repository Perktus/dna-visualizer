#include <core/InputManager.h>
#include <GLFW/glfw3.h>

namespace dna {

void InputManager::onMouseButton(int button, int action) {
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        m_mouseHeld = (action == GLFW_PRESS);
}

void InputManager::onMouseMove(double x, double y) {
    glm::vec2 newPos = { static_cast<float>(x), static_cast<float>(y) };

    if (m_mouseHeld)
        m_mouseDelta += newPos - m_mousePos;

    m_mousePos = newPos;
}

void InputManager::onScroll(double yOffset) {
    m_scrollDelta += static_cast<float>(yOffset);
}

void InputManager::onKey(int key, int action) {
    // rozbudujemy później
    (void)key; (void)action;
}

void InputManager::flush() {
    m_mouseDelta = { 0.0f, 0.0f };
    m_scrollDelta = 0.0f;
}

} // namespace dna