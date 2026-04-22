#ifndef AISYSTEM_H
#define AISYSTEM_H

#include "Engine/gameobject.h"
#include "system.h"
#include "Engine/Graphics/camera.h"
#include "Engine/navmesh.h"
#include <any>

enum class BTStatus {
    SUCCESS,
    FAIL,
    RUNNING
};

class BTNode {
public:
    virtual BTStatus update(float seconds) = 0;
};

class AISystem : public System {
public:
    void updateWorld(GameWorld& world, float dt) override;
    void setRoot(BTNode* node);

private:
    BTNode* root = nullptr;
};

class Sequence : public BTNode {
public:
    std::vector<BTNode*> children;
    BTStatus update(float seconds) override;
};

class Action : public BTNode {
public:
    virtual BTStatus update(float seconds) override = 0;
};

class UpdatePathAction : public Action {
public:
    GameObject* enemy_obj;
    GameObject* player_obj;
    NavMesh* nav_mesh;
    std::vector<int>* enemy_path;
    int* enemy_path_index;

    UpdatePathAction(GameObject* enemy, GameObject* player, NavMesh* mesh,
                      std::vector<int>* path, int* path_index);
    BTStatus update(float seconds) override;
};

class MovePathAction : public Action {
public:
    GameObject* enemy_obj;
    NavMesh* nav_mesh;
    std::vector<int>* enemy_path;
    int* enemy_path_index;
    float enemy_speed;

    MovePathAction(GameObject* enemy, NavMesh* mesh,
                      std::vector<int>* path, int* path_index, float speed);
    BTStatus update(float seconds) override;
};

#endif // AISYSTEM_H
