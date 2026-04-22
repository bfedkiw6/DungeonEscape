#include "Engine/Collider.h"

#include "Engine/GJK.h"
#include "Engine/collision/BoxCollider.h"
#include "Engine/collision/CylinderCollider.h"
#include "Engine/collision/SphereCollider.h"


bool Collider::gjk_mode = false;


Collider::Collider(const ColliderType type, TransformComponent* transform):
    type(type), transform(transform) {

}

// Defaults colliders to use GJK for collision detection if no specific implementation exists
MTV Collider::collideCylinder(const CylinderCollider* cylinder) const {
    return GJK::getMTV(this, cylinder);
}

MTV Collider::collideSphere(const SphereCollider* sphere) const {
    return GJK::getMTV(this, sphere);
}

MTV Collider::collideBox(const BoxCollider* box) const {
    return GJK::getMTV(this, box);
}

