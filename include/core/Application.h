#pragma once

#include <core/Window.h>
#include <dna/HelixGeometry.h>
#include <renderer/Shader.h>
#include <renderer/Camera.h>
#include <renderer/Texture.h>
#include <memory>

namespace dna {

class Application {
public:
    Application();
    int run();

private:
    void update();
    void render();

    Window m_window { 1280, 720, "DNA Visualizer" };
    Camera m_camera { 45.0f, 1280.0f / 720.0f, 0.1f, 100.0f };
    std::unique_ptr<Shader> m_shader;
    HelixMeshes m_helix;
    std::unique_ptr<Texture> m_texture;
    float m_helixCenterY { 0.0f };

    glm::vec3 m_lightPos { 3.0f, 3.0f, 3.0f };
    glm::vec3 m_lightColor { 1.0f, 1.0f, 1.0f };
};

} // namespace dna