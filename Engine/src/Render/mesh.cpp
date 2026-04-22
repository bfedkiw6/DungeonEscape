#include "Engine/Graphics/mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Engine/debug.h"
#include "Engine/file.h"
#include "Engine/Graphics/graphics.h"
#include "Engine/Graphics/material.h"
#include "Engine/Graphics/Shapes/cone.h"
#include "Engine/Graphics/Shapes/cube.h"
#include "Engine/Graphics/Shapes/cylinder.h"
#include "Engine/Graphics/Shapes/primitive.h"
#include "Engine/Graphics/Shapes/quad.h"
#include "Engine/Graphics/Shapes/sphere.h"


namespace gl {

    std::unordered_map<std::string, std::unique_ptr<DrawShape>> Mesh::shapes_;
    std::unordered_map<std::string, std::unique_ptr<DrawMesh>> Mesh::static_meshes_;
    std::unordered_map<std::string, std::unique_ptr<SkinnedMesh>> Mesh::skinned_meshes_;

    /**
     * Initializes default primitive shapes (cube, cone, cylinder, sphere, quad)
     */
    void Mesh::initializeDefaultShapes() {
        auto cube = Cube(1);
        auto cone = Cone(16, 8);
        auto cylinder = Cylinder(16, 1);
        auto sphere = Sphere(16, 16);
        auto quad = Quad(1);

        loadPrimitive("cube", cube);
        loadPrimitive("cone", cone);
        loadPrimitive("cylinder", cylinder);
        loadPrimitive("sphere", sphere);
        loadPrimitive("quad", quad);
    }

    /**
     * Loads a DrawShape from raw buffer data and indices.
     * Uses indexed rendering for better performance and memory efficiency.
     * @param buffer_data The interleaved vertex buffer data (positions, normals, texcoords)
     * @param indices The index buffer data
     * @return DrawShape struct containing the loaded OpenGL draw data
     */
    static DrawShape loadStaticShapeIndexed(const std::vector<float>& buffer_data, const std::vector<unsigned int>& indices) {
        constexpr int attribute_size = (3 + 3 + 2); // pos + normal + texcoord
        constexpr GLsizei stride = attribute_size * sizeof(float);

        GLuint vao, vbo, ebo;

        // Generate and bind VAO
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Generate and setup VBO
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, buffer_data.size() * sizeof(float), buffer_data.data(), GL_STATIC_DRAW);

        // Generate and setup EBO (must be done while VAO is bound)
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Setup vertex attributes
        glEnableVertexAttribArray(0); // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)(0));

        glEnableVertexAttribArray(1); // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

        glEnableVertexAttribArray(2); // texcoord
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

        // Unbind
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // Calculate bounding box from unique vertices
        glm::vec3 min(FLT_MAX);
        glm::vec3 max(-FLT_MAX);
        for (size_t i = 0; i < buffer_data.size(); i += attribute_size) {
            glm::vec3 v(buffer_data[i], buffer_data[i + 1], buffer_data[i + 2]);
            min = glm::min(min, v);
            max = glm::max(max, v);
        }

        int num_triangles = static_cast<int>(indices.size()) / 3;
        DrawShape shape = {vao, vbo, ebo, num_triangles, min, max};



