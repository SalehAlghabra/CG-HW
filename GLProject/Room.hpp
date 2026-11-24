#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <learnopengl/shader.h>
#include <learnopengl/model.h>
#include <iostream>
#include <vector>
#include "MeshBuilder.hpp"

class Room {
public:
    // مجسمات الغرفة الأساسية
    SimpleMesh floorMesh;
    SimpleMesh wallMesh;
    SimpleMesh ceilingMesh;

    // مجسم الملصق الفني الممزق في المنتصف (Requirement 6 - The Torn Art Poster)
    SimpleMesh tornPosterMesh;

    // قطع الديكور الأساسية:
    // Decor 1: قاعدة فاخرة + مجسم تمثال الأوسكار الذهبي ثلاثي الأبعاد
    SimpleMesh oscarPedestalMesh;
    SimpleMesh oscarPedestalCapMesh;
    Model* oscar3DModel = nullptr;

    // Decor 2: خزانة المشاهير للأسطوانات والجوائز (VIP Showcase في الزاوية اليمينية وتواجه اللاعب)
    SimpleMesh showcaseFrameMesh;
    SimpleMesh showcaseFrontMesh;

    // Decor 3: نجمة ممر الشهرة في المنتصف أعلى الملصق بدون أي تداخل
    SimpleMesh starCrestMesh;
    SimpleMesh beamBladeMesh;

    // Decor 4: السجادة الحمراء في الزاوية اليسارية مع أعمدة الحواجز والحبال المخملية
    SimpleMesh redCarpetMesh;
    SimpleMesh redCarpetBorderMesh;
    SimpleMesh stanchionPostMesh;
    SimpleMesh stanchionBaseMesh;
    SimpleMesh stanchionBallMesh;

    SimpleMesh velvetRopeLeft1;
    SimpleMesh velvetRopeLeft2;
    SimpleMesh velvetRopeLeft3;
    SimpleMesh velvetRopeRight1;
    SimpleMesh velvetRopeRight2;
    SimpleMesh velvetRopeRight3;

    // معرفات الإكساءات
    unsigned int floorTextureId = 0;
    unsigned int wallpaperTextureId = 0;
    unsigned int posterTextureId = 0;
    unsigned int bookshelfTextureId = 0;
    unsigned int shieldTextureId = 0;

    // أبعاد الغرفة الواسعة
    float roomWidth = 16.0f;   // محور X (-8 إلى +8)
    float roomDepth = 16.0f;   // محور Z (-8 إلى +8)
    float roomHeight = 9.0f;   // محور Y (0 إلى 9.0)

    Room() = default;

