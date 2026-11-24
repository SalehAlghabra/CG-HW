#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <learnopengl/shader.h>
#include <string>
#include <cmath>
#include "MeshBuilder.hpp"

enum class CardState {
    IN_DECK,            // Card in deck (always flat on table, face-down showing back texture)
    ANIMATING_TO_PLAY,  // Card in flight: lifts, rotates 180 degrees to face player
    PLAYED              // Card settled in played zone, standing/facing player
};

class Card {
public:
    std::string characterName;
    unsigned int frontTextureId = 0;
    unsigned int backTextureId = 0;
    CardState state = CardState::IN_DECK;

    int slotIndex = -1; // Target played slot index (0 to 4)

    // Spatial coordinates
    glm::vec3 currentPos;
    glm::vec3 startPos;
    glm::vec3 targetPos;

    float moveProgress = 0.0f;     // Flight progress [0.0 -> 1.0]
    float moveDuration = 1.15f;    // Flight duration in seconds

    // Standard card dimensions
    float width = 0.72f;
    float height = 1.08f;

    Card() = default;

    Card(const std::string& name, unsigned int frontTex, unsigned int backTex, const glm::vec3& deckPos)
        : characterName(name), frontTextureId(frontTex), backTextureId(backTex),
          state(CardState::IN_DECK), currentPos(deckPos), startPos(deckPos), targetPos(deckPos) {}

    // Initiate 3D card draw movement to target slot
    void playToSlot(int slot, const glm::vec3& slotPosition) {
        state = CardState::ANIMATING_TO_PLAY;
        slotIndex = slot;
        startPos = currentPos;
        targetPos = slotPosition;
        moveProgress = 0.0f;
    }

    // Per-frame physics and animation update
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
                // Parabolic arc along Y axis: lifts high then lands smoothly
                float arcHeight = 0.95f * std::sin(moveProgress * 3.14159265f);
                currentPos.y = glm::mix(startPos.y, targetPos.y, ease) + arcHeight;
            }
        }
    }

    // Render card with model matrices (Translate, Rotate, Scale)
    // standProgress: 0.0 = flat on table (top-down view), 1.0 = upright facing player (player view)
    void render(Shader& shader, const SimpleMesh& frontMesh, const SimpleMesh& backMesh, float standProgress) {
        shader.use();

        glm::mat4 model = glm::mat4(1.0f);

        float pitchAngle = -90.0f;
        float flipAngle = 0.0f;
        float liftPivot = 0.0f;

        if (state == CardState::IN_DECK) {
            // Undrawn cards in deck: ALWAYS flat on table regardless of camera mode,
            // face-down so the standard luxury card back is visible on top
            pitchAngle = -90.0f;
            flipAngle = 180.0f; // Back texture faces UP towards player/camera
            liftPivot = 0.0f;
        } else if (state == CardState::ANIMATING_TO_PLAY) {
            // During flight: card flips 180 degrees to reveal celebrity face
            float t = moveProgress;
            float ease = t * t * (3.0f - 2.0f * t);

            // Smooth 180 -> 0 degree flip rotation
            flipAngle = (1.0f - ease) * 180.0f;

            // Smooth pitch interpolation to target camera pitch
            float targetPitch = glm::mix(-90.0f, -15.0f, standProgress);
            pitchAngle = glm::mix(-90.0f, targetPitch, ease);

            liftPivot = (height * 0.5f) * std::sin(glm::radians(pitchAngle + 90.0f));
        } else { // CardState::PLAYED
            // Played cards: stand upright in Player view (State 2) or lie flat in Top-Down view (State 1)
            pitchAngle = glm::mix(-90.0f, -15.0f, standProgress);
            flipAngle = 0.0f; // Front face (celebrity portrait) faces player
            liftPivot = (height * 0.5f) * std::sin(glm::radians(pitchAngle + 90.0f));
        }

        // Lift card center as it tilts so bottom edge rests perfectly on table surface
        glm::vec3 renderPos = currentPos;
        renderPos.y += liftPivot;

        // 1. Translation to 3D position
        model = glm::translate(model, renderPos);

        // 2. Gentle flight wobble
        if (state == CardState::ANIMATING_TO_PLAY) {
            float wobble = std::sin(moveProgress * 3.14159265f) * 14.0f;
            model = glm::rotate(model, glm::radians(wobble), glm::vec3(0.0f, 0.0f, 1.0f));
        }

        // 3. Pitch rotation (tilt towards player or lie flat)
        model = glm::rotate(model, glm::radians(pitchAngle), glm::vec3(1.0f, 0.0f, 0.0f));

        // 4. Flip rotation (180 deg face-down in deck -> 0 deg face-up to player)
        model = glm::rotate(model, glm::radians(flipAngle), glm::vec3(0.0f, 1.0f, 0.0f));

        shader.setMat4("model", model);
        shader.setBool("useTexture", true);
        shader.setVec4("objectColor", glm::vec4(1.0f));

        // Front Face (Celebrity Portrait)
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frontTextureId);
        shader.setInt("texture_diffuse1", 0);
        frontMesh.draw();

        // Back Face (Standard Luxury Card Back)
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backTextureId);
        shader.setInt("texture_diffuse1", 0);
        backMesh.draw();
    }
};
