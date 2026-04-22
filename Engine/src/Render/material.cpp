#include "Engine/Graphics/material.h"

#include <assimp/material.h>
#include <assimp/scene.h>
#include <assimp/texture.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Engine/debug.h"
#include "Engine/file.h"

namespace gl {

    std::unordered_map<std::string, GLuint> Material::loaded_textures_;
    GLuint Material::blank_texture_ = 0;

    DrawMaterial Material::loadMaterialFromScene(const aiScene* scene, const aiMaterial* material, const std::string& directory) {
        aiColor3D ambient(0.f, 0.f, 0.f);
        aiColor3D diffuse(0.f, 0.f, 0.f);
        aiColor3D specular(0.f, 0.f, 0.f);
        float shininess = 0.0f;
        float opacity = 1.0f;

        material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
        material->Get(AI_MATKEY_SHININESS, shininess);
        material->Get(AI_MATKEY_OPACITY, opacity);

        DrawMaterial draw_material;
        draw_material.ambient = glm::vec3(ambient.r, ambient.g, ambient.b);
        draw_material.diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
        draw_material.specular = glm::vec3(specular.r, specular.g, specular.b);
        draw_material.shininess = shininess;
        draw_material.opacity = opacity;
        draw_material.textures = loadMaterialTextures(scene, material, directory);

        return draw_material;
    }




    GLTextures Material::loadTexture(const char* diffuse_path, const char* specular_path) {
        return {loadFromFile(diffuse_path), loadFromFile(specular_path)};
    }

    std::unordered_map<std::string, DrawMaterial> Material::loadSceneMaterials(const aiScene* scene,const std::string& directory) {
        std::unordered_map<std::string, DrawMaterial> materials;
        for (size_t i = 0; i < scene->mNumMaterials; i++) {
            const aiMaterial* material = scene->mMaterials[i];

            const std::string material_name = std::string(material->GetName().C_Str());
            materials[material_name] = loadMaterialFromScene(scene, material, directory);
        }
        return materials;
    }

    GLuint Material::getLoadedTexture(const std::string& path) {
        if (loaded_textures_.contains(path)) {
            return loaded_textures_[path];
        }
        debug::error("Texture '{}' not already loaded", path);
        return 0; // GL null texture
    }

    void Material::unloadAllTextures() {
        for (const auto& [_, texture_id] : loaded_textures_) {
            glDeleteTextures(1, &texture_id);
        }
        glDeleteTextures(1, &blank_texture_);
        loaded_textures_.clear();
    }

    void Material::initializeBlankTexture() {
        glGenTextures(1, &blank_texture_);
        glBindTexture(GL_TEXTURE_2D, blank_texture_);
        unsigned char white_pixel[4] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white_pixel);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLuint Material::getBlankTexture() {
        return blank_texture_;
    }

    GLuint Material::loadTextureFromScene(const aiScene* scene, const aiString* tex_string, const std::string& directory) {
        // Check if texture is embedded
        if (const auto texture = scene->GetEmbeddedTexture(tex_string->C_Str())) {
            return loadEmbedded(texture);
        }
        return loadFromFile(tex_string, directory);

    }

    GLuint Material::loadEmbedded(const aiTexture* texture) {
        const auto tex_name = texture->mFilename.C_Str();
        if (loaded_textures_.contains(tex_name)) {
            return loaded_textures_[tex_name];
        }
        int width, height, channels;
        void* image = stbi_load_from_memory((const stbi_uc*)texture->pcData, texture->mWidth, &width, &height, &channels, 0);
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        GLenum format = GL_RGB;
        if (channels == 1) format = GL_RED;
        else if (channels == 2) format = GL_RG;
        else if (channels == 4) format = GL_RGBA;

        // Set pixel alignment to 1 byte to handle textures with non-4-byte-aligned rows
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Restore default alignment

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(image);

        loaded_textures_[tex_name] = textureID;
        return loaded_textures_[tex_name];
    }

    GLuint Material::loadFromFile(const std::string& path) {

        if (path.empty()) return 0; // GL null texture
        if (loaded_textures_.contains(path)) {
            return loaded_textures_[path];
        }

        const std::string full_path = file::getPath(path);
        GLuint texture_id;
        int w, h, comp;
        unsigned char* image = stbi_load(full_path.c_str(), &w, &h, &comp, STBI_default);
        if (!image) {
            debug::error("Unable to load texture at: {}", full_path);
            return 0; // GL null texture
        }

        debug::print("Loaded texture: {}", path);

        GLint format;
        if (comp == 1) format = GL_RED;
        else if (comp == 2) format = GL_RG;
        else if (comp == 3) format = GL_RGB;
        else if (comp == 4) format = GL_RGBA;
        else {
            debug::error("Unsupported texture format for: {}, format: {}", path, comp);
            return 0; // GL null texture
        }

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Set pixel alignment to 1 byte to handle textures with non-4-byte-aligned rows
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, image);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Restore default alignment

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
        stbi_image_free(image); // Free image memory

        loaded_textures_[path] = texture_id;
        return texture_id;
    }

    GLuint Material::loadFromFile(const aiString* ai_string, const std::string& directory) {
        std::string tex_name = ai_string->C_Str();
        if (tex_name.empty()) return 0; // GL null texture


        namespace fs = std::filesystem;

        // Normalize the texture name (removes leading ./ or .\)
        std::string normalized_tex_name = file::normalizePath(tex_name);

        // Combine directory and texture name using filesystem
        auto tex_path = (fs::path(directory) / normalized_tex_name).lexically_normal().string();
        return loadFromFile(tex_path);
    }

    GLTextures Material::loadMaterialTextures(const aiScene* scene, const aiMaterial* material, const std::string& directory) {
        aiString diffuse_texture;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuse_texture);
        aiString specular_texture;
        material->GetTexture(aiTextureType_SPECULAR, 0, &specular_texture);

        GLTextures textures;
        textures.diffuse = loadTextureFromScene(scene, &diffuse_texture, directory);
        textures.specular = loadTextureFromScene(scene, &specular_texture, directory);
        return textures;
    }

}
