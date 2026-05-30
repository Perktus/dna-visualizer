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
    Application(); // constructor
    int run(); // function to run the application

private:
    void update();
    void render();
    void renderScene();
    void renderPostProcess();
    void rebuildHelix(int length);

    Window m_window{ 1280, 720, "DNA Visualizer" }; // application window
    Camera m_camera{ 45.0f, 1280.0f / 720.0f, 0.1f, 100.0f }; // camera
    Timer m_timer; // timer

    std::unique_ptr<Shader>  m_shader;
    std::unique_ptr<Shader>  m_postShader;
    std::unique_ptr<Texture> m_texture;
    Framebuffer m_framebuffer;
    std::unique_ptr<UILayer> m_uiLayer; // UI layer

    DNASequence m_sequence{ DNASequence::random(32) }; // DNA sequence
    MutationEngine m_mutationEngine{ m_sequence }; // mutation engine
    HelixMeshes m_helix; // helix meshes
    Mesh m_highlightSphere; // unit sphere for mutation flash

    float m_helixCenterY { 0.0f }; // helix center y
    float m_rotationAngle{ 0.0f }; // rotation angle

    int m_highlightIndex{ -1 }; // highlight index
    float m_highlightTimer{ 0.0f }; // highlight timer
    float m_highlightDuration{ 2.0f }; // highlight duration

    UIState m_uiState; // UI state

    glm::vec3 m_lightPos { 3.0f, 3.0f, 3.0f }; // light position
    glm::vec3 m_lightColor { 1.0f, 1.0f, 1.0f }; // light    color
};

} // namespace dna