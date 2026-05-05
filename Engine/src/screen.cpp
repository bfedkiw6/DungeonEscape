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

    initPuzzleGrid();
    initWordPuzzle();
    initPotionPuzzle();


    //skull_mesh = gl::Mesh::loadStaticMesh("resources/models/Skull/12140_Skull_v3_L2.obj");
}

void Screen::setCamPos(glm::vec3 pos) {
    cam_pos = pos;
}

void Screen::draw() {
    if (type_ == ScreenType::MAINMENU) {
        Window::showMouse();
        //std::cout << "Draw main\n";
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
    } else if (type_ == ScreenType::PUZZLE2) {
        Window::showMouse();
        drawWordPuzzle();
    } else if (type_ == ScreenType::PUZZLE3) {
        Window::showMouse();
        drawPotionPuzzle();
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
                if (key == GLFW_KEY_R && action == GLFW_PRESS && !puzzle1_done) {
                    initPuzzleGrid(); // Reset puzzle
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
                if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                    switchScreen = true;
                    type_ = ScreenType::GAME;
                    Window::hideMouse();
                    return;
                }
                break;
            case ScreenType::PUZZLE3:
                if (key == GLFW_KEY_R && action == GLFW_PRESS && !puzzle3_done) {
                    initPotionPuzzle(); // reset puzzle
                    return;
                }
                if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                    switchScreen = true;
                    type_ = ScreenType::GAME;
                    Window::hideMouse();
                    return;
                }
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

glm::vec3 clampColor(glm::vec3 c) {
    return glm::clamp(c, 0.0f, 1.0f);
}

void Screen::toggleCell(int i, int j) {
    const int N = 5;

    if (i < 0 || i >= N || j < 0 || j >= N)
        return;

    puzzle_grid[i][j] = !puzzle_grid[i][j];
}

void Screen::mouseButtonEvent(int button, int action) {
    if (type_ == ScreenType::PUZZLE1){
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !puzzle1_done) {
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
    if (type_ == ScreenType::PUZZLE2){
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !puzzle2_done) {
            double mx = mouse_pos.x;
            double my = mouse_pos.y;
            const int N = 11;

            for (int i = 0; i < N; i++) {
                if (letterUsed[i]) continue;
                Rect r = letterRects[i];

                if (mx >= r.x && mx <= r.x + r.w && my <= r.y && my >= r.y - r.h) {
                    currentWord += letterGrid[i];
                    letterUsed[i] = true;

                    //WIN CHECK
                    if (currentWord == targetWord) {
                        wordPuzzleWon = true;
                        incrementGems();
                        puzzle2_done = true;
                        gl::AudioEngine::playSound("yay", 0.3f);
                        return;
                    }
                    //AUTO RESET CONDITION
                    if (!wordPuzzleWon && currentWord.length() >= targetWord.length()) {
                        currentWord = "";
                        for (int j = 0; j < N; j++) {
                            letterUsed[j] = false;
                        }
                        gl::AudioEngine::playSound("boo", 0.6f);
                    }
                    return;
                }
            }
        }
    }

    if (type_ == ScreenType::PUZZLE3){
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !puzzle3_done) {
            double mx = mouse_pos.x;
            double my = mouse_pos.y;

            const int N = 3;
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {

                    Rect r = ingredientRects[i][j];

                    if (mx >= r.x && mx <= r.x + r.w &&
                        my <= r.y && my >= r.y - r.h) {

                        selected[i][j] = true;
                        glm::vec3 newColor = ingredientColors[i][j];
                        float count = static_cast<float>(mixCount);
                        currentPotion = (currentPotion * count + newColor) / (count + 1.0f);
                        mixCount++;
                        return;
                    }
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
    if (!puzzle1_done) {
        initPuzzleGrid();
    }
    if (!puzzle2_done) {
        initWordPuzzle();
    }
    if (!puzzle3_done) {
       initPotionPuzzle();
    }
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
    gl::Graphics::drawText("Gembound",
                           glm::vec2(center.x, center.y - 280.0f), 100.0f,
                           glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER, "magic");
    gl::Graphics::drawText("You've woken up in a wizard's dungeon with",
                           glm::vec2(center.x, center.y - 100.0f), 70.0f,
                           glm::vec3(0.322f, 0.784f, 1.0f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("no memory. Explore your surroundings,",
                           glm::vec2(center.x, center.y - 30.0f), 70.0f,
                           glm::vec3(0.322f, 0.784f, 1.0f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("maybe there is some way out...",
                           glm::vec2(center.x, center.y + 40.0f), 70.0f,
                           glm::vec3(0.322f, 0.784f, 1.0f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Press ENTER to play",
                           glm::vec2(center.x, center.y + 220.0f), 60.0f,
                           glm::vec3(0.68f, 0.957f, 1.0f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Press T to interact",
                           glm::vec2(center.x, center.y + 280.0f), 60.0f,
                           glm::vec3(0.68f, 0.957f, 1.0f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Press ESC to pause & see controls",
                           glm::vec2(center.x, center.y + 340.0f), 60.0f,
                           glm::vec3(0.68f, 0.957f, 1.0f), gl::TextAlign::CENTER);
}


void Screen::drawPauseScreen() {
    drawGivenShape(background, tb, background_mat);

    gl::Graphics::useTextShader();
    glm::vec2 center = Window::getSize()/2.f;
    gl::Graphics::drawText("Paused",
                           glm::vec2(center.x, center.y - 140.0f), 100.0f,
                           glm::vec3(0.678f, 0.42f, 0.988f), gl::TextAlign::CENTER, "magic");
    gl::Graphics::drawText("ESC = Resume Game",
                           glm::vec2(center.x, center.y + 40.0f), 70.0f,
                           glm::vec3(1.0f, 0.541f, 0.953f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("MOUSE = Look Around",
                           glm::vec2(center.x, center.y + 110.0f), 70.0f,
                          glm::vec3(1.0f, 0.541f, 0.953f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("WASD = Move Around",
                           glm::vec2(center.x, center.y + 180.0f), 70.0f,
                           glm::vec3(1.0f, 0.541f, 0.953f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("SPACE = Jump",
                           glm::vec2(center.x, center.y + 250.0f), 70.0f,
                           glm::vec3(1.0f, 0.541f, 0.953f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("T = Interact",
                           glm::vec2(center.x, center.y + 320.0f), 70.0f,
                           glm::vec3(1.0f, 0.541f, 0.953f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Press EQUAL for a hard reset",
                           glm::vec2(center.x, center.y + 390.0f), 70.0f,
                           glm::vec3(1.0f, 0.541f, 0.953f), gl::TextAlign::CENTER);
}

void Screen::drawWinScreen() {
    drawGivenShape(background, tb, background_mat);
    // Text
    gl::Graphics::useTextShader();
    glm::vec2 center = Window::getSize()/2.f;
    gl::Graphics::drawText("Game by Briana Fedkiw & Veronika Grytsai",
                           glm::vec2(center.x, center.y - 400.0f), 60.0f,
                           glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Winner Winner!",
                           glm::vec2(center.x, center.y - 80.0f), 100.0f,
                           glm::vec3(0.188f, 0.749f, 0.278f), gl::TextAlign::CENTER, "magic");
    gl::Graphics::drawText("You collected all the gems like a true escapist",
                           glm::vec2(center.x, center.y + 20.0f), 90.0f,
                           glm::vec3(0.561f, 1.0f, 0.651f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Thanks for playing :)",
                           glm::vec2(center.x, center.y + 120.0f), 90.0f,
                           glm::vec3(0.561f, 1.0f, 0.651f), gl::TextAlign::CENTER);
    gl::Graphics::drawText("Press ENTER to play again",
                           glm::vec2(center.x, center.y + 400.0f), 50.0f,
                           glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER, "magic");
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
                           glm::vec2(20.0f, 60.0f), 60.0f,
                           glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::LEFT);

}

void Screen::drawGuardDialogue() {
    gl::Graphics::useTextShader();
    glm::vec2 center = Window::getSize()/2.f;

    if (guard_dg_num == 0) {
        gl::Graphics::drawText("HEY YOU! I've seemed to misplaced the door key.",
                               glm::vec2(center.x, center.y - 320.0f), 60.0f,
                               glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER);
    } else if (guard_dg_num == 1) {
        gl::Graphics::drawText("Go collect the three magic gems around the dungeon so we can be free.",
                               glm::vec2(center.x, center.y - 320.0f), 60.0f,
                               glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER);
    } else if (guard_dg_num == 2) {
        gl::Graphics::drawText("The gems are guarded by puzzles hidden around.",
                               glm::vec2(center.x, center.y - 320.0f), 60.0f,
                               glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER);
    } else if (guard_dg_num == 3) {
        gl::Graphics::drawText("The magic particles may point you in the right direction!",
                               glm::vec2(center.x, center.y - 320.0f), 60.0f,
                               glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER);
    }
    gl::Graphics::drawText("Press ENTER to continue",
                           glm::vec2(center.x, center.y - 260.0f), 50.0f,
                           glm::vec3(1.0f, 0.416f, 0.416f), gl::TextAlign::CENTER);
}

void Screen::drawDoorDialogue() {
    gl::Graphics::useTextShader();
    glm::vec2 center = Window::getSize()/2.f;

    if (gems_ != 3) {
        if (door_dg_num == 0) {
            gl::Graphics::drawText("Here's the door where I should enter in the gems.",
                                   glm::vec2(center.x, center.y - 320.0f), 60.0f,
                                   glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER);
        } else if (door_dg_num == 1) {
            gl::Graphics::drawText("I only have " + std::to_string(gems_) + " out of 3.",
                                   glm::vec2(center.x, center.y - 320.0f), 60.0f,
                                   glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER);
        } else if (door_dg_num == 2) {
            gl::Graphics::drawText("I should come back when I have found all of them.",
                                   glm::vec2(center.x, center.y - 320.0f), 60.0f,
                                   glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER);
        }
        gl::Graphics::drawText("Press ENTER to continue",
                               glm::vec2(center.x, center.y - 260.0f), 50.0f,
                               glm::vec3(1.0f, 0.416f, 0.416f), gl::TextAlign::CENTER);
    } else {
        gl::Graphics::drawText("I've found all the gems, we can escape!",
                               glm::vec2(center.x, center.y - 320.0f), 60.0f,
                               glm::vec3(1.0f, 1.0f, 1.0f), gl::TextAlign::CENTER);
        gl::Graphics::drawText("Press ENTER to continue",
                               glm::vec2(center.x, center.y - 260.0f), 50.0f,
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
                               glm::vec2(center.x, center.y + 320), 60.0f,
                               glm::vec3(1.0f, 0.416f, 0.416f), gl::TextAlign::CENTER);
    }
    // Near exit door
    else if (cam_pos.y == 0.75f && cam_pos.x < -12.0696f && cam_pos.x > -16.163f
               && cam_pos.z < 6.32462f && cam_pos.z > -1.02483f) {
        gl::Graphics::drawText("Press T to interact",
                               glm::vec2(center.x, center.y + 320), 60.0f,
                               glm::vec3(1.0f, 0.416f, 0.416f), gl::TextAlign::CENTER);
    }
    // Near puzzle 1
    else if (puzzle1_done == false && cam_pos.y == 0.75f && cam_pos.x < 29.6f && cam_pos.x > 25.0971f
             && cam_pos.z < 30.6541f && cam_pos.z > 24.6f) {
        gl::Graphics::drawText("Press T to interact",
                               glm::vec2(center.x, center.y + 320), 60.0f,
                               glm::vec3(1.0f, 0.416f, 0.416f), gl::TextAlign::CENTER);
    }
    // Near puzzle 2
    else if (puzzle2_done == false && cam_pos.y == 0.75f && cam_pos.x < 29.4851f && cam_pos.x > 24.5941f
             && cam_pos.z < -19.1622f && cam_pos.z > -25.5f) {
        gl::Graphics::drawText("Press T to interact",
                               glm::vec2(center.x, center.y + 320), 60.0f,
                               glm::vec3(1.0f, 0.416f, 0.416f), gl::TextAlign::CENTER);
    }
    // Near puzzle 3
    else if (puzzle3_done == false && cam_pos.y == 0.75f && cam_pos.x < 37.2276f && cam_pos.x > 32.9495f
             && cam_pos.z < 15.5551f && cam_pos.z > 8.86806f) {
        gl::Graphics::drawText("Press T to interact",
                               glm::vec2(center.x, center.y + 320), 60.0f,
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

    float cellW = 100.0f;
    float cellH = 100.0f;
    float spacing = 60.0f;

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
    glClearColor(0.569f, 0.467f, 0.322f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::vec2 size = Window::getSize();
    gl::Graphics::useTextShader();

    // draw grid squares
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            Rect r = grid[i][j];

            std::string label = puzzle_grid[i][j] ? "ON" : "OFF";

            gl::Graphics::drawText(label,
                                   glm::vec2(r.x + r.w/2, r.y - r.h/2), 50.0f,
                                   puzzle_grid[i][j] ? glm::vec3(0.0f, 1.0f, 0.0f)
                                                     : glm::vec3(0.0f, 0.0f, 0.0f),
                                   gl::TextAlign::CENTER);
        }
    }
    // bottom message
    bool won = checkWin1();
    std::string msg;
    if (won) {
        if (!puzzle1_done) {
            incrementGems();
            puzzle1_done = true;
            gl::AudioEngine::playSound("yay", 0.3f);
        }
        gl::Graphics::drawText("You won a gem!",
                               glm::vec2(size.x / 2, (size.y / 2) - 440.0f), 80.0f,
                               glm::vec3(0.0f, 1.0f, 0.0f),
                               gl::TextAlign::CENTER, "magic");
        gl::Graphics::drawText("ESC = Exit Puzzle",
                               glm::vec2(size.x / 2, size.y - 50.0f), 70.0f,
                               glm::vec3(0.729f, 0.0f, 0.0f), gl::TextAlign::CENTER);
    }
    else {
        gl::Graphics::drawText("Turn off the lights!",
                               glm::vec2(size.x / 2, (size.y / 2) - 440.0f), 80.0f,
                               glm::vec3(0.384f, 0.0f, 0.49f),
                               gl::TextAlign::CENTER, "magic");
        gl::Graphics::drawText("R = Restart   |   ESC = Exit Puzzle",
                               glm::vec2(size.x / 2, size.y - 50.0f), 60.0f,
                               glm::vec3(0.384f, 0.0f, 0.49f),
                               gl::TextAlign::CENTER);
    }
}

void Screen::initWordPuzzle() {
    glm::vec2 size = Window::getSize();
    float cellW = 100.0f;
    float cellH = 100.0f;
    float spacing = 15.0f;

    const int N = 11;

    float totalW = N * cellW + (N - 1) * spacing;

    float startX = (size.x - totalW) / 2.0f;
    float startY = size.y / 2.0f;

    std::string scrambled = "TNCEHAMTNEN";

    for (int i = 0; i < N; i++) {
        letterGrid[i] = scrambled[i];
        letterUsed[i] = false;

        letterRects[i] = {
            startX + i * (cellW + spacing),
            startY,
            cellW,
            cellH
        };
    }

    currentWord = "";
    wordPuzzleWon = false;
}

void Screen::drawWordPuzzle() {
    glClearColor(1.0f, 0.914f, 0.733f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::vec2 size = Window::getSize();
    gl::Graphics::useTextShader();
    const int N = 11;

    // Draw letters
    for (int i = 0; i < N; i++) {
        if (letterUsed[i]) continue;
        Rect r = letterRects[i];

        std::string s(1, letterGrid[i]);

        gl::Graphics::drawText(s,
                               glm::vec2(r.x + r.w/2, r.y - r.h/2), 60.0f,
                               glm::vec3(0.0f), gl::TextAlign::CENTER);
    }

    // Progress display
    std::string msg;
    if (wordPuzzleWon) {
        gl::Graphics::drawText("You won a gem!",
                               glm::vec2(size.x / 2, size.y / 2), 100.0f,
                               glm::vec3(0.0f, 0.6f, 0.016f),
                               gl::TextAlign::CENTER, "magic");
        gl::Graphics::drawText("ESC = Exit Puzzle",
                               glm::vec2(size.x / 2, (size.y / 2) + 120.0f), 70.0f,
                               glm::vec3(1.0f, 0.0f, 0.0f), gl::TextAlign::CENTER);
    } else {
        gl::Graphics::drawText("Click the above letters in the right order",
                               glm::vec2(size.x / 2, (size.y / 2) + 180.0f), 70.0f,
                               glm::vec3(0.0f, 0.0f, 1.0f),
                               gl::TextAlign::CENTER);
        gl::Graphics::drawText("ESC = Exit Puzzle",
                               glm::vec2(size.x / 2, size.y - 50.0f), 60.0f,
                               glm::vec3(0.384f, 0.0f, 0.49f),
                               gl::TextAlign::CENTER);
    }
    gl::Graphics::drawText("Unscramble letters: " + currentWord,
                           glm::vec2(size.x / 2, (size.y / 2) - 300.0f), 80.0f,
                           glm::vec3(0.0f, 0.0f, 1.0f),
                           gl::TextAlign::CENTER, "magic");
}

void Screen::initPotionPuzzle() {
    const int N = 3;
    glm::vec2 size = Window::getSize();

    float cellW = 220.0f;
    float cellH = 60.0f;
    float spacing = 50.0f;

    float totalW = N * cellW + (N - 1) * spacing;
    float totalH = N * cellH + (N - 1) * spacing;

    float startX = (size.x - totalW) / 2.0f;
    float startY = (size.y + totalH) / 2.0f + 100.0f;

    std::string names[N][N] = {
        {"Unicorn Horn", "Dragon Scale", "Mandrake"},
        {"Amber", "Nightshade", "Fairy Dust"},
        {"Goblin Ear", "Phoenix Ash", "Moonwater"}
    };

    glm::vec3 colors[N][N] = {
        { {1,1,1}, {1,0,0}, {0.5,0.3,0.1} },
        { {1,0.6,0}, {0.2,0,0.4}, {1,0.4,1} },
        { {0.3,0.8,0.3}, {1,0.5,0}, {0.4,0.6,1} }
    };

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            ingredientRects[i][j] = {
                startX + j * (cellW + spacing),
                startY - i * (cellH + spacing),
                cellW,
                cellH
            };

            ingredientNames[i][j] = names[i][j];
            ingredientColors[i][j] = colors[i][j];
            selected[i][j] = false;
        }
    }

    currentPotion = glm::vec3(0.0f);
    targetPotion = glm::vec3(1.0f, 0.5f, 0.5f);
    mixCount = 0;
    wonPotion = false;
}

void Screen::drawPotionPuzzle() {
    glClearColor(0.18f, 0.18f, 0.18f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::vec2 size = Window::getSize();
    gl::Graphics::useTextShader();
    const int N = 3;

    // GRID
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            Rect r = ingredientRects[i][j];
            glm::vec3 col = selected[i][j]
                                ? glm::vec3(1.0f)
                                : ingredientColors[i][j];
            gl::Graphics::drawText(ingredientNames[i][j],
                                   glm::vec2(r.x + r.w/2, r.y - r.h/2), 50.0f,
                                   col,
                                   gl::TextAlign::CENTER);
        }
    }
    float gridTopY = ingredientRects[0][0].y;
    float gridBottomY = ingredientRects[2][0].y - ingredientRects[2][0].h;

    gl::Graphics::drawText("Your Potion",
                           glm::vec2(size.x / 2, gridTopY + 120.0f), 60.0f,
                           currentPotion, gl::TextAlign::CENTER, "magic");
    gl::Graphics::drawText("Target Potion",
                           glm::vec2(size.x / 2, gridTopY - 360.0f), 60.0f,
                           targetPotion,
                           gl::TextAlign::CENTER, "magic");

    // WIN
    float eps = 0.05f;
    bool won = glm::length(currentPotion - targetPotion) < eps;

    if (won) {
        wonPotion = true;
        if (!puzzle3_done) {
            incrementGems();
            puzzle3_done = true;
            gl::AudioEngine::playSound("yay", 0.3f);
        }
        gl::Graphics::drawText("You won a gem!",
                               glm::vec2(size.x / 2, (size.y / 2) - 400.0f), 100.0f,
                               glm::vec3(0.0f, 1.0f, 0.0f),
                               gl::TextAlign::CENTER, "magic");
        gl::Graphics::drawText("ESC = Exit Puzzle",
                               glm::vec2(size.x / 2, (size.y / 2) - 300.0f), 70.0f,
                               glm::vec3(1.0f, 0.0f, 0.0f), gl::TextAlign::CENTER);
    } else {
        gl::Graphics::drawText("Click ingredients to mix a potion that",
                               glm::vec2(size.x/2, (size.y / 2) - 340.0f), 60.0f,
                               glm::vec3(1.0f),
                               gl::TextAlign::CENTER);
        gl::Graphics::drawText("matches the color of the target potion",
                               glm::vec2(size.x/2, (size.y / 2) - 280.0f), 60.0f,
                               glm::vec3(1.0f),
                               gl::TextAlign::CENTER);
        gl::Graphics::drawText("R = Restart   |   ESC = Exit Puzzle",
                               glm::vec2(size.x / 2, (size.y / 2) - 440.0f), 50.0f,
                               glm::vec3(0.941f, 0.961f, 0.714f),
                               gl::TextAlign::CENTER);
    }
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

void Screen::resetPuzzles() {
    puzzle1_done = false;
    puzzle2_done = false;
    puzzle3_done = false;

    initPuzzleGrid();
    initWordPuzzle();
    initPotionPuzzle();
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
