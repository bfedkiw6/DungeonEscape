#pragma once

#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <utility>

#define INVALID_UNIFORM_LOCATION 0xffffffff

namespace gl {

    class ShaderProgram {
    public:
        ShaderProgram() : program_id_(0), vertex_id_(0), fragment_id_(0) {}
        ShaderProgram(const GLuint program_id, const GLuint vertex_id, const GLuint fragment_id, std::string name) :
        program_id_(program_id), vertex_id_(vertex_id), fragment_id_(fragment_id), name_(std::move(name)) {}

        void use() const;
        void deleteProgram() const;

        void setMat4Vec(const char* name, size_t size, const std::vector<glm::mat4>& matrices) const;
        void setMat4(const char* name, const glm::mat4& matrix) const;
        void setMat3(const char* name, const glm::mat3& matrix) const;
        void setVec4(const char* name, const glm::vec4& vector) const;
        void setVec3(const char* name, const glm::vec3& vector) const;
        void setVec2(const char* name, const glm::vec2& vector) const;
        void setFloat(const char* name, const float& value) const;
        void setInt(const char* name, int value) const;

        GLint getLocation(const char* uniform_name) const;
        [[nodiscard]] GLuint getVertexID() const;
        [[nodiscard]] GLuint getFragmentID() const;



    private:

        GLuint program_id_;
        GLuint vertex_id_;
        GLuint fragment_id_;
        std::string name_;
    };


    class Shaders {
    public:
        static ShaderProgram createShaderProgram(const char* vertex_path, const char* fragment_path, const std::string& name);
        static ShaderProgram createShaderProgram(const char* vertex_path, GLuint fragment_program_id, const std::string& name);

    private:
        static GLuint initializeProgram(GLuint vertex_shader, GLuint fragment_shader);
        static GLuint initializeShader(GLenum type, const char* source);
        static void getShaderErrors(GLuint shader);
        static void getProgramErrors(GLuint program);
        static std::string readTextFile(const char* filename);
    };
}
