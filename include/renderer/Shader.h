#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>

namespace dna {

class Shader {
public:
    Shader(const std::string& vertPath, const std::string& fragPath);
    ~Shader();

    Shader(const Shader&)            = delete;
    Shader& operator=(const Shader&) = delete;

    void bind()   const;
    void unbind() const;

    void setInt  (const std::string& name, int value)              const;
    void setFloat(const std::string& name, float value)            const;
    void setVec3 (const std::string& name, const glm::vec3& value) const;
    void setMat4 (const std::string& name, const glm::mat4& value) const;

private:
    GLuint m_id{ 0 };

    static GLuint  compileShader(GLenum type, const std::string& source);
    static std::string readFile(const std::string& path);
    GLint getUniformLocation(const std::string& name) const;
};

} // namespace dna