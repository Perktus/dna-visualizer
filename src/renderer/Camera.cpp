#include <renderer/Camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace dna {

// ── Constructor ── //
Camera::Camera(float fovDeg, float aspect, float nearPlane, float farPlane)
    : m_fov(fovDeg), m_aspect(aspect), m_near(nearPlane), m_far(farPlane) {   
}

// ── Arcball ── //
void Camera::rotate(float deltaX, float deltaY) {
    m_yaw += deltaX * m_sensitivity;
    m_pitch += deltaY * m_sensitivity;
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
}

void Camera::zoom(float delta) {
    m_distance -= delta * 0.5f;
    m_distance = std::clamp(m_distance, 1.0f, 50.0f);
}

// ── Matrices ── //
glm::mat4 Camera::getView() const {
    float yawRad = glm::radians(m_yaw);
    float pitchRad = glm::radians(m_pitch);

    glm::vec3 eye = {
        m_distance * std::cos(pitchRad) * std::sin(yawRad),
        m_distance * std::sin(pitchRad),
        m_distance * std::cos(pitchRad) * std::cos(yawRad)
    };

    return glm::lookAt(eye, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::getProjection() const {
    return glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
}

glm::vec3 Camera::getPosition() const {
    float yawRad   = glm::radians(m_yaw);
    float pitchRad = glm::radians(m_pitch);

    return {
        m_distance * std::cos(pitchRad) * std::sin(yawRad),
        m_distance * std::sin(pitchRad),
        m_distance * std::cos(pitchRad) * std::cos(yawRad)
    };
}

void Camera::setAspect(float aspect) {
    m_aspect = aspect;
}

} // namespace dna