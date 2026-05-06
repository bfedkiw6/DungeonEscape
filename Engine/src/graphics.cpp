#include "Engine/Graphics/graphics.h"

#include <string>

#include "Engine/debug.h"
#include "Engine/window.h"
#include "Engine/Graphics/camera.h"
#include "Engine/Graphics/light.h"
#include "Engine/Graphics/shaders.h"

namespace gl {
    ShaderProgram* Graphics::active_shader_;
    ShaderProgram Graphics::phong_;
    ShaderProgram Graphics::skinned_;
    ShaderProgram Graphics::text_;
    ShaderProgram Graphics::phong_instanced_;

    void Graphics::initialize() {
        initializeShaders();

        Mesh::initializeDefaultShapes();
        Material::initializeBlankTexture();
        TextRenderer::initialize();
        //TextRenderer::loadFont("default", "resources/fonts/default.ttf");
        TextRenderer::loadFont("default", "resources/fonts/OldJakarta.ttf");
        TextRenderer::loadFont("magic", "resources/fonts/Magic.ttf");
    }

    void Graphics::tearDown() {
        phong_.deleteProgram();
        skinned_.deleteProgram();
        text_.deleteProgram();
        phong_instanced_.deleteProgram();

        Material::unloadAllTextures();
        Mesh::unloadAllMeshes();
    }

    // ================ Shader Management ================= //
    void Graphics::initializeShaders() {

        const auto frag = "resources/shaders/phong_frag.glsl";
        const auto phong_vert = "resources/shaders/phong_vert.glsl";
        phong_ = Shaders::createShaderProgram(phong_vert, frag, "phong"); //ShaderProgram(vert, frag);

        const auto skinned_vert = "resources/shaders/skinned_vert.glsl";
        const auto phong_frag_id = phong_.getFragmentID();
        skinned_ = Shaders::createShaderProgram(skinned_vert, phong_frag_id, "skinned");

        const auto text_vert = "resources/shaders/text_vert.glsl";
        const auto text_frag = "resources/shaders/text_frag.glsl";
        text_ = Shaders::createShaderProgram(text_vert, text_frag, "text");

        // Use same frag, only vert is different
        const auto phong_instanced_vert = "resources/shaders/phong_instanced_vert.glsl";
        phong_instanced_ = Shaders::createShaderProgram(phong_instanced_vert, frag, "phong_instanced");

        usePhongShader();
    }

    void Graphics::usePhongShader() {
        phong_.use();
        active_shader_ = &phong_;
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPolygonOffset(1.0, 1.0);
    }

    void Graphics::useSkinnedShader() {
        // For skinned mesh rendering - will be covered in Lab 5

        skinned_.use();
        active_shader_ = &skinned_;
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPolygonOffset(1.0, 1.0);
    }

