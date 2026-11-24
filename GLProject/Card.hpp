#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <learnopengl/shader.h>
#include <string>
#include <cmath>
#include "MeshBuilder.hpp"

enum class CardState {
    IN_DECK,            // البطاقة داخل مخزون اللاعب (مسطحة ومقلوبة على وجهها الخلفي)
    ANIMATING_TO_PLAY,  // البطاقة في حالة حركة وتحليق وتدوير لتواجه اللاعب
    PLAYED              // البطاقة مستقرة في منطقة اللعب وتواجه اللاعب
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
    float moveDuration = 1.15f;    // مدة حركة الطيران بالثواني

    // أبعاد البطاقة القياسية
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
                // Smoothstep easing for fluid motion: 3t^2 - 2t^3
                float t = moveProgress;
                float ease = t * t * (3.0f - 2.0f * t);

                currentPos.x = glm::mix(startPos.x, targetPos.x, ease);
                currentPos.z = glm::mix(startPos.z, targetPos.z, ease);
                // Parabolic arc in Y axis: lift up into air then settle down
                float arcHeight = 0.95f * std::sin(moveProgress * 3.14159265f);
                currentPos.y = glm::mix(startPos.y, targetPos.y, ease) + arcHeight;
            }
        }
    }

    // رسم البطاقة مع تطبيق مصفوفات التحويل (Translate, Rotate, Scale)
    // standProgress: 0.0 = مستوية أفقياً على الطاولة (منظور علوي)، 1.0 = واقفة طولياً تواجه اللاعب (منظور اللاعب)
    void render(Shader& shader, const SimpleMesh& frontMesh, const SimpleMesh& backMesh, float standProgress) {
        shader.use();

        glm::mat4 model = glm::mat4(1.0f);

        float pitchAngle = -90.0f;
        float flipAngle = 0.0f;
        float liftPivot = 0.0f;

        if (state == CardState::IN_DECK) {
            // البطاقات غير المسحوبة: تبقى دائماً مستوية على الطاولة أفقياً بغض النظر عن وضع الكاميرا
            // ومقلوبة لأسفل بحيث يظهر وجه البطاقة الخلفي الموحد للأعلى كما في ألعاب الورق الحقيقية
            pitchAngle = -90.0f;
            flipAngle = 180.0f; // الوجه الخلفي يواجه اللاعب/الكاميرا من الأعلى
            liftPivot = 0.0f;
        } else if (state == CardState::ANIMATING_TO_PLAY) {
            // أثناء الطيران واللعب: تدور البطاقة 180 درجة لتواجه اللاعب وترتفع بنعومة
            float t = moveProgress;
            float ease = t * t * (3.0f - 2.0f * t);

            // تدوير البطاقة من الوجه الخلفي (180 درجة) إلى الوجه الأمامي (0 درجة)
            flipAngle = (1.0f - ease) * 180.0f;

            // الانتقال السلس لزاوية الميل نحو اللاعب
            float targetPitch = glm::mix(-90.0f, -15.0f, standProgress);
            pitchAngle = glm::mix(-90.0f, targetPitch, ease);

            liftPivot = (height * 0.5f) * std::sin(glm::radians(pitchAngle + 90.0f));
        } else { // CardState::PLAYED
            // البطاقات الملعوبة: تقف طولياً لتواجه اللاعب في المنظور 2، أو تستوي في المنظور 1
            pitchAngle = glm::mix(-90.0f, -15.0f, standProgress);
            flipAngle = 0.0f; // الوجه الأمامي (صورة المشاهير) يواجه اللاعب تماماً
            liftPivot = (height * 0.5f) * std::sin(glm::radians(pitchAngle + 90.0f));
        }

        // رفع مركز البطاقة مع وقوفها بحيث ترتكز حافتها السفلية دائماً على سطح الطاولة بنعومة
        glm::vec3 renderPos = currentPos;
        renderPos.y += liftPivot;

        // 1. نقل البطاقة إلى موقعها ثلاثي الأبعاد
        model = glm::translate(model, renderPos);

        // 2. دوران فيزيائي خفيف أثناء الطيران
        if (state == CardState::ANIMATING_TO_PLAY) {
            float wobble = std::sin(moveProgress * 3.14159265f) * 14.0f;
            model = glm::rotate(model, glm::radians(wobble), glm::vec3(0.0f, 0.0f, 1.0f));
        }

        // 3. ميلان البطاقة (Pitch) نحو اللاعب أو استوائها أفقياً
        model = glm::rotate(model, glm::radians(pitchAngle), glm::vec3(1.0f, 0.0f, 0.0f));

        // 4. قلب البطاقة (Flip 180 -> 0) لإظهار صورة الشخصية عندما تلعب
        model = glm::rotate(model, glm::radians(flipAngle), glm::vec3(0.0f, 1.0f, 0.0f));

        shader.setMat4("model", model);
        shader.setBool("useTexture", true);
        shader.setVec4("objectColor", glm::vec4(1.0f));

        // رسم الوجه الأمامي (صورة المشاهير)
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frontTextureId);
        shader.setInt("texture_diffuse1", 0);
        frontMesh.draw();

        // رسم الوجه الخلفي (الإكساء الخلفي الفخم الموحد)
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backTextureId);
        shader.setInt("texture_diffuse1", 0);
        backMesh.draw();
    }
};
