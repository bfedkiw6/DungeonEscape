#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include "system.h"
#include "Engine/Graphics/camera.h"
#include "Engine/transform.h"


class CameraSystem : public System {
public:
    void updateWorld(GameWorld& world, float dt) override;
    void setCamera(gl::Camera* cam);
    void setMouseDelta(glm::vec2 delta);
    void resizeWindowEvent(int new_width, int new_height);
    float getCameraDist();
    void reset();

private:
    gl::Camera* m_cam = nullptr;
    float camera_dist = 2.0f;
    glm::vec2 m_delta;
    float m_player_height;
};

#endif // CAMERASYSTEM_H
