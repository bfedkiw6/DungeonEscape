#include "Engine/Graphics/Skinned/animation.h"

#include <glm/ext.hpp>

namespace gl {

     // Helper function to find the index of the keyframe just before or at the given time
    template<typename T>
    int findKeyframeIndex(const std::vector<Keyframe<T>>& keys, double time) {
        for (int i = 0; i < keys.size() - 1; i++) {
            if (time < keys[i + 1].time) {
                return i;
            }
        }
        return keys.size() - 1;
    }

    template<typename T>
    T interpolate(const std::vector<Keyframe<T>>& keys, double time) {
        if (keys.empty()) {
            return T();
        }
        if (keys.size() == 1) {
            return keys[0].value;
        }

        int index = findKeyframeIndex(keys, time);
        int next_index = index + 1;

        // Clamp to last keyframe if we're past the end
        if (next_index >= keys.size()) {
            return keys[index].value;
        }

        const auto& key1 = keys[index];
        const auto& key2 = keys[next_index];

        double delta_time = key2.time - key1.time;
        float factor = static_cast<float>((time - key1.time) / delta_time);

        // Linear interpolation
        return glm::mix(key1.value, key2.value, factor);
    }

    glm::mat4 AnimationChannel::calculateTransform(double animation_time) const {
        const glm::vec3 position = interpolate(position_keys, animation_time);
        const glm::quat rotation = interpolate(rotation_keys, animation_time);
        const glm::vec3 scale = interpolate(scale_keys, animation_time);

        return glm::translate(glm::mat4(1.0f), position)
             * glm::mat4_cast(rotation)
             * glm::scale(glm::mat4(1.0f), scale);
    }

    glm::mat4 blendTransforms(const glm::mat4& A, const glm::mat4& B, float blend_factor) {

        // Get positions from A and B Transformations
        glm::vec3 posA(A[3].x, A[3].y, A[3].z);
        glm::vec3 posB(B[3].x, B[3].y, B[3].z);

        // Get scales
        glm::vec3 a0(A[0].x, A[0].y, A[0].z);
        glm::vec3 a1(A[1].x, A[1].y, A[1].z);
        glm::vec3 a2(A[2].x, A[2].y, A[2].z);
        glm::vec3 scaleA(glm::length(a0), glm::length(a1), glm::length(a2));
        glm::vec3 b0(B[0].x, B[0].y, B[0].z);
        glm::vec3 b1(B[1].x, B[1].y, B[1].z);
        glm::vec3 b2(B[2].x, B[2].y, B[2].z);
        glm::vec3 scaleB(glm::length(b0), glm::length(b1), glm::length(b2));

        // Be careful about divisions by 0
        if (scaleA.x != 0) a0 /= scaleA.x;
        if (scaleA.y != 0) a1 /= scaleA.y;
        if (scaleA.z != 0) a2 /= scaleA.z;
        if (scaleB.x != 0) b0 /= scaleB.x;
        if (scaleB.y != 0) b1 /= scaleB.y;
        if (scaleB.z != 0) b2 /= scaleB.z;

        // Get rotations
        glm::mat3 rotA = glm::mat3(a0, a1, a2);
        glm::mat3 rotB = glm::mat3(b0, b1, b2);


        // TODO: Animation Lab
        // TODO: Calculate the blended positon, using a weighted sum approach
        glm::vec3 pos = glm::vec3(0);

        // TODO: Calculate the blended scale, using a weighted sum approach
        glm::vec3 scale = glm::vec3(0);

        // TODO: Calculate the rotation, using a weighted sum approach
        glm::mat3 rot = glm::mat3(0);

        // Convert into mat4
        glm::mat4 R(1.0f);
        R[0] = glm::vec4(rot[0], 0.0f);
        R[1] = glm::vec4(rot[1], 0.0f);
        R[2] = glm::vec4(rot[2], 0.0f);
        glm::mat4 T = glm::translate(glm::mat4(1.0f), pos);
        glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);

        // TODO: Calculate and return the final, combined blended transformation
        glm::mat4 result = glm::mat4(0);;

        return result;
    }

    double Animation::getCurrentAnimationTime() const {
        return fmod(time_in_seconds * ticks_per_second, duration);
    }

    glm::vec3 Animation::getRootTranslation() const {
        if (!channels.contains(root_bone_id)) {
            return glm::vec3(0.0f);
        }
        const auto& position_keys = channels.at(root_bone_id).position_keys;
        return interpolate(position_keys, getCurrentAnimationTime());
    }
}
