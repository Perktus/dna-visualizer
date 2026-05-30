#pragma once

#include <glad/gl.h>

namespace dna {

class Framebuffer {
public:
    Framebuffer(int width, int height);
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    void resize(int width, int height);
    void bind() const;
    void unbind() const;

    GLuint colorTexture() const { return m_colorTexture; }
    int width() const { return m_width; }
    int height() const { return m_height; }

    static void drawFullscreenQuad();

private:
    void destroy();
    void create(int width, int height);

    GLuint m_fbo{ 0 };
    GLuint m_colorTexture{ 0 };
    GLuint m_depthRbo{ 0 };
    int m_width{ 0 };
    int m_height{ 0 };

    static GLuint s_quadVao;
    static GLuint s_quadVbo;
    static void initQuad();
};

} // namespace dna
