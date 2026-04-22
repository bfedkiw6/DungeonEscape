#pragma once
#include "../Collider.h"


class SphereCollider : public Collider {
public:
    SphereCollider(TransformComponent* transform, float radius);

    MTV getMTV(const Collider* other) const override;
    glm::vec3 getSupportPoint(const glm::vec3& direction) const override;

    float radius;
};
