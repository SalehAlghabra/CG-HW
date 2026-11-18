#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

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

    // Creates the TORN POSTER required by Requirement 6:
    // "احد جدران الغرفة يحوي على ملصق poster لاحد الاعمال الفنية الذي يختارها الفريق
    //  و يجب ان يكون الملصق ناقص او ممزق من احد اطرافه."
    // The bottom-right corner is geometrically ripped/missing with jagged tear edges!
    inline SimpleMesh createTornPoster(float width, float height) {
        float w2 = width * 0.5f;
        float h2 = height * 0.5f;

        // Custom polygon vertices on the wall (facing +X into the room):
        // Normal points in +X direction: (1.0f, 0.0f, 0.0f)
        // Coords: Y is up/down, Z is left/right along the wall
        // Bottom-Right corner is torn away!
        // We define vertices and their UV mapping:
        // V0: Top-Left (-Z, +Y)
        // V1: Bottom-Left (-Z, -Y)
        // V2: Tear start on bottom edge
        // V3: Jagged tear notch 1
        // V4: Jagged tear notch 2
        // V5: Jagged tear notch 3
        // V6: Tear end on right edge
        // V7: Top-Right (+Z, +Y)
        // V8: Center fan point for triangulation

        glm::vec3 normal(1.0f, 0.0f, 0.0f);

        // UV mapping helper: maps (z in [-w2, w2], y in [-h2, h2]) to [0,1]x[0,1]
        auto getUV = [&](float z, float y) -> glm::vec2 {
            float u = (z + w2) / width;
            float v = (y + h2) / height;
            return glm::vec2(u, v);
        };

        std::vector<SceneVertex> vertices = {
            // V0: Top-Left
            {{0.0f,  h2, -w2}, normal, getUV(-w2,  h2)},
            // V1: Bottom-Left
            {{0.0f, -h2, -w2}, normal, getUV(-w2, -h2)},
            // V2: Tear start on bottom edge (z = 0.1 * w2)
            {{0.0f, -h2, 0.1f * w2}, normal, getUV(0.1f * w2, -h2)},
            // V3: Jagged tear tooth 1
            {{0.0f, -0.6f * h2, 0.35f * w2}, normal, getUV(0.35f * w2, -0.6f * h2)},
            // V4: Jagged tear notch 2
            {{0.0f, -0.45f * h2, 0.65f * w2}, normal, getUV(0.65f * w2, -0.45f * h2)},
            // V5: Jagged tear tooth 3
            {{0.0f, -0.2f * h2, 0.85f * w2}, normal, getUV(0.85f * w2, -0.2f * h2)},
            // V6: Tear end on right edge (y = 0.0f)
            {{0.0f,  0.05f * h2, w2}, normal, getUV(w2, 0.05f * h2)},
            // V7: Top-Right
            {{0.0f,  h2,  w2}, normal, getUV(w2,  h2)},
            // V8: Interior triangulation hub point
            {{0.0f,  0.1f * h2, -0.1f * w2}, normal, getUV(-0.1f * w2, 0.1f * h2)}
        };

        // Triangulate around hub V8:
        std::vector<unsigned int> indices = {
            8, 0, 1,
            8, 1, 2,
            8, 2, 3,
            8, 3, 4,
            8, 4, 5,
            8, 5, 6,
            8, 6, 7,
            8, 7, 0
        };

        SimpleMesh mesh;
        mesh.setup(vertices, indices);
        return mesh;
    }

} // namespace MeshBuilder
