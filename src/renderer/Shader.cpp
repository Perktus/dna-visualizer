#include "renderer/Shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

namespace dna {

// ── Constructor ── //
Shader::Shader(const std::string& vertPath, const std::string& fragPath) {
    const std::string vertSrc = readFile(vertPath);
    const std::string fragSrc = readFile(fragPath);

    const GLuint vert = compileShader(GL_VERTEX_SHADER,   vertSrc);
    const GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);

    // Linking the program //
    m_id = glCreateProgram();
    glAttachShader(m_id, vert);
    glAttachShader(m_id, frag);
    glLinkProgram(m_id);

    // Checking for linking errors //
    GLint success = 0;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(m_id, sizeof(log), nullptr, log);
        throw std::runtime_error(std::string("Shader link error:\n") + log);
    }

    // Shaders are no longer needed after linking //
    glDeleteShader(vert);
    glDeleteShader(frag);
}

// ── Destructor ── //
Shader::~Shader() {
    glDeleteProgram(m_id);
}

// ── Bind / Unbind ── //
void Shader::bind()   const { glUseProgram(m_id); }
void Shader::unbind() const { glUseProgram(0);    }

// ── Uniforms ── //
void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(getUniformLocation(name), value);
}

// ── Setters ── //
void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

// ── Compile Shader ── //
GLuint Shader::compileShader(GLenum type, const std::string& source) {
    const GLuint id     = glCreateShader(type);
    const char*  src    = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    GLint success = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(id, sizeof(log), nullptr, log);
        const std::string typeName = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
        throw std::runtime_error(typeName + " shader compile error:\n" + log);
    }

    return id;
}

// ── Read File ── //
std::string Shader::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot open shader file: " + path);

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// ── Get Uniform Location ── //
GLint Shader::getUniformLocation(const std::string& name) const {
    const GLint loc = glGetUniformLocation(m_id, name.c_str());
    if (loc == -1)
        std::cerr << "[Shader] Warning: uniform '" << name << "' not found\n";
    return loc;
}} // namespace dna