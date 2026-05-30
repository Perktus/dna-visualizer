#include <dna/HelixGeometry.h>
#include <glm/gtc/constants.hpp>
#include <cmath>

namespace dna {

// ── Nucleotide Position ── //
glm::vec3 HelixGeometry::nucleotidePos(int index, const HelixParams& params, float phaseOffset) {
    float angle = glm::radians(params.twist * index) + glm::radians(phaseOffset);
    return {
        params.radius * std::cos(angle),
        params.rise * index,
        params.radius * std::sin(angle)
    };
}

// ── Build Strand ── //
Mesh HelixGeometry::buildStrand(const DNASequence& seq, const HelixParams& params, float phaseOffset) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    const int n = seq.length();

    for (int i = 0; i < n; ++i) {
        glm::vec3 center = nucleotidePos(i, params, phaseOffset);
        const glm::vec3 baseColor = nucleotideColor(seq.at(i));

        const int stacks = params.sphereStacks;
        const int slices = params.sphereStacks * 2;
        const float r = params.sphereRadius;
        const float PI = glm::pi<float>();

        int baseIndex = static_cast<int>(vertices.size());

        for (int s = 0; s <= stacks; ++s) {
            float phi = PI * float(s) / float(stacks);
            for (int sl = 0; sl <= slices; ++sl) {
                float theta = 2.0f * PI * float(sl) / float(slices);

                Vertex v;
                v.position = center + glm::vec3{
                    r * std::sin(phi) * std::cos(theta),
                    r * std::cos(phi),
                    r * std::sin(phi) * std::sin(theta)
                };
                v.normal = glm::normalize(v.position - center);
                v.uv = { float(sl) / slices, float(s) / stacks };
                v.color = baseColor;
                vertices.push_back(v);
            }
        }

        for (int s = 0; s < stacks; ++s) {
            for (int sl = 0; sl < slices; ++sl) {
                unsigned int a = baseIndex + s  * (slices + 1) + sl;
                unsigned int b = baseIndex + (s + 1) * (slices + 1) + sl;
                unsigned int c = baseIndex + (s + 1) * (slices + 1) + sl + 1;
                unsigned int d = baseIndex + s * (slices + 1) + sl + 1;

                indices.push_back(a); indices.push_back(b); indices.push_back(c);
                indices.push_back(a); indices.push_back(c); indices.push_back(d);
            }
        }

        if (i < n - 1) {
            glm::vec3 next = nucleotidePos(i + 1, params, phaseOffset);
            glm::vec3 dir = glm::normalize(next - center);
            glm::vec3 up = std::abs(dir.y) < 0.99f ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
            glm::vec3 right = glm::normalize(glm::cross(up, dir));
            glm::vec3 fwd  = glm::cross(dir, right);

            const int sides = params.tubeSides;
            const float tr = params.tubeRadius;
            int tubeBase = static_cast<int>(vertices.size());

            for (int k = 0; k <= sides; ++k) {
                float ang = 2.0f * glm::pi<float>() * float(k) / float(sides);
                glm::vec3 offset = tr * (std::cos(ang) * right + std::sin(ang) * fwd);

                Vertex v0, v1;
                v0.position = center + offset;
                v1.position = next   + offset;
                v0.normal = v1.normal = glm::normalize(offset);
                v0.uv = { float(k) / sides, 0.0f };
                v1.uv = { float(k) / sides, 1.0f };
                v0.color = v1.color = baseColor;
                vertices.push_back(v0);
                vertices.push_back(v1);
            }

            for (int k = 0; k < sides; ++k) {
                unsigned int a = tubeBase + k * 2;
                unsigned int b = tubeBase + k * 2 + 1;
                unsigned int c = tubeBase + (k + 1) * 2 + 1;
                unsigned int d = tubeBase + (k + 1) * 2;

                indices.push_back(a); indices.push_back(b); indices.push_back(c);
                indices.push_back(a); indices.push_back(c); indices.push_back(d);
            }
        }
    }

    return Mesh(vertices, indices);
}

// ── Bridges ── //
Mesh HelixGeometry::buildBonds(const DNASequence& seq, const HelixParams& params) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    const int n = seq.length();

    for (int i = 0; i < n; ++i) {
        glm::vec3 posA = nucleotidePos(i, params,   0.0f);
        glm::vec3 posB = nucleotidePos(i, params, 180.0f);
        glm::vec3 dir = glm::normalize(posB - posA);
        glm::vec3 up = std::abs(dir.x) < 0.99f ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
        glm::vec3 right = glm::normalize(glm::cross(up, dir));
        glm::vec3 fwd = glm::cross(dir, right);

        const int sides = params.tubeSides;
        const float tr = params.tubeRadius * 0.6f;
        int bondBase = static_cast<int>(vertices.size());

        for (int k = 0; k <= sides; ++k) {
            float ang = 2.0f * glm::pi<float>() * float(k) / float(sides);
            glm::vec3 off = tr * (std::cos(ang) * right + std::sin(ang) * fwd);

            Vertex v0, v1;
            v0.position = posA + off;
            v1.position = posB + off;
            v0.normal = v1.normal = glm::normalize(off);
            v0.uv = { float(k) / sides, 0.0f };
            v1.uv = { float(k) / sides, 1.0f };
            vertices.push_back(v0);
            vertices.push_back(v1);
        }

        for (int k = 0; k < sides; ++k) {
            unsigned int a = bondBase + k * 2;
            unsigned int b = bondBase + k * 2 + 1;
            unsigned int c = bondBase + (k + 1) * 2 + 1;
            unsigned int d = bondBase + (k + 1) * 2;

            indices.push_back(a); indices.push_back(b); indices.push_back(c);
            indices.push_back(a); indices.push_back(c); indices.push_back(d);
        }
    }

    return Mesh(vertices, indices);
}

// ── Build ── //
HelixMeshes HelixGeometry::build(const DNASequence& seq, const HelixParams& params) {
    DNASequence complement = seq.complementary();
    return {
        buildStrand(seq,        params,   0.0f),
        buildStrand(complement, params, 180.0f),
        buildBonds (seq, params)
    };
}

} // namespace dna