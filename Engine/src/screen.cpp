#include <Engine/screen.h>
#include <Engine/window.h>
#include <Engine/Graphics/graphics.h>

#include <GLFW/glfw3.h>

Screen::Screen() {
    cam = std::make_unique<gl::Camera>();
    cam->setPosition(glm::vec3(0.0f, cam->getHeight(), 0.0f));

    background = gl::Mesh::getLoadedShape("cube");
    tb.setPosition(glm::vec3(0.0f, 0.0f, 10.0f));
    tb.setScale(glm::vec3(40.0f, 40.0f, 1.0f));
    tb.setRotation(glm::mat4(1.0f));
    background_mat.textures = gl::Material::loadTexture("resources/images/magic.png");

    //skull_mesh = gl::Mesh::loadStaticMesh("resources/models/Skull/12140_Skull_v3_L2.obj");
}

void Screen::draw() {
    if (type_ == ScreenType::MAINMENU) {
        Window::showMouse();
        drawMainMenu();
    } else if (type_ == ScreenType::PAUSE) {
        Window::showMouse();
        drawPauseScreen();
    } else if (type_ == ScreenType::WIN) {
        Window::showMouse();
        drawWinScreen();
    } else if (type_ == ScreenType::LOSE) {
        Window::showMouse();
        drawLoseScreen();
    } else {
        drawGameScreen();
        Window::hideMouse();
    }
}

void Screen::update(double delta_time) {

}

void Screen::keyEvent(int key, int action) {
    // Deal with showing mouse for game immersion
    if (action == GLFW_PRESS) {
        switch(type_) {
            case ScreenType::MAINMENU:
                if (key == GLFW_KEY_ENTER) {
                    switchScreen = true;
                    type_ = ScreenType::GAME;
                    Window::hideMouse();
                }
                break;
            case ScreenType::PAUSE:
                if (key == GLFW_KEY_ESCAPE) {
                    switchScreen = true;
                    type_ = ScreenType::GAME;
                    Window::hideMouse();
                }
                break;
            case ScreenType::GAME:
                if (key == GLFW_KEY_ESCAPE) {
                    switchScreen = true;
                    type_ = ScreenType::PAUSE;
                    Window::showMouse();
                }
                break;
            case ScreenType::WIN:
                if (key == GLFW_KEY_ENTER) {
                    switchScreen = true;
                    type_ = ScreenType::GAME;
                    Window::hideMouse();
                }
                break;
            case ScreenType::LOSE:
                if (key == GLFW_KEY_ENTER) {
                    switchScreen = true;
                    type_ = ScreenType::GAME;
                    Window::hideMouse();
                }
                break;
        }
    }
}

void Screen::mouseButtonEvent(int button, int action) {

}

void Screen::mousePositionEvent(double x_pos, double y_pos) {

}

void Screen::mouseScrolledEvent(double x_offset, double y_offset) {

}

void Screen::resizeWindowEvent(int new_width, int new_height) {

}

ScreenType Screen::getType() {
    return type_;
}

void Screen::setType(ScreenType type) {
    type_ = type;
}

void Screen::drawMainMenu() {
    drawGivenShape(background, tb, background_mat);

    // Text
    gl::Graphics::useTextShader();
    glm::vec2 size = Window::getSize();
    glm::vec2 center = size/2.f;
    gl::Graphics::drawText("Welcome to Magic Escape!",
                           glm::vec2(center.x, center.y - 200.0f), 48.0f,
                           glm::vec3(0.561f, 1.0f, 0.651f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Solve puzzles to escape from the wizard's dungeon",
                           glm::vec2(center.x, center.y - 140.0f), 48.0f,
                           glm::vec3(0.561f, 1.0f, 0.651f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Happy Solving!",
                           glm::vec2(center.x, center.y - 80.0f), 48.0f,
                           glm::vec3(0.561f, 1.0f, 0.651f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Press ENTER to play",
                           glm::vec2(center.x, center.y + 140.0f), 48.0f,
                           glm::vec3(0.188f, 0.749f, 0.278f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Press ESC to pause & see controls",
                           glm::vec2(center.x, center.y + 200.0f), 48.0f,
                           glm::vec3(0.188f, 0.749f, 0.278f), gl::TextAlign::CENTER);
}

void Screen::drawPauseScreen() {
    drawGivenShape(background, tb, background_mat);

    gl::Graphics::useTextShader();
    glm::vec2 center = Window::getSize()/2.f;
    gl::Graphics::drawText("Paused",
                           glm::vec2(center.x, center.y - 80.0f), 48.0f,
                           glm::vec3(0.561, 0.0f, 1.0f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("ESC = Resume Game",
                           glm::vec2(center.x, center.y - 20.0f), 48.0f,
                           glm::vec3(1.0f, 0.541f, 0.953f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("MOUSE = Look Around",
                           glm::vec2(center.x, center.y + 40.0f), 48.0f,
                          glm::vec3(1.0f, 0.541f, 0.953f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("WASD = Move Around",
                           glm::vec2(center.x, center.y + 100.0f), 48.0f,
                           glm::vec3(1.0f, 0.541f, 0.953f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("SPACE = Jump",
                           glm::vec2(center.x, center.y + 160.0f), 48.0f,
                           glm::vec3(1.0f, 0.541f, 0.953f), gl::TextAlign::CENTER);
}

void Screen::drawWinScreen() {
    // Text
    gl::Graphics::useTextShader();
    glm::vec2 center = Window::getSize()/2.f;
    gl::Graphics::drawText("YOU WIN!",
                           glm::vec2(center.x, center.y - 20.0f), 48.0f,
                           glm::vec3(0.0f, 0.0f, 1.0f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("You collected " + std::to_string(prev_coins_) + " coins",
                           glm::vec2(center.x, center.y + 40.0f), 48.0f,
                           glm::vec3(0.322f, 0.784f, 1.0f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Press ENTER to play again",
                           glm::vec2(center.x, center.y + 100.0f), 48.0f,
                           glm::vec3(0.322f, 0.784f, 1.0f), gl::TextAlign::CENTER);
}

void Screen::drawLoseScreen() {
    // Text
    gl::Graphics::useTextShader();
    glm::vec2 center = Window::getSize()/2.f;
    gl::Graphics::drawText("GAME OVER",
                           glm::vec2(center.x, center.y - 20.0f), 48.0f,
                           glm::vec3(1.0f, 0.0f, 0.0f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Press ENTER to try again",
                           glm::vec2(center.x, center.y + 40.0f), 48.0f,
                           glm::vec3(1.0f, 0.416f, 0.416f), gl::TextAlign::CENTER);
}

void Screen::drawGameScreen() {
    gl::Graphics::useTextShader();
    /*gl::Graphics::drawText("Coins Collected: " + std::to_string(coins_),
                           glm::vec2(20.0f, 40.0f), 32.0f,
                           glm::vec3(0.0f, 0.0f, 0.0f), gl::TextAlign::LEFT);*/
}

void Screen::incrementCoins() {
    coins_++;
}

void Screen::resetCoins() {
    prev_coins_ = coins_;
    coins_ = 0;
}

void Screen::drawGivenShape(gl::DrawShape* shape, Transform t, gl::DrawMaterial mat) {
    gl::Graphics::usePhongShader();
    gl::Graphics::setAmbientLight(glm::vec3(0.5f));
    gl::Graphics::setCameraUniforms(cam.get());
    gl::Graphics::drawObject(shape, t, mat);
}

