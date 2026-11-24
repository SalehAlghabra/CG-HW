#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>

// Structure matching shader vertex layout:
// layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec3 aNormal;
// layout (location = 2) in vec2 aTexCoords;
struct SceneVertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class SimpleMesh {
public:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    unsigned int indexCount = 0;
    unsigned int vertexCount = 0;
    bool hasIndices = false;

    SimpleMesh() = default;

    void setup(const std::vector<SceneVertex>& vertices, const std::vector<unsigned int>& indices = {}) {
        vertexCount = static_cast<unsigned int>(vertices.size());
        hasIndices = !indices.empty();
        indexCount = static_cast<unsigned int>(indices.size());

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        if (hasIndices) {
            glGenBuffers(1, &EBO);
        }

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(SceneVertex), vertices.data(), GL_STATIC_DRAW);

        if (hasIndices) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        }

        // Location 0: Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SceneVertex), (void*)offsetof(SceneVertex, Position));
        glEnableVertexAttribArray(0);

        // Location 1: Normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SceneVertex), (void*)offsetof(SceneVertex, Normal));
        glEnableVertexAttribArray(1);

        // Location 2: TexCoords
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SceneVertex), (void*)offsetof(SceneVertex, TexCoords));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void draw() const {
        glBindVertexArray(VAO);
        if (hasIndices) {
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        }
        glBindVertexArray(0);
    }

    void cleanup() {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);
        VAO = VBO = EBO = 0;
    }
};

namespace MeshBuilder {

