#include <core/Timer.h>
#include <GLFW/glfw3.h>

namespace dna {

Timer::Timer() {
    m_lastTime = static_cast<float>(glfwGetTime());
}

void Timer::tick() {
    float currentTime = static_cast<float>(glfwGetTime());
    m_deltaTime = currentTime - m_lastTime;
    m_totalTime += m_deltaTime;
    m_lastTime = currentTime;
}

} // namespace dna