#include <Engine/Systems/charactercontrollersystem.h>
#include <Engine/gameobject.h>
#include <iostream>

void CharacterControllerSystem::setCamera(gl::Camera* cam) {
    m_cam = cam;
}

void CharacterControllerSystem::setKey(int key, bool pressed) {
    key_state[key] = pressed;
}

bool CharacterControllerSystem::getKey(int key) {
    return key_state[key];
}

void CharacterControllerSystem::setHorizSpeed(float h_speed) {
    horiz_speed = h_speed;
}

void CharacterControllerSystem::setVertSpeed(float v_speed) {
    vert_speed = v_speed;
}

void CharacterControllerSystem::setGravity(float grav) {
    gravity = grav;
}

void CharacterControllerSystem::setFloorWidth(int width) {
    floor_width = (float) width;
}

void CharacterControllerSystem::setPlayerWidth(float width) {
    player_width = width;
}

void CharacterControllerSystem::reset() {
    on_ground = true;
    velocity = 0;
}

void CharacterControllerSystem::updateWorld(GameWorld& world, float dt) {
    if (!m_cam || m_objects.empty()) {
        return;
    }

    GameObject* player = m_objects[0];
    TransformComponent* transform = player->getTransformComp();
    if (!transform) {
        return;
    }

    glm::vec3 curr_pos = transform->pos;
    float height = 0.25f;

    glm::vec3 pos = glm::vec3(0.0f);

    // Horizontal
    // Only move when keys are pressed
    if (key_state[GLFW_KEY_W]) {
        pos += glm::vec3(0.0f, 0.0f, 1.0f);
    }
    if (key_state[GLFW_KEY_S]) {
        pos += glm::vec3(0.0f, 0.0f, -1.0f);
    }
    if (key_state[GLFW_KEY_A]) {
        pos += glm::vec3(1.0f, 0.0f, 0.0f);
    }
    if (key_state[GLFW_KEY_D]) {
        pos += glm::vec3(-1.0f, 0.0f, 0.0f);
    }

    if (glm::length(pos) > 0.0f) {
        curr_pos += horiz_speed * dt * glm::normalize(pos);

        // Make sure can't go off floor horizontally
        if (curr_pos.x > floor_width - player_width) {
            curr_pos.x = floor_width - player_width;
        } else if (curr_pos.x < -floor_width) {
            curr_pos.x = -floor_width;
        }
    }

    // Vertical
    if (curr_pos.y <= height) {
        curr_pos.y = height;
        velocity = 0.0f;
        on_ground = true;
    } else {
        on_ground = false;
    }

    if (on_ground && key_state[GLFW_KEY_SPACE]) {
        velocity = vert_speed;
        on_ground = false;
    }

    curr_pos.y += velocity * dt + 0.5f * gravity * dt * dt;
    velocity += gravity * dt;

    if (curr_pos.y < height) {
        curr_pos.y = height;
        velocity = 0.0f;
        on_ground = true;
    }

    transform->pos = curr_pos;
}
