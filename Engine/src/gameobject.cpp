#include <Engine/gameobject.h>

GameObject::GameObject() {}

void GameObject::addTransformComp() {
    m_transform = new TransformComponent();
}

void GameObject::addDrawableComp() {
    m_drawable = new DrawableComponent();
}

void GameObject::addCollisionComp() {
    m_collision = new CollisionComponent;
}

void GameObject::addSkinnedMeshComp() {
    m_skinned = new SkinnedMeshComponent;
}

void GameObject::removeTransformComp() {
    delete m_transform;
    m_transform = nullptr;
}

void GameObject::removeDrawableComp()  {
    delete m_drawable;
    m_drawable = nullptr;
}

void GameObject::removeCollisionComp() {
    delete m_collision;
    m_collision = nullptr;
}

void GameObject::removeSkinnedMeshComp() {
    delete m_skinned;
    m_skinned = nullptr;
}

TransformComponent* GameObject::getTransformComp() {
    return m_transform;
}

DrawableComponent* GameObject::getDrawableComp()  {
    return m_drawable;
}

CollisionComponent* GameObject::getCollisionComp() {
    return m_collision;
}

SkinnedMeshComponent* GameObject::getSkinnedMeshComp() {
    return m_skinned;
}
