#pragma once
#include "../Collider.h"


class TriangleCollider : public Collider {
public:
    TriangleCollider(TransformComponent* transform, std::array<glm::vec3, 3> vertices)
        : Collider(TRIANGLE, transform), vertices_(vertices) {
    }

    MTV getMTV(const Collider* other) const override;
    glm::vec3 getSupportPoint(const glm::vec3& direction) const override;
    const std::array<glm::vec3, 3>& getVertices() const;

private:
    std::array<glm::vec3, 3> vertices_;
};
