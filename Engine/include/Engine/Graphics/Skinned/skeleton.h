#pragma once

#include "Engine/Graphics/Skinned/animation.h"

namespace gl {

#define MAX_BONES_PER_VERTEX 4
    using BoneIDs = std::array<unsigned int, MAX_BONES_PER_VERTEX>;
    using BoneWeights = std::array<float, MAX_BONES_PER_VERTEX>;

    struct Bone {
        std::string name;              // Bone name from FBX file
        unsigned int id;                        // Unique bone index
        int parent_id;                 // Parent bone index (-1 for root)
        glm::mat4 offset_matrix = glm::mat4(1.0);       // Inverse bind pose - transforms from mesh space to bone space
        glm::mat4 local_transform = glm::mat4(1.0);   // Local transform relative to parent bone
        glm::mat4 bind_pose_transform = glm::mat4(1.0);; // Bind pose local transform (immutable after construction)
        std::vector<unsigned int> children;     // Indices of child bones
        bool is_virtual = false;        // For bones without weights

        std::unordered_map<unsigned int, float> vertex_weights; // vertex_id -> weight

        Bone(const std::string& bone_name, const unsigned int bone_id, const int parent,
            const glm::mat4& offset, const glm::mat4& bind_pose_transform, bool virtual_bone = false) :
        name(bone_name),
        id(bone_id),
        parent_id(parent),
        offset_matrix(offset),
        local_transform(bind_pose_transform),
        bind_pose_transform(bind_pose_transform),
        is_virtual(virtual_bone) {}


        void addChild(const Bone& child) {
            children.push_back(child.id);
        }
    };

    class Skeleton {
    public:
        Skeleton() = default;
        void addBone(const std::string& bone_name, const unsigned int current_id, const int parent_id,
            const glm::mat4& offset_matrix, const glm::mat4& local_transform, bool is_virtual = false);
        void updateBoneMatrices();


        void setCurrentAnimation(size_t index);
        void setCurrentAnimation(const std::string& animation_name);
        void setNextAnimation(size_t index);
        void setNextAnimation(const std::string& animation_name);
        void playCurrentAnimation(double time_since_previous);
        void resetToBindPose();

        void traverseBoneHierarchy(unsigned int bone_id, const glm::mat4& parent_transform);
        unsigned int num_bones_ = 0;
        std::vector<Bone> bones_;
        std::unordered_map<std::string, unsigned int> bone_map_;
        std::vector<glm::mat4> bone_matrices_;

        std::vector<glm::vec3> vertices_;
        std::vector<glm::ivec3> faces_;
        std::unordered_map<unsigned int, std::vector<unsigned int>> vertex_to_boneID_map_;

        std::unordered_map<std::string, size_t> animations_name_to_index;
        std::vector<Animation> animations_;
        Animation* current_animation_ = nullptr;
        Animation* next_animation_ = nullptr;

        glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());
    };
}
