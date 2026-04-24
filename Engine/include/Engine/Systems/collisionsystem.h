#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "Engine/gameobject.h"
#include "system.h"
#include <glm/glm.hpp>
#include <map>

class CollisionSystem : public System {
public:
    void updateWorld(GameWorld& world, float dt) override;
    std::pair<bool, ObjectType> getCollided();
    void setOldPosition(glm::vec3 old_pos);

private:
    bool collided = false;
    ObjectType collided_obj;
    glm::vec3 player_pos;

    // Uniform grid collision accelerations vars/funcs
    float cell_size = 5.0f;
    std::map<std::pair<int,int>, std::vector<GameObject*>> grid;
    std::pair<int,int> getCell(glm::vec3& pos);
    void buildUniformGrid();
    std::vector<GameObject*> findCloseObjs(GameObject* player);
};

#endif // COLLISIONSYSTEM_H
