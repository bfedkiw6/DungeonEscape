#include "Engine/transform.h"

Transform::Transform() = default;

Transform::Transform(const glm::vec3& position, const glm::vec3& scale, const glm::mat4& rotation) {
    position_ = position;
    scale_ = scale;
    rotation_ = rotation;
}

void Transform::setPosition(const glm::vec3 pos) {
    position_ = pos;
}

void Transform::setScale(const glm::vec3 scale) {
    scale_ = scale;
}

void Transform::setScale(const float scale) {
    scale_ = glm::vec3(scale);
}


void Transform::setRotation(const glm::mat4& rotation_matrix) {
    rotation_ = rotation_matrix;
}

void Transform::translate(const glm::vec3& translation) {
    position_ += translation;
}

/**
 * Rotate the transform by the given angle in Radians around the given axis
 * @param radians - rotation angle in radians
 * @param axis - axis to rotate around
 */
void Transform::rotate(const float radians, const glm::vec3 axis) {
    rotation_ = glm::rotate(rotation_, radians, axis);
}


glm::vec3 Transform::getPosition() const {
    return position_;
}

glm::vec3 Transform::getScale() const {
    return scale_;
}

glm::mat4 Transform::getRotation() const {
    return rotation_;
}

glm::mat4 Transform::getModelMatrix() const {
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, position_);
    model = model * rotation_;
    model = glm::scale(model, scale_);
    return model;
}
