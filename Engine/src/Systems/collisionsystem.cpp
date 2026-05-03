#include <Engine/Systems/collisionsystem.h>
#include <Engine/gameobject.h>

#include <Engine/Collider.h>
#include <Engine/collision/CylinderCollider.h>
#include <Engine/collision/SphereCollider.h>
#include <Engine/collision/BoxCollider.h>
#include <Engine/GJK.h>
#include <iostream>
#include <set>

void CollisionSystem::updateWorld(GameWorld& world, float dt) {
    auto start = std::chrono::high_resolution_clock::now();
    collided = false;
    if (m_objects.empty()) {
        return;
    }
    // Get player vals to check objects against
    GameObject* player = m_objects[0];
    TransformComponent* p_transform = player->getTransformComp();
    CollisionComponent* p_collision = player->getCollisionComp();
    if (!p_transform || !p_collision) {
        return;
    }
    CylinderCollider p_col(p_transform, p_collision->radius, p_collision->height);

    // Find close objects and ignore far ones for collisions
    buildUniformGrid();
    std::vector<GameObject*> check_objs = findCloseObjs(player);

    for (GameObject* obj : check_objs) {
    //for (int i = 1; i < m_objects.size(); i++) { // Keep for testing purposes
        //GameObject* obj = m_objects[i];
        TransformComponent* obj_transform = obj->getTransformComp();
        CollisionComponent* obj_collision = obj->getCollisionComp();
        DrawableComponent* draw = obj->getDrawableComp();
        if (!obj_transform || !obj_collision) {
            continue;
        }
        if (draw && !draw->visible) {
            continue;
        }

         // Check for collision
        if (obj_collision->shape == CollisionShape::CYLINDER) {
            CylinderCollider obj_col(obj_transform, obj_collision->radius, obj_collision->height);
            MTV result = obj_col.getMTV(&p_col).invert();
            if (result.collision) {
                collided = true;
                collided_obj = obj->type;
                if (obj->type == ObjectType::ITEMS) {
                    p_transform->pos = player_pos;
                } else {
                    // Nudge
                    p_transform->pos += result.normal * result.depth;
                }
            }
        }else if (obj_collision->shape == CollisionShape::BOX) {
            BoxCollider obj_col(obj_transform, obj_collision->height);
            MTV result = obj_col.getMTV(&p_col).invert();
            if (result.collision) {
                collided = true;
                collided_obj = obj->type;
                if (obj->type == ObjectType::WALLS || obj->type == ObjectType::ITEMS) {
                    p_transform->pos = player_pos;
                } else {
                    // Nudge
                    p_transform->pos += result.normal * result.depth;
                }
            }
        } else if (obj_collision->shape == CollisionShape::SPHERE) {
            SphereCollider obj_col(obj_transform, obj_collision->radius);
            MTV result = obj_col.getMTV(&p_col).invert();
            if (result.collision) {
                collided = true;
                collided_obj = obj->type;
            }
        }
    }

    // Debugging
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //std::cout << "CollisionSystem time: " << time << " ms\n";
}

std::pair<bool, ObjectType> CollisionSystem::getCollided() {
    return {collided, collided_obj};
}

std::pair<int,int> CollisionSystem::getCell(glm::vec3& pos) {
    // Find a given object's cell
    int cell_x = int(std::floor(pos.x / cell_size));
    int cell_z = int(std::floor(pos.z / cell_size));
    return {cell_x, cell_z};
}

void CollisionSystem::buildUniformGrid() {
    grid.clear();
    for (GameObject* obj : m_objects) {
        TransformComponent* transform = obj->getTransformComp();
        CollisionComponent* collision = obj->getCollisionComp();
        DrawableComponent* draw = obj->getDrawableComp();

        if (!transform || !collision) {
            continue;
        }
        if (draw && !draw->visible) {
            continue;
        }

        glm::vec3 pos = transform->pos;
        float minX, maxX, minZ, maxZ;
        if (collision->shape == CollisionShape::BOX) {
            glm::vec3 half = transform->scale * 0.5f;
            minX = pos.x - half.x;
            maxX = pos.x + half.x;
            minZ = pos.z - half.z;
            maxZ = pos.z + half.z;
        } else {
            float r = collision->radius;
            minX = pos.x - r;
            maxX = pos.x + r;
            minZ = pos.z - r;
            maxZ = pos.z + r;
        }

        // Loop through all grid cells an object shows up in
        int minCellX = int(std::floor(minX / cell_size));
        int maxCellX = int(std::floor(maxX / cell_size));
        int minCellZ = int(std::floor(minZ / cell_size));
        int maxCellZ = int(std::floor(maxZ / cell_size));
        for (int x = minCellX; x <= maxCellX; x++) {
            for (int z = minCellZ; z <= maxCellZ; z++) {
                grid[{x, z}].push_back(obj);
            }
        }
    }
}

std::vector<GameObject*> CollisionSystem::findCloseObjs(GameObject* player) {
    std::vector<GameObject*> close;
    std::set<GameObject*> checked;

    // Get player position in grid
    std::pair<int,int> player_cell = getCell(player->getTransformComp()->pos);

    // Check all cells touching player cell (left/right & top/bottom)
    for (int dx = -1; dx <= 1; dx++) {
        for (int dz = -1; dz <= 1; dz++) {
            std::pair<int,int> cell = {player_cell.first + dx, player_cell.second + dz};

            // Verify cell is in grid (player could be on edge of grid so cell doesn't exist)
            auto it = grid.find(cell);
            if (it == grid.end()) {
                continue;
            }
            // Look through objects in cell
            for (GameObject* obj : it->second) {
                // Avoid self collision or object could be in multiple cells & was already checked
                if (obj == player || checked.find(obj) != checked.end()) {
                    continue;
                }
                checked.insert(obj);
                close.push_back(obj);
            }
        }
    }
    return close;
}

void CollisionSystem::setOldPosition(glm::vec3 old_pos) {
    player_pos = old_pos;
}
