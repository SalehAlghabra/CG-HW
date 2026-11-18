#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <learnopengl/shader.h>
#include <string>
#include <cmath>
#include "MeshBuilder.hpp"

enum class CardState {
    IN_DECK,            // البطاقة داخل مخزون اللاعب
    ANIMATING_TO_PLAY,  // البطاقة في حالة حركة وتحليق من المخزون إلى منطقة اللعب
    PLAYED              // البطاقة مستقرة في منطقة اللعب
};

class Card {
public:
    std::string characterName;
    unsigned int frontTextureId = 0;
    unsigned int backTextureId = 0;
    CardState state = CardState::IN_DECK;

    int slotIndex = -1; // الفتحة التي ستستقر فيها البطاقة (0 إلى 4)

    // إحداثيات الحركة والموقع
    glm::vec3 currentPos;
    glm::vec3 startPos;
    glm::vec3 targetPos;

    float moveProgress = 0.0f;     // نسبة تقدم تحريك البطاقة [0.0 -> 1.0]
    float moveDuration = 1.1f;     // مدة حركة الطيران بالثواني

    // أبعاد البطاقة
    float width = 0.72f;
    float height = 1.08f;

    Card() = default;

    Card(const std::string& name, unsigned int frontTex, unsigned int backTex, const glm::vec3& deckPos)
        : characterName(name), frontTextureId(frontTex), backTextureId(backTex),
          state(CardState::IN_DECK), currentPos(deckPos), startPos(deckPos), targetPos(deckPos) {}

    // بدء حركة نقل البطاقة إلى منطقة اللعب
    void playToSlot(int slot, const glm::vec3& slotPosition) {
        state = CardState::ANIMATING_TO_PLAY;
        slotIndex = slot;
        startPos = currentPos;
        targetPos = slotPosition;
        moveProgress = 0.0f;
    }

    // تحديث تحريك البطاقة في كل إطار
    void update(float deltaTime) {
        if (state == CardState::ANIMATING_TO_PLAY) {
            moveProgress += deltaTime / moveDuration;
            if (moveProgress >= 1.0f) {
                moveProgress = 1.0f;
                state = CardState::PLAYED;
                currentPos = targetPos;
            } else {
                // Smoothstep easing for natural fluid motion: 3t^2 - 2t^3
                float t = moveProgress;
                float ease = t * t * (3.0f - 2.0f * t);

                currentPos.x = glm::mix(startPos.x, targetPos.x, ease);
                currentPos.z = glm::mix(startPos.z, targetPos.z, ease);
                // Parabolic arc in Y axis: lift up into air then settle down
                float arcHeight = 0.85f * std::sin(moveProgress * 3.14159265f);
                currentPos.y = glm::mix(startPos.y, targetPos.y, ease) + arcHeight;
            }
        }
    }

    // رسم البطاقة مع تطبيق مصفوفات التحويل (Translate, Rotate, Scale)
    // standProgress: 0.0 = مستوية أفقياً على الطاولة (منظور علوي)، 1.0 = واقفة طولياً تواجه اللاعب (منظور اللاعب)
    void render(Shader& shader, const SimpleMesh& frontMesh, const SimpleMesh& backMesh, float standProgress) {
        shader.use();

        // 1. حساب مصفوفة التحويل الأساسية model matrix
        glm::mat4 model = glm::mat4(1.0f);

        // ج) التحويل بين الوقوف (منظور اللاعب) والتسطيح (المنظور العلوي)
        // عند standProgress = 0.0 (المنظور 1): زاوية -90 درجة حول محور X فتستوي أفقياً على الطاولة
        // عند standProgress = 1.0 (المنظور 2): زاوية -15 درجة حول محور X فتقف طولياً متجهة نحو عين اللاعب
        float pitchAngle = glm::mix(-90.0f, -15.0f, standProgress);

        // رفع مركز البطاقة مع وقوفها بحيث ترتكز حافتها السفلية دائماً على سطح الطاولة بنعومة
        glm::vec3 renderPos = currentPos;
        float liftPivot = (height * 0.5f) * std::sin(glm::radians(pitchAngle + 90.0f));
        renderPos.y += liftPivot;

        // أ) نقل البطاقة إلى موقعها ثلاثي الأبعاد
        model = glm::translate(model, renderPos);

        // ب) دوران خفيف أثناء الطيران لإعطاء حركة فيزيائية جذابة
        if (state == CardState::ANIMATING_TO_PLAY) {
            float wobble = std::sin(moveProgress * 3.14159265f) * 12.0f;
            model = glm::rotate(model, glm::radians(wobble), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        model = glm::rotate(model, glm::radians(pitchAngle), glm::vec3(1.0f, 0.0f, 0.0f));

        shader.setMat4("model", model);
        shader.setBool("useTexture", true);
        shader.setVec4("objectColor", glm::vec4(1.0f));

        // 2. رسم الوجه الأمامي (صورة الشخصية المفضلة)
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frontTextureId);
        shader.setInt("texture_diffuse1", 0);
        frontMesh.draw();

        // 3. رسم الوجه الخلفي (الزخرفة السحرية لخلفية البطاقة)
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backTextureId);
        shader.setInt("texture_diffuse1", 0);
        backMesh.draw();
    }
};
