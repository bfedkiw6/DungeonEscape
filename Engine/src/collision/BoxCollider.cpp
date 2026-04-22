#include "Engine/collision/BoxCollider.h"

#include "Engine/GJK.h"

BoxCollider::BoxCollider(TransformComponent* transform): Collider(ColliderType::BOX, transform) {

}

MTV BoxCollider::getMTV(const Collider* other) const {
    return GJK::getMTV(this, other);
}

glm::vec3 BoxCollider::getSupportPoint(const glm::vec3& direction) const {
    return GJK::getBoxSupportPoint(this, direction);
}



