#ifndef CHARACTERCONTROLLERSYSTEM_H
#define CHARACTERCONTROLLERSYSTEM_H

#include "system.h"
#include "Engine/Graphics/camera.h"
#include <unordered_map>

class CharacterControllerSystem : public System {
public:
    void updateWorld(GameWorld& world, float dt) override;

    void setCamera(gl::Camera* cam);
    void setKey(int key, bool pressed);
    bool getKey(int key);
    void setHorizSpeed(float h_speed);
    void setVertSpeed(float v_speed);
    void setGravity(float grav);
    void setFloorWidth(int width);
    void setPlayerWidth(float width);
    void reset();

private:
    gl::Camera* m_cam = nullptr;
    std::unordered_map<int, bool> key_state;

    bool player_visible = false;
    bool on_ground = true;
    float horiz_speed = 3.0f;
    float vert_speed = 4.0f;
    float gravity = -9.8f;
    float velocity = 0.0f;
    float floor_width = 0.0f;
    float player_width = 0.0f;
};

#endif // CHARACTERCONTROLLERSYSTEM_H
