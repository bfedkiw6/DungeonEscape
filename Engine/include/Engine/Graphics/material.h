#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

struct aiScene;
struct aiString;
struct aiMaterial;
struct aiTexture;

namespace gl {

    constexpr int TEXTURE_UNIT_DIFFUSE  = 0;
    constexpr int TEXTURE_UNIT_SPECULAR = 1;


    struct GLTextures {
        GLuint diffuse = 0;
        GLuint specular = 0;
    };

    struct DrawMaterial {

        glm::vec3 ambient = glm::vec3(0.5f);
        glm::vec3 diffuse = glm::vec3(0.5f);
        glm::vec3 specular = glm::vec3(0.5f);
        float shininess = 100.0f;
        float opacity = 1.0f;
        GLTextures textures = {};
    };

    static constexpr DrawMaterial DEFAULT_MATERIAL = {};

    class Material {
    public:


        static GLTextures loadTexture(const char* diffuse_path, const char* specular_path = "");
        static std::unordered_map<std::string, DrawMaterial> loadSceneMaterials(const aiScene* scene, const std::string& directory);

        static GLuint getLoadedTexture(const std::string& path);
        static void initializeBlankTexture();
        static GLuint getBlankTexture();

        static void unloadAllTextures();

    private:
        static DrawMaterial loadMaterialFromScene(const aiScene* scene, const aiMaterial* material, const std::string& directory);
        static GLuint loadTextureFromScene(const aiScene* scene, const aiString* tex_string, const std::string& directory);
        static GLuint loadEmbedded(const aiTexture* texture);

        static GLuint loadFromFile(const std::string& path);
        static GLuint loadFromFile(const aiString* ai_string, const std::string& directory);
        static GLTextures loadMaterialTextures(const aiScene* scene, const aiMaterial* material, const std::string& directory);

        static std::unordered_map<std::string, GLuint> loaded_textures_;
        static GLuint blank_texture_;
    };
}
