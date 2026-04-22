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
    LOSE
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
    void incrementCoins();
    void resetCoins();

private:
    void drawMainMenu();
    void drawPauseScreen();
    void drawWinScreen();
    void drawLoseScreen();
    void drawGameScreen();
    void drawGivenShape(gl::DrawShape* shape, Transform t, gl::DrawMaterial mat);

    ScreenType type_;
    int coins_;
    int prev_coins_;
    std::unique_ptr<gl::Camera> cam;

    Transform tb, tf;
    gl::DrawShape* background;
};
