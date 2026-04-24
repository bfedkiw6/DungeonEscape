#pragma once
#include "../Collider.h"


class BoxCollider : public Collider {
public:
    BoxCollider(TransformComponent* transform, float height);

    MTV getMTV(const Collider* other) const override;
    glm::vec3 getSupportPoint(const glm::vec3& direction) const override;

    float height;

};
