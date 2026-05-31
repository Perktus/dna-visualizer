#pragma once

#include <glad/gl.h>
#include <string>

namespace dna {

enum class TextureFilterMode {
    Nearest,
    Linear,
    LinearMipmap
};

class Texture {
public:
    explicit Texture(const std::string& path);
    static Texture createCheckerboard(int size, int cells);

    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    void bind(unsigned int slot = 0) const;
    void unbind() const;
    void setFilterMode(TextureFilterMode mode) const;

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    Texture() = default;
    void uploadRGBA(const unsigned char* data, int width, int height);
    void applyDefaultSampling() const;

    GLuint m_id{ 0 };
    int m_width{ 0 };
    int m_height{ 0 };
};

} // namespace dna
