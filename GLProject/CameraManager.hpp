#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

class CameraManager {
public:
    // تعريف بارامترات الحالة الأولى (المنظور العلوي من فوق الطاولة تماماً)
    // Camera State 1: Top-down view looking directly down onto the table
    const glm::vec3 posState1    = glm::vec3(0.0f, 7.8f, 0.001f);
    const glm::vec3 targetState1 = glm::vec3(0.0f, 2.05f, 0.0f);
    const glm::vec3 upState1     = glm::vec3(0.0f, 0.0f, -1.0f);

    // تعريف بارامترات الحالة الثانية (منظور اللاعب من جهته أمام الطاولة)
    // Camera State 2: Player perspective view seated in front of the table
    const glm::vec3 posState2    = glm::vec3(0.0f, 4.4f, 4.8f);
    const glm::vec3 targetState2 = glm::vec3(0.0f, 2.1f, -0.2f);
    const glm::vec3 upState2     = glm::vec3(0.0f, 1.0f, 0.0f);

    int currentState = 2; // نبدأ من منظور اللاعب افتراضياً أو المنظور العلوي
    int targetState = 2;

    float transitionProgress = 1.0f; // 0.0 = الحالة 1 (علوي)، 1.0 = الحالة 2 (اللاعب)
    float transitionSpeed = 1.2f;    // سرعة الانتقال والتحريك

    glm::vec3 currentPos;
    glm::vec3 currentTarget;
    glm::vec3 currentUp;

    CameraManager() {
        setStateImmediate(2);
    }

    void switchToState(int state) {
        if (state == 1 || state == 2) {
            targetState = state;
        }
    }

    void setStateImmediate(int state) {
        currentState = state;
        targetState = state;
        transitionProgress = (state == 1) ? 0.0f : 1.0f;
        updateVectors();
    }

    void update(float deltaTime) {
        float targetT = (targetState == 1) ? 0.0f : 1.0f;

        if (std::abs(transitionProgress - targetT) > 0.001f) {
            if (transitionProgress < targetT) {
                transitionProgress += transitionSpeed * deltaTime;
                if (transitionProgress >= targetT) {
                    transitionProgress = targetT;
                    currentState = targetState;
                }
            } else {
                transitionProgress -= transitionSpeed * deltaTime;
                if (transitionProgress <= targetT) {
                    transitionProgress = targetT;
                    currentState = targetState;
                }
            }
            updateVectors();
        } else {
            currentState = targetState;
        }
    }

    void updateVectors() {
        // تطبيق دالة التهدئة Smoothstep لانتقال سينمائي سلس ومرن: 3t^2 - 2t^3
        float t = transitionProgress;
        float ease = t * t * (3.0f - 2.0f * t);

        currentPos    = glm::mix(posState1, posState2, ease);
        currentTarget = glm::mix(targetState1, targetState2, ease);
        currentUp     = glm::normalize(glm::mix(upState1, upState2, ease));
    }

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(currentPos, currentTarget, currentUp);
    }

    // نسبة وقوف البطاقات: 0.0 = مستوية، 1.0 = واقفة طولياً
    float getCardStandProgress() const {
        float t = transitionProgress;
        return t * t * (3.0f - 2.0f * t);
    }
};
