#include <core/Application.h>
#include <dna/DNASequence.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace dna {

namespace {
    constexpr int kHelixLength = 32;
    constexpr float kKeyboardRotateSpeed = 1.5f;
    constexpr float kKeyboardScaleSpeed = 0.8f;
    constexpr float kPedestalHalfHeight = 0.25f;
    constexpr float kPedestalTopGap = 0.02f;
}

// ── Inicjalizacja: GLFW (Window), OpenGL 3.3 Core (GLAD), shadery, tekstury ──
Application::Application()
    : m_sequence(DNASequence::random(kHelixLength))
    , m_helix(HelixGeometry::build(m_sequence))
    , m_highlightSphere(Mesh::createSphere(1.0f, 8, 16))
    , m_groundPlane(Mesh::createPlane(8.0f, 8.0f))
    , m_pedestal(Mesh::createCube(1.0f))
    , m_uiLayer(std::make_unique<UILayer>(m_window.getHandle()))
    , m_framebuffer(m_window.getWidth(), m_window.getHeight())
    , m_groundTexture(Texture::createCheckerboard(512, 16))
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

    m_sphereTexture = std::make_unique<Texture>("assets/textures/sphere.jpg");

    m_uiState.helixLength = kHelixLength;
    m_uiState.rotationSpeed = 0.5f;
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
    const float dt = m_timer.getDeltaTime();
    const bool uiMouse = m_uiLayer->wantsCaptureMouse();
    const bool uiKeyboard = m_uiLayer->wantsCaptureKeyboard();

    // Interakcja myszą: orbita kamery (perspektywa 3D) + zoom scroll
    if (input.isMouseHeld() && !uiMouse) {
        auto delta = input.getMouseDelta();
        m_camera.rotate(delta.x, -delta.y);
    }

    if (input.getScrollDelta() != 0.0f && !uiMouse)
        m_camera.zoom(input.getScrollDelta());

    m_camera.setAspect(m_window.getAspect());

    // Interakcja klawiaturą: obrót helisy, skalowanie, reset kamery
    static bool spaceWasDown = false;
    static bool resetWasDown = false;

    if (!uiKeyboard) {
        if (input.isKeyDown(GLFW_KEY_LEFT))
            m_rotationAngle -= kKeyboardRotateSpeed * dt;
        if (input.isKeyDown(GLFW_KEY_RIGHT))
            m_rotationAngle += kKeyboardRotateSpeed * dt;
        if (input.isKeyDown(GLFW_KEY_EQUAL) || input.isKeyDown(GLFW_KEY_KP_ADD))
            m_uiState.helixScale = std::min(m_uiState.helixScale + kKeyboardScaleSpeed * dt, 2.5f);
        if (input.isKeyDown(GLFW_KEY_MINUS) || input.isKeyDown(GLFW_KEY_KP_SUBTRACT))
            m_uiState.helixScale = std::max(m_uiState.helixScale - kKeyboardScaleSpeed * dt, 0.4f);

        const bool spaceDown = input.isKeyDown(GLFW_KEY_SPACE);
        if (spaceDown && !spaceWasDown)
            m_uiState.animationRunning = !m_uiState.animationRunning;
        spaceWasDown = spaceDown;

        const bool resetDown = input.isKeyDown(GLFW_KEY_R);
        if (resetDown && !resetWasDown)
            m_camera.reset();
        resetWasDown = resetDown;
    } else {
        spaceWasDown = false;
        resetWasDown = false;
    }

    // Animacja obrotu helisy wokół osi Y
    if (m_uiState.animationRunning)
        m_rotationAngle += m_uiState.rotationSpeed * dt;

    // Animacja pozycji światła punktowego (orbita)
    if (m_uiState.animateLight) {
        m_lightAnimPhase += m_uiState.lightAnimSpeed * dt;
        m_uiState.lightPos = {
            4.0f * std::cos(m_lightAnimPhase),
            2.0f + std::sin(m_lightAnimPhase * 0.7f),
            4.0f * std::sin(m_lightAnimPhase)
        };
    }

    if (m_highlightTimer > 0.0f)
        m_highlightTimer -= dt;

    // Filtrowanie tekstury (Nearest / Linear / Mipmap + anizotropia)
    const TextureFilterMode filterMode = static_cast<TextureFilterMode>(m_uiState.textureFilter);
    m_sphereTexture->setFilterMode(filterMode);
    m_groundTexture.setFilterMode(filterMode);

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

