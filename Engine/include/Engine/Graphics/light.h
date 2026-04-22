#pragma once

#include <glm/glm.hpp>

namespace gl {
    enum LightType {
        // Matching Assimp's aiLightSourceType
        UNDEFINED = 0,
        DIRECTIONAL = 1,
        POINT = 2,
    };

    struct Light {
        Light(LightType type) : type_(type) {}

        int type_ = UNDEFINED;
        glm::vec3 position_ = glm::vec3(0, 0, 0);
        glm::vec3 direction_ = glm::vec3(0, -1, 0);
        glm::vec3 color_ = glm::vec3(1.f, 1.f, 1.f);
        glm::vec3 attenuation_ = glm::vec3(1.f, 1.f, 1.f);
    };

    struct DirectionalLight : Light {
        DirectionalLight(glm::vec3 direction) : Light(DIRECTIONAL) {
            assert(glm::length(direction) > 0);
            direction_ = glm::normalize(direction);
        }
    };

    struct PointLight : Light {
        PointLight(glm::vec3 position, glm::vec3 attenuation = glm::vec3(1)) : Light(POINT) {
            position_ = position;
            attenuation_ = attenuation;
        }
    };
}
