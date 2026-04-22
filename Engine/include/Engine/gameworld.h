#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include "gameobject.h"
#include "Systems/system.h"

#include "vector"

class GameWorld
{
public:
    GameWorld();
    GameObject* addGameObject();
    void removeGameObject(GameObject* obj);
    GameObject* getGameObject(int index);
    void addSystem(System* sys);
    void removeSystem(System* sys);
    System* getSystem(int index);

private:
    std::vector<std::unique_ptr<GameObject>> m_objects;
    std::vector<System*> m_systems;
};

#endif // GAMEWORLD_H
