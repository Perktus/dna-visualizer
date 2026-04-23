#pragma once

#include <glm/glm.hpp>

namespace dna {

class Camera {
public:
    Camera(float fovDeg, float aspect, float nearPlane, float farPlane);
    ~Camera() = default;

    void rotate(float deltaX, float deltaY);
    void zoom(float delta);

    glm::mat4 getView() const;
    glm::mat4 getProjection() const;

    void setAspect(float aspect);

private:
    float m_yaw { 0.0f };   
    float m_pitch { 0.0f };   
    float m_distance { 5.0f };   
    float m_sensitivity { 0.3f };   

    float m_fov { 45.0f };
    float m_aspect {  1.0f };
    float m_near {  0.1f };
    float m_far { 100.0f };
};

} // namespace dna