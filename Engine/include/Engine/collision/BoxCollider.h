#pragma once
#include "../Collider.h"


class BoxCollider : public Collider {
public:
    BoxCollider(TransformComponent* transform);

    MTV getMTV(const Collider* other) const override;
    glm::vec3 getSupportPoint(const glm::vec3& direction) const override;

    MTV collideCylinder(const CylinderCollider* cylinder) const override;
    MTV collideSphere(const SphereCollider* sphere) const override;
    MTV collideBox(const BoxCollider* box) const override;

};