        return shape;
    }




    /**
     * Loads a DrawShape from a Primitive object.
     * @param name The name to associate with the loaded shape
     * @param primitive The Primitive object to load the shape from
     * @return DrawShape struct containing the loaded OpenGL draw data
     */
    DrawShape* Mesh::loadPrimitive(const std::string& name, const Primitive& primitive) {

        auto shape = loadStaticShapeIndexed(primitive.getBufferData(), primitive.getIndices());
        if (shapes_.contains(name)) {
            debug::warn("Shape '{}' already exists", name);
            return shapes_.at(name).get();
        }
        shapes_.emplace(name, std::make_unique<DrawShape>(shape));
        return shapes_.at(name).get();
    }

    constexpr unsigned int IMPORT_PRESET =  aiProcess_Triangulate |
                                            aiProcess_JoinIdenticalVertices |
                                            aiProcess_OptimizeMeshes |
                                            aiProcess_GenNormals |
                                            aiProcess_CalcTangentSpace |
                                            aiProcess_FlipUVs | // since OpenGL's UVs are flipped
                                            aiProcess_LimitBoneWeights; // Limit bone weights to 4 per vertex

    /**
     * Loads a static mesh from file, supporting various formats (OBJ, FBX, etc.)
     * See also @code loadStaticMesh(const char* filename, std::vector<glm::vec3>& out_vertices, std::vector<glm::ivec3>& out_faces)@endcode
     * for loading vertex and face data for collision / navmesh generation.
     * @param filename The file path to the mesh from project root, e.g "Resources/Models/model.obj"
     * @return DrawMesh struct containing the loaded mesh data
     */
    DrawMesh* Mesh::loadStaticMesh(const char* filename) {
        std::vector<glm::vec3> vertices;
        std::vector<glm::ivec3> faces;
        return loadStaticMesh(filename, vertices, faces);

    }

    /**
     * Loads a static mesh from file, supporting various formats (OBJ, FBX, etc.)
     * @param filename The file path to the mesh from project root, e.g "Resources/Models/model.obj"
     * @param out_vertices Output vector to store loaded vertex positions for collision / navmesh generation
     * @param out_faces Output vector to store loaded face indices for collision / navmesh generation
     * @return DrawMesh struct containing the loaded mesh OpenGL data
     */
    DrawMesh* Mesh::loadStaticMesh(const char* filename, std::vector<glm::vec3>& out_vertices,
                                  std::vector<glm::ivec3>& out_faces) {

        if (static_meshes_.contains(filename)) {
            return static_meshes_.at(filename).get();
        }
        // Get directory for loading materials (relative to project root)
        auto directory = file::getDirectory(filename);

        // Get absolute path for Assimp
        auto full_path = file::getPath(filename);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(full_path, IMPORT_PRESET);

        DrawMesh mesh;
        glm::vec3 min(std::numeric_limits<float>::max());
        glm::vec3 max(std::numeric_limits<float>::lowest());

        auto materials = Material::loadSceneMaterials(scene, directory);

        unsigned int num_vertices = 0;
        unsigned int num_faces = 0;
        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            num_vertices += scene->mMeshes[i]->mNumVertices;
            num_faces += scene->mMeshes[i]->mNumFaces;
        }
        out_vertices.reserve(num_vertices);
        out_faces.reserve(num_faces);

        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            const aiMesh* aimesh = scene->mMeshes[i];

            if (!aimesh->HasNormals() || !aimesh->HasPositions()) {
                continue;
            }

            //TEMP: Compute local bounds for this mesh so we can center it
            glm::vec3 localMin(std::numeric_limits<float>::max());
            glm::vec3 localMax(std::numeric_limits<float>::lowest());

            for (unsigned int v = 0; v < aimesh->mNumVertices; v++) {
                const aiVector3D& pos = aimesh->mVertices[v];
                glm::vec3 p(pos.x, pos.y, pos.z);
                localMin = glm::min(localMin, p);
                localMax = glm::max(localMax, p);
            }

            glm::vec3 center = 0.5f * (localMin + localMax);

            // Build index buffer from faces
            std::vector<unsigned int> indices;
            indices.reserve(aimesh->mNumFaces * 3);

            for (unsigned int f = 0; f < aimesh->mNumFaces; f++) {
                const aiFace& face = aimesh->mFaces[f];
                glm::ivec3 face_indices;
                for (int v = 0; v < face.mNumIndices; v++) {
                    indices.push_back(face.mIndices[v]);
                    face_indices[v] = face.mIndices[v] + static_cast<int>(out_vertices.size());
                }
                out_faces.push_back(face_indices);
            }

            // Build vertex buffer from unique vertices (indexed approach)
            std::vector<float> vertices;
            vertices.reserve(aimesh->mNumVertices * 8); // pos(3) + normal(3) + texcoord(2)

            for (unsigned int v = 0; v < aimesh->mNumVertices; v++) {
                const aiVector3D& pos = aimesh->mVertices[v];
                const aiVector3D& normal = aimesh->mNormals[v];
                const aiVector3D& texcoord = aimesh->HasTextureCoords(0) ?
                    aimesh->mTextureCoords[0][v] : aiVector3D(0.0f, 0.0f, 0.0f);

                out_vertices.emplace_back(pos.x, pos.y, pos.z);

                // Position
                vertices.push_back(pos.x);
                vertices.push_back(pos.y);
                vertices.push_back(pos.z);

                // TEMP:
                /*glm::vec3 centered = glm::vec3(pos.x, pos.y, pos.z) - center;
                out_vertices.emplace_back(centered.x, centered.y, centered.z);
                vertices.push_back(centered.x);
                vertices.push_back(centered.y);
                vertices.push_back(centered.z);*/

                // Normal
                vertices.push_back(normal.x);
                vertices.push_back(normal.y);
                vertices.push_back(normal.z);

                // Texcoord
                vertices.push_back(texcoord.x);
                vertices.push_back(texcoord.y);
            }


            auto material_name = std::string(scene->mMaterials[aimesh->mMaterialIndex]->GetName().C_Str());

            DrawObject object = {
                loadStaticShapeIndexed(vertices, indices),
                materials[material_name]
            };
            mesh.objects.push_back(object);
            min = glm::min(min, object.shape.min);
            max = glm::max(max, object.shape.max);
        }
        mesh.min = min;
        mesh.max = max;

        static_meshes_.emplace(filename,std::make_unique<DrawMesh>(mesh));

        return static_meshes_.at(filename).get();
    }

    /**
     * Loads a skinned DrawShape from buffer data and indices.
     * @return The loaded DrawShape with skinned vertex attributes
     */
    DrawShape loadSkinnedShape(
        const std::vector<glm::vec3>& positions,
        const std::vector<glm::vec3>& normals,
        const std::vector<glm::vec2>& texcoords,
        const std::vector<BoneIDs>& bone_ids,
        const std::vector<BoneWeights>& bone_weights,
        const std::vector<unsigned int>& indices) {

        GLuint vao, vbo, ebo;

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Calculate interleaved buffer size
        const size_t num_vertices = positions.size();
        constexpr size_t floats_per_vertex = 3 + 3 + 2;  // pos + normal + texcoord
        constexpr size_t uints_per_vertex = 4;           // bone IDs
        constexpr size_t weights_per_vertex = 4;         // bone weights

        // Create interleaved VBO: [pos(3), normal(3), texcoord(2), bone_ids(4), bone_weights(4)] per vertex
        std::vector<float> interleaved_data;
        interleaved_data.reserve(num_vertices * (floats_per_vertex + weights_per_vertex));

        std::vector<unsigned int> bone_id_data;
        bone_id_data.reserve(num_vertices * uints_per_vertex);

        for (size_t i = 0; i < num_vertices; i++) {
            // Position
            interleaved_data.push_back(positions[i].x);
            interleaved_data.push_back(positions[i].y);
            interleaved_data.push_back(positions[i].z);

            // Normal
            interleaved_data.push_back(normals[i].x);
            interleaved_data.push_back(normals[i].y);
            interleaved_data.push_back(normals[i].z);

            // Texcoord
            interleaved_data.push_back(texcoords[i].x);
            interleaved_data.push_back(texcoords[i].y);

            // Bone weights (after float data)
            interleaved_data.push_back(bone_weights[i][0]);
            interleaved_data.push_back(bone_weights[i][1]);
            interleaved_data.push_back(bone_weights[i][2]);
            interleaved_data.push_back(bone_weights[i][3]);

            // Bone IDs (stored separately since they're integers)
            bone_id_data.push_back(bone_ids[i][0]);
            bone_id_data.push_back(bone_ids[i][1]);
            bone_id_data.push_back(bone_ids[i][2]);
            bone_id_data.push_back(bone_ids[i][3]);
        }

        constexpr GLsizei stride = (floats_per_vertex + weights_per_vertex) * sizeof(float);

        // Create VBO for float data (pos, normal, texcoord, weights)
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, interleaved_data.size() * sizeof(float), interleaved_data.data(), GL_STATIC_DRAW);

        // Position (attribute 0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

        // Normal (attribute 1)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

        // Texcoord (attribute 2)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

        // Bone weights (attribute 4)
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));

        // Create separate VBO for bone IDs (integers)
        GLuint vbo_bone_ids;
        glGenBuffers(1, &vbo_bone_ids);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_bone_ids);
        glBufferData(GL_ARRAY_BUFFER, bone_id_data.size() * sizeof(unsigned int), bone_id_data.data(), GL_STATIC_DRAW);

        // Bone IDs (attribute 3) - must use glVertexAttribIPointer for integers
        glEnableVertexAttribArray(3);
        glVertexAttribIPointer(3, 4, GL_UNSIGNED_INT, 0, nullptr);

        // Create and setup EBO
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Unbind VAO (preserves EBO binding)
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Calculate bounding box
        glm::vec3 bmin(std::numeric_limits<float>::max());
        glm::vec3 bmax(std::numeric_limits<float>::lowest());
        for (const auto& pos : positions) {
            bmin = glm::min(bmin, pos);
            bmax = glm::max(bmax, pos);
        }

        int num_triangles = indices.size() / 3;
        DrawShape shape = {vao, vbo, ebo, num_triangles, bmin, bmax};

        return shape;
    }

    static glm::mat4 aiToGlmMat4(const aiMatrix4x4& mat) {
        return {
            mat.a1, mat.b1, mat.c1, mat.d1,
            mat.a2, mat.b2, mat.c2, mat.d2,
            mat.a3, mat.b3, mat.c3, mat.d3,
            mat.a4, mat.b4, mat.c4, mat.d4
        };
    }


    glm::mat4 getBoneMatrix(const std::string& bone_name, const aiScene* scene) {
        if (auto bone = scene->findBone(aiString(bone_name))) {
            return aiToGlmMat4(bone->mOffsetMatrix);
        }

        debug::error("Failed to find bone {}", bone_name);
        return {1.0f};
    }

    void createHierarchy(Skeleton& skeleton, const aiScene* scene, const std::unordered_set<std::string>& bone_names) {

        for (const auto& bone_name : bone_names) {
            auto node = scene->mRootNode->FindNode(aiString(bone_name));
            if (node && node->mParent) {
                std::string parent_name = node->mParent->mName.C_Str();
                if (bone_names.contains(parent_name)) {
                    unsigned int bone_id = skeleton.bone_map_[bone_name];
                    int parent_id = skeleton.bone_map_[parent_name];
                    skeleton.bones_[bone_id].parent_id = parent_id;
                    skeleton.bones_[parent_id].addChild(skeleton.bones_[bone_id]);
                }
            }
        }

    }

    void constructSkeleton(Skeleton& skeleton, const aiNode* curr_node, const aiScene* scene,
                          int parent_id, const glm::mat4& parent_global_transform) {
        aiBone* ai_bone = scene->findBone(curr_node->mName);
        const glm::mat4 node_local = aiToGlmMat4(curr_node->mTransformation);
        const glm::mat4 node_global = parent_global_transform * node_local;
        const int current_id = static_cast<int>(skeleton.bones_.size());
        if (ai_bone) {

            const glm::mat4 offset_matrix = aiToGlmMat4(ai_bone->mOffsetMatrix);
            // The offset matrix is the INVERSE of the bind pose global transform
            // So the bind pose global transform is the inverse of the offset matrix
            const glm::mat4 bind_pose_global = glm::inverse(offset_matrix);
            // Determine the local transform to store
            glm::mat4 local_transform;
            if (parent_id == -1) {
                // Root bone: local_transform IS the bind pose global transform
                local_transform = bind_pose_global;
            } else {
                // Child bone: extract local transform by dividing out parent's bind pose
                // child_global = parent_global × child_local
                // child_local = inverse(parent_global) × child_global
                const glm::mat4& parent_bind_global = glm::inverse(skeleton.bones_[parent_id].offset_matrix);
                local_transform = glm::inverse(parent_bind_global) * bind_pose_global;
            }

            skeleton.addBone(curr_node->mName.C_Str(), current_id, parent_id, offset_matrix, local_transform);

            // Continue to children with this bone as parent (use node_global for hierarchy traversal)
            for (size_t i = 0; i < curr_node->mNumChildren; i++) {
                constructSkeleton(skeleton, curr_node->mChildren[i], scene, current_id, node_global);
            }
        }
        else {
            skeleton.addBone(curr_node->mName.C_Str(), current_id, parent_id,
                             glm::mat4(1.0f), node_local, true); // Virtual bone
            for (size_t i = 0; i < curr_node->mNumChildren; i++) {
                constructSkeleton(skeleton, curr_node->mChildren[i], scene, current_id, node_global);
            }
        }
    }

    constexpr unsigned int SKINNED_IMPORT_PRESET =  aiProcess_Triangulate |
                                                    aiProcess_JoinIdenticalVertices |
                                                    aiProcess_OptimizeMeshes |
                                                    aiProcess_GenNormals |
                                                    aiProcess_CalcTangentSpace |
                                                    aiProcess_FlipUVs | // since OpenGL's UVs are flipped
                                                    aiProcess_LimitBoneWeights |  // Limit bone weights to 4 per vertex
                                                    aiProcess_GlobalScale; // Normalizes scale of model


    Skeleton loadSkeleton(const aiScene* scene) {
        Skeleton skeleton;
        std::unordered_set<std::string> bone_names;

        for (int i=0; i < scene->mNumMeshes; i++) {
            const aiMesh* aimesh = scene->mMeshes[i];
            if (!aimesh->HasBones()) { continue; }


            for (size_t b = 0; b < aimesh->mNumBones; b++) {
                const aiBone* bone = aimesh->mBones[b];

                std::string bone_name = bone->mName.C_Str();
                if (!bone_names.contains(bone_name)) {
                    bone_names.insert(bone_name);
                }
            }
        }

        constructSkeleton(skeleton, scene->mRootNode, scene, -1, glm::mat4(1.0f));
        return skeleton;
    }

    unsigned int findRootAnimationBone(const Animation& animation, const Skeleton& skeleton, const Bone& curr_bone) {
        if (skeleton.bones_.empty()) { return 0; }

        while (!animation.channels.contains(curr_bone.id)) {
            if (curr_bone.children.empty()) {
                debug::error("Failed to find root animation bone in skeleton.");
                return 0;
            }
            for (const auto& child_bone : curr_bone.children) {
                return findRootAnimationBone(animation, skeleton, skeleton.bones_[child_bone]);
            }
        }
        return curr_bone.id;
    }

    static void loadAnimationsFromScene(const aiScene* scene, Skeleton& skeleton, const std::string& animation_name= "") {
        if (!scene->HasAnimations()) return;
        std::unordered_map<std::string, Animation> animations_map;
        for (size_t i = 0; i < scene->mNumAnimations; i++) {
            const aiAnimation* ai_anim = scene->mAnimations[i];


            Animation animation;
            animation.ticks_per_second = ai_anim->mTicksPerSecond != 0.0 ? ai_anim->mTicksPerSecond : 25.0;
            animation.duration = ai_anim->mDuration;
            for (size_t c = 0; c < ai_anim->mNumChannels; c++) {
                const aiNodeAnim* ai_channel = ai_anim->mChannels[c];
                const std::string bone_name = ai_channel->mNodeName.C_Str();
                AnimationChannel channel;

                // Skip channels for bones not in skeleton
                if (!skeleton.bone_map_.contains(bone_name)) {
                    debug::print("Skipping animation channel for unknown bone: {}", bone_name);
                    continue;
                }

                channel.bone_id = skeleton.bone_map_.at(bone_name);
                channel.bone_name = bone_name;

                // Position keys
                for (size_t pk = 0; pk < ai_channel->mNumPositionKeys; pk++) {
                    const aiVectorKey& ai_pos_key = ai_channel->mPositionKeys[pk];
                    PositionKey pos_key;
                    pos_key.time = ai_pos_key.mTime;
                    pos_key.value = glm::vec3(ai_pos_key.mValue.x, ai_pos_key.mValue.y, ai_pos_key.mValue.z);
                    channel.position_keys.push_back(pos_key);
                }

                // Rotation keys
                for (size_t rk = 0; rk < ai_channel->mNumRotationKeys; rk++) {
                    const aiQuatKey& ai_rot_key = ai_channel->mRotationKeys[rk];
                    RotationKey rot_key;
                    rot_key.time = ai_rot_key.mTime;
                    rot_key.value = glm::quat(ai_rot_key.mValue.w, ai_rot_key.mValue.x, ai_rot_key.mValue.y, ai_rot_key.mValue.z);
                    channel.rotation_keys.push_back(rot_key);
                }

                // Scale keys
                for (size_t sk = 0; sk < ai_channel->mNumScalingKeys; sk++) {
                    const aiVectorKey& ai_scale_key = ai_channel->mScalingKeys[sk];
                    ScaleKey scale_key;
                    scale_key.time = ai_scale_key.mTime;
                    scale_key.value = glm::vec3(ai_scale_key.mValue.x, ai_scale_key.mValue.y,  ai_scale_key.mValue.z);
                    channel.scale_keys.push_back(scale_key);
                }

                animation.channels[channel.bone_id] = channel;
            }

            animation.root_bone_id = findRootAnimationBone(animation, skeleton, skeleton.bones_[0]);
            animations_map[ai_anim->mName.C_Str()] = animation;
        }

        for (const auto& [name, anim] : animations_map) {
            auto anim_name = name;
            if (!animation_name.empty()) {
                anim_name = animation_name;
            }
            auto n = anim_name;
            int idx = 0;
            while (skeleton.animations_name_to_index.contains(n)) {
                n = anim_name + "_" + std::to_string(idx++);
            }

            skeleton.animations_name_to_index[n] = skeleton.animations_.size();
            skeleton.animations_.push_back(anim);
        }
    }


    /**
     * Loads animations from a file into the provided skeleton. Note that the skeleton bone names must match those
     * in the animation file for proper mapping. Useful for adding multiple mixamo animations to a single skeleton.
     * @param filename - The file path to the animation file from project root, e.g "Resources/Models/animation.fbx"
     * @param skeleton - The Skeleton object to load animations into
     */
    void Mesh::loadAnimations(const char* filename, Skeleton& skeleton) {
        Assimp::Importer importer;
        importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

        auto path = file::getPath(filename);
        const aiScene* scene = importer.ReadFile(path,SKINNED_IMPORT_PRESET);     // Calculate tangent space for normal mapping

        if (!scene || !scene->mRootNode) {
            debug::error("Failed to load FBX: {}", importer.GetErrorString());

            return;
        }
        loadAnimationsFromScene(scene,skeleton);
    }

    DrawShape* Mesh::getLoadedShape(const std::string& name) {
        if (shapes_.contains(name)) {
            return shapes_.at(name).get();
        }
        debug::error("Shape '{}' not found", name);
        return nullptr;
    }

    DrawMesh* Mesh::getLoadedStaticMesh(const std::string& filename) {
        if (static_meshes_.contains(filename)) {
            return static_meshes_.at(filename).get();
        }
        debug::error("Static mesh '{}' not found", filename);
        return nullptr;
    }

    SkinnedMesh* Mesh::getLoadedSkinnedMesh(const std::string& filename) {
        if (skinned_meshes_.contains(filename)) {
            return skinned_meshes_.at(filename).get();
        }
        debug::error("Skinned mesh '{}' not found", filename);
        return nullptr;
    }


    SkinnedMesh* Mesh::loadSkinnedMesh(const char* filename) {
        if (skinned_meshes_.contains(filename)) return skinned_meshes_.at(filename).get();
        auto directory = file::getDirectory(filename);

        Assimp::Importer importer;
        importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

        auto path = file::getPath(filename);
        const aiScene* scene = importer.ReadFile(path,SKINNED_IMPORT_PRESET);     // Calculate tangent space for normal mapping

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            debug::error("Failed to load FBX: {}", importer.GetErrorString());
            return {};
        }



        auto skeleton = loadSkeleton(scene);
        skeleton.updateBoneMatrices();  // Calculate bone matrices for bind pose
        loadAnimationsFromScene(scene, skeleton);

        auto materials = Material::loadSceneMaterials(scene, directory);
        DrawMesh mesh;
        for (size_t i=0; i<scene->mNumMeshes; i++) {
            const aiMesh* aimesh = scene->mMeshes[i];
            if (!aimesh->HasBones()) { continue; }

            for (auto vi =0; vi < aimesh->mNumVertices; vi++) {
                const aiVector3D& pos = aimesh->mVertices[vi];
                skeleton.vertices_.emplace_back(pos.x, pos.y, pos.z);
            }

            for (auto fi = 0; fi < aimesh->mNumFaces; fi++) {
                const aiFace& face = aimesh->mFaces[fi];
                skeleton.faces_.emplace_back(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
            }

            std::vector<glm::vec3> vertices(aimesh->mNumVertices);
            std::vector<BoneIDs> bone_ids(aimesh->mNumVertices, BoneIDs{});
            std::vector<BoneWeights> bone_weights(aimesh->mNumVertices, BoneWeights{});



            for (size_t b = 0; b < aimesh->mNumBones; b++) { // populate bone data
                const aiBone* bone = aimesh->mBones[b];
                auto bone_id = skeleton.bone_map_[bone->mName.C_Str()];



                for (size_t w = 0; w < bone->mNumWeights; w++) {
                    const aiVertexWeight& weight = bone->mWeights[w];
                    const size_t vertex_id = weight.mVertexId;

                    auto& s_bone = skeleton.bones_[bone_id];
                    s_bone.vertex_weights[vertex_id] = weight.mWeight;
                    skeleton.vertex_to_boneID_map_[vertex_id].push_back(bone_id);


                    for (size_t j = 0; j < MAX_BONES_PER_VERTEX; j++) {
                        if (bone_weights[vertex_id][j] == 0.0f) {
                            bone_ids[vertex_id][j] = bone_id;
                            bone_weights[vertex_id][j] = weight.mWeight;
                            break;
                        }
                    }
                }
            }

            // Normalize bone weights, default to 1.0 for root bone if no weights
            for (auto& b_w : bone_weights) {
                if (b_w == BoneWeights{}) {
                    b_w[0] = 1.0f;
                } else {
                    float sum = 0;
                    for (auto w : b_w) sum += w;
                    if (sum != 1.0f) for (auto& w : b_w) w /= sum; // normalize
                }
            }

            // Build unique vertex data (indexed approach)
            std::vector<glm::vec3> positions;
            std::vector<glm::vec3> normals;
            std::vector<glm::vec2> texcoords;
            std::vector<BoneIDs> vertex_bone_ids;
            std::vector<BoneWeights> vertex_bone_weights;

            positions.reserve(aimesh->mNumVertices);
            normals.reserve(aimesh->mNumVertices);
            texcoords.reserve(aimesh->mNumVertices);
            vertex_bone_ids.reserve(aimesh->mNumVertices);
            vertex_bone_weights.reserve(aimesh->mNumVertices);

            // Store unique vertices
            for (unsigned int v = 0; v < aimesh->mNumVertices; v++) {
                const aiVector3D& pos = aimesh->mVertices[v];
                const aiVector3D& normal = aimesh->mNormals[v];
                const aiVector3D& texcoord = aimesh->HasTextureCoords(0) ?
                    aimesh->mTextureCoords[0][v] : aiVector3D(0.0f, 0.0f, 0.0f);

                vertices[v] = glm::vec3(pos.x, pos.y, pos.z);
                positions.emplace_back(pos.x, pos.y, pos.z);
                normals.emplace_back(normal.x, normal.y, normal.z);
                texcoords.emplace_back(texcoord.x, texcoord.y);
                vertex_bone_ids.push_back(bone_ids[v]);
                vertex_bone_weights.push_back(bone_weights[v]);
            }

            // Build index buffer from faces
            std::vector<unsigned int> indices;
            indices.reserve(aimesh->mNumFaces * 3);

            for (unsigned int f = 0; f < aimesh->mNumFaces; f++) {
                const aiFace& face = aimesh->mFaces[f];
                for (unsigned int v = 0; v < face.mNumIndices; v++) {
                    indices.push_back(face.mIndices[v]);
                }
            }

            auto material_name = scene->mMaterials[aimesh->mMaterialIndex]->GetName().C_Str();
            DrawObject object = {
                loadSkinnedShape(positions, normals, texcoords, vertex_bone_ids, vertex_bone_weights, indices),
                materials[material_name]
            };

            mesh.min = glm::min(mesh.min, object.shape.min);
            mesh.max = glm::max(mesh.max, object.shape.max);
            mesh.objects.push_back(object);

        }

        skeleton.updateBoneMatrices();

        skinned_meshes_.emplace(filename, std::make_unique<SkinnedMesh>(SkinnedMesh{mesh, skeleton}));
        return skinned_meshes_.at(filename).get();
    }


    void unloadShape(const DrawShape& shape) {
        glDeleteVertexArrays(1, &shape.vao);
        glDeleteBuffers(1, &shape.vbo);
        glDeleteBuffers(1, &shape.ebo);
    }

    /**
     * Deletes all OpenGL vao, vbo and ebo from loaded shapes/meshes
     */
    void Mesh::unloadAllMeshes() {
        for (const auto& [name, shape] : shapes_) {
            unloadShape(*shape);
        }
        for (const auto& [name, mesh] : static_meshes_) {
            for (auto& [shape, material] : mesh->objects) {
                unloadShape(shape);
            }
        }

        for (const auto& [name, skinned_mesh] : skinned_meshes_) {
            for (auto& [shape, material] : skinned_mesh->draw_mesh.objects) {
                unloadShape(shape);

            }
        }
    }
}
