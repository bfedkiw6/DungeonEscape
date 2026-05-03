#pragma once

#include "Engine/Graphics/camera.h"
#include "Engine/Graphics/mesh.h"
#include "Engine/transform.h"
#include <Engine/game.h>

enum class ScreenType {
    MAINMENU,
    PAUSE,
    GAME,
    WIN,
    LOSE,
    GUARD,
    DOOR,
    /*PUZZLE1,
    PUZZLE2,
    PUZZLE3*/
};

class Screen : public Game {

public:
    Screen();
    virtual ~Screen() = default;
    virtual void draw() override;

    virtual void update(double delta_time) override;

    virtual void keyEvent(int key, int action) override;
    virtual void mouseButtonEvent(int button, int action) override;
    virtual void mousePositionEvent(double x_pos, double y_pos) override;
    virtual void mouseScrolledEvent(double x_offset, double y_offset) override;
    virtual void resizeWindowEvent(int new_width, int new_height) override;

    ScreenType getType();
    void setType(ScreenType type);
    bool switchScreen = false;

    void setCamPos(glm::vec3 pos);

    void incrementGems();
    void resetGems();

    bool shouldReset();
    void clearShouldReset();

    void setPuzzleBool(int puzzle_num, bool val);
    bool getPuzzleBool(int puzzle_num);

private:
    void drawMainMenu();
    void drawPauseScreen();
    void drawWinScreen();
    void drawLoseScreen();
    void drawGameScreen();

    void drawGuardDialogue();
    void drawDoorDialogue();
    void drawNearbyDialogue();

    void drawGivenShape(gl::DrawShape* shape, Transform t, gl::DrawMaterial mat);

    ScreenType type_;
    int gems_ = 0;
    int prev_gems_;
    std::unique_ptr<gl::Camera> cam;

    glm::vec3 cam_pos;

    Transform tb, tf;
    gl::DrawShape* background;
    gl::DrawMesh* skull_mesh;
    gl::DrawMaterial background_mat;

    int guard_dg_num = 0;
    int door_dg_num = 0;
    bool should_reset;

    bool puzzle1_done = false;
    bool puzzle2_done = false;
    bool puzzle3_done = false;
};
