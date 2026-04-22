#pragma once

#include <glm/glm.hpp>

namespace gl {
    class Camera {
    public:
        Camera();
        ~Camera() = default;

        void setPosition(glm::vec3 position);
        void setLook(glm::vec3 look);
        void setAspectRatio(float aspect_ratio);
        void setHeight(float height);

        void increaseHorizAngle(float amt);
        void increaseVertAngle(float amt);
        void setVertAngle(float amt);
        void setHorizAngle(float amt);
        glm::vec3 calcNewLook();

        [[nodiscard]] glm::mat4 getViewMatrix() const;
        [[nodiscard]] glm::mat4 getProjection() const;
        [[nodiscard]] glm::mat4 getProjection(float aspect_ratio) const;

        [[nodiscard]] glm::vec3 getPosition() const;
        [[nodiscard]] glm::vec3 getLook() const;
        [[nodiscard]] glm::vec3 getRight() const;
        [[nodiscard]] glm::vec3 getUp() const;
        [[nodiscard]] float getHeight() const;
        [[nodiscard]] float getVertAngle() const;
        [[nodiscard]] float getHorizAngle() const;

        float getAspectRatio();
        float getNear();
        float getFar();
        float getFOV();

    private:
        glm::vec3 position_;
        glm::vec3 look_;
        glm::vec3 up_;
        glm::vec3 world_up_;
        float near_;
        float far_;
        float fov_;
        float aspect_ratio_;
        float horiz_angle_;
        float vert_angle_;
        float height_;

    };
};
