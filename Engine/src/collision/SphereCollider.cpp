#include "Engine/collision/SphereCollider.h"

#include "Engine/GJK.h"

SphereCollider::SphereCollider(TransformComponent* transform, float radius):
    Collider(ColliderType::SPHERE, transform), radius(radius) {
}

MTV SphereCollider::getMTV(const Collider* other) const {
    return GJK::getMTV(this, other);
}

glm::vec3 SphereCollider::getSupportPoint(const glm::vec3& direction) const {
    return GJK::getSphereSupportPoint(this, direction);
}



