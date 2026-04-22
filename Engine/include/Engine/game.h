#pragma once

class Game {
public:
    Game() {}
    virtual ~Game() = default;

    virtual void draw() = 0;
    virtual void update(double delta_time) = 0;

    virtual void keyEvent(int key, int action) {}
    virtual void mouseButtonEvent(int button, int action) {}
    virtual void mousePositionEvent(double x_pos, double y_pos) {}
    virtual void mouseScrolledEvent(double x_offset, double y_offset) {}
    virtual void resizeWindowEvent(int new_width, int new_height) {}
};
