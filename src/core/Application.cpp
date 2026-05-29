#include <core/Application.h>
#include <dna/DNASequence.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace dna {

namespace {
constexpr int kHelixLength = 32;
}

// ── Constructor ── //
Application::Application()
    : m_helix(HelixGeometry::build(DNASequence::random(kHelixLength)))
{
    m_shader = std::make_unique<Shader>(
        "assets/shaders/phong.vert",
        "assets/shaders/phong.frag"
    );

    HelixParams params;
    m_helixCenterY = (kHelixLength - 1) * params.rise * 0.5f;

    m_texture = std::make_unique<Texture>(
        "assets/textures/sphere.jpg"
    );
}

// ── Run ── //
int Application::run() {
    while (!m_window.shouldClose()) {
        m_window.pollEvents();
        update();
        render();
        m_window.swapBuffers();
        m_window.getInput().flush();
    }
    return 0;
}

// ── Update ── //
void Application::update() {
    auto& input = m_window.getInput();

    // Camera rotation
    if (input.isMouseHeld()) {
        auto delta = input.getMouseDelta();
        m_camera.rotate(delta.x, -delta.y);
    }

    // Camera zoom
    if (input.getScrollDelta() != 0.0f)
        m_camera.zoom(input.getScrollDelta());

    // Aspect ratio when changing window size
    m_camera.setAspect(m_window.getAspect());
}

// ── Render ── //
void Application::render() {
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec3 viewPos = m_camera.getPosition();

    m_shader->bind();

    const glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -m_helixCenterY, 0.0f));

    m_shader->setFloat("uShininess", 32.0f);
    m_shader->setMat4("uModel", model);
    m_shader->setMat4("uView", m_camera.getView());
    m_shader->setMat4("uProjection", m_camera.getProjection());
    m_texture->bind(0);
    m_shader->setInt("uDiffuseTex", 0);
    m_shader->setVec3("uLightPos", m_lightPos);
    m_shader->setVec3("uLightColor", m_lightColor);
    m_shader->setVec3("uViewPos", viewPos);

    m_shader->setVec3("uColor", glm::vec3(0.25f, 0.55f, 1.0f));
    m_helix.strandA.draw();

    m_shader->setVec3("uColor", glm::vec3(1.0f, 0.45f, 0.15f));
    m_helix.strandB.draw();

    m_shader->setVec3("uColor", glm::vec3(0.85f, 0.85f, 0.9f));
    m_helix.bonds.draw();

    m_shader->unbind();
}

} // namespace dna