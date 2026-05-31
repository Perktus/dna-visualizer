#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>

namespace dna {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 color{ 1.0f, 1.0f, 1.0f };
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void draw() const;

    static Mesh createSphere(float radius, int stacks, int slices);
    static Mesh createPlane(float halfWidth, float halfDepth);
    static Mesh createCube(float halfExtent);

private:
    GLuint m_vao{ 0 };
    GLuint m_vbo{ 0 };
    GLuint m_ebo{ 0 };
    GLsizei m_indexCount{ 0 };

    void upload(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
};

} // namespace dna