void Application::setupLightingUniforms(const glm::vec3& viewPos) {
    m_shader->setVec3("uAmbientColor", m_uiState.ambientColor);
    m_shader->setFloat("uAmbientStrength", m_uiState.ambientStrength);
    m_shader->setVec3("uDirLightDir", m_uiState.dirLightDir);
    m_shader->setVec3("uDirLightColor", m_uiState.dirLightColor);
    m_shader->setVec3("uLightPos", m_uiState.lightPos);
    m_shader->setVec3("uLightColor", m_uiState.lightColor);
    m_shader->setFloat("uShininess", m_uiState.shininess);
    m_shader->setVec3("uViewPos", viewPos);
}

void Application::drawMesh(const Mesh& mesh, const glm::mat4& model, const glm::vec3& color,
                           bool useTexture, const Texture* texture) {
    m_shader->setMat4("uModel", model);
    m_shader->setVec3("uColor", color);
    m_shader->setInt("uUseTexture", useTexture ? 1 : 0);

    if (useTexture && texture) {
        texture->bind(0);
        m_shader->setInt("uDiffuseTex", 0);
    }

    mesh.draw();
}

void Application::renderScene() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::vec3 viewPos = m_camera.getPosition();

    m_shader->bind();
    m_shader->setMat4("uView", m_camera.getView());
    m_shader->setMat4("uProjection", m_camera.getProjection());
    setupLightingUniforms(viewPos);

    HelixParams params;
    // Pierwsza baza ma y=0; dolna połowa kuli (promień sphereRadius) musi leżeć na postumencie
    const float helixBaseY = -m_helixCenterY + params.sphereRadius + kPedestalTopGap;
    const float pedestalCenterY = helixBaseY - params.sphereRadius - kPedestalTopGap - kPedestalHalfHeight;

    // Macierz modelu helisy: centrowanie, skala, obrót (animacja)
    glm::mat4 helixModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, helixBaseY, 0.0f));
    helixModel = glm::rotate(helixModel, m_rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    helixModel = glm::scale(helixModel, glm::vec3(m_uiState.helixScale));

    const glm::vec3 white(1.0f);

    // Model 1: płaszczyzna podłoża (teksturowana szachownica)
    {
        glm::mat4 groundModel = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(0.0f, pedestalCenterY - kPedestalHalfHeight - 0.05f, 0.0f)
        );
        drawMesh(m_groundPlane, groundModel, white, true, &m_groundTexture);
    }

    // Model 2: postument (sześcian, kolor jednolity)
    {
        glm::mat4 pedestalModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, pedestalCenterY, 0.0f));
        pedestalModel = glm::scale(pedestalModel, glm::vec3(2.5f, kPedestalHalfHeight, 2.5f));
        drawMesh(m_pedestal, pedestalModel, glm::vec3(0.35f, 0.35f, 0.4f), false, nullptr);
    }

    // Model 3: nić A helisy (teksturowana mapą diffuse)
    drawMesh(m_helix.strandA, helixModel, white, true, m_sphereTexture.get());

    // Model 4: nić B (kolory wierzchołkowe baz A/T/G/C)
    drawMesh(m_helix.strandB, helixModel, white, false, nullptr);

    // Model 5: wiązania (mostki między nićmi)
    drawMesh(m_helix.bonds, helixModel, glm::vec3(0.85f, 0.85f, 0.9f), false, nullptr);

    // Podświetlenie mutacji — dodatkowa kula
    if (m_highlightIndex >= 0 && m_highlightTimer > 0.0f) {
        HelixParams params;
        const float t = m_highlightTimer / m_highlightDuration;
        const float pulse = t * std::sin(m_highlightTimer * 10.0f) * 0.5f + 0.5f;

        glm::vec3 localPos = HelixGeometry::nucleotidePos(m_highlightIndex, params, 0.0f);
        const float r = params.sphereRadius * 1.15f;
        glm::mat4 sphereModel = helixModel;
        sphereModel = glm::translate(sphereModel, localPos);
        sphereModel = glm::scale(sphereModel, glm::vec3(r));

        drawMesh(m_highlightSphere, sphereModel,
                 glm::mix(white, glm::vec3(1.0f, 1.0f, 0.0f), pulse), true, m_sphereTexture.get());
    }

    m_shader->unbind();
}

void Application::renderPostProcess() {
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Filtrowanie obrazu: Sobel, FXAA, winieta
    m_postShader->bind();
    m_postShader->setInt("uScreenTex", 0);
    m_postShader->setInt("uSobelEnabled", m_uiState.sobelEnabled ? 1 : 0);
    m_postShader->setInt("uFxaaEnabled", m_uiState.fxaaEnabled ? 1 : 0);
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
