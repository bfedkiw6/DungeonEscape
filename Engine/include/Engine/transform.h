#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Transform {
public:
    Transform();
    Transform(const glm::vec3& position, const glm::vec3& scale, const glm::mat4& rotation);
    ~Transform() = default;

    void setPosition(glm::vec3 pos);
    void setScale(glm::vec3 scale);
    void setScale(float scale);
    void setRotation(const glm::mat4& rotation_matrix);

    void translate(const glm::vec3& translation);
    void rotate(float radians, glm::vec3 axis);

    [[nodiscard]] glm::vec3 getPosition() const;
    [[nodiscard]] glm::vec3 getScale() const;
    [[nodiscard]] glm::mat4 getRotation() const;

    [[nodiscard]] glm::mat4 getModelMatrix() const;


    private:
    glm::vec3 position_ = glm::vec3(0.0f);
    glm::vec3 scale_ = glm::vec3(1.0f);
    glm::mat4 rotation_ = glm::mat4(1.0f);
};
