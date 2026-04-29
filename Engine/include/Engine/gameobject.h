#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "component.h"
#include "vector"

enum class ObjectType {
    PLAYER,
    MAP,
    WALLS
};

class GameObject
{
public:
    ObjectType type;

    GameObject();

    void addTransformComp();
    void addDrawableComp();
    void addCollisionComp();
    void addSkinnedMeshComp();

    void removeTransformComp();
    void removeDrawableComp();
    void removeCollisionComp();
    void removeSkinnedMeshComp();

    TransformComponent* getTransformComp();
    DrawableComponent*  getDrawableComp();
    CollisionComponent* getCollisionComp();
    SkinnedMeshComponent* getSkinnedMeshComp();

private:
    TransformComponent* m_transform;
    DrawableComponent*  m_drawable;
    CollisionComponent* m_collision;
    SkinnedMeshComponent* m_skinned;
};

#endif // GAMEOBJECT_H
