#pragma once

#include <glad/gl.h>
#include <string>

namespace dna {

class Texture {
public:
    explicit Texture(const std::string& path);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    void bind(unsigned int slot = 0) const;
    void unbind() const;

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    GLuint m_id { 0 };
    int m_width { 0 };
    int m_height { 0 };
};

} // namespace dna