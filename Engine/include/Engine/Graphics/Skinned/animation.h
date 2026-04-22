#pragma once

#include <string>
#include <glm/glm.hpp>

namespace gl {
    template<typename T>
        struct Keyframe {
        double time;        // Time in ticks
        T value;           // Position (vec3), Rotation (quat), or Scale (vec3)
    };

    using PositionKey = Keyframe<glm::vec3>;
    using RotationKey = Keyframe<glm::quat>;
    using ScaleKey = Keyframe<glm::vec3>;

    // Animation channel - one per animated bone
    struct AnimationChannel {
        std::string bone_name;
        unsigned int bone_id;                    // Which bone this channel affects
        std::vector<PositionKey> position_keys;  // Translation keyframes
        std::vector<RotationKey> rotation_keys;  // Rotation keyframes
        std::vector<ScaleKey> scale_keys;        // Scale keyframes

        glm::mat4 calculateTransform(double animation_time) const;
    };


    struct Animation {
        double duration; // in ticks
        double ticks_per_second;
        double time_in_seconds = 0.0;

        double getCurrentAnimationTime() const;
        glm::vec3 getRootTranslation() const;

        // Map from bone_id to its animation channel
        std::unordered_map<unsigned int, AnimationChannel> channels;
        unsigned int root_bone_id = 0;

    };

    glm::mat4 blendTransforms(const glm::mat4& A, const glm::mat4& B, float blend_factor);
}
