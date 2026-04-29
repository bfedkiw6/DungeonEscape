#include <Engine/Systems/drawsystem.h>
#include <Engine/Systems/particlesystem.h>

void DrawSystem::setCamera(gl::Camera* cam) {
    m_cam = cam;
}

void DrawSystem::updateWorld(GameWorld& world, float dt) {
    auto start = std::chrono::high_resolution_clock::now();
    int culled, kept = 0;

    // Necessary drawing functions below
    gl::Graphics::clearScreen(glm::vec3(0.0f, 0.0f, 0.0f));
    gl::Graphics::usePhongShader();
    gl::Graphics::setAmbientLight(glm::vec3(0.5f));
    gl::Graphics::setCameraUniforms(m_cam);
    gl::Graphics::setLights(lights);

    //Frustum frustum = createFrustumFromCamera();
    std::vector<InstanceInput> instanced_objs;

    for (GameObject* obj : m_objects) {
        TransformComponent* transform = obj->getTransformComp();
        DrawableComponent* draw = obj->getDrawableComp();
        SkinnedMeshComponent* skin = obj->getSkinnedMeshComp();

        // Animation drawing
        if (transform != nullptr && skin != nullptr && skin->mesh != nullptr) {
            drawAnimation(skin, transform);
            continue;
        }

        // Other drawing
        if (transform != nullptr && draw != nullptr && draw->visible == true) {
            // Set up the transform before drawing
            Transform obj_transform;
            obj_transform.setScale(transform->scale);
            obj_transform.translate(transform->pos);
            obj_transform.setRotation(transform->rotate);

            if (draw->shape != nullptr) {
                //gl::Graphics::drawObject(draw->shape, obj_transform, draw->mat); // Keep for speed test purposes
                bool defined = false;
                for (auto& in: instanced_objs) {
                    // If object with same texture already exists, add to models list
                    if (in.shape == draw->shape && in.material.textures.diffuse == draw->mat.textures.diffuse) {
                        in.models.push_back(obj_transform.getModelMatrix());
                        defined = true;
                        break;
                    }
                }
                 // Create new instance object since one doesn't already exist
                if (!defined) {
                    InstanceInput new_in;
                    new_in.shape = draw->shape;
                    new_in.material = draw->mat;
                    new_in.models.push_back(obj_transform.getModelMatrix());
                    instanced_objs.push_back(new_in);
                }
            } else if (draw->mesh != nullptr) {
                gl::Graphics::drawMesh(draw->mesh, obj_transform);
            }
        }
    }
    drawParticles();

    // Now draw all objects
    gl::Graphics::usePhongInstancedShader();
    gl::Graphics::setAmbientLight(glm::vec3(2.0f));
    gl::Graphics::setCameraUniforms(m_cam);
    gl::Graphics::setLights(lights);
    for (auto& in : instanced_objs) {
        gl::Graphics::drawObjectInstanced(in.shape, in.models, in.material);
    }

    // Debugging
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    /*std::cout << "DrawSystem time: " << time << " ms\n";
    std::cout << "Culled: " << culled << std::endl;
    std::cout << "Submitted: " << kept << std::endl;*/
}

void DrawSystem::makeLights() {
    torch_positions.clear();
    torch_positions.push_back(glm::vec3( -11.4f, 1.0f, 3.09f));
    torch_positions.push_back(glm::vec3( 18.9f, 1.0f, 22.7f));
    torch_positions.push_back(glm::vec3(47.4f, 1.0f, -20.1f));
    torch_positions.push_back(glm::vec3(47.9f, 1.0f, 9.4f));
    torch_positions.push_back(glm::vec3( 20.7f, 1.0f, -16.7f));
    torch_positions.push_back(glm::vec3(-5.7f, 1.0f, -18.6f));
    torch_positions.push_back(glm::vec3(14.2f, 2.0f, 12.8f));

    for (const auto& pos : torch_positions) {
        gl::PointLight torch(pos, glm::vec3(1.0f, 0.06f, 0.01f));
        torch.color_ = glm::vec3(2.0f, 1.0f, 0.4f);
        lights.push_back(torch);
    }

    gl::Graphics::setLights(lights);
}

void DrawSystem::setParticleSystem(ParticleSystem* ps) {
    particle_system = ps;
}

void DrawSystem::drawParticles() {
    if (!particle_system) {
        return;
    }

    gl::DrawShape* sphere = gl::Mesh::getLoadedShape("sphere");
    if (!sphere) {
        return;
    }

    for (const auto& p : particle_system->getParticles()) {
        Transform t;
        t.setScale(glm::vec3(p.size));
        t.translate(p.pos);

        gl::DrawMaterial mat;
        mat.ambient = glm::vec3(p.color.r, p.color.g, p.color.b);
        mat.diffuse = glm::vec3(p.color.r, p.color.g, p.color.b);
        mat.specular = glm::vec3(0.0f);
        mat.shininess = 1.0f;
        mat.opacity = p.color.a;

        gl::Graphics::drawObject(sphere, t, mat);
    }
}

void DrawSystem::drawAnimation(SkinnedMeshComponent* skin, TransformComponent* transform) {
    Transform obj_transform;
    obj_transform.setScale(transform->scale);
    obj_transform.translate(transform->pos);
    obj_transform.setRotation(transform->rotate);

    gl::Graphics::useSkinnedShader();
    gl::Graphics::setCameraUniforms(m_cam);
    gl::Graphics::setAmbientLight(glm::vec3(0.5f));
    gl::Graphics::setLights(lights);

    gl::Graphics::drawSkinnedMesh(skin->mesh, obj_transform);

    // Switch shader back
    gl::Graphics::usePhongShader();
    gl::Graphics::setAmbientLight(glm::vec3(0.5f));
    gl::Graphics::setCameraUniforms(m_cam);
    gl::Graphics::setLights(lights);
}

Frustum DrawSystem::createFrustumFromCamera() {
    glm::vec3 position = m_cam->getPosition();
    glm::vec3 up = m_cam->getUp();
    glm::vec3 right = m_cam->getRight();
    glm::vec3 look = m_cam->getLook();
    float aspect = m_cam->getAspectRatio();
    float fov = m_cam->getFOV();
    float near = m_cam->getNear();
    float far = m_cam->getFar();

    Frustum frustum;
    const float half_vside = far * tanf(fov * 0.5f);
    const float half_hside = half_vside * aspect;
    const glm::vec3 front_mult_far = far * look;

    frustum.near_face.normal = glm::normalize(look);
    frustum.near_face.distance = glm::dot(frustum.near_face.normal, position + near * look);

    frustum.far_face.normal = glm::normalize(-look);
    frustum.far_face.distance = glm::dot(frustum.far_face.normal, position + front_mult_far);

    frustum.right_face.normal = glm::normalize(glm::cross(front_mult_far - right * half_hside, up));
    frustum.right_face.distance = glm::dot(frustum.right_face.normal, position);

    frustum.left_face.normal = glm::normalize(glm::cross(up, front_mult_far + right * half_hside));
    frustum.left_face.distance = glm::dot(frustum.left_face.normal, position);

    frustum.top_face.normal = glm::normalize(glm::cross(right, front_mult_far - up * half_vside));
    frustum.top_face.distance = glm::dot(frustum.top_face.normal, position);

    frustum.bottom_face.normal = glm::normalize(glm::cross(front_mult_far + up * half_vside, right));
    frustum.bottom_face.distance = glm::dot(frustum.bottom_face.normal, position);

    return frustum;
}

bool Sphere::isOnOrForwardPlane(const Plane& plane, const glm::vec3& center, float radius) const {
    return glm::dot(plane.normal, center) - plane.distance >= -radius;
}

bool Sphere::isOnFrustum(const Frustum& frustum, const glm::vec3& center, float radius) const {
    return isOnOrForwardPlane(frustum.left_face, center, radius) &&
           isOnOrForwardPlane(frustum.right_face, center, radius) &&
           isOnOrForwardPlane(frustum.far_face, center, radius) &&
           isOnOrForwardPlane(frustum.near_face, center, radius) &&
           isOnOrForwardPlane(frustum.top_face, center, radius) &&
           isOnOrForwardPlane(frustum.bottom_face, center, radius);
};

void DrawSystem::drawSky() {
    GameObject* obj = sky_obj;
    TransformComponent* transform = obj->getTransformComp();
    DrawableComponent* draw = obj->getDrawableComp();
    if (transform && draw && draw->visible) {
        gl::Graphics::usePhongShader();
        gl::Graphics::setAmbientLight(glm::vec3(2.0f));
        gl::Graphics::setCameraUniforms(m_cam);

        Transform obj_transform;
        obj_transform.setScale(transform->scale);
        obj_transform.translate(transform->pos);
        obj_transform.setRotation(transform->rotate);

        // Need below since objects are inside sphere
        glDepthMask(GL_FALSE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        gl::Graphics::drawObject(draw->shape, obj_transform, draw->mat);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDepthMask(GL_TRUE);
    }
}

// Frustum Culling --> was in update world
/*Sphere cull;
            cull.center = glm::vec3(0.0f);
            float max_scale = std::max(transform->scale.x, std::max(transform->scale.y, transform->scale.z));
            CollisionComponent* col = obj->getCollisionComp();
            if (col != nullptr) {
                cull.radius = col->radius;
            } else if (obj->type == ObjectType::MAP) {
                cull.radius = 500.0f;
            } else {
                cull.radius = 5.0f;
            }
            // Don't draw object if not in view
            if (!cull.isOnFrustum(frustum, transform->pos, cull.radius * max_scale)) {
                culled++;
                continue;
            }
            kept++;*/
