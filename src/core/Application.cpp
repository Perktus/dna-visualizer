#include <core/Application.h>
#include <dna/DNASequence.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace dna {

namespace {
    constexpr int kHelixLength = 32; // helix length
}

// ── Constructor ── //
Application::Application()
    : m_sequence(DNASequence::random(kHelixLength))
    , m_helix(HelixGeometry::build(m_sequence))
    , m_highlightSphere(Mesh::createSphere(1.0f, 8, 16))
    , m_uiLayer(std::make_unique<UILayer>(m_window.getHandle()))
{
    // Create shader
    m_shader = std::make_unique<Shader>( 
        "assets/shaders/phong.vert",
        "assets/shaders/phong.frag"
    );

    // Create helix center y
    HelixParams params; // helix parameters
    m_helixCenterY = (kHelixLength - 1) * params.rise * 0.5f; // update helix center y

    // Create texture
    m_texture = std::make_unique<Texture>("assets/textures/sphere.jpg");

    // Create UI state
    m_uiState.helixLength = kHelixLength; // update helix length
    m_uiState.rotationSpeed = 0.5f; // update rotation speed
    m_uiState.lightPos = m_lightPos; // update light position
    m_uiState.lightColor = m_lightColor; // update light color
}

// ── Run ── //
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

// ── Update ── //
void Application::update() {
    auto& input = m_window.getInput();

    if (input.isMouseHeld()) {
        auto delta = input.getMouseDelta();
        m_camera.rotate(delta.x, -delta.y);
    }

    if (input.getScrollDelta() != 0.0f)
        m_camera.zoom(input.getScrollDelta());

    m_camera.setAspect(m_window.getAspect());

    // Animation
    if (m_uiState.animationRunning)
        m_rotationAngle += m_uiState.rotationSpeed * m_timer.getDeltaTime();

    // Synchronize light with UI
    m_lightPos = m_uiState.lightPos;
    m_lightColor = m_uiState.lightColor;

    // Highlight timer
    if (m_highlightTimer > 0.0f)
        m_highlightTimer -= m_timer.getDeltaTime();

    // Mutation
    if (m_uiState.triggerMutation) {
        const Nucleotide bases[] = {
            Nucleotide::A, Nucleotide::T,
            Nucleotide::G, Nucleotide::C
        };
        m_mutationEngine.substitute(
            m_uiState.mutationIndex,
            bases[m_uiState.mutationBase]
        );

        // Run visual effect 
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

    // Regenerate helix
    if (m_uiState.triggerRegenerate) {
        m_sequence = DNASequence::random(m_uiState.helixLength);
        m_mutationEngine.clearHistory();
        m_highlightIndex = -1;
        rebuildHelix(m_uiState.helixLength);
        m_uiState.triggerRegenerate = false;
    }
}

// ── Render ── //
void Application::render() {
    // Clear color and depth buffer
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get view position
    glm::vec3 viewPos = m_camera.getPosition();

    // Bind shader
    m_shader->bind();

    // Create model matrix
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -m_helixCenterY, 0.0f));
    model = glm::rotate(model, m_rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    // Set uniforms
    m_shader->setMat4 ("uModel", model);
    m_shader->setMat4 ("uView", m_camera.getView());
    m_shader->setMat4 ("uProjection", m_camera.getProjection());
    m_shader->setFloat("uShininess", m_uiState.shininess);
    m_shader->setVec3 ("uLightPos", m_lightPos);
    m_shader->setVec3 ("uLightColor", m_lightColor);
    m_shader->setVec3 ("uViewPos", viewPos);

    // Bind texture
    m_texture->bind(0);
    m_shader->setInt("uDiffuseTex", 0);

    const glm::vec3 white(1.0f);

    // Draw strands (per-vertex colors from A/T/G/C; strand B is complementary)
    m_shader->setVec3("uColor", white);
    m_helix.strandA.draw();
    m_helix.strandB.draw();

    m_shader->setVec3("uColor", glm::vec3(0.85f, 0.85f, 0.9f));
    m_helix.bonds.draw();

    // Flash mutated base on strand A only
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

    // Unbind shader
    m_shader->unbind();

    // Render UI
    m_uiLayer->render(m_uiState, m_sequence.toString());
}

// ── Rebuild Helix ── //
void Application::rebuildHelix(int length) {
    HelixParams params; // helix parameters
    m_helix = HelixGeometry::build(m_sequence); // rebuild helix
    m_helixCenterY = (length - 1) * params.rise * 0.5f; // update helix center y
}

} // namespace dna