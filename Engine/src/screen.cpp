#include <Engine/screen.h>
#include <Engine/window.h>
#include <Engine/Graphics/graphics.h>
#include <Engine/Systems/drawsystem.h>
#include <Engine/Audio/audio-engine.h>

#include <GLFW/glfw3.h>
#include <iostream>

// struct Rect {
//     float x, y, w, h;
// };

Screen::Screen() {
    cam = std::make_unique<gl::Camera>();
    cam->setPosition(glm::vec3(0.0f, cam->getHeight(), 0.0f));

    background = gl::Mesh::getLoadedShape("cube");
    tb.setPosition(glm::vec3(0.0f, 0.0f, 10.0f));
    tb.setScale(glm::vec3(40.0f, 40.0f, 1.0f));
    tb.setRotation(glm::mat4(1.0f));
    background_mat.textures = gl::Material::loadTexture("resources/images/magic.png");

    //Rect grid[3][3];
    initPuzzleGrid();

    //skull_mesh = gl::Mesh::loadStaticMesh("resources/models/Skull/12140_Skull_v3_L2.obj");
}

void Screen::setCamPos(glm::vec3 pos) {
    cam_pos = pos;
}

void Screen::draw() {
    std::cout << "DRAW CALLED\n";
    if (type_ == ScreenType::MAINMENU) {
        Window::showMouse();
        std::cout << "Draw main\n";
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
    } else if (type_ == ScreenType::PUZZLE1) {
        Window::showMouse();
        drawPuzzle1();
    } else {
        Window::hideMouse();
        std::cout << "draw game\n";
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
                    std::cout << "ENTER PRESSED\n";
                    switchScreen = true;
                    type_ = ScreenType::GAME;
                    switchScreen = false;
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
                    // TODO: add being near puzzles --> switch screen to appropiate puzzle
                    else if (puzzle1_done == false && cam_pos.y == 0.75f && cam_pos.x < 29.6f && cam_pos.x > 25.0971f
                             && cam_pos.z < 30.6541f && cam_pos.z > 24.6f) {
                        switchScreen = true;
                        type_ = ScreenType::PUZZLE1;
                        Window::showMouse();
                    }
                    else if (puzzle2_done == false && cam_pos.y == 0.75f && cam_pos.x < 29.4851f && cam_pos.x > 24.5941f
                             && cam_pos.z < -19.1622f && cam_pos.z > -25.5f) {
                        switchScreen = true;
                        type_ = ScreenType::PUZZLE2;
                        Window::showMouse();
                    }
                    else if (puzzle3_done == false && cam_pos.y == 0.75f && cam_pos.x < 37.2276f && cam_pos.x > 32.9495f
                             && cam_pos.z < 15.5551f && cam_pos.z > 8.86806f) {
                        switchScreen = true;
                        type_ = ScreenType::PUZZLE3;
                        Window::showMouse();
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
            case ScreenType::PUZZLE1:
                if (key == GLFW_KEY_R && action == GLFW_PRESS) {
                    initPuzzleGrid(); // reset puzzle
                    return;
                }

                if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                    switchScreen = true;
                    type_ = ScreenType::GAME;
                    Window::hideMouse();
                    return;
                }

                break;
            case ScreenType::PUZZLE2:

                break;
            case ScreenType::PUZZLE3:

                break;

        }
    }
}


bool inside(Screen::Rect r, double x, double y) {
    return x >= r.x && x <= r.x + r.w &&
           y <= r.y && y >= r.y - r.h;
}

void Screen::setMousePos(glm::vec2 pos) {
    mouse_pos = pos;
}

void Screen::toggleCell(int i, int j) {
    const int N = 5;

    if (i < 0 || i >= N || j < 0 || j >= N)
        return;

    puzzle_grid[i][j] = !puzzle_grid[i][j];
}

void Screen::mouseButtonEvent(int button, int action) {
    //if (type_ != ScreenType::PUZZLE1) return;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

        double mx = mouse_pos.x;
        double my = mouse_pos.y;

        const int N = 5;

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {

                Rect r = grid[i][j];

                if (mx >= r.x && mx <= r.x + r.w &&
                    my <= r.y && my >= r.y - r.h) {

                    // Lights Out rule:
                    toggleCell(i, j);       // self
                    toggleCell(i + 1, j);   // down
                    toggleCell(i - 1, j);   // up
                    toggleCell(i, j + 1);   // right
                    toggleCell(i, j - 1);   // left

                    last_clicked_row = i;
                    last_clicked_col = j;

                    return;
                }
            }
        }
    }

}

void Screen::mousePositionEvent(double x_pos, double y_pos) {

}

void Screen::mouseScrolledEvent(double x_offset, double y_offset) {

}

