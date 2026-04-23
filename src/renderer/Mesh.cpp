#include <renderer/Mesh.h>
#include <cmath>

namespace dna {

// ── Constructor ── //
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    upload(vertices, indices);
}

// ── Destructor ── //
Mesh::~Mesh() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

Mesh::Mesh(Mesh&& other) noexcept : m_vao(other.m_vao), m_vbo(other.m_vbo), m_ebo(other.m_ebo), m_indexCount(other.m_indexCount) {
    other.m_vao        = 0;
    other.m_vbo        = 0;
    other.m_ebo        = 0;
    other.m_indexCount = 0;
}

// ── Draw ── //
void Mesh::draw() const {
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

// ── Upload to GPU ── //
void Mesh::upload(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    m_indexCount = static_cast<GLsizei>(indices.size());

    // Create VAO, VBO, EBO //
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    // Upload vertices // 
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Upload indexes //
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Attribute Layout - must match vertex shader //
    // location = 0 : position (vec3) //
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // location = 1 : normal (vec3) //
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // location = 2 : uv (vec2) //
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

// ── Create Sphere ── //
Mesh Mesh::createSphere(float radius, int stacks, int slices) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    const float PI = 3.14159265359f;

    // Generate vertices //
    for (int i = 0; i <= stacks; ++i) {
        float phi = PI * float(i) / float(stacks); // 0 -> PI

        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * PI * float(j) / float(slices); // 0 -> 2PI

            Vertex v;

            // Position //
            v.position = {
                radius * std::sin(phi) * std::cos(theta),
                radius * std::cos(phi),
                radius * std::sin(phi) * std::sin(theta)
            };

            // Normal = normalized position (unit sphere) //
            v.normal = glm::normalize(v.position);

            // UV //
            v.uv = {
                float(j) / float(slices),
                float(i) / float(stacks)
            };

            vertices.push_back(v);
        }
    }

    // Generate indices (two triangles for each quad) // 
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            unsigned int a = i       * (slices + 1) + j;
            unsigned int b = (i + 1) * (slices + 1) + j;
            unsigned int c = (i + 1) * (slices + 1) + j + 1;
            unsigned int d = i       * (slices + 1) + j + 1;

            // Triangle 1 //
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(c);

            // Triangle 2 //
            indices.push_back(a);
            indices.push_back(c);
            indices.push_back(d);
        }
    }

    return Mesh(vertices, indices);
}

} // namespace dna