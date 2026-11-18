#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <learnopengl/shader.h>
#include <learnopengl/model.h>
#include <iostream>
#include "MeshBuilder.hpp"

class Room {
public:
    // مجسمات الغرفة
    SimpleMesh floorMesh;
    SimpleMesh wallMesh;
    SimpleMesh ceilingMesh;

    // مجسم الملصق الممزق (Requirement 6)
    SimpleMesh tornPosterMesh;

    // مجسمات قطع الديكور الثلاث (Decor Pieces 1, 2, 3 - Requirement 5)
    // Decor 1: قاعدة خشبية + موديل الكأس الذهبي ثلاثي الأبعاد (Assimp 3D Model)
    SimpleMesh chalicePedestalMesh;
    Model* chalice3DModel = nullptr;

    // Decor 2: خزانة كتب فخمة ثلاثية الأبعاد مع صفوف كتب ملونة
    SimpleMesh bookshelfMesh;

    // Decor 3: درع فروسية عتيق وسيوف متقاطعة على الحائط
    SimpleMesh shieldMesh;
    SimpleMesh swordBladeMesh;
    SimpleMesh swordGuardMesh;

    // معرفات التكستشر للغرفة والديكور
    unsigned int floorTextureId = 0;
    unsigned int wallpaperTextureId = 0;
    unsigned int posterTextureId = 0;
    unsigned int bookshelfTextureId = 0;
    unsigned int shieldTextureId = 0;

    // أبعاد الغرفة الواسعة (لتتسع لقطع الديكور كما هو مطلوب)
    float roomWidth = 16.0f;   // محور X (-8 إلى +8)
    float roomDepth = 16.0f;   // محور Z (-8 إلى +8)
    float roomHeight = 9.0f;   // محور Y (0 إلى 9.0) جدران مرتفعة وأنيقة بدون سقف مصمت يحجب الرؤية العلوية

    Room() = default;

    void init(unsigned int floorTex, unsigned int wallTex, unsigned int posterTex,
              unsigned int bookTex, unsigned int shieldTex, const std::string& modelPath) {
        floorTextureId = floorTex;
        wallpaperTextureId = wallTex;
        posterTextureId = posterTex;
        bookshelfTextureId = bookTex;
        shieldTextureId = shieldTex;

        // 1. إنشاء مجسمات أرضية وجدران وسقف الغرفة
        floorMesh = MeshBuilder::createQuad(roomWidth, roomDepth, 4.0f, 4.0f, true);
        wallMesh = MeshBuilder::createQuad(roomWidth, roomHeight, 4.0f, 2.0f, false);
        ceilingMesh = MeshBuilder::createQuad(roomWidth, roomDepth, 4.0f, 4.0f, true);

        // 2. إنشاء مجسم الملصق الفني الممزق (Vincent van Gogh - The Starry Night)
        // الملصق مقطوع وممزق من الزاوية السفلية اليمينية بشكل هندسي واضح تظهر خلفه خلفية الحائط!
        tornPosterMesh = MeshBuilder::createTornPoster(2.4f, 2.4f);

        // 3. تجهيز قطع الديكور الثلاث:
        // أ) قطعة الديكور 1: قاعدة خشبية للكأس الذهبي
        chalicePedestalMesh = MeshBuilder::createBox(0.9f, 2.3f, 0.9f, 1.0f, 2.0f);
        try {
            chalice3DModel = new Model(modelPath);
            std::cout << "[Room] 3D Model loaded successfully: " << modelPath << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[Room] Error loading 3D model: " << e.what() << std::endl;
        }

        // ب) قطعة الديكور 2: خزانة الكتب الجدارية الكبيرة (Bookshelf)
        bookshelfMesh = MeshBuilder::createBox(1.2f, 4.8f, 3.4f, 1.0f, 1.0f);

        // ج) قطعة الديكور 3: درع الفروسية والسيوف المتقاطعة (Knight Shield & Crossed Swords)
        shieldMesh = MeshBuilder::createQuad(1.8f, 2.2f, 1.0f, 1.0f, false);
        swordBladeMesh = MeshBuilder::createBox(0.08f, 3.2f, 0.04f, 1.0f, 1.0f);
        swordGuardMesh = MeshBuilder::createBox(0.55f, 0.08f, 0.08f, 1.0f, 1.0f);
    }

    void render(Shader& shader) {
        shader.use();
        shader.setBool("useTexture", true);
        shader.setVec4("objectColor", glm::vec4(1.0f));

        // -------------------------------------------------------------
        // 1. رسم أرضية الغرفة الخشبية (Wooden Floor)
        // -------------------------------------------------------------
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTextureId);
        shader.setInt("texture_diffuse1", 0);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        shader.setMat4("model", model);
        floorMesh.draw();

        // -------------------------------------------------------------
        // 2. رسم جدران الغرفة مع ورق الجدران الأنيق (Wallpaper)
        // -------------------------------------------------------------
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

        // تم الاستغناء عن السقف المصمت للسماح برؤية علوية صافية تماماً (Top-down view)
        shader.setBool("useTexture", true);
        shader.setVec4("objectColor", glm::vec4(1.0f));

