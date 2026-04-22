#include "Engine/collision/TriangleCollider.h"

#include "Engine/GJK.h"

MTV TriangleCollider::getMTV(const Collider* other) const {
    return GJK::getMTV(this, other);
}

glm::vec3 TriangleCollider::getSupportPoint(const glm::vec3& direction) const {
    return GJK::getTriangleSupportPoint(this, direction);
}

const std::array<glm::vec3, 3>& TriangleCollider::getVertices() const {
    return vertices_;
}
