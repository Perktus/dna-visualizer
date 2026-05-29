#pragma once

namespace dna {

class Timer {
public:
    Timer();

    void tick();
    float getDeltaTime() const { return m_deltaTime; }
    float getTotalTime() const { return m_totalTime; }

private:
    float m_deltaTime { 0.0f };
    float m_totalTime { 0.0f };
    float m_lastTime { 0.0f };
};

} // namespace dna