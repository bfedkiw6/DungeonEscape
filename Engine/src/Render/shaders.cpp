#include "Engine/Graphics/shaders.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>

#include "Engine/debug.h"
#include "Engine/file.h"

namespace gl {

    void ShaderProgram::use() const {
        glUseProgram(program_id_);
    }

    void ShaderProgram::deleteProgram() const {
        glDeleteProgram(program_id_);
    }

    void ShaderProgram::setMat4Vec(const char* name, const size_t size, const std::vector<glm::mat4>& matrices) const {
        glUniformMatrix4fv(getLocation(name), static_cast<GLsizei>(size), GL_FALSE, glm::value_ptr(matrices[0]));
    }

    void ShaderProgram::setMat4(const char* name, const glm::mat4& matrix) const {
        glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void ShaderProgram::setMat3(const char* name, const glm::mat3& matrix) const {
        glUniformMatrix3fv(getLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void ShaderProgram::setVec4(const char* name, const glm::vec4& vector) const {
        glUniform3fv(getLocation(name), 1, glm::value_ptr(vector));
    }

    void ShaderProgram::setVec3(const char* name, const glm::vec3& vector) const {
        glUniform3fv(getLocation(name), 1, glm::value_ptr(vector));
    }

    void ShaderProgram::setVec2(const char* name, const glm::vec2& vector) const {
        glUniform2fv(getLocation(name), 1, glm::value_ptr(vector));
    }

    void ShaderProgram::setFloat(const char* name, const float& value) const {
        glUniform1f(getLocation(name), value);
    }

    void ShaderProgram::setInt(const char* name, int value) const {
        glUniform1i(getLocation(name), value);
    }

    GLuint ShaderProgram::getVertexID() const {
        return vertex_id_;
    }


    GLuint ShaderProgram::getFragmentID() const {
        return fragment_id_;
    }

    GLint ShaderProgram::getLocation(const char* uniform_name) const {
        if (const auto location = glGetUniformLocation(program_id_, uniform_name); location != INVALID_UNIFORM_LOCATION) {
            return location;
        }
        debug::error("Failed to get uniform location '{}' for shader: '{}'", uniform_name, name_);
        return 0;
    }

    /**
     *
     * @param vertex_path The file path to the vertex shader from project root, e.g "Resources/Shaders/phong_vert.glsl"
     * @param fragment_path The file path to the vertex shader from project root
     * @param name The name of the shader program
     * @return ShaderProgram struct containing the program ID and an empty map of uniform locations
     */
    ShaderProgram Shaders::createShaderProgram(const char* vertex_path, const char* fragment_path, const std::string& name) {
        const auto full_vert_path = file::getPath(vertex_path); // Get full path of the file
        const auto full_frag_path = file::getPath(fragment_path);

        const auto vertex_shader = initializeShader(GL_VERTEX_SHADER, full_vert_path.c_str());
        const auto fragment_shader = initializeShader(GL_FRAGMENT_SHADER, full_frag_path.c_str());

        const auto program_id = initializeProgram(vertex_shader, fragment_shader);

        return {program_id, vertex_shader, fragment_shader, name};
    }

    ShaderProgram Shaders::createShaderProgram(const char* vertex_path, const GLuint fragment_program_id, const std::string& name) {
        const auto full_vert_path = file::getPath(vertex_path); // Get full path of the file

        const auto vertex_shader = initializeShader(GL_VERTEX_SHADER, full_vert_path.c_str());

        const auto program_id = initializeProgram(vertex_shader, fragment_program_id);

        return {program_id, vertex_shader, fragment_program_id, name};
    }


    GLuint Shaders::initializeProgram(GLuint vertex_shader, GLuint fragment_shader){
        GLint linked;
        GLuint program = glCreateProgram();

        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &linked);

        if (linked) glUseProgram(program);
        else {
            getProgramErrors(program);
            throw std::runtime_error("Shader program did not link correctly!");
        }
        return program;
    }

    GLuint Shaders::initializeShader(GLenum type, const char* filename) {
        GLuint shader = glCreateShader(type);
        GLint compiled;

        std::string str = readTextFile(filename);
        const char* cstr = str.c_str();

        glShaderSource(shader, 1, &cstr, nullptr);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (!compiled) {
            getShaderErrors(shader);
            throw std::runtime_error("Shader did not compile!");
        }
        return shader;
    }

    void Shaders::getShaderErrors(const GLuint shader) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        std::string log(length, '\0');
        glGetShaderInfoLog(shader, length, nullptr, &log[0]);
        glDeleteShader(shader);
        debug::error("GL Error: {}", log);
        throw std::runtime_error("Shader Compile Error");
    }

    void Shaders::getProgramErrors(const GLuint program) {
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        std::string log(length, '\0');
        glGetProgramInfoLog(program, length, &length, &log[0]);
        glDeleteProgram(program);
        debug::error("GL Error: {}", log);
        throw std::runtime_error("Shader Linking Error");
    }


    std::string Shaders::readTextFile(const char* filename) {
        std::string line;
        std::string content;


        std::string path = filename;
        std::ifstream fileStream(path, std::ios::in);

        if (!fileStream.is_open()) {
            debug::error("Could not open file \"{}\"", filename);
            throw std::runtime_error("Shader initialization failed");
        }

        while (std::getline(fileStream, line)) {
            content.append(line + "\n");
        }

        fileStream.close();
        return content;
    }
}
