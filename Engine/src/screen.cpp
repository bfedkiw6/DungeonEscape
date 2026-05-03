#include <Engine/screen.h>
#include <Engine/window.h>
#include <Engine/Graphics/graphics.h>
#include <Engine/Systems/drawsystem.h>
#include <Engine/Audio/audio-engine.h>

#include <GLFW/glfw3.h>
#include <iostream>

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

void Screen::setCamPos(glm::vec3 pos) {
    cam_pos = pos;
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
    } else if (type_ == ScreenType::GUARD) {
        Window::hideMouse();
        drawGuardDialogue();
    } else if (type_ == ScreenType::DOOR) {
        Window::hideMouse();
        drawDoorDialogue();
    } else {
        Window::hideMouse();
        drawNearbyDialogue();
        drawGameScreen();
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
                else if (key == GLFW_KEY_T) {
                    // Player needs to be close enough to guard for dialogue to trigger
                    if (cam_pos.y == 0.75f && cam_pos.x < -12.0f && cam_pos.x > -16.0f
                        && cam_pos.z < 18.0f && cam_pos.z > 14.0f) {
                        gl::AudioEngine::playSound("guard_hey", 0.45f);
                        switchScreen = true;
                        type_ = ScreenType::GUARD;
                        Window::hideMouse();
                    }
                    // Door dialogue proximity
                    else if (cam_pos.y == 0.75f && cam_pos.x < -12.0696f && cam_pos.x > -16.163f
                        && cam_pos.z < 6.32462f && cam_pos.z > -1.02483f) {
                        gl::AudioEngine::playSound("door", 0.6f);
                        switchScreen = true;
                        type_ = ScreenType::DOOR;
                        Window::hideMouse();
                    }
                }
                break;
            case ScreenType::WIN:
                if (key == GLFW_KEY_ENTER) {
                    should_reset = true;
                    switchScreen = true;
                    type_ = ScreenType::MAINMENU;
                }
                break;
            case ScreenType::LOSE:
                if (key == GLFW_KEY_ENTER) {
                    switchScreen = true;
                    type_ = ScreenType::GAME;
                    Window::hideMouse();
                }
                break;
            case ScreenType::GUARD:
                if (key == GLFW_KEY_ENTER) {
                    if (guard_dg_num == 1) {
                        gl::AudioEngine::playSound("guard_grunt", 0.4f);
                    }
                    if (guard_dg_num == 3) {
                        gl::AudioEngine::playSound("guard_grunt", 0.4f);
                        guard_dg_num = 0;
                        switchScreen = true;
                        type_ = ScreenType::GAME;
                        Window::hideMouse();
                    } else {
                        // Progress dialogue
                        guard_dg_num++;
                    }
                }
                break;
            case ScreenType::DOOR:
                if (key == GLFW_KEY_ENTER) {
                    if (gems_ == 3) {
                        gl::AudioEngine::playSound("yay", 0.1f);
                        gl::AudioEngine::playSound("applause", 0.4f);
                        switchScreen = true;
                        type_ = ScreenType::WIN;
                    } else if (door_dg_num == 2) {
                        door_dg_num = 0;
                        switchScreen = true;
                        type_ = ScreenType::GAME;
                        Window::hideMouse();
                    } else {
                        // Progress dialogue
                        door_dg_num++;
                    }
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
    gl::Graphics::drawText("You've woken up in a wizard's dungeon with",
                           glm::vec2(center.x, center.y - 200.0f), 48.0f,
                           glm::vec3(0.322f, 0.784f, 1.0f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("no memory. Explore your surroundings,",
                           glm::vec2(center.x, center.y - 140.0f), 48.0f,
                           glm::vec3(0.322f, 0.784f, 1.0f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("maybe there is some way out...",
                           glm::vec2(center.x, center.y - 80.0f), 48.0f,
                           glm::vec3(0.322f, 0.784f, 1.0f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Press ENTER to play",
                           glm::vec2(center.x, center.y + 140.0f), 48.0f,
                           glm::vec3(0.68f, 0.957f, 1.0f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Press T to interact",
                           glm::vec2(center.x, center.y + 200.0f), 48.0f,
                           glm::vec3(0.68f, 0.957f, 1.0f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Press ESC to pause & see controls",
                           glm::vec2(center.x, center.y + 260.0f), 48.0f,
                           glm::vec3(0.68f, 0.957f, 1.0f), gl::TextAlign::CENTER);
}


void Screen::drawPauseScreen() {
    drawGivenShape(background, tb, background_mat);

    gl::Graphics::useTextShader();
    glm::vec2 center = Window::getSize()/2.f;
    gl::Graphics::drawText("Paused",
                           glm::vec2(center.x, center.y - 140.0f), 48.0f,
                           glm::vec3(0.561, 0.0f, 1.0f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("ESC = Resume Game",
                           glm::vec2(center.x, center.y - 80.0f), 48.0f,
                           glm::vec3(1.0f, 0.541f, 0.953f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("MOUSE = Look Around",
                           glm::vec2(center.x, center.y - 20.0f), 48.0f,
                          glm::vec3(1.0f, 0.541f, 0.953f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("WASD = Move Around",
                           glm::vec2(center.x, center.y + 40.0f), 48.0f,
                           glm::vec3(1.0f, 0.541f, 0.953f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("SPACE = Jump",
                           glm::vec2(center.x, center.y + 100.0f), 48.0f,
                           glm::vec3(1.0f, 0.541f, 0.953f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("T = Interact",
                           glm::vec2(center.x, center.y + 160.0f), 48.0f,
                           glm::vec3(1.0f, 0.541f, 0.953f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Press EQUAL for a hard reset",
                           glm::vec2(center.x, center.y + 220.0f), 48.0f,
                           glm::vec3(1.0f, 0.541f, 0.953f), gl::TextAlign::CENTER);
}

void Screen::drawWinScreen() {
    drawGivenShape(background, tb, background_mat);
    // Text
    gl::Graphics::useTextShader();
    glm::vec2 center = Window::getSize()/2.f;
    gl::Graphics::drawText("YOU WIN!",
                           glm::vec2(center.x, center.y - 80.0f), 48.0f,
                           glm::vec3(0.188f, 0.749f, 0.278f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("You collected all the gems!",
                           glm::vec2(center.x, center.y - 20.0f), 48.0f,
                           glm::vec3(0.561f, 1.0f, 0.651f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Thanks for playing :)",
                           glm::vec2(center.x, center.y + 40.0f), 48.0f,
                           glm::vec3(0.561f, 1.0f, 0.651f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Press ENTER to play again!",
                           glm::vec2(center.x, center.y + 100.0f), 48.0f,
                           glm::vec3(0.188f, 0.749f, 0.278f), gl::TextAlign::CENTER);
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
    gl::Graphics::drawText("Gems Collected: " + std::to_string(gems_),
                           glm::vec2(20.0f, 40.0f), 32.0f,
                           glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::LEFT);

}

void Screen::drawGuardDialogue() {
    gl::Graphics::useTextShader();
    glm::vec2 center = Window::getSize()/2.f;

    if (guard_dg_num == 0) {
        gl::Graphics::drawText("HEY YOU! I've seemed to misplaced the door key.",
                               glm::vec2(center.x, center.y - 320.0f), 30.0f,
                               glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER);
    } else if (guard_dg_num == 1) {
        gl::Graphics::drawText("Go collect the three magic gems around the dungeon so we can be free.",
                               glm::vec2(center.x, center.y - 320.0f), 30.0f,
                               glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER);
    } else if (guard_dg_num == 2) {
        gl::Graphics::drawText("The gems are guarded by puzzles hidden around.",
                               glm::vec2(center.x, center.y - 320.0f), 30.0f,
                               glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER);
    } else if (guard_dg_num == 3) {
        gl::Graphics::drawText("The magic particles may point you in the right direction!",
                               glm::vec2(center.x, center.y - 320.0f), 30.0f,
                               glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER);
    }
    gl::Graphics::drawText("Press ENTER to continue",
                           glm::vec2(center.x, center.y - 280.0f), 25.0f,
                           glm::vec3(1.0f, 0.416f, 0.416f), gl::TextAlign::CENTER);
}

void Screen::drawDoorDialogue() {
    gl::Graphics::useTextShader();
    glm::vec2 center = Window::getSize()/2.f;

    if (gems_ != 3) {
        if (door_dg_num == 0) {
            gl::Graphics::drawText("Here's the door where I should enter in the gems.",
                                   glm::vec2(center.x, center.y - 320.0f), 30.0f,
                                   glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER);
        } else if (door_dg_num == 1) {
            gl::Graphics::drawText("I only have " + std::to_string(gems_) + " out of 3.",
                                   glm::vec2(center.x, center.y - 320.0f), 30.0f,
                                   glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER);
        } else if (door_dg_num == 2) {
            gl::Graphics::drawText("I should come back when I have found all of them.",
                                   glm::vec2(center.x, center.y - 320.0f), 30.0f,
                                   glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER);
        }
        gl::Graphics::drawText("Press ENTER to continue",
                               glm::vec2(center.x, center.y - 280.0f), 25.0f,
                               glm::vec3(1.0f, 0.416f, 0.416f), gl::TextAlign::CENTER);
    } else {
        gl::Graphics::drawText("I've found all the gems, we can escape!",
                               glm::vec2(center.x, center.y - 320.0f), 30.0f,
                               glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER);
        gl::Graphics::drawText("Press ENTER to continue",
                               glm::vec2(center.x, center.y - 280.0f), 25.0f,
                               glm::vec3(1.0f, 0.416f, 0.416f), gl::TextAlign::CENTER);
    }
}

void Screen::drawNearbyDialogue() {
    gl::Graphics::useTextShader();
    glm::vec2 center = Window::getSize()/2.f;

    // Near guard
    if (cam_pos.y == 0.75f && cam_pos.x < -12.0f && cam_pos.x > -16.0f
        && cam_pos.z < 18.0f && cam_pos.z > 14.0f) {
        gl::Graphics::drawText("Press T to interact",
                               glm::vec2(center.x, center.y + 320), 32.0f,
                               glm::vec3(1.0f, 0.416f, 0.416f), gl::TextAlign::CENTER);
    }
    // Near exit door
    else if (cam_pos.y == 0.75f && cam_pos.x < -12.0696f && cam_pos.x > -16.163f
               && cam_pos.z < 6.32462f && cam_pos.z > -1.02483f) {
        gl::Graphics::drawText("Press T to interact",
                               glm::vec2(center.x, center.y + 320), 32.0f,
                               glm::vec3(1.0f, 0.416f, 0.416f), gl::TextAlign::CENTER);
    }
}

void Screen::drawGivenShape(gl::DrawShape* shape, Transform t, gl::DrawMaterial mat) {
    gl::Graphics::usePhongShader();
    gl::Graphics::setAmbientLight(glm::vec3(0.5f));
    gl::Graphics::setCameraUniforms(cam.get());
    gl::Graphics::drawObject(shape, t, mat);
}

void Screen::incrementGems() {
    gems_++;
}

void Screen::resetGems() {
    prev_gems_ = gems_;
    gems_ = 0;
}

bool Screen::shouldReset() {
    return should_reset;
}

void Screen::clearShouldReset() {
    should_reset = false;
}
