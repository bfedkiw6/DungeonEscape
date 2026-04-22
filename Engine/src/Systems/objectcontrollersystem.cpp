#include <Engine/Systems/objectcontrollersystem.h>
#include <Engine/gameobject.h>
#include <iostream>

void ObjectControllerSystem::updateWorld(GameWorld& world, float dt) {
    if (m_objects.empty()) {
        return;
    }

    for (int i = 0; i < m_objects.size(); i++) {
        GameObject* obj = m_objects[i];
        TransformComponent* t = obj->getTransformComp();
        if (!t) {
            continue;
        }
        t->pos.x += t->speed * dt;

        // Switch direction at edges
        if (t->pos.x >= 3.0f) {
            t->pos.x = 3.0f;
            t->speed = -std::abs(t->speed);
        }
        else if (t->pos.x <= -4.0f) {
            t->pos.x = -4.0f;
            t->speed = std::abs(t->speed);
        }
    }
}
