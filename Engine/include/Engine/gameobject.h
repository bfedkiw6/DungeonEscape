#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "component.h"
#include "vector"

enum class ObjectType {
    PLAYER,
    LOG,
    COIN,
    SKY,
    ENEMY
};

class GameObject
{
public:
    ObjectType type;

    GameObject();

    void addTransformComp();
    void addDrawableComp();
    void addCollisionComp();

    void removeTransformComp();
    void removeDrawableComp();
    void removeCollisionComp();

    TransformComponent* getTransformComp();
    DrawableComponent*  getDrawableComp();
    CollisionComponent* getCollisionComp();

private:
    TransformComponent* m_transform;
    DrawableComponent*  m_drawable;
    CollisionComponent* m_collision;
};

#endif // GAMEOBJECT_H
