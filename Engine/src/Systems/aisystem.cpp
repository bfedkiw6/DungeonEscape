#include <Engine/Systems/aisystem.h>
#include <Engine/gameobject.h>
#include <iostream>

void AISystem::updateWorld(GameWorld& world, float dt) {
    if (root) {
        root->update(dt);
    }
}

void AISystem::setRoot(BTNode* node) {
    root = node;
}

BTStatus Sequence::update(float seconds) {
    for (BTNode* child : children) {
        BTStatus status = child->update(seconds);

        if (status == BTStatus::FAIL) {
            return BTStatus::FAIL;
        }
        if (status == BTStatus::RUNNING) {
            return BTStatus::RUNNING;
        }
    }
    // Only if no fails happened
    return BTStatus::SUCCESS;
}

UpdatePathAction::UpdatePathAction(GameObject* enemy, GameObject* player, NavMesh* mesh,
                                     std::vector<int>* path, int* path_index) :
    enemy_obj(enemy),
    player_obj(player),
    nav_mesh(mesh),
    enemy_path(path),
    enemy_path_index(path_index) {}

BTStatus UpdatePathAction::update(float seconds) {
    if (!enemy_obj || !player_obj) {
        return BTStatus::FAIL;
    }
    glm::vec3 enemy_pos = enemy_obj->getTransformComp()->pos;
    glm::vec3 player_pos = player_obj->getTransformComp()->pos;
    int start = nav_mesh->findClosestTriangle(enemy_pos);
    int goal = nav_mesh->findClosestTriangle(player_pos);

    // Use A star
    std::vector<int> new_path = nav_mesh->AStar(start, goal);
    if (new_path.empty()) {
        return BTStatus::FAIL;
    }
    else if (new_path != *enemy_path) {
        // New path, reset current index
        *enemy_path = new_path;
        *enemy_path_index = 0;
    }

    return BTStatus::SUCCESS;
}

MovePathAction::MovePathAction(GameObject* enemy, NavMesh* mesh,
                                std::vector<int>* path, int* path_index, float speed) :
    enemy_obj(enemy),
    nav_mesh(mesh),
    enemy_path(path),
    enemy_path_index(path_index),
    enemy_speed(speed) {}

BTStatus MovePathAction::update(float seconds) {
    if (!enemy_obj || enemy_path->empty()) {
        return BTStatus::FAIL;
    }
    if (*enemy_path_index >= enemy_path->size()) {
        // Reached goal
        return BTStatus::SUCCESS;
    }

    // Get target location to move to & the corresponding direction
    TransformComponent* enemy_trans = enemy_obj->getTransformComp();
    glm::vec3 target = nav_mesh->getTriangles()[(*enemy_path)[*enemy_path_index]].center;
    glm::vec3 dir = target - enemy_trans->pos;
    dir.y = 0.0f;

    float dist = glm::length(dir);
    if (dist < 0.1f) {
        // Close enough to the target location on the path, continue along to next location
        (*enemy_path_index)++;
        if (*enemy_path_index >= enemy_path->size()) {
            // Reached goal
            return BTStatus::SUCCESS;
        }
        return BTStatus::RUNNING; // Still looking for goal
    }
    // Keep moving enemy to target location
    dir = glm::normalize(dir);
    enemy_trans->pos += dir * enemy_speed * seconds;
    return BTStatus::RUNNING;
}