    void Graphics::useTextShader() {
        text_.use();
        active_shader_ = &text_;
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Graphics::usePhongInstancedShader() {
        phong_instanced_.use();
        active_shader_ = &phong_instanced_;
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPolygonOffset(1.0, 1.0);
    }


    void Graphics::clearScreen(const glm::vec3& color) {
        glClearColor(color.r, color.g, color.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }


    // ================ Uniform Setting Functions ================= //
    void Graphics::setCameraUniforms(const Camera* camera) {
        active_shader_->setMat4("view", camera->getViewMatrix());
        active_shader_->setMat4("projection", camera->getProjection());
        active_shader_->setVec3("camera_pos", camera->getPosition());
    }

    void Graphics::setLights(const std::vector<Light>& lights) {
        constexpr int MAX_LIGHTS = 40;
        int num_lights = std::min(static_cast<int>(lights.size()), MAX_LIGHTS);

        active_shader_->setInt("num_lights", num_lights);
        for (int i = 0; i < num_lights; i++) {
            const auto& light = lights[i];
            std::string prefix = "lights[" + std::to_string(i) + "].";

            active_shader_->setInt((prefix + "type").c_str(), light.type_);
            active_shader_->setVec3((prefix + "position").c_str(), light.position_);
            active_shader_->setVec3((prefix + "color").c_str(), light.color_);
            active_shader_->setVec3((prefix + "direction").c_str(), light.direction_);
            active_shader_->setVec3((prefix + "attenuation").c_str(), light.attenuation_);
        }
    }


    void Graphics::setAmbientLight(const glm::vec3& ambient) {
        active_shader_->setVec3("ambient_light", ambient);
    }


    void Graphics::setMaterialUniforms(const DrawMaterial& material) {
        active_shader_->setVec3("ambient", material.ambient);
        active_shader_->setVec3("diffuse", material.diffuse);
        active_shader_->setVec3("specular", material.specular);
        active_shader_->setFloat("shininess", material.shininess);
        active_shader_->setFloat("opacity", material.opacity);

        // if (material.has_height_tex) {
        //     active_shader_->setInt("has_height_tex", true);
        //     active_shader_->setFloat("height_scale", material.height_scale);
        // } else {
        //     active_shader_->setInt("has_height_tex", false);
        // }

        bindMaterialTextures(material.textures);
    }


    // ================ Drawing Functions ================= //
    void Graphics::drawObject(const DrawShape* draw_shape, const Transform& transform, const DrawMaterial& material) {
        const auto model_matrix = transform.getModelMatrix();

        active_shader_->setMat4("model", model_matrix);
        active_shader_->setMat3("normal", glm::transpose(glm::inverse(glm::mat3(model_matrix))));

        setMaterialUniforms(material);
        glBindVertexArray(draw_shape->vao);
        glDrawElements(GL_TRIANGLES, 3 * draw_shape->numTriangles, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void Graphics::drawObject(const DrawObject* draw_object, const Transform& transform) {
        drawObject(&draw_object->shape, transform, draw_object->material);
    }

    void Graphics::drawMesh(const DrawMesh* draw_mesh, const Transform& transform) {
        const auto model_matrix = transform.getModelMatrix();
        active_shader_->setMat4("model", model_matrix);
        active_shader_->setMat3("normal", glm::transpose(glm::inverse(glm::mat3(model_matrix))));

        for (const auto& [shape, material] : draw_mesh->objects) {
            setMaterialUniforms(material);
            glBindVertexArray(shape.vao);
            glDrawElements(GL_TRIANGLES, 3 * shape.numTriangles, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
    }


    void Graphics::drawSkinnedMesh(SkinnedMesh* skinned_mesh, const Transform& transform) {
        // TODO implement correct drawing with bone transformations in Animation lab
        const auto model_matrix = transform.getModelMatrix();
        active_shader_->setMat4("model", model_matrix);
        active_shader_->setMat3("normal", glm::transpose(glm::inverse(glm::mat3(model_matrix))));

        skinned_mesh->skeleton.updateBoneMatrices();
        const std::vector<glm::mat4>& bones = skinned_mesh->skeleton.getBoneMatrices();
        active_shader_->setMat4Vec("bones", skinned_mesh->skeleton.getNumBones(), bones);

        drawMesh(&skinned_mesh->draw_mesh, transform);
    }

    void Graphics::drawText(const std::string& text, const glm::vec2 pos, const float font_size, const glm::vec3& color,
                            const TextAlign align, const std::string& font_name) {
        const auto size = Window::getSize();
        const glm::mat4 projection = glm::ortho(0.0f, size.x, size.y, 0.0f);

        active_shader_->setMat4("projection", projection);
        active_shader_->setVec3("textColor", color);
        active_shader_->setInt("text", 0);
        TextRenderer::drawText(text, pos, font_size, align, font_name);
    }


    // ================ Texture Binding Functions ================= //
    void Graphics::bindMaterialTextures(const GLTextures& textures) {
        bindTexture(textures.diffuse, TEXTURE_UNIT_DIFFUSE, "diffuse_tex");
        bindTexture(textures.specular, TEXTURE_UNIT_SPECULAR, "specular_tex");
        //bindTexture(textures.height, TEXTURE_UNIT_HEIGHT, "height_tex");
    }

    void Graphics::bindTexture(const GLuint texture, const int unit, const char* uniform_name) {
        glActiveTexture(GL_TEXTURE0 + unit);
        auto has_texture_uniform = std::string("has_" + std::string(uniform_name));
        if (texture != 0) {
            active_shader_->setInt(has_texture_uniform.c_str(), true);
            glBindTexture(GL_TEXTURE_2D, texture);
        }
        else {
            active_shader_->setInt(has_texture_uniform.c_str(), false);
            glBindTexture(GL_TEXTURE_2D, Material::getBlankTexture());
        }
        active_shader_->setInt(uniform_name, unit); // sampler2D bound by binding the texture unit to the uniform
    }

    // ================ Instanced Rendering ================= //
    void Graphics::drawObjectInstanced(DrawShape* shape, const std::vector<glm::mat4>& models, const DrawMaterial& material) {
        if (models.empty()) return;

        // Set up instance vbo once
        if (shape->instance_vbo == 0) {
            glBindVertexArray(shape->vao);
            glGenBuffers(1, &shape->instance_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, shape->instance_vbo);

            // Build for cols of mat4 one at a time
            for (int i = 0; i < 4; i++) {
                GLuint location = 3 + i;
                glEnableVertexAttribArray(location);
                glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                                      (void*)(i * sizeof(glm::vec4)));
                glVertexAttribDivisor(location, 1);

            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        // Update step
        size_t instance_count = models.size();
        size_t data_size = instance_count * sizeof(glm::mat4);
        glBindBuffer(GL_ARRAY_BUFFER, shape->instance_vbo);
        glBufferData(GL_ARRAY_BUFFER, data_size, models.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Use special draw elements to make it instanced
        setMaterialUniforms(material);
        glBindVertexArray(shape->vao);
        glDrawElementsInstanced(
            GL_TRIANGLES,
            3 * shape->numTriangles,
            GL_UNSIGNED_INT,
            nullptr,
            instance_count
            );
        glBindVertexArray(0);
    }

    void Graphics::initSSAO() {
        const auto size = Window::getSize();
        int width = size.x;
        int height = size.y;

        glGenFramebuffers(1, &gBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

        // Normal texture
        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gNormal, 0);

        // Depth texture
        glGenTextures(1, &gDepth);
        glBindTexture(GL_TEXTURE_2D, gDepth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);

        GLenum attachments[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, attachments);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Graphics::beginGeometryPass() {
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Graphics::endGeometryPass() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}