    // Creates a textured 3D Box centered at origin
    inline SimpleMesh createBox(float width, float height, float depth, float tileU = 1.0f, float tileV = 1.0f) {
        float w2 = width * 0.5f;
        float h2 = height * 0.5f;
        float d2 = depth * 0.5f;

        std::vector<SceneVertex> vertices = {
            // Front face (+Z)
            {{-w2, -h2,  d2}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{ w2, -h2,  d2}, {0.0f, 0.0f, 1.0f}, {tileU, 0.0f}},
            {{ w2,  h2,  d2}, {0.0f, 0.0f, 1.0f}, {tileU, tileV}},
            {{-w2,  h2,  d2}, {0.0f, 0.0f, 1.0f}, {0.0f, tileV}},
            // Back face (-Z)
            {{ w2, -h2, -d2}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
            {{-w2, -h2, -d2}, {0.0f, 0.0f, -1.0f}, {tileU, 0.0f}},
            {{-w2,  h2, -d2}, {0.0f, 0.0f, -1.0f}, {tileU, tileV}},
            {{ w2,  h2, -d2}, {0.0f, 0.0f, -1.0f}, {0.0f, tileV}},
            // Left face (-X)
            {{-w2, -h2, -d2}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{-w2, -h2,  d2}, {-1.0f, 0.0f, 0.0f}, {tileU, 0.0f}},
            {{-w2,  h2,  d2}, {-1.0f, 0.0f, 0.0f}, {tileU, tileV}},
            {{-w2,  h2, -d2}, {-1.0f, 0.0f, 0.0f}, {0.0f, tileV}},
            // Right face (+X)
            {{ w2, -h2,  d2}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{ w2, -h2, -d2}, {1.0f, 0.0f, 0.0f}, {tileU, 0.0f}},
            {{ w2,  h2, -d2}, {1.0f, 0.0f, 0.0f}, {tileU, tileV}},
            {{ w2,  h2,  d2}, {1.0f, 0.0f, 0.0f}, {0.0f, tileV}},
            // Top face (+Y)
            {{-w2,  h2,  d2}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{ w2,  h2,  d2}, {0.0f, 1.0f, 0.0f}, {tileU, 0.0f}},
            {{ w2,  h2, -d2}, {0.0f, 1.0f, 0.0f}, {tileU, tileV}},
            {{-w2,  h2, -d2}, {0.0f, 1.0f, 0.0f}, {0.0f, tileV}},
            // Bottom face (-Y)
            {{-w2, -h2, -d2}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
            {{ w2, -h2, -d2}, {0.0f, -1.0f, 0.0f}, {tileU, 0.0f}},
            {{ w2, -h2,  d2}, {0.0f, -1.0f, 0.0f}, {tileU, tileV}},
            {{-w2, -h2,  d2}, {0.0f, -1.0f, 0.0f}, {0.0f, tileV}},
        };

        std::vector<unsigned int> indices;
        for (unsigned int f = 0; f < 6; ++f) {
            unsigned int base = f * 4;
            indices.push_back(base + 0);
            indices.push_back(base + 1);
            indices.push_back(base + 2);
            indices.push_back(base + 0);
            indices.push_back(base + 2);
            indices.push_back(base + 3);
        }

        SimpleMesh mesh;
        mesh.setup(vertices, indices);
        return mesh;
    }

    // Creates a horizontal or vertical textured quad
    inline SimpleMesh createQuad(float width, float height, float tileU = 1.0f, float tileV = 1.0f, bool horizontal = false) {
        float w2 = width * 0.5f;
        float h2 = height * 0.5f;

        std::vector<SceneVertex> vertices;
        if (horizontal) {
            // Facing +Y
            vertices = {
                {{-w2, 0.0f,  h2}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                {{ w2, 0.0f,  h2}, {0.0f, 1.0f, 0.0f}, {tileU, 0.0f}},
                {{ w2, 0.0f, -h2}, {0.0f, 1.0f, 0.0f}, {tileU, tileV}},
                {{-w2, 0.0f, -h2}, {0.0f, 1.0f, 0.0f}, {0.0f, tileV}}
            };
        } else {
            // Facing +Z
            vertices = {
                {{-w2, -h2, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
                {{ w2, -h2, 0.0f}, {0.0f, 0.0f, 1.0f}, {tileU, 0.0f}},
                {{ w2,  h2, 0.0f}, {0.0f, 0.0f, 1.0f}, {tileU, tileV}},
                {{-w2,  h2, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, tileV}}
            };
        }

        std::vector<unsigned int> indices = {0, 1, 2, 0, 2, 3};
        SimpleMesh mesh;
        mesh.setup(vertices, indices);
        return mesh;
    }

    // Creates a double-sided Card Quad:
    // Front face faces +Z (TexCoords [0,1])
    // Back face faces -Z (TexCoords [0,1])
    inline SimpleMesh createCardFront(float width, float height) {
        float w2 = width * 0.5f;
        float h2 = height * 0.5f;
        std::vector<SceneVertex> vertices = {
            {{-w2, -h2, 0.001f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{ w2, -h2, 0.001f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{ w2,  h2, 0.001f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-w2,  h2, 0.001f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}
        };
        std::vector<unsigned int> indices = {0, 1, 2, 0, 2, 3};
        SimpleMesh mesh;
        mesh.setup(vertices, indices);
        return mesh;
    }

    inline SimpleMesh createCardBack(float width, float height) {
        float w2 = width * 0.5f;
        float h2 = height * 0.5f;
        std::vector<SceneVertex> vertices = {
            {{ w2, -h2, -0.001f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
            {{-w2, -h2, -0.001f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
            {{-w2,  h2, -0.001f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
            {{ w2,  h2, -0.001f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}
        };
        std::vector<unsigned int> indices = {0, 1, 2, 0, 2, 3};
        SimpleMesh mesh;
        mesh.setup(vertices, indices);
        return mesh;
    }

    // Creates the TORN POSTER mounted on the Back Wall (facing +Z into the room):
    // Requirement 6: "احد جدران الغرفة يحوي على ملصق poster ناقص او ممزق من احد اطرافه"
    // The bottom-right corner is geometrically ripped away with jagged polygonal edges!
    inline SimpleMesh createTornPosterBackWall(float width, float height) {
        float w2 = width * 0.5f;
        float h2 = height * 0.5f;

        glm::vec3 normal(0.0f, 0.0f, 1.0f);

        auto getUV = [&](float x, float y) -> glm::vec2 {
            float u = (x + w2) / width;
            float v = (y + h2) / height;
            return glm::vec2(u, v);
        };

        std::vector<SceneVertex> vertices = {
            // V0: Top-Left
            {{-w2,  h2, 0.0f}, normal, getUV(-w2,  h2)},
            // V1: Bottom-Left
            {{-w2, -h2, 0.0f}, normal, getUV(-w2, -h2)},
            // V2: Tear start on bottom edge
            {{0.15f * w2, -h2, 0.0f}, normal, getUV(0.15f * w2, -h2)},
            // V3: Jagged notch 1
            {{0.35f * w2, -0.60f * h2, 0.0f}, normal, getUV(0.35f * w2, -0.60f * h2)},
            // V4: Jagged notch 2
            {{0.65f * w2, -0.45f * h2, 0.0f}, normal, getUV(0.65f * w2, -0.45f * h2)},
            // V5: Jagged tooth 3
            {{0.85f * w2, -0.20f * h2, 0.0f}, normal, getUV(0.85f * w2, -0.20f * h2)},
            // V6: Tear end on right edge
            {{w2, 0.10f * h2, 0.0f}, normal, getUV(w2, 0.10f * h2)},
            // V7: Top-Right
            {{w2,  h2, 0.0f}, normal, getUV(w2,  h2)},
            // V8: Center hub point
            {{-0.10f * w2, 0.10f * h2, 0.0f}, normal, getUV(-0.10f * w2, 0.10f * h2)}
        };

        // Counter-Clockwise triangulation around hub V8 facing +Z:
        std::vector<unsigned int> indices = {
            8, 1, 2,
            8, 2, 3,
            8, 3, 4,
            8, 4, 5,
            8, 5, 6,
            8, 6, 7,
            8, 7, 0,
            8, 0, 1
        };

        SimpleMesh mesh;
        mesh.setup(vertices, indices);
        return mesh;
    }

    // Creates a smooth 3D Cylinder / Truncated Cone (for Stanchions & Pedestals)
    inline SimpleMesh createCylinder(float radiusBottom, float radiusTop, float height, int segments = 20) {
        std::vector<SceneVertex> vertices;
        std::vector<unsigned int> indices;

        float h2 = height * 0.5f;

        // Side vertices
        for (int i = 0; i <= segments; ++i) {
            float angle = static_cast<float>(i) / static_cast<float>(segments) * 2.0f * 3.14159265f;
            float cosA = std::cos(angle);
            float sinA = std::sin(angle);
            float u = static_cast<float>(i) / static_cast<float>(segments);

            glm::vec3 normal(cosA, (radiusBottom - radiusTop) / height, sinA);
            normal = glm::normalize(normal);

            // Bottom ring vertex
            vertices.push_back({{radiusBottom * cosA, -h2, radiusBottom * sinA}, normal, {u, 0.0f}});
            // Top ring vertex
            vertices.push_back({{radiusTop * cosA,  h2, radiusTop * sinA}, normal, {u, 1.0f}});
        }

        // Side indices
        for (int i = 0; i < segments; ++i) {
            unsigned int b1 = i * 2;
            unsigned int t1 = i * 2 + 1;
            unsigned int b2 = (i + 1) * 2;
            unsigned int t2 = (i + 1) * 2 + 1;

            indices.push_back(b1);
            indices.push_back(b2);
            indices.push_back(t1);

            indices.push_back(t1);
            indices.push_back(b2);
            indices.push_back(t2);
        }

        // Top cap center
        unsigned int topCenterIdx = static_cast<unsigned int>(vertices.size());
        vertices.push_back({{0.0f, h2, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f}});
        for (int i = 0; i <= segments; ++i) {
            float angle = static_cast<float>(i) / static_cast<float>(segments) * 2.0f * 3.14159265f;
            vertices.push_back({{radiusTop * std::cos(angle), h2, radiusTop * std::sin(angle)}, {0.0f, 1.0f, 0.0f}, {0.5f + 0.5f * std::cos(angle), 0.5f + 0.5f * std::sin(angle)}});
        }
        for (int i = 0; i < segments; ++i) {
            indices.push_back(topCenterIdx);
            indices.push_back(topCenterIdx + 1 + i);
            indices.push_back(topCenterIdx + 2 + i);
        }

        SimpleMesh mesh;
        mesh.setup(vertices, indices);
        return mesh;
    }

    // Creates a smooth 3D UV Sphere (for polished brass stanchion finials)
    inline SimpleMesh createSphere(float radius, int rings = 12, int sectors = 16) {
        std::vector<SceneVertex> vertices;
        std::vector<unsigned int> indices;

        for (int r = 0; r <= rings; ++r) {
            float phi = static_cast<float>(r) / static_cast<float>(rings) * 3.14159265f;
            float cosPhi = std::cos(phi);
            float sinPhi = std::sin(phi);

            for (int s = 0; s <= sectors; ++s) {
                float theta = static_cast<float>(s) / static_cast<float>(sectors) * 2.0f * 3.14159265f;
                float cosTheta = std::cos(theta);
                float sinTheta = std::sin(theta);

                glm::vec3 normal(sinPhi * cosTheta, cosPhi, sinPhi * sinTheta);
                glm::vec3 pos = normal * radius;
                glm::vec2 uv(static_cast<float>(s) / sectors, static_cast<float>(r) / rings);

                vertices.push_back({pos, normal, uv});
            }
        }

        for (int r = 0; r < rings; ++r) {
            for (int s = 0; s < sectors; ++s) {
                unsigned int cur = r * (sectors + 1) + s;
                unsigned int next = cur + sectors + 1;

                indices.push_back(cur);
                indices.push_back(next);
                indices.push_back(cur + 1);

                indices.push_back(cur + 1);
                indices.push_back(next);
                indices.push_back(next + 1);
            }
        }

        SimpleMesh mesh;
        mesh.setup(vertices, indices);
        return mesh;
    }

    // Creates a realistic 3D curved velvet rope draped between two stanchion posts
    inline SimpleMesh createCurvedRope(const glm::vec3& p0, const glm::vec3& p1, float sag, float radius, int segments = 16, int radialSegments = 8) {
        std::vector<SceneVertex> vertices;
        std::vector<unsigned int> indices;

        glm::vec3 dir = p1 - p0;
        glm::vec3 right = glm::normalize(glm::cross(dir, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::vec3 up = glm::normalize(glm::cross(right, dir));

        for (int i = 0; i <= segments; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(segments);
            // Parabolic catenary sag
            glm::vec3 center = glm::mix(p0, p1, t);
            center.y -= sag * 4.0f * t * (1.0f - t);

            for (int j = 0; j <= radialSegments; ++j) {
                float ang = static_cast<float>(j) / static_cast<float>(radialSegments) * 2.0f * 3.14159265f;
                glm::vec3 offset = (right * std::cos(ang) + up * std::sin(ang)) * radius;
                glm::vec3 pos = center + offset;
                glm::vec3 norm = glm::normalize(offset);
                glm::vec2 uv(t * 3.0f, static_cast<float>(j) / radialSegments);

                vertices.push_back({pos, norm, uv});
            }
        }

        for (int i = 0; i < segments; ++i) {
            for (int j = 0; j < radialSegments; ++j) {
                unsigned int b1 = i * (radialSegments + 1) + j;
                unsigned int b2 = b1 + 1;
                unsigned int t1 = (i + 1) * (radialSegments + 1) + j;
                unsigned int t2 = t1 + 1;

                indices.push_back(b1);
                indices.push_back(t1);
                indices.push_back(b2);

                indices.push_back(b2);
                indices.push_back(t1);
                indices.push_back(t2);
            }
        }

        SimpleMesh mesh;
        mesh.setup(vertices, indices);
        return mesh;
    }

} // namespace MeshBuilder