        // -------------------------------------------------------------
        // 3. رسم الملصق الفني الممزق (The Torn Art Poster - Requirement 6)
        // معلق على الجدار الأيسر للغرفة وتظهر خلفه خلفية الجدار من مكان التمزق
        // -------------------------------------------------------------
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, posterTextureId);
        shader.setInt("texture_diffuse1", 0);

        model = glm::mat4(1.0f);
        // نضعه على بعد ميليمترات طفيفة من الجدار الأيسر لمنع z-fighting (-7.96)
        model = glm::translate(model, glm::vec3(-roomWidth * 0.5f + 0.04f, 4.2f, 0.0f));
        shader.setMat4("model", model);
        tornPosterMesh.draw();

        // -------------------------------------------------------------
        // 4. رسم قطع الديكور الثلاث (The 3 Diverse Decor Pieces - Requirement 5)
        // -------------------------------------------------------------

        // --- قطعة الديكور 1: قاعدة خشبية + موديل 3D للكأس الذهبي الأثري (Assimp 3D Model) ---
        // موقعها في زاوية الغرفة
        glm::vec3 chalicePos(-4.8f, 2.3f, -4.5f);

        // رسم القاعدة الخشبية
        glBindTexture(GL_TEXTURE_2D, floorTextureId);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(chalicePos.x, 1.15f, chalicePos.z));
        shader.setMat4("model", model);
        chalicePedestalMesh.draw();

        // رسم موديل الكأس الذهبي فوق القاعدة باستخدام كلاس Model (Assimp)
        if (chalice3DModel) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, chalicePos);
            model = glm::scale(model, glm::vec3(1.2f)); // مقياس مناسب لحجم الكأس
            shader.setMat4("model", model);
            chalice3DModel->Draw(shader);
        }

        // --- قطعة الديكور 2: خزانة الكتب الضخمة (Decor 2: Grand Library Bookshelf) ---
        // بجانب الجدار الأيمن للغرفة
        glBindTexture(GL_TEXTURE_2D, bookshelfTextureId);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(roomWidth * 0.5f - 0.65f, 2.4f, -1.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", model);
        bookshelfMesh.draw();

        // --- قطعة الديكور 3: درع الفروسية والسيوف المتقاطعة (Decor 3: Knight Shield & Crossed Swords) ---
        // معلقة في أعلى الجدار الخلفي فوق الطاولة
        glm::vec3 shieldPos(0.0f, 5.2f, -roomDepth * 0.5f + 0.12f);

        // أولاً: رسم السيفين الفولاذيين المتقاطعين خلف الدرع
        shader.setBool("useTexture", false);
        shader.setVec4("objectColor", glm::vec4(0.85f, 0.88f, 0.92f, 1.0f)); // لمعان الفولاذ

        // السيف الأول (بزاوية +45 درجة)
        model = glm::mat4(1.0f);
        model = glm::translate(model, shieldPos + glm::vec3(0.0f, 0.0f, -0.04f));
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        shader.setMat4("model", model);
        swordBladeMesh.draw();
        // مقبض السيف الأول الذهبي
        shader.setVec4("objectColor", glm::vec4(0.85f, 0.70f, 0.20f, 1.0f));
        model = glm::translate(model, glm::vec3(0.0f, -1.2f, 0.0f));
        shader.setMat4("model", model);
        swordGuardMesh.draw();

        // السيف الثاني (بزاوية -45 درجة)
        shader.setVec4("objectColor", glm::vec4(0.85f, 0.88f, 0.92f, 1.0f));
        model = glm::mat4(1.0f);
        model = glm::translate(model, shieldPos + glm::vec3(0.0f, 0.0f, -0.04f));
        model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        shader.setMat4("model", model);
        swordBladeMesh.draw();
        // مقبض السيف الثاني الذهبي
        shader.setVec4("objectColor", glm::vec4(0.85f, 0.70f, 0.20f, 1.0f));
        model = glm::translate(model, glm::vec3(0.0f, -1.2f, 0.0f));
        shader.setMat4("model", model);
        swordGuardMesh.draw();

        // ثانياً: رسم درع الفروسية الملكي في المنتصف
        shader.setBool("useTexture", true);
        shader.setVec4("objectColor", glm::vec4(1.0f));
        glBindTexture(GL_TEXTURE_2D, shieldTextureId);

        model = glm::mat4(1.0f);
        model = glm::translate(model, shieldPos);
        shader.setMat4("model", model);
        shieldMesh.draw();
    }

    void cleanup() {
        floorMesh.cleanup();
        wallMesh.cleanup();
        ceilingMesh.cleanup();
        tornPosterMesh.cleanup();
        chalicePedestalMesh.cleanup();
        bookshelfMesh.cleanup();
        shieldMesh.cleanup();
        swordBladeMesh.cleanup();
        swordGuardMesh.cleanup();
        if (chalice3DModel) {
            delete chalice3DModel;
            chalice3DModel = nullptr;
        }
    }
};
