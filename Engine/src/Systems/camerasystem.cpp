#include <Engine/Systems/camerasystem.h>
#include <Engine/gameobject.h>
#include <iostream>
#include <ostream>

void CameraSystem::setCamera(gl::Camera* cam) {
    m_cam = cam;
}

void CameraSystem::setMouseDelta(glm::vec2 delta) {
    m_delta = delta;
}

void CameraSystem::resizeWindowEvent(int new_width, int new_height) {
    if (m_cam) {
        m_cam->setAspectRatio((float)new_width / (float)new_height);
    }
}

float CameraSystem::getCameraDist() {
    return camera_dist;
}

glm::vec3 CameraSystem::getCameraPos() {
    return m_cam->getPosition();
}

void CameraSystem::reset() {
    if (!m_cam) {
        return;
    }
    m_cam->setHorizAngle(0.0f);
    m_cam->setVertAngle(0.0f);
}

void CameraSystem::updateWorld(GameWorld& world, float /*dt*/) {
    if (!m_cam || m_objects.empty()) {
        return;
    }

    glm::vec3 camPos = m_cam->getPosition();
    std::cout << "cam pos: "
              << camPos.x << ", "
              << camPos.y << ", "
              << camPos.z << std::endl;


    m_player_height = 0.5f;
    std::cout << "m_objects size = " << m_objects.size() << std::endl;
    GameObject* player = m_objects[0];
    TransformComponent* transform = player->getTransformComp();
    if (!transform) {
        std::cout << "couldn't get transform"<< std::endl;
        return;
    }

    m_cam->increaseHorizAngle(0.1f * m_delta.x);
    m_cam->increaseVertAngle(0.1f * m_delta.y);
    m_cam->setVertAngle(glm::clamp(m_cam->getVertAngle(), -89.0f, 89.0f));
    m_delta = glm::vec2(0.0f);

    // Move position from the feet of player to eye level
    glm::vec3 head_height = transform->pos + glm::vec3(0.0f, m_player_height, 0.0f);
    // Place camera at player height
    glm::vec3 new_look = m_cam->calcNewLook();
    glm::vec3 cam_pos = head_height;
    m_cam->setPosition(cam_pos);
    m_cam->setLook(glm::normalize(new_look));
    std::cout << "exiting camera update" << std::endl;
}