    void init(unsigned int floorTex, unsigned int wallTex, unsigned int posterTex,
              unsigned int bookTex, unsigned int shieldTex, const std::string& modelPath) {
        floorTextureId = floorTex;
        wallpaperTextureId = wallTex;
        posterTextureId = posterTex;
        bookshelfTextureId = bookTex;
        shieldTextureId = shieldTex;

        // 1. أرضية وجدران الغرفة
        floorMesh = MeshBuilder::createQuad(roomWidth, roomDepth, 4.0f, 4.0f, true);
        wallMesh = MeshBuilder::createQuad(roomWidth, roomHeight, 4.0f, 2.0f, false);
        ceilingMesh = MeshBuilder::createQuad(roomWidth, roomDepth, 4.0f, 4.0f, true);

        // 2. الملصق الفني الممزق: بأبعاد متوافقة تماماً مع نسبة أبعاد الصورة (Aspect Ratio = 960x1200 = 0.8)
        tornPosterMesh = MeshBuilder::createTornPosterBackWall(1.68f, 2.10f);

        // 3. قاعدة تمثال الأوسكار ثلاثي الأبعاد
        oscarPedestalMesh = MeshBuilder::createBox(0.85f, 2.15f, 0.85f, 1.0f, 2.0f);
        oscarPedestalCapMesh = MeshBuilder::createBox(1.0f, 0.10f, 1.0f, 1.0f, 1.0f);
        try {
            oscar3DModel = new Model(modelPath);
            std::cout << "[Room] Celebrity 3D Model loaded successfully: " << modelPath << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[Room] Error loading 3D model: " << e.what() << std::endl;
        }

        // 4. خزانة المشاهير (VIP Showcase): في الزاوية اليمينية خلف الخصم ومواجهة لمنظور اللاعب
        showcaseFrameMesh = MeshBuilder::createBox(2.8f, 3.6f, 0.65f, 1.0f, 1.0f);
        showcaseFrontMesh = MeshBuilder::createQuad(2.65f, 3.45f, 1.0f, 1.0f, false);

        // 5. نجمة ممر الشهرة وأشعتها: في المنطقة الوسطى أعلى الملصق بدون أي تقاطع أو تداخل
        starCrestMesh = MeshBuilder::createQuad(1.1f, 1.1f, 1.0f, 1.0f, false);
        beamBladeMesh = MeshBuilder::createBox(0.06f, 1.6f, 0.03f, 1.0f, 1.0f);

        // 6. السجادة الحمراء في الزاوية اليسارية مع أعمدة الحواجز الذهبية
        redCarpetMesh = MeshBuilder::createQuad(2.2f, 9.0f, 1.0f, 1.0f, true);
        redCarpetBorderMesh = MeshBuilder::createQuad(2.35f, 9.15f, 1.0f, 1.0f, true);

        stanchionPostMesh = MeshBuilder::createCylinder(0.042f, 0.042f, 1.15f, 16);
        stanchionBaseMesh = MeshBuilder::createCylinder(0.22f, 0.17f, 0.08f, 16);
        stanchionBallMesh = MeshBuilder::createSphere(0.072f, 12, 16);

        // حبال المخمل على جانبي مسار السجادة اليسارية (x = -6.5)
        velvetRopeLeft1 = MeshBuilder::createCurvedRope(glm::vec3(-7.65f, 1.05f,  3.0f), glm::vec3(-7.65f, 1.05f,  0.5f), 0.18f, 0.032f);
        velvetRopeLeft2 = MeshBuilder::createCurvedRope(glm::vec3(-7.65f, 1.05f,  0.5f), glm::vec3(-7.65f, 1.05f, -2.0f), 0.18f, 0.032f);
        velvetRopeLeft3 = MeshBuilder::createCurvedRope(glm::vec3(-7.65f, 1.05f, -2.0f), glm::vec3(-7.65f, 1.05f, -4.5f), 0.18f, 0.032f);

        velvetRopeRight1 = MeshBuilder::createCurvedRope(glm::vec3(-5.35f, 1.05f,  3.0f), glm::vec3(-5.35f, 1.05f,  0.5f), 0.18f, 0.032f);
        velvetRopeRight2 = MeshBuilder::createCurvedRope(glm::vec3(-5.35f, 1.05f,  0.5f), glm::vec3(-5.35f, 1.05f, -2.0f), 0.18f, 0.032f);
        velvetRopeRight3 = MeshBuilder::createCurvedRope(glm::vec3(-5.35f, 1.05f, -2.0f), glm::vec3(-5.35f, 1.05f, -4.5f), 0.18f, 0.032f);
    }

    void render(Shader& shader) {
        shader.use();
        shader.setBool("useTexture", true);
        shader.setVec4("objectColor", glm::vec4(1.0f));

        // -------------------------------------------------------------
        // 1. أرضية وجدران الغرفة
        // -------------------------------------------------------------
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTextureId);
        shader.setInt("texture_diffuse1", 0);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        shader.setMat4("model", model);
        floorMesh.draw();

        glBindTexture(GL_TEXTURE_2D, wallpaperTextureId);

