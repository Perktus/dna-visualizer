#include <ui/UILayer.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

namespace dna {

// ── Constructor ── //
UILayer::UILayer(GLFWwindow* window) {
    IMGUI_CHECKVERSION(); // check version
    ImGui::CreateContext(); // create context
    ImGui::StyleColorsDark(); // style colors dark

    ImGuiStyle& style = ImGui::GetStyle(); // get style
    style.WindowRounding = 8.0f; // window rounding
    style.FrameRounding = 4.0f; // frame rounding
    style.ItemSpacing = { 8.0f, 6.0f }; // item spacing
    style.Colors[ImGuiCol_WindowBg] = { 0.08f, 0.08f, 0.12f, 0.92f }; // window background color

    ImGui_ImplGlfw_InitForOpenGL(window, true); // init for opengl
    ImGui_ImplOpenGL3_Init("#version 330"); // init for opengl
}

// ── Destructor ── //
UILayer::~UILayer() {
    ImGui_ImplOpenGL3_Shutdown(); // shutdown opengl
    ImGui_ImplGlfw_Shutdown(); // shutdown glfw
    ImGui::DestroyContext(); // destroy context
}

// ── Start Frame ── //
void UILayer::beginFrame() {
    ImGui_ImplOpenGL3_NewFrame(); // new frame opengl
    ImGui_ImplGlfw_NewFrame(); // new frame glfw
    ImGui::NewFrame(); // new frame
}

// ── End Frame ── //
void UILayer::endFrame() {
    ImGui::Render(); // render
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // render draw data
}

// ── Render ── //
void UILayer::render(UIState& state, const std::string& sequence) {

    // Panel DNA // 
    ImGui::SetNextWindowPos ({ 10.0f, 10.0f }, ImGuiCond_Once); // set next window position
    ImGui::SetNextWindowSize ({ 320.0f, 0.0f }, ImGuiCond_Once); // set next window size
    ImGui::Begin("DNA Sequence"); // begin panel

    // text to display sequence //
    ImGui::Text("Sequence (%d bp):", (int)sequence.size()); // text 
    ImGui::TextWrapped("%s", sequence.c_str()); // text wrapped
    ImGui::TextDisabled("Strand B shows Watson-Crick complements.");

    ImGui::Separator(); // separator 
    ImGui::Text("Regenerate helix:"); // text 
    ImGui::SliderInt("Length", &state.helixLength, 8, 64); // slider
    if (ImGui::Button("Regenerate")) // button 
        state.triggerRegenerate = true; // trigger regenerate

    ImGui::Separator(); // separator 
    ImGui::Text("Point mutation:"); // text 
    ImGui::SliderInt("Index", &state.mutationIndex, 0, (int)sequence.size() - 1);

    // combo box to select base //
    const char* bases[] = { "A", "T", "G", "C" };
    ImGui::Combo("Base", &state.mutationBase, bases, 4);

    // button to mutate //
    if (ImGui::Button("Mutate")) // button 
        state.triggerMutation = true; // trigger mutation

    ImGui::End(); // end panel

    // Panel Rendering // 
    ImGui::SetNextWindowPos ({ 10.0f, 300.0f }, ImGuiCond_Once); // set next window position
    ImGui::SetNextWindowSize({ 320.0f, 0.0f  }, ImGuiCond_Once); // set next window size
    ImGui::Begin("Render Settings"); // begin panel

    // Animation //
    ImGui::Text("Animation:"); // text 
    ImGui::Checkbox("Running", &state.animationRunning); // checkbox 
    ImGui::SliderFloat("Speed", &state.rotationSpeed, 0.0f, 3.0f); // slider

    // Lighting //
    ImGui::Separator(); // separator 
    ImGui::Text("Lighting:"); // text 
    ImGui::DragFloat3("Light Pos", &state.lightPos.x, 0.1f); // drag float 3
    ImGui::ColorEdit3("Light Color", &state.lightColor.r); // color edit 3
    ImGui::SliderFloat("Shininess",  &state.shininess, 1.0f, 128.0f); // slider float

    // Post-processing //
    ImGui::Separator(); // separator 
    ImGui::Text("Post-processing:"); // text 
    ImGui::Checkbox("Sobel Edge Detection", &state.sobelEnabled);
    ImGui::Checkbox("Vignette", &state.vignetteEnabled);
    if (state.vignetteEnabled)
        ImGui::SliderFloat("Vignette Strength", &state.vignetteStrength, 0.0f, 1.0f);

    // Mutation history //
    ImGui::Separator(); // separator 
    ImGui::Text("Mutation history:"); // text 
    ImGui::TextWrapped("%s", state.mutationLog.c_str()); // text wrapped

    // End //
    ImGui::End();

    // Panel Stats // 
    ImGui::SetNextWindowPos ({ 10.0f, 560.0f }, ImGuiCond_Once); // set next window position
    ImGui::SetNextWindowSize({ 320.0f, 0.0f  }, ImGuiCond_Once); // set next window size
    ImGui::Begin("Stats"); // begin panel
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate); // text 
    ImGui::Text("Frame: %.3f ms", 1000.0f / ImGui::GetIO().Framerate); // text 
    ImGui::End(); // end panel
}

} // namespace dna