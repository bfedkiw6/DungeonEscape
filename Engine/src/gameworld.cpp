#include <Engine/gameworld.h>

GameWorld::GameWorld() {}

GameObject* GameWorld::addGameObject() {
    m_objects.push_back(std::make_unique<GameObject>());
    return m_objects.back().get();
}

void GameWorld::removeGameObject(GameObject* obj) {
    for (int i = 0; i < m_objects.size(); i++) {
        if (m_objects[i].get() == obj) {
            m_objects.erase(m_objects.begin() + i);
            return;
        }
    }
}

GameObject* GameWorld::getGameObject(int index) {
    if (index < 0 || index >= (int) m_objects.size()) {
        return nullptr;
    }
    return m_objects[index].get();
}

void GameWorld::addSystem(System* sys) {
    m_systems.push_back(sys);
}

void GameWorld::removeSystem(System* sys) {
    auto new_end = std::remove(m_systems.begin(), m_systems.end(), sys);
    m_systems.erase(new_end, m_systems.end());
}

System* GameWorld::getSystem(int index) {
    if (index < m_systems.size()) {
        return m_systems[index];
    }
    return nullptr;
}
