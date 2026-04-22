#ifndef SYSTEM_H
#define SYSTEM_H

#include "vector"

class GameWorld;
class GameObject;

class System
{
public:
    virtual ~System() = default;
    virtual void updateWorld(GameWorld& world, float dt) = 0;

    void addGameObject(GameObject* obj);
    void removeGameObject(GameObject* obj);

    std::vector<GameObject*> m_objects;
};

#endif // SYSTEM_H
