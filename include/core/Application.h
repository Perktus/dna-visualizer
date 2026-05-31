#pragma once

#include <core/Window.h>
#include <core/Timer.h>
#include <dna/DNASequence.h>
#include <dna/MutationEngine.h>
#include <dna/HelixGeometry.h>
#include <renderer/Shader.h>
#include <renderer/Camera.h>
#include <renderer/Texture.h>
#include <renderer/Mesh.h>
#include <renderer/Framebuffer.h>
#include <ui/UILayer.h>
#include <memory>

namespace dna {

class Application {
public:
    Application();
    int run();

private:
    void update();
    void render();
    void renderScene();
    void renderPostProcess();
    void rebuildHelix(int length);
    void setupLightingUniforms(const glm::vec3& viewPos);
    void drawMesh(const Mesh& mesh, const glm::mat4& model, const glm::vec3& color,
                  bool useTexture, const Texture* texture);

    Window m_window{ 1280, 720, "DNA Visualizer" };
    Camera m_camera{ 45.0f, 1280.0f / 720.0f, 0.1f, 100.0f };
    Timer m_timer;

    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<Shader> m_postShader;
    std::unique_ptr<Texture> m_sphereTexture;
    Texture m_groundTexture;
    Framebuffer m_framebuffer;
    std::unique_ptr<UILayer> m_uiLayer;

    DNASequence m_sequence{ DNASequence::random(32) };
    MutationEngine m_mutationEngine{ m_sequence };
    HelixMeshes m_helix;
    Mesh m_highlightSphere;
    Mesh m_groundPlane;
    Mesh m_pedestal;

    float m_helixCenterY{ 0.0f };
    float m_rotationAngle{ 0.0f };
    float m_lightAnimPhase{ 0.0f };

    int m_highlightIndex{ -1 };
    float m_highlightTimer{ 0.0f };
    float m_highlightDuration{ 2.0f };

    UIState m_uiState;
};

} // namespace dna
