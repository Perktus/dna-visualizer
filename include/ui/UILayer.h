#pragma once

#include <dna/DNASequence.h>
#include <glm/glm.hpp>
#include <string>

struct GLFWwindow;

namespace dna {

struct UIState {
    // Animation of the helix and light
    bool animationRunning{ true };
    float rotationSpeed{ 0.5f };
    bool animateLight{ false };
    float lightAnimSpeed{ 0.8f };

    // Lighting: ambient + directional + point
    glm::vec3 ambientColor{ 0.15f, 0.15f, 0.2f };
    float ambientStrength{ 1.0f };
    glm::vec3 dirLightDir{ -0.3f, -1.0f, -0.4f };
    glm::vec3 dirLightColor{ 0.5f, 0.5f, 0.55f };
    glm::vec3 lightPos{ 3.0f, 3.0f, 3.0f };
    glm::vec3 lightColor{ 1.0f, 1.0f, 1.0f };
    float shininess{ 32.0f };

    // Helix scale (keyboard +/-)
    float helixScale{ 1.0f };

    // Texture filtering (Nearest / Linear / Mipmap)
    int textureFilter{ 2 };

    // Mutation
    int mutationIndex{ 0 };
    int mutationBase{ 0 };
    bool triggerMutation{ false };

    // Post-processing (image filtering in screen space)
    bool sobelEnabled{ false };
    bool fxaaEnabled{ true };
    bool vignetteEnabled{ false };
    float vignetteStrength{ 0.65f };

    bool triggerRegenerate{ false };
    int helixLength{ 32 };

    std::string mutationLog;
};

class UILayer {
public:
    UILayer(GLFWwindow* window);
    ~UILayer();

    UILayer(const UILayer&) = delete;
    UILayer& operator=(const UILayer&) = delete;

    void beginFrame();
    void render(UIState& state, const std::string& sequence);
    void endFrame();

    bool wantsCaptureMouse() const;
    bool wantsCaptureKeyboard() const;
};

} // namespace dna
