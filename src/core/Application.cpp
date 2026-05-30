#include <core/Application.h>
#include <dna/DNASequence.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace dna {

namespace {
    constexpr int kHelixLength = 32;
}

Application::Application()
    : m_sequence(DNASequence::random(kHelixLength))
    , m_helix(HelixGeometry::build(m_sequence))
    , m_highlightSphere(Mesh::createSphere(1.0f, 8, 16))
    , m_uiLayer(std::make_unique<UILayer>(m_window.getHandle()))
    , m_framebuffer(m_window.getWidth(), m_window.getHeight())
{
    m_shader = std::make_unique<Shader>(
        "assets/shaders/phong.vert",
        "assets/shaders/phong.frag"
    );

    m_postShader = std::make_unique<Shader>(
        "assets/shaders/screen.vert",
        "assets/shaders/post.frag"
    );

    HelixParams params;
    m_helixCenterY = (kHelixLength - 1) * params.rise * 0.5f;

    m_texture = std::make_unique<Texture>("assets/textures/sphere.jpg");

    m_uiState.helixLength = kHelixLength;
    m_uiState.rotationSpeed = 0.5f;
    m_uiState.lightPos = m_lightPos;
    m_uiState.lightColor = m_lightColor;
}

int Application::run() {
    while (!m_window.shouldClose()) {
        m_timer.tick();
        m_window.pollEvents();
        m_uiLayer->beginFrame();
        update();
        render();
        m_uiLayer->endFrame();
        m_window.swapBuffers();
        m_window.getInput().flush();
    }
    return 0;
}

void Application::update() {
    auto& input = m_window.getInput();

    if (input.isMouseHeld()) {
        auto delta = input.getMouseDelta();
        m_camera.rotate(delta.x, -delta.y);
    }

    if (input.getScrollDelta() != 0.0f)
        m_camera.zoom(input.getScrollDelta());

    m_camera.setAspect(m_window.getAspect());

    if (m_uiState.animationRunning)
        m_rotationAngle += m_uiState.rotationSpeed * m_timer.getDeltaTime();

    m_lightPos = m_uiState.lightPos;
    m_lightColor = m_uiState.lightColor;

    if (m_highlightTimer > 0.0f)
        m_highlightTimer -= m_timer.getDeltaTime();

    if (m_uiState.triggerMutation) {
        const Nucleotide bases[] = {
            Nucleotide::A, Nucleotide::T,
            Nucleotide::G, Nucleotide::C
        };
        m_mutationEngine.substitute(
            m_uiState.mutationIndex,
            bases[m_uiState.mutationBase]
        );

        m_highlightIndex = m_uiState.mutationIndex;
        m_highlightTimer = m_highlightDuration;

        rebuildHelix(m_sequence.length());
        m_uiState.triggerMutation = false;

        std::string log;
        for (auto& e : m_mutationEngine.history()) {
            log += std::string(1, toChar(e.oldBase)) + std::to_string(e.index)
                + std::string(1, toChar(e.newBase)) + "\n";
        }
        m_uiState.mutationLog = log;
    }

    if (m_uiState.triggerRegenerate) {
        m_sequence = DNASequence::random(m_uiState.helixLength);
        m_mutationEngine.clearHistory();
        m_highlightIndex = -1;
        rebuildHelix(m_uiState.helixLength);
        m_uiState.triggerRegenerate = false;
    }
}

void Application::render() {
    const int width = m_window.getWidth();
    const int height = m_window.getHeight();
    m_framebuffer.resize(width, height);

    m_framebuffer.bind();
    renderScene();
    m_framebuffer.unbind();

    glViewport(0, 0, width, height);
    renderPostProcess();

    m_uiLayer->render(m_uiState, m_sequence.toString());
}

void Application::renderScene() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec3 viewPos = m_camera.getPosition();

    m_shader->bind();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -m_helixCenterY, 0.0f));
    model = glm::rotate(model, m_rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    m_shader->setMat4("uModel", model);
    m_shader->setMat4("uView", m_camera.getView());
    m_shader->setMat4("uProjection", m_camera.getProjection());
    m_shader->setFloat("uShininess", m_uiState.shininess);
    m_shader->setVec3("uLightPos", m_lightPos);
    m_shader->setVec3("uLightColor", m_lightColor);
    m_shader->setVec3("uViewPos", viewPos);

    m_texture->bind(0);
    m_shader->setInt("uDiffuseTex", 0);

    const glm::vec3 white(1.0f);

    m_shader->setVec3("uColor", white);
    m_helix.strandA.draw();
    m_helix.strandB.draw();

    m_shader->setVec3("uColor", glm::vec3(0.85f, 0.85f, 0.9f));
    m_helix.bonds.draw();

    if (m_highlightIndex >= 0 && m_highlightTimer > 0.0f) {
        HelixParams params;
        const float t = m_highlightTimer / m_highlightDuration;
        const float pulse = t * std::sin(m_highlightTimer * 10.0f) * 0.5f + 0.5f;

        glm::vec3 localPos = HelixGeometry::nucleotidePos(m_highlightIndex, params, 0.0f);
        const float r = params.sphereRadius * 1.15f;
        glm::mat4 sphereModel = model;
        sphereModel = glm::translate(sphereModel, localPos);
        sphereModel = glm::scale(sphereModel, glm::vec3(r));

        m_shader->setMat4("uModel", sphereModel);
        m_shader->setVec3("uColor", glm::mix(white, glm::vec3(1.0f, 1.0f, 0.0f), pulse));
        m_highlightSphere.draw();
        m_shader->setMat4("uModel", model);
        m_shader->setVec3("uColor", white);
    }

    m_shader->unbind();
}

void Application::renderPostProcess() {
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_postShader->bind();
    m_postShader->setInt("uScreenTex", 0);
    m_postShader->setInt("uSobelEnabled", m_uiState.sobelEnabled ? 1 : 0);
    m_postShader->setInt("uVignetteEnabled", m_uiState.vignetteEnabled ? 1 : 0);
    m_postShader->setFloat("uVignetteStrength", m_uiState.vignetteStrength);
    m_postShader->setVec2(
        "uTexelSize",
        glm::vec2(
            1.0f / static_cast<float>(m_framebuffer.width()),
            1.0f / static_cast<float>(m_framebuffer.height())
        )
    );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_framebuffer.colorTexture());

    Framebuffer::drawFullscreenQuad();
    m_postShader->unbind();

    glEnable(GL_DEPTH_TEST);
}

void Application::rebuildHelix(int length) {
    HelixParams params;
    m_helix = HelixGeometry::build(m_sequence);
    m_helixCenterY = (length - 1) * params.rise * 0.5f;
}

} // namespace dna
