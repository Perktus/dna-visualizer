#pragma once

#include <dna/DNASequence.h>
#include <glm/glm.hpp>
#include <string>

struct GLFWwindow;

namespace dna {

struct UIState {
    // Animation //
    bool animationRunning { true };
    float rotationSpeed { 0.5f };

    // Lighting //
    glm::vec3 lightPos { 3.0f, 3.0f, 3.0f };
    glm::vec3 lightColor { 1.0f, 1.0f, 1.0f };
    float shininess { 32.0f };

    // Mutation //
    int mutationIndex { 0 };
    int mutationBase { 0 }; // 0=A 1=T 2=G 3=C
    bool triggerMutation { false };

    // Post-processing //
    bool sobelEnabled { false };

    // Regenerate helix //
    bool triggerRegenerate{ false };
    int helixLength { 32 };
    
    // Mutation log //
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
};

} // namespace dna