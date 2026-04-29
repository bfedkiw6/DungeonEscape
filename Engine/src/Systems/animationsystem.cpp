#include <Engine/Systems/animationsystem.h>
#include <Engine/gameobject.h>
#include <iostream>

void AnimationSystem::updateWorld(GameWorld& world, float dt) {
    for (GameObject* obj : m_objects) {
        SkinnedMeshComponent* skin = obj->getSkinnedMeshComp();

        if (!skin || !skin->mesh) {
            continue;
        }

        skin->mesh->skeleton.playCurrentAnimation(dt);
    }
}
