#pragma once
#include "../Collider.h"


class CylinderCollider : public Collider {
public:
    CylinderCollider(TransformComponent* transform, float radius, float height);

    MTV getMTV(const Collider* other) const override;
    glm::vec3 getSupportPoint(const glm::vec3& direction) const override;

    float radius;
    float height;
};
