#include <ui/UILayer.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

namespace dna {

UILayer::UILayer(GLFWwindow* window) {
    IMGUI_CHECKVERSION(); // Check if the ImGui version is correct
    ImGui::CreateContext(); // Create the ImGui context
    ImGui::StyleColorsDark(); // Set the ImGui style to dark

    ImGuiStyle& style = ImGui::GetStyle(); // Get the ImGui style
    style.WindowRounding = 8.0f; // Set the window rounding
    style.FrameRounding = 4.0f; // Set the frame rounding
    style.ItemSpacing = { 8.0f, 6.0f }; // Set the item spacing
    style.Colors[ImGuiCol_WindowBg] = { 0.08f, 0.08f, 0.12f, 0.92f }; // Set the window background color

    ImGui_ImplGlfw_InitForOpenGL(window, true); // Initialize the ImGui GLFW backend
    ImGui_ImplOpenGL3_Init("#version 330"); // Initialize the ImGui OpenGL backend
}

UILayer::~UILayer() {
    ImGui_ImplOpenGL3_Shutdown(); // Shutdown the ImGui OpenGL backend
    ImGui_ImplGlfw_Shutdown(); // Shutdown the ImGui GLFW backend
    ImGui::DestroyContext(); // Destroy the ImGui context
}

void UILayer::beginFrame() {
    ImGui_ImplOpenGL3_NewFrame(); // New frame for the ImGui OpenGL backend
    ImGui_ImplGlfw_NewFrame(); // New frame for the ImGui GLFW backend
    ImGui::NewFrame(); // New frame for the ImGui context
}

void UILayer::endFrame() {
    ImGui::Render(); // Render the ImGui context
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Render the ImGui draw data
}

bool UILayer::wantsCaptureMouse() const {
    return ImGui::GetIO().WantCaptureMouse; // Check if the mouse is captured by the ImGui context
}

bool UILayer::wantsCaptureKeyboard() const {
    return ImGui::GetIO().WantCaptureKeyboard; // Check if the keyboard is captured by the ImGui context
}

void UILayer::render(UIState& state, const std::string& sequence) {
    const ImVec2 display = ImGui::GetIO().DisplaySize; // Get the display size
    constexpr float kPanelWidth = 320.0f; // Set the panel width
    constexpr float kMargin = 10.0f; // Set the margin
    const float rightX = display.x - kPanelWidth - kMargin; // Set the right x position
    constexpr ImGuiWindowFlags kFixedPanel = ImGuiWindowFlags_NoMove; // Set the fixed panel flags

    ImGui::SetNextWindowPos({ kMargin, kMargin }, ImGuiCond_Always); // Set the next window position
    ImGui::SetNextWindowSize({ kPanelWidth, 0.0f }, ImGuiCond_FirstUseEver); // Set the next window size
    ImGui::Begin("DNA Sequence", nullptr, kFixedPanel); // Begin the DNA sequence window

    ImGui::Text("Sequence (%d bp):", (int)sequence.size()); // Show the sequence length
    ImGui::TextWrapped("%s", sequence.c_str()); // Show the sequence
    ImGui::TextDisabled("Strand B shows Watson-Crick complements."); // Show the Watson-Crick complements

    ImGui::Separator();
    ImGui::Text("Regenerate helix:"); // Show the regenerate helix text
    ImGui::SliderInt("Length", &state.helixLength, 8, 64); // Show the helix length slider
    if (ImGui::Button("Regenerate")) // Show the regenerate button
        state.triggerRegenerate = true; // Trigger the regenerate

    ImGui::Separator();
    ImGui::Text("Point mutation:"); // Show the point mutation text
    ImGui::SliderInt("Index", &state.mutationIndex, 0, (int)sequence.size() - 1); // Show the mutation index slider

    const char* bases[] = { "A", "T", "G", "C" }; // Set the bases
    ImGui::Combo("Base", &state.mutationBase, bases, 4); // Show the base combo

    if (ImGui::Button("Mutate")) // Show the mutate button
        state.triggerMutation = true; // Trigger the mutation

    ImGui::End();

    ImGui::SetNextWindowPos({ rightX, kMargin }, ImGuiCond_Always); // Set the next window position
    ImGui::SetNextWindowSize({ kPanelWidth, 0.0f }, ImGuiCond_FirstUseEver); // Set the next window size
    ImGui::Begin("Render Settings", nullptr, kFixedPanel); // Begin the render settings window

    ImGui::Text("Animation:"); // Show the animation text
    ImGui::Checkbox("Helix rotation", &state.animationRunning); // Show the helix rotation checkbox
    ImGui::SliderFloat("Rotation speed", &state.rotationSpeed, 0.0f, 3.0f); // Show the rotation speed slider
    ImGui::Checkbox("Light orbit", &state.animateLight); // Show the light orbit checkbox
    ImGui::SliderFloat("Light speed", &state.lightAnimSpeed, 0.0f, 3.0f);
    ImGui::SliderFloat("Helix scale", &state.helixScale, 0.4f, 2.5f); // Show the helix scale slider

    ImGui::Separator();
    ImGui::Text("Lighting (ambient + directional + point):"); // Show the lighting text
    ImGui::ColorEdit3("Ambient", &state.ambientColor.r); // Show the ambient color editor           
    ImGui::SliderFloat("Ambient strength", &state.ambientStrength, 0.0f, 2.0f); // Show the ambient strength slider
    ImGui::DragFloat3("Dir. light dir", &state.dirLightDir.x, 0.02f); // Show the directional light direction drag float
    ImGui::ColorEdit3("Dir. light color", &state.dirLightColor.r); // Show the directional light color editor
    ImGui::DragFloat3("Point light pos", &state.lightPos.x, 0.1f); // Show the point light position drag float
    ImGui::ColorEdit3("Point light color", &state.lightColor.r); // Show the point light color editor
    ImGui::SliderFloat("Shininess", &state.shininess, 1.0f, 128.0f); // Show the shininess slider

    ImGui::Separator();
    ImGui::Text("Texture filtering:"); // Show the texture filtering text
    const char* filters[] = { "Nearest", "Linear", "Mipmap (trilinear)" };
    ImGui::Combo("Filter mode", &state.textureFilter, filters, 3); // Show the filter mode combo

    ImGui::Separator();
    ImGui::Text("Post-processing:"); // Show the post-processing text
    ImGui::Checkbox("Sobel Edge Detection", &state.sobelEnabled); // Show the sobel edge detection checkbox
    ImGui::Checkbox("FXAA (anti-aliasing)", &state.fxaaEnabled); // Show the fxaa checkbox
    if (state.sobelEnabled && state.fxaaEnabled)
        ImGui::TextDisabled("FXAA applies only when Sobel is off."); // Show the fxaa applies only when sobel is off text
    ImGui::Checkbox("Vignette", &state.vignetteEnabled); // Show the vignette checkbox
    if (state.vignetteEnabled)
        ImGui::SliderFloat("Vignette Strength", &state.vignetteStrength, 0.0f, 1.0f); // Show the vignette strength slider

    ImGui::Separator();
    ImGui::Text("Mutation history:"); // Show the mutation history text
    ImGui::TextWrapped("%s", state.mutationLog.c_str()); // Show the mutation log

    ImGui::End();

    ImGui::SetNextWindowPos({ display.x - kMargin, display.y - kMargin }, ImGuiCond_Always, { 1.0f, 1.0f });
    ImGui::SetNextWindowSize({ kPanelWidth, 0.0f }, ImGuiCond_FirstUseEver); // Set the next window size
    ImGui::Begin("Controls & Stats", nullptr, kFixedPanel); // Begin the controls & stats window
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate); // Show the fps
    ImGui::Text("Frame: %.3f ms", 1000.0f / ImGui::GetIO().Framerate); // Show the frame time
    ImGui::Separator();
    ImGui::TextDisabled("Mouse: drag = orbit, scroll = zoom"); // Show the mouse drag = orbit, scroll = zoom text
    ImGui::TextDisabled("Keys: Left/Right = rotate helix"); // Show the keys left/right = rotate helix text
    ImGui::TextDisabled("      +/- = scale, Space = toggle anim"); // Show the keys +/- = scale, space = toggle anim text
    ImGui::TextDisabled("      R = reset camera, Esc = quit"); // Show the r = reset camera, esc = quit text
    ImGui::End();
}

} // namespace dna
