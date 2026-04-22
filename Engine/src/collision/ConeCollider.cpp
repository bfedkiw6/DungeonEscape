#include "Engine/collision/ConeCollider.h"

#include "Engine/GJK.h"

ConeCollider::ConeCollider(TransformComponent* transform): Collider(CONE, transform) {
}

MTV ConeCollider::getMTV(const Collider* other) const {
    return GJK::getMTV(this, other);
}

glm::vec3 ConeCollider::getSupportPoint(const glm::vec3& direction) const {
    return GJK::getConeSupportPoint(this, direction);
}
