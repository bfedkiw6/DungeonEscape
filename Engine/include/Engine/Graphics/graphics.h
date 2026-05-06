#pragma once

#include "Engine/transform.h"
#include "Engine/Graphics/Text/text-renderer.h"
#include "Engine/Graphics/mesh.h"
#include "Engine/Graphics/camera.h"
#include "Engine/Graphics/shaders.h"
#include "Engine/Graphics/light.h"

namespace gl {

    class Graphics {
    public:
        static void initialize();
        static void tearDown();

        static void usePhongShader();
        static void useSkinnedShader();
        static void useTextShader();
        static void usePhongInstancedShader();

        static void clearScreen(const glm::vec3& color);

        static void setCameraUniforms(const Camera* camera);
        static void setLights(const std::vector<Light>& lights);
        static void setAmbientLight(const glm::vec3& ambient);

        static void drawObject(const DrawShape* draw_shape, const Transform& transform, const DrawMaterial& material = DEFAULT_MATERIAL);
        static void drawObject(const DrawObject* draw_object, const Transform& transform);
        static void drawMesh(const DrawMesh* draw_mesh, const Transform& transform);
        static void drawSkinnedMesh(SkinnedMesh* skinned_mesh, const Transform& transform);
        static void drawText(const std::string& text, glm::vec2 pos, float font_size, const glm::vec3& color, TextAlign align = TextAlign::LEFT, const std::string& font_name = "default");

        static void drawObjectInstanced(DrawShape* shape, const std::vector<glm::mat4>& models, const DrawMaterial& material);
        void initSSAO();
        void beginGeometryPass();
        void endGeometryPass();

    private:
        static void initializeShaders();
        static void setMaterialUniforms(const DrawMaterial& material);
        static void bindMaterialTextures(const GLTextures& textures);
        static void bindTexture(GLuint texture, int unit, const char* uniform_name);

        static ShaderProgram* active_shader_;

        static ShaderProgram phong_;
        static ShaderProgram skinned_;
        static ShaderProgram text_;
        static ShaderProgram phong_instanced_;

        GLuint gBuffer = 0;
        GLuint gNormal = 0;
        GLuint gDepth = 0;

        GLuint ssaoFBO = 0;
        GLuint ssaoBlurFBO = 0;

        GLuint ssaoTex = 0;
        GLuint ssaoBlurTex = 0;
    };
}

