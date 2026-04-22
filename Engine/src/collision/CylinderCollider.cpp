#include "Engine/collision/CylinderCollider.h"

#include "Engine/GJK.h"

CylinderCollider::CylinderCollider(TransformComponent* transform, float radius, float height)
    : Collider(ColliderType::CYLINDER, transform), radius(radius), height(height) {

}

MTV CylinderCollider::getMTV(const Collider* other) const {
    return GJK::getMTV(this, other);
}

glm::vec3 CylinderCollider::getSupportPoint(const glm::vec3& direction) const {
    return GJK::getCylinderSupportPoint(this, direction);
}
