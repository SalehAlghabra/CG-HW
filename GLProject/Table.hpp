#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <learnopengl/shader.h>
#include <vector>
#include "MeshBuilder.hpp"

class Table {
public:
    SimpleMesh tabletopMesh;
    SimpleMesh legMesh;
    SimpleMesh zoneMatMesh;

    unsigned int tableTextureId = 0;

    // أبعاد الطاولة
    float width = 7.2f;   // عرض الطاولة (محور X)
    float length = 5.2f;  // طول الطاولة (محور Z)
    float height = 2.0f;  // ارتفاع سطح الطاولة عن الأرض (محور Y)
    float thickness = 0.2f; // سماكة لوح الطاولة

    // مواقع الأقسام الثلاثة المطلوبة في الوظيفة:
    // 1. قسم مخزون اللاعب (Player Deck)
    glm::vec3 deckAreaCenter = glm::vec3(-2.4f, 2.11f, 1.2f);

    // 2. قسم بطاقات اللاعب الملعوبة (5 فتحات)
    std::vector<glm::vec3> playedSlots;

    // 3. قسم بطاقات الخصم (فارغ)
    std::vector<glm::vec3> opponentSlots;

    Table() = default;

    void init(unsigned int textureId) {
        tableTextureId = textureId;

        // إنشاء مجسم سطح الطاولة وقوائمها
        tabletopMesh = MeshBuilder::createBox(width, thickness, length, 1.0f, 1.0f);
        legMesh = MeshBuilder::createBox(0.28f, height - thickness * 0.5f, 0.28f, 1.0f, 2.0f);
        zoneMatMesh = MeshBuilder::createQuad(0.85f, 1.25f, 1.0f, 1.0f, true);

        // حساب إحداثيات الفتحات الخمس لبطاقات اللاعب في منطقة اللعب (Section 2)
        float slotStartX = -1.6f;
        float slotGap = 0.8f;
        for (int i = 0; i < 5; ++i) {
            playedSlots.push_back(glm::vec3(slotStartX + i * slotGap, 2.11f, 0.85f));
        }

        // حساب إحداثيات الفتحات الخمس لمنطقة الخصم - فارغة (Section 3)
        for (int i = 0; i < 5; ++i) {
            opponentSlots.push_back(glm::vec3(slotStartX + i * slotGap, 2.11f, -1.15f));
        }
    }

    void render(Shader& shader) {
        shader.use();
        shader.setBool("useTexture", true);
        shader.setVec4("objectColor", glm::vec4(1.0f));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tableTextureId);
        shader.setInt("texture_diffuse1", 0);

        // 1. رسم سطح الطاولة (Tabletop)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, height, 0.0f));
        shader.setMat4("model", model);
        tabletopMesh.draw();

        // 2. رسم قوائم الطاولة الأربعة (4 Table Legs)
        float legX = width * 0.5f - 0.35f;
        float legZ = length * 0.5f - 0.35f;
        float legY = (height - thickness * 0.5f) * 0.5f;

        glm::vec3 legPositions[4] = {
            {-legX, legY, -legZ},
            { legX, legY, -legZ},
            {-legX, legY,  legZ},
            { legX, legY,  legZ}
        };

        for (int i = 0; i < 4; ++i) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, legPositions[i]);
            shader.setMat4("model", model);
            legMesh.draw();
        }

        // 3. رسم إطارات الأقسام الثلاثة ثلاثية الأبعاد على سطح الطاولة لتعزيز الوضوح البصري:
        // أ) إطار قسم مخزون اللاعب (Deck Zone)
        shader.setBool("useTexture", false);
        shader.setVec4("objectColor", glm::vec4(0.85f, 0.70f, 0.20f, 0.7f)); // لون ذهبي لقسم المخزون
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(deckAreaCenter.x, height + thickness * 0.5f + 0.002f, deckAreaCenter.z));
        shader.setMat4("model", model);
        zoneMatMesh.draw();

        // ب) إطارات فتحات منطقة اللعب للاعب (Player Slots)
        shader.setVec4("objectColor", glm::vec4(0.20f, 0.55f, 0.35f, 0.6f)); // لون مخملي زمردي لفتحات اللاعب
        for (const auto& slot : playedSlots) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(slot.x, height + thickness * 0.5f + 0.002f, slot.z));
            shader.setMat4("model", model);
            zoneMatMesh.draw();
        }

        // ج) إطارات قسم الخصم (Opponent Zone - الفارغ)
        shader.setVec4("objectColor", glm::vec4(0.55f, 0.25f, 0.25f, 0.45f)); // لون خافت هادئ لقسم الخصم الفارغ
        for (const auto& slot : opponentSlots) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(slot.x, height + thickness * 0.5f + 0.002f, slot.z));
            shader.setMat4("model", model);
            zoneMatMesh.draw();
        }

        shader.setBool("useTexture", true);
        shader.setVec4("objectColor", glm::vec4(1.0f));
    }

    void cleanup() {
        tabletopMesh.cleanup();
        legMesh.cleanup();
        zoneMatMesh.cleanup();
    }
};
