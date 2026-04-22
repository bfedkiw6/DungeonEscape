#pragma once

#include "Engine/component.h"
class SphereCollider;
class CylinderCollider;
class BoxCollider;
class ConvexCollider;
class TriangleMeshCollider;

enum ColliderType {
    BOX,
    CYLINDER,
    SPHERE,
    CONE, // GJK
    CONVEXHULL, // GJK
    TRIANGLE // GJK
};

struct MTV {
    bool collision = false;
    glm::vec3 normal = glm::vec3(0);
    float depth = 0.0f;

    MTV invert() const {
        return MTV{collision, -normal, depth};
    }
};

class Collider {

public:
    static bool gjk_mode;
    Collider(ColliderType type, TransformComponent* transform);
    virtual ~Collider() = default;
    virtual MTV getMTV(const Collider* other) const = 0;
    virtual glm::vec3 getSupportPoint(const glm::vec3& direction) const = 0;

    virtual MTV collideCylinder(const CylinderCollider* cylinder) const;
    virtual MTV collideSphere(const SphereCollider* sphere) const;
    virtual MTV collideBox(const BoxCollider* box) const;

    const ColliderType type;
    TransformComponent* transform;
};


