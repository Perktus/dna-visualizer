#include <renderer/Texture.h>
#include <stb_image.h>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

namespace dna {

namespace {
    void applyAnisotropicFiltering() {
        if (GLAD_GL_EXT_texture_filter_anisotropic) {
            float maxAniso = 0.0f;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
            if (maxAniso > 1.0f)
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
        }
    }
}

Texture::Texture(const std::string& path) {
    // OpenGL ma (0,0) na dole — obrazy z pliku mają (0,0) u góry
    stbi_set_flip_vertically_on_load(true);

    int channels = 0;
    unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &channels, 4);

    if (!data)
        throw std::runtime_error("Cannot load texture: " + path);

    uploadRGBA(data, m_width, m_height);
    stbi_image_free(data);
}

Texture Texture::createCheckerboard(int size, int cells) {
    Texture tex;
    tex.m_width = size;
    tex.m_height = size;

    std::vector<unsigned char> pixels(size * size * 4);
    const int cellSize = std::max(1, size / cells);

    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            const bool dark = ((x / cellSize) + (y / cellSize)) % 2 == 0;
            const unsigned char v = dark ? 40 : 90;
            const int i = (y * size + x) * 4;
            pixels[i + 0] = v;
            pixels[i + 1] = v;
            pixels[i + 2] = static_cast<unsigned char>(v + 20);
            pixels[i + 3] = 255;
        }
    }

    tex.uploadRGBA(pixels.data(), size, size);
    return tex;
}

Texture::Texture(Texture&& other) noexcept
    : m_id(other.m_id), m_width(other.m_width), m_height(other.m_height) {
    other.m_id = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this == &other)
        return *this;
    if (m_id)
        glDeleteTextures(1, &m_id);
    m_id = other.m_id;
    m_width = other.m_width;
    m_height = other.m_height;
    other.m_id = 0;
    return *this;
}

Texture::~Texture() {
    if (m_id)
        glDeleteTextures(1, &m_id);
}

void Texture::uploadRGBA(const unsigned char* data, int width, int height) {
    m_width = width;
    m_height = height;

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    applyDefaultSampling();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::applyDefaultSampling() const {
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    applyAnisotropicFiltering();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setFilterMode(TextureFilterMode mode) const {
    glBindTexture(GL_TEXTURE_2D, m_id);

    switch (mode) {
    case TextureFilterMode::Nearest:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case TextureFilterMode::Linear:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    case TextureFilterMode::LinearMipmap:
    default:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        applyAnisotropicFiltering();
        break;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace dna
