#include "Engine/Imgui/ui.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Engine/Graphics/Skinned/animation.h"


/**
 * Initialize ImGui UI for the given GLFW window. Call this when you initialize Graphics
 * @param window - Pointer to the GLFW window.
 */
void UI::initialize(GLFWwindow* window) {
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410 core");
    ImGui::StyleColorsClassic();

}

/**
 * Starts a new ImGui frame. Call this at the beginning of every frame before drawing UI elements.
 * Can be used outside the UI class, e.g. in Core::draw before drawing any UI.
 */
void UI::newImguiFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

/**
 * Renders the ImGui draw data. Call this after all UI elements have been drawn.
 * Can be used outside the UI class, e.g. in Core::draw after drawing all UI.
 */
void UI::render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/**
 * Example draw function that creates a simple ImGui window with some UI elements.
 */
void UI::draw() {
    newImguiFrame();
    ImGui::SetNextWindowPos({0,0});
    ImGui::SetNextWindowSize({200,200}, ImGuiCond_Once); // Set initial size only once to allow resizing
    ImGui::Begin("Debug UI"); // Begin
    // All UI elements goes between Begin and End

    // Example UI element: Colored Text
    ImGui::PushStyleColor(ImGuiCol_Text, toImVec4(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)));
    ImGui::Text("Hello, ImGui!");
    ImGui::PopStyleColor();

    ImGui::End(); // End
    render();


}

ImVec4 UI::toImVec4(glm::vec4 vec) {
    return ImVec4(vec.x, vec.y, vec.z, vec.w);
}