void Screen::resizeWindowEvent(int new_width, int new_height) {
    initPuzzleGrid();
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
    // Near puzzle 1
    else if (puzzle1_done == false && cam_pos.y == 0.75f && cam_pos.x < 29.6f && cam_pos.x > 25.0971f
             && cam_pos.z < 30.6541f && cam_pos.z > 24.6f) {
        gl::Graphics::drawText("Press T to interact",
                               glm::vec2(center.x, center.y + 320), 32.0f,
                               glm::vec3(1.0f, 0.416f, 0.416f), gl::TextAlign::CENTER);
    }
    // Near puzzle 2
    else if (puzzle2_done == false && cam_pos.y == 0.75f && cam_pos.x < 29.4851f && cam_pos.x > 24.5941f
             && cam_pos.z < -19.1622f && cam_pos.z > -25.5f) {
        gl::Graphics::drawText("Press T to interact",
                               glm::vec2(center.x, center.y + 320), 32.0f,
                               glm::vec3(1.0f, 0.416f, 0.416f), gl::TextAlign::CENTER);
    }
    // Near puzzle 3
    else if (puzzle3_done == false && cam_pos.y == 0.75f && cam_pos.x < 37.2276f && cam_pos.x > 32.9495f
             && cam_pos.z < 15.5551f && cam_pos.z > 8.86806f) {
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

void Screen::initPuzzleGrid() {
    glm::vec2 size = Window::getSize();

    const int N = 5;

    float cellW = 120.0f;
    float cellH = 120.0f;
    float spacing = 20.0f;

    float totalW = N * cellW + (N - 1) * spacing;
    float totalH = N * cellH + (N - 1) * spacing;

    float startX = (size.x - totalW) / 2.0f;
    float startY = (size.y + totalH) / 2.0f;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            grid[i][j] = {
                startX + j * (cellW + spacing),
                startY - i * (cellH + spacing),
                cellW,
                cellH
            };
            puzzle_grid[i][j] = false;
        }
    }
    gridBottomY = (size.y / 2.0f - totalH / 2.0f);

    puzzle_grid[0][0] = true;
    puzzle_grid[1][1] = true;
    puzzle_grid[1][0] = true;

    puzzle_grid[0][4] = true;
    puzzle_grid[0][3] = true;
    puzzle_grid[1][4] = true;

    puzzle_grid[3][1] = true;
    puzzle_grid[4][1] = true;

}

bool Screen::checkWin1() {
    const int N = 5;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (puzzle_grid[i][j])
                return false;
        }
    }
    return true;
}

void Screen::drawPuzzle1() {
    glm::vec2 size = Window::getSize();

    gl::Graphics::useTextShader();

    // draw grid squares
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {

            Rect r = grid[i][j];

            std::string label = puzzle_grid[i][j] ? "ON" : "OFF";

            gl::Graphics::drawText(
                label,
                glm::vec2(r.x + r.w/2, r.y - r.h/2),
                32.0f,
                puzzle_grid[i][j] ? glm::vec3(0.2f, 1.0f, 0.2f)
                                  : glm::vec3(1.0f, 1.0f, 1.0f),
                gl::TextAlign::CENTER
                );
        }
    }

    // bottom message
    bool won = checkWin1();

    std::string msg;

    if (won) {
        incrementGems();
        setPuzzleBool(1,true);
        msg = "YOU WON A GEM!";
    }
    // else if (last_clicked_row != -1) {
    //     msg = "Clicked: (" +
    //           std::to_string(last_clicked_row) + "," +
    //           std::to_string(last_clicked_col) + ")";
    // }
    else {
        msg = "Turn off the lights!";
    }
    gl::Graphics::drawText(
        msg,
        glm::vec2(size.x / 2, 100.0f),
        40.0f,
        won ? glm::vec3(0.2f, 1.0f, 0.2f)
            : glm::vec3(1.0f),
        gl::TextAlign::CENTER
        );

    //glm::vec2 size = Window::getSize();

    gl::Graphics::drawText(
        "R = Restart   |   ESC = Exit Puzzle",
        glm::vec2(size.x / 2, gridBottomY - 120.0f),
        28.0f,
        glm::vec3(1.0f),
        gl::TextAlign::CENTER
        );
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

void Screen::setPuzzleBool(int puzzle_num, bool val) {
    if (puzzle_num == 1) {
        puzzle1_done = val;
    } else if (puzzle_num == 2) {
        puzzle2_done = val;
    } else if (puzzle_num == 3) {
        puzzle3_done = val;
    }
}

bool Screen::getPuzzleBool(int puzzle_num) {
    if (puzzle_num == 1) {
        return puzzle1_done;
    } else if (puzzle_num == 2) {
        return puzzle2_done;
    } else if (puzzle_num == 3) {
        return puzzle3_done;
    }
    return false;
}