        // الجدار الخلفي (-Z)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, roomHeight * 0.5f, -roomDepth * 0.5f));
        shader.setMat4("model", model);
        wallMesh.draw();

        // الجدار الأيسر (-X)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-roomWidth * 0.5f, roomHeight * 0.5f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", model);
        wallMesh.draw();

        // الجدار الأيمن (+X)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(roomWidth * 0.5f, roomHeight * 0.5f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", model);
        wallMesh.draw();

        // -------------------------------------------------------------
        // 2. السجادة الحمراء وأعمدة الحواجز في الزاوية اليسارية (Left Bottom Corner at x = -6.5)
        // -------------------------------------------------------------
        glm::vec3 carpetPos(-6.5f, 0.0f, -1.0f);

        // إطار السجادة الذهبي
        shader.setBool("useTexture", false);
        shader.setVec4("objectColor", glm::vec4(0.85f, 0.70f, 0.22f, 1.0f));
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(carpetPos.x, 0.003f, carpetPos.z));
        shader.setMat4("model", model);
        redCarpetBorderMesh.draw();

        // السجادة الحمراء
        shader.setVec4("objectColor", glm::vec4(0.75f, 0.07f, 0.12f, 1.0f));
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(carpetPos.x, 0.006f, carpetPos.z));
        shader.setMat4("model", model);
        redCarpetMesh.draw();

        // أعمدة الحواجز النحاسية على جانبي السجادة اليسارية
        shader.setVec4("objectColor", glm::vec4(0.92f, 0.78f, 0.28f, 1.0f));

        float stanchionXs[2] = {-7.65f, -5.35f};
        float stanchionZs[4] = {3.0f, 0.5f, -2.0f, -4.5f};

        for (float sx : stanchionXs) {
            for (float sz : stanchionZs) {
                // القاعدة
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(sx, 0.04f, sz));
                shader.setMat4("model", model);
                stanchionBaseMesh.draw();

                // العمود
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(sx, 0.58f, sz));
                shader.setMat4("model", model);
                stanchionPostMesh.draw();

                // الكرة العلوية
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(sx, 1.18f, sz));
                shader.setMat4("model", model);
                stanchionBallMesh.draw();
            }
        }

        // حبال المخمل الحمراء على السجادة اليسارية
        shader.setVec4("objectColor", glm::vec4(0.70f, 0.06f, 0.10f, 1.0f));
        model = glm::mat4(1.0f);
        shader.setMat4("model", model);

        velvetRopeLeft1.draw();
        velvetRopeLeft2.draw();
        velvetRopeLeft3.draw();
        velvetRopeRight1.draw();
        velvetRopeRight2.draw();
        velvetRopeRight3.draw();

        // تمثال الأوسكار في نهاية ممر السجادة اليسارية
        glm::vec3 oscarPos(-6.5f, 2.2f, -5.0f);

        glBindTexture(GL_TEXTURE_2D, floorTextureId);
        shader.setBool("useTexture", true);
        shader.setVec4("objectColor", glm::vec4(1.0f));
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(oscarPos.x, 1.075f, oscarPos.z));
        shader.setMat4("model", model);
        oscarPedestalMesh.draw();

        shader.setBool("useTexture", false);
        shader.setVec4("objectColor", glm::vec4(0.85f, 0.70f, 0.22f, 1.0f));
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(oscarPos.x, 2.15f, oscarPos.z));
        shader.setMat4("model", model);
        oscarPedestalCapMesh.draw();

        if (oscar3DModel) {
            shader.setBool("useTexture", true);
            shader.setVec4("objectColor", glm::vec4(1.0f));
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(oscarPos.x, oscarPos.y, oscarPos.z));
            model = glm::scale(model, glm::vec3(1.15f));
            shader.setMat4("model", model);
            oscar3DModel->Draw(shader);
        }

        // -------------------------------------------------------------
        // 3. خزانة المشاهير في الزاوية اليمينية (Right Bottom Corner - VIP Showcase)
        // موضوعة خلف الخصم وموجهة مباشرة نحو منظور اللاعب
        // -------------------------------------------------------------
        glm::vec3 showcasePos(3.5f, 1.8f, -3.8f);

        // هيكل الخزانة
        shader.setBool("useTexture", true);
        shader.setVec4("objectColor", glm::vec4(1.0f));
        glBindTexture(GL_TEXTURE_2D, floorTextureId);
        model = glm::mat4(1.0f);
        model = glm::translate(model, showcasePos);
        shader.setMat4("model", model);
        showcaseFrameMesh.draw();

        // واجهة الأسطوانات والجوائز
        glBindTexture(GL_TEXTURE_2D, bookshelfTextureId);
        model = glm::mat4(1.0f);
        model = glm::translate(model, showcasePos + glm::vec3(0.0f, 0.0f, 0.33f));
        shader.setMat4("model", model);
        showcaseFrontMesh.draw();

        // -------------------------------------------------------------
        // -------------------------------------------------------------
        // 4. المنطقة الوسطى: نجمة ممر الشهرة في المنتصف تماماً، والملصق الفني الممزق على يسارها
        // -------------------------------------------------------------
        // أ) نجمة ممر الشهرة في المنتصف تماماً (X = 0.0, Y = 2.25) واضحة في مجال رؤية اللاعب
        glm::vec3 starPos(0.0f, 2.25f, -roomDepth * 0.5f + 0.07f);

        // أشعة الأضواء الذهبية المتقاطعة
        shader.setBool("useTexture", false);
        shader.setVec4("objectColor", glm::vec4(0.92f, 0.78f, 0.25f, 1.0f));

        model = glm::mat4(1.0f);
        model = glm::translate(model, starPos + glm::vec3(0.0f, 0.0f, -0.02f));
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        shader.setMat4("model", model);
        beamBladeMesh.draw();

        model = glm::mat4(1.0f);
        model = glm::translate(model, starPos + glm::vec3(0.0f, 0.0f, -0.02f));
        model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        shader.setMat4("model", model);
        beamBladeMesh.draw();

        // نجمة المشاهير الذهبية
        shader.setBool("useTexture", true);
        shader.setVec4("objectColor", glm::vec4(1.0f));
        glBindTexture(GL_TEXTURE_2D, shieldTextureId);

        model = glm::mat4(1.0f);
        model = glm::translate(model, starPos);
        shader.setMat4("model", model);
        starCrestMesh.draw();

        // ب) الملصق الفني الممزق على يسار النجمة (X = -2.10, Y = 2.15) بأبعاده الصحيحة
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, posterTextureId);
        shader.setInt("texture_diffuse1", 0);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.10f, 2.15f, -roomDepth * 0.5f + 0.05f));
        shader.setMat4("model", model);
        tornPosterMesh.draw();
    }

    void cleanup() {
        floorMesh.cleanup();
        wallMesh.cleanup();
        ceilingMesh.cleanup();
        tornPosterMesh.cleanup();
        oscarPedestalMesh.cleanup();
        oscarPedestalCapMesh.cleanup();
        showcaseFrameMesh.cleanup();
        showcaseFrontMesh.cleanup();
        starCrestMesh.cleanup();
        beamBladeMesh.cleanup();
        redCarpetMesh.cleanup();
        redCarpetBorderMesh.cleanup();
        stanchionPostMesh.cleanup();
        stanchionBaseMesh.cleanup();
        stanchionBallMesh.cleanup();
        velvetRopeLeft1.cleanup();
        velvetRopeLeft2.cleanup();
        velvetRopeLeft3.cleanup();
        velvetRopeRight1.cleanup();
        velvetRopeRight2.cleanup();
        velvetRopeRight3.cleanup();
        if (oscar3DModel) {
            delete oscar3DModel;
            oscar3DModel = nullptr;
        }
    }
};
