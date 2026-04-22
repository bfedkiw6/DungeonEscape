#include <Engine/Systems/system.h>

void System::addGameObject(GameObject* obj) {
    m_objects.push_back(obj);
}

void System::removeGameObject(GameObject* obj) {
    auto new_end = std::remove(m_objects.begin(), m_objects.end(), obj);
    m_objects.erase(new_end, m_objects.end());
}
