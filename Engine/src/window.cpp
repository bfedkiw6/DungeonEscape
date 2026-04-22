#include "Engine/window.h"
#include "Engine/debug.h"
#include "Engine/Graphics/graphics.h"

#include <GLFW/glfw3.h>
#include <imgui/backends/imgui_impl_glfw.h>

GLFWwindow* Window::window_ = nullptr;

int Window::width_, Window::height_;
float Window::aspect_ratio_;
bool Window::cursor_visible_ = true;

std::unique_ptr<Game> Window::game_ = nullptr;

Window::~Window() {
    shutDown();
}

static const char* s_title = "";

int Window::initialize(
    const int width,
    const int height,
    const char* title
) {
    s_title = title;
    initializeGLFW(width, height);
    initializeGLEW();

    gl::Graphics::initialize();

    return 0;
}

void Window::addGame(std::unique_ptr<Game> game) {
    game_ = std::move(game);
}

bool Window::isActive() {
    return !glfwWindowShouldClose(window_);
}

static double s_current_time = glfwGetTime();
static double s_last_time = s_current_time;

void Window::update() {
    s_current_time = glfwGetTime();
    // Calculate delta time
    float delta_time = s_current_time - s_last_time;
    game_->update(delta_time);

    display();

    glfwSwapBuffers(window_);
    glfwPollEvents();
    displayFrameRate();
    s_last_time = s_current_time;
}

void Window::display() {
    if (!game_) {
        debug::error("No game instance available for rendering.");
        return;
    }
    glViewport(0, 0, width_, height_);
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, width_, height_);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    game_->draw();
}

void Window::shutDown() {
    if (window_) {
        glfwDestroyWindow(window_);
    }
    gl::Graphics::tearDown();
    glfwTerminate();
}


void Window::hideMouse() {
    cursor_visible_ = false;
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::showMouse() {
    cursor_visible_ = true;
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

glm::vec2 Window::getSize() {
    return glm::vec2(width_, height_);
}


float Window::getAspectRatio() {
    return aspect_ratio_;
}

bool Window::isCursorVisible() {
    return cursor_visible_;
}

double Window::getCurrentTime() {
    return s_current_time;
}

int Window::initializeGLFW(int width, int height) {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#ifdef __APPLE__
     glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE); // Disable Retina scaling
#endif

    window_ = glfwCreateWindow(width, height, s_title, nullptr, nullptr);

    if (!window_) {
        const char* error_message;
        int error_code = glfwGetError(&error_message);
        debug::error("Failed to create GLFW window: {}",  error_message);
        glfwTerminate();
        return error_code;
    }
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);

    glfwSetKeyCallback(window_, keyboardCallback);
    glfwSetCursorPosCallback(window_, mousePositionCallback);
    glfwSetMouseButtonCallback(window_, mouseButtonCallback);
    glfwSetScrollCallback(window_, scrollCallback);
    glfwSetWindowSizeCallback(window_, resizeWindowCallback);

    glfwSetInputMode(window_, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Set initial variables
    glfwGetWindowSize(window_, &width_, &height_);
    aspect_ratio_ = static_cast<float>(width_) / static_cast<float>(height_);

    return 1;
}

int Window::initializeGLEW() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        debug::error("GLEW Initialization Failed\n");
        return -1;
    }
    return 0;
}


void Window::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    game_->keyEvent(key, action);
}



void Window::mousePositionCallback(GLFWwindow* window, const double x_pos, const double y_pos) {
    game_->mousePositionEvent(x_pos, y_pos);
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    game_->mouseButtonEvent(button, action);
}

void Window::scrollCallback(GLFWwindow* window, double x_offset, double y_offset) {
    game_->mouseScrolledEvent(x_offset, y_offset);
}

void Window::resizeWindowCallback(GLFWwindow* window, int width, int height) {
    width_ = width;
    height_ = height;
    aspect_ratio_ = static_cast<float>(width_) / static_cast<float>(height_);
    glViewport(0, 0, width_, height_);

    game_->resizeWindowEvent(width_, height_);
}

static int s_num_frames = 0;
static double s_previous_frame_time = s_last_time;

void Window::displayFrameRate() {
    s_num_frames++;
    // Calculate FPS
    if (s_current_time - s_previous_frame_time >= 1.0) {
        // If last update was more than 1 second ago
        const int fps = static_cast<int>(static_cast<double>(s_num_frames) / (s_current_time - s_previous_frame_time));
        std::stringstream ss;
        ss << s_title << " | FPS: " << fps;
        glfwSetWindowTitle(window_, ss.str().c_str());

        s_num_frames = 0;
        s_previous_frame_time = s_current_time;
    }
}
