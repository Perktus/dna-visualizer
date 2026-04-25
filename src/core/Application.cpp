#include <core/Application.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace dna {

// ── Constructor ── //
Application::Application() {
    m_shader = std::make_unique<Shader>(
        "assets/shaders/phong.vert",
        "assets/shaders/phong.frag"
    );

    m_sphere = std::make_unique<Mesh>(
        std::move(Mesh::createSphere(1.0f, 32, 32))
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

    m_shader->setFloat("uShininess", 32.0f);
    m_shader->setMat4("uModel", glm::mat4(1.0f));
    m_shader->setMat4("uView", m_camera.getView());
    m_shader->setMat4("uProjection", m_camera.getProjection());
    m_shader->setVec3("uColor", glm::vec3(0.2f, 0.8f, 0.4f));
    m_shader->setVec3("uLightPos", m_lightPos);
    m_shader->setVec3("uLightColor", m_lightColor);
    m_shader->setVec3("uViewPos", viewPos);

    m_sphere->draw();
    m_shader->unbind();
}

} // namespace dna