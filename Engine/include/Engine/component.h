#ifndef COMPONENT_H
#define COMPONENT_H

#include <Engine/Graphics/graphics.h>
#include <GLFW/glfw3.h>

class Component {
public:
    Component();
    virtual ~Component();
};

struct TransformComponent : Component {
    glm::vec3 pos{0.0f};
    glm::mat4 rotate{1.0f};
    glm::vec3 scale{1.0f};
    float speed{0.0f}; // Using for log movement

    glm::mat4 getModelMatrix() const {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        model = model * rotate;
        return model;
    }
};

struct DrawableComponent : Component {
    gl::DrawMaterial mat{};
    gl::DrawShape* shape{};
    gl::DrawMesh* mesh{};
    bool visible = true;
};

enum class CollisionShape {
    CYLINDER,
    SPHERE,
    BOX
};

struct CollisionComponent : Component {
    CollisionShape shape;
    float radius;
    float height;
};


#endif // COMPONENT_H
