#pragma once
#include "../Collider.h"


class ConvexCollider : public Collider {
public:
    ConvexCollider(TransformComponent* transform, const std::vector<glm::vec3>& points);

    MTV getMTV(const Collider* other) const override;
    glm::vec3 getSupportPoint(const glm::vec3& direction) const override;
    const std::vector<glm::vec3>& getPoints() const;

private:
    std::vector<glm::vec3> points_;

};
