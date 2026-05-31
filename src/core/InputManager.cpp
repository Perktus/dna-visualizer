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
    if (key < 0 || key >= kMaxKeys)
        return;

    if (action == GLFW_PRESS)
        m_keys[key] = true;
    else if (action == GLFW_RELEASE)
        m_keys[key] = false;
}

bool InputManager::isKeyDown(int key) const {
    if (key < 0 || key >= kMaxKeys)
        return false;
    return m_keys[key];
}

void InputManager::flush() {
    m_mouseDelta = { 0.0f, 0.0f };
    m_scrollDelta = 0.0f;
}

} // namespace dna