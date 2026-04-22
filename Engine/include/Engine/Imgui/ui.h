#pragma once
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
/**
 * Boilerplate UI class for initializing and managing ImGui interface.
 * To be built upon with ImGUI elements as needed for debugging purposes.
 */
class UI {

public:

    static void initialize(GLFWwindow* window);
    static void newImguiFrame();
    static void render();
    static void draw();

private:
    static ImVec4 toImVec4(glm::vec4 vec);





};
