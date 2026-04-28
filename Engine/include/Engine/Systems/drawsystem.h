#ifndef DRAWSYSTEM_H
#define DRAWSYSTEM_H

#include "system.h"
#include "Engine/Graphics/camera.h"
#include "Engine/Graphics/mesh.h"
#include "Engine/Systems/particlesystem.h"
#include "Engine/Graphics/light.h"

class GameWorld;

struct InstanceInput {
    gl::DrawShape* shape;
    gl::DrawMaterial material;
    std::vector<glm::mat4> models;
};

struct Plane {
    glm::vec3 normal = {0.0f, 1.0f, 0.0f};
    float distance = 0.0f;
};
struct Frustum {
    Plane top_face;
    Plane bottom_face;
    Plane right_face;
    Plane left_face;
    Plane far_face;
    Plane near_face;
};

struct Volume {
    virtual bool isOnFrustum(const Frustum& frustum, const glm::vec3& center, float radius) const = 0;
};
struct Sphere : public Volume {
    glm::vec3 center{ 0.0f, 0.0f, 0.0f };
    float radius{ 0.0f };
    bool isOnOrForwardPlane(const Plane& plane, const glm::vec3& center, float radius) const;
    bool isOnFrustum(const Frustum& frustum, const glm::vec3& center, float radius) const override;
};

class DrawSystem : public System {
public:
    void updateWorld(GameWorld& world, float dt) override;
    void makeLights();
    void setCamera(gl::Camera* cam);
    Frustum createFrustumFromCamera();
    void drawSky();
    void setParticleSystem(ParticleSystem* ps);
    void drawParticles();

private:
    gl::Camera* m_cam = nullptr;
    GameObject* sky_obj;
    std::vector<gl::Light> lights;
    std::vector<glm::vec3> torch_positions;
    ParticleSystem* particle_system = nullptr;
};

#endif // DRAWSYSTEM_H
