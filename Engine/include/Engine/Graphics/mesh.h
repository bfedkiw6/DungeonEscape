#pragma once

#include "Engine/Graphics/material.h"
#include "Engine/Graphics/Skinned/skeleton.h"

#include <memory>
#include <string>

/**
 * This file contains structures and a class for loading and managing 3D mesh data for rendering.
 */
namespace gl {

    /**
     * Represents a drawable shape with associated OpenGL buffers and bounding box.
     */
    struct DrawShape {
        const GLuint vao = 0;
        const GLuint vbo = 0; // vertex buffer id
        const GLuint ebo = 0; // element buffer id (for indexed rendering)
        const int numTriangles = 0;
        glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());

        GLuint instance_vbo = 0;
    };

    /**
     * Represents a drawable object consisting of a shape and its material.
     */
    struct DrawObject {
        const DrawShape shape;
        DrawMaterial material = DEFAULT_MATERIAL;
    };

    /**
     * Represents a mesh composed of multiple drawable objects and its overall bounding box.
     */
    struct DrawMesh {
        std::vector<DrawObject> objects;
        glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());
    };

    struct SkinnedMesh {
        DrawMesh draw_mesh;
        Skeleton skeleton;
    };

    class Primitive;
    /**
     * Mesh class is responsible for loading static and skinned meshes from files or primitives.
     */

    class Mesh {
    public:

        static void initializeDefaultShapes();

        static DrawShape* loadPrimitive(const std::string& name, const Primitive& primitive);
        static DrawMesh* loadStaticMesh(const char* filename);
        static DrawMesh* loadStaticMesh(const char* filename, std::vector<glm::vec3>& out_vertices, std::vector<glm::ivec3>& out_faces);

        // For skinned meshes with animations - will be covered in Lab 5
        static SkinnedMesh* loadSkinnedMesh(const char* filename);
        static void loadAnimations(const char* filename, Skeleton& skeleton);


        static DrawShape* getLoadedShape(const std::string& name);
        static DrawMesh* getLoadedStaticMesh(const std::string& filename);
        static SkinnedMesh* getLoadedSkinnedMesh(const std::string& filename);

        static void unloadAllMeshes();

    private:
        static std::unordered_map<std::string, std::unique_ptr<DrawShape>> shapes_;
        static std::unordered_map<std::string, std::unique_ptr<DrawMesh>> static_meshes_;
        static std::unordered_map<std::string, std::unique_ptr<SkinnedMesh>> skinned_meshes_;
    };
}
