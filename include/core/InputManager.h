#pragma once

#include <glm/glm.hpp>

namespace dna {

class InputManager {
public:
    static constexpr int kMaxKeys = 512;

    void onMouseButton(int button, int action);
    void onMouseMove(double x, double y);
    void onScroll(double yOffset);
    void onKey(int key, int action);

    bool isMouseHeld() const { return m_mouseHeld; }
    glm::vec2 getMouseDelta() const { return m_mouseDelta; }
    float getScrollDelta() const { return m_scrollDelta; }

    /// Czy klawisz jest aktualnie wciśnięty (GLFW key code).
    bool isKeyDown(int key) const;

    void flush();

private:
    bool m_mouseHeld{ false };
    glm::vec2 m_mousePos{ 0.0f, 0.0f };
    glm::vec2 m_mouseDelta{ 0.0f, 0.0f };
    float m_scrollDelta{ 0.0f };
    bool m_keys[kMaxKeys]{};
};

} // namespace dna