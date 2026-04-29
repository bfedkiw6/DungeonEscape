#include "Engine/Graphics/Skinned/skeleton.h"

#include "Engine/debug.h"


namespace gl {
    void Skeleton::addBone(const std::string& bone_name, const unsigned int current_id, const int parent_id,
            const glm::mat4& offset_matrix, const glm::mat4& local_transform, const bool is_virtual) {

        const auto to_add = Bone(bone_name, current_id, parent_id, offset_matrix, local_transform, is_virtual);
        bones_.push_back(to_add);
        bone_matrices_.push_back(offset_matrix);
        bone_map_[to_add.name] = current_id;
        num_bones_ = (unsigned int) bones_.size();
        if (parent_id != -1) bones_[parent_id].addChild(to_add);
    }


    void Skeleton::traverseBoneHierarchy(const unsigned int bone_id, const glm::mat4& parent_transform) {
        auto& bone = bones_[bone_id];

        // Accumulate transforms: parent's global transform × this bone's local transform
        const glm::mat4 global_transform = parent_transform * bone.local_transform;

        // Calculate final bone matrix for shader
        bone_matrices_[bone_id] = global_transform * bone.offset_matrix;

        // Recursively update all children with this bone's global transform as their parent
        for (const auto& child_id : bone.children) {
            traverseBoneHierarchy(child_id, global_transform);
        }
    }

    void Skeleton::updateBoneMatrices() {
        // Ensure bone_matrices_ is sized correctly
        if (bone_matrices_.size() != num_bones_) {
            bone_matrices_.resize(num_bones_, glm::mat4(1.0f));
        }

        // Find and traverse from all root bones (those with no parent)
        for (unsigned int i = 0; i < bones_.size(); i++) {
            if (bones_[i].parent_id == -1) {
                // Start traversal from root with identity parent transform
                traverseBoneHierarchy(i, glm::mat4(1.0f));
                break;
            }
        }
    }

    const std::vector<glm::mat4>& Skeleton::getBoneMatrices() {
        return bone_matrices_;
    }

    unsigned int Skeleton::getNumBones() {
        return num_bones_;
    }

    void Skeleton::setCurrentAnimation(size_t index) {
        current_animation_ = &animations_[index];
    }

    void Skeleton::setCurrentAnimation(const std::string& animation_name) {
        if (!animations_name_to_index.contains(animation_name)) {
            debug::error("Animation {} not found in skeleton.", animation_name);
            return;
        }
        current_animation_ = &animations_[animations_name_to_index.at(animation_name)];
    }

    void Skeleton::setNextAnimation(size_t index) {
        next_animation_ = &animations_[index];
    }

    void Skeleton::setNextAnimation(const std::string& animation_name) {
        if (!animations_name_to_index.contains(animation_name)) {
            debug::error("Animation {} not found in skeleton.", animation_name);
            return;
        }
        next_animation_ = &animations_[animations_name_to_index.at(animation_name)];
    }

    void Skeleton::playCurrentAnimation(double time_since_previous) {
        if (current_animation_) {
            current_animation_->time_in_seconds += time_since_previous;

            const double curr_animation_time = current_animation_->getCurrentAnimationTime();

            // Update each bone's local transform based on animation channels
            for (auto& [bone_id, channelA] : current_animation_->channels) {
                // TODO: Modify the code inside of this loop
                // to implement the animation blending

                bones_[bone_id].local_transform = channelA.calculateTransform(curr_animation_time);

                // You may want to un-comment the below line of code, when working on blending
                // You may also want to add some safety check (either here or elsewhere)
                // to ensure bone_id exists and your two different animations are actually compatible
                // auto channelB = next_animation_->channels.find(bone_id)->second;
            }
        }

        updateBoneMatrices();
    }

    void Skeleton::resetToBindPose() {
        for (auto& bone : bones_) {
            bone.local_transform = bone.bind_pose_transform;
        }
    }
}
