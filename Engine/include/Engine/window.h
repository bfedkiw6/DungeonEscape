#pragma once

#include "Engine/game.h"

#include <glm/glm.hpp>
#include <memory>

struct GLFWwindow;

class Window {
public:
    ~Window();

    static int initialize(
        int width,
        int height,
        const char* title
    );

    static void addGame(std::unique_ptr<Game> game);

    static bool isActive();

    static void update();
    static void shutDown();

    static void hideMouse();
    static void showMouse();

    static glm::vec2 getSize();
    static float getAspectRatio();
    static bool isCursorVisible();
    static double getCurrentTime();

private:
    static void display();
    static void displayFrameRate();
    static int initializeGLFW(int width, int height);
    static int initializeGLEW();

    // GLFW callback functions
    static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mousePositionCallback(GLFWwindow* window, double x_pos, double y_pos);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double x_offset, double y_offset);
    static void resizeWindowCallback(GLFWwindow* window, int width, int height);

    static GLFWwindow* window_;

    static int width_, height_;
    static float aspect_ratio_;
    static bool cursor_visible_;

    static std::unique_ptr<Game> game_;
};
