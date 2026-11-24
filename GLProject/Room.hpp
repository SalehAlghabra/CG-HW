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

    // مجسم الملصق الفني الممزق في الجدار الخلفي (Requirement 6 - The Torn Art Poster)
    SimpleMesh tornPosterMesh;

    // قطع الديكور الثلاث الأساسية (Decor Pieces 1, 2, 3 - Requirement 5):
    // Decor 1: قاعدة خشبية فاخرة + مجسم ثلاثي الأبعاد لتمثال الأوسكار الذهبي (Assimp 3D Model)
    SimpleMesh oscarPedestalMesh;
    SimpleMesh oscarPedestalCapMesh;
    Model* oscar3DModel = nullptr;

    // Decor 2: خزانة المشاهير الفاخرة للأسطوانات البلاتينية والجوائز (Celebrity VIP Showcase)
    SimpleMesh showcaseFrameMesh;
    SimpleMesh showcaseFrontMesh;

    // Decor 3: شعار نجمة هوليوود وأشعة الأضواء الذهبية (Walk of Fame Star Crest & Golden Beams)
    SimpleMesh starCrestMesh;
    SimpleMesh beamBladeMesh;

    // قطعة الديكور الإضافية المميزة لسمة المشاهير (Decor 4 - VIP Red Carpet & Stanchions):
    SimpleMesh redCarpetMesh;
    SimpleMesh redCarpetBorderMesh;
    SimpleMesh stanchionPostMesh;
    SimpleMesh stanchionBaseMesh;
    SimpleMesh stanchionBallMesh;
    SimpleMesh velvetRopeLeft1;
    SimpleMesh velvetRopeLeft2;
    SimpleMesh velvetRopeRight1;
    SimpleMesh velvetRopeRight2;

    // معرفات التكستشر للغرفة والديكور
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

        // 1. إنشاء مجسمات أرضية وجدران الغرفة
        floorMesh = MeshBuilder::createQuad(roomWidth, roomDepth, 4.0f, 4.0f, true);
        wallMesh = MeshBuilder::createQuad(roomWidth, roomHeight, 4.0f, 2.0f, false);
        ceilingMesh = MeshBuilder::createQuad(roomWidth, roomDepth, 4.0f, 4.0f, true);

        // 2. إنشاء مجسم الملصق الفني الممزق في الجدار الخلفي (Torn Celebrity Gala Poster)
        // موضوع على الجدار الخلفي ليظهر بوضوح تام خلف طاولة اللعب في منظور اللاعب، مع زاوية سفلية ممزقة هندسياً
        tornPosterMesh = MeshBuilder::createTornPosterBackWall(2.8f, 2.8f);

        // 3. تجهيز قطع الديكور الثلاث:
        // أ) قطعة الديكور 1: قاعدة تمثال الأوسكار الذهبي ثلاثي الأبعاد
        oscarPedestalMesh = MeshBuilder::createBox(0.85f, 2.15f, 0.85f, 1.0f, 2.0f);
        oscarPedestalCapMesh = MeshBuilder::createBox(1.0f, 0.10f, 1.0f, 1.0f, 1.0f);
        try {
            oscar3DModel = new Model(modelPath);
            std::cout << "[Room] Celebrity 3D Model loaded successfully: " << modelPath << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[Room] Error loading 3D model: " << e.what() << std::endl;
        }

        // ب) قطعة الديكور 2: خزانة المشاهير للأسطوانات البلاتينية والجوائز (Celebrity VIP Showcase)
        // إطار ثلاثي الأبعاد أنيق مع لوحة أمامية مكسوة بالأسطوانات البلاتينية والذهبية دون أي تشوه
        showcaseFrameMesh = MeshBuilder::createBox(2.8f, 4.2f, 0.65f, 1.0f, 1.0f);
        showcaseFrontMesh = MeshBuilder::createQuad(2.65f, 4.05f, 1.0f, 1.0f, false);

        // ج) قطعة الديكور 3: نجمة هوليوود وأشعة الأضواء الذهبية أعلى الجدار الخلفي
        starCrestMesh = MeshBuilder::createQuad(2.2f, 2.2f, 1.0f, 1.0f, false);
        beamBladeMesh = MeshBuilder::createBox(0.08f, 3.8f, 0.04f, 1.0f, 1.0f);

        // د) قطعة الديكور الإضافية المميزة لسمة المشاهير (Decor 4: VIP Red Carpet & Brass Stanchions)
        redCarpetMesh = MeshBuilder::createQuad(2.3f, 6.2f, 1.0f, 1.0f, true);
        redCarpetBorderMesh = MeshBuilder::createQuad(2.45f, 6.3f, 1.0f, 1.0f, true);

        stanchionPostMesh = MeshBuilder::createCylinder(0.042f, 0.042f, 1.15f, 16);
        stanchionBaseMesh = MeshBuilder::createCylinder(0.22f, 0.17f, 0.08f, 16);
        stanchionBallMesh = MeshBuilder::createSphere(0.072f, 12, 16);

        // حبال المخمل الحمراء المنحنية بدقة في الفضاء ثلاثي الأبعاد
        velvetRopeLeft1  = MeshBuilder::createCurvedRope(glm::vec3(-1.35f, 1.05f, -2.0f), glm::vec3(-1.35f, 1.05f, -4.4f), 0.22f, 0.032f);
        velvetRopeLeft2  = MeshBuilder::createCurvedRope(glm::vec3(-1.35f, 1.05f, -4.4f), glm::vec3(-1.35f, 1.05f, -6.8f), 0.22f, 0.032f);
        velvetRopeRight1 = MeshBuilder::createCurvedRope(glm::vec3( 1.35f, 1.05f, -2.0f), glm::vec3( 1.35f, 1.05f, -4.4f), 0.22f, 0.032f);
        velvetRopeRight2 = MeshBuilder::createCurvedRope(glm::vec3( 1.35f, 1.05f, -4.4f), glm::vec3( 1.35f, 1.05f, -6.8f), 0.22f, 0.032f);
    }

    void render(Shader& shader) {
        shader.use();
        shader.setBool("useTexture", true);
        shader.setVec4("objectColor", glm::vec4(1.0f));

        // -------------------------------------------------------------
        // 1. رسم أرضية الغرفة الخشبية الفاخرة المصقولة (Polished Hardwood Floor)
        // -------------------------------------------------------------
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTextureId);
        shader.setInt("texture_diffuse1", 0);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        shader.setMat4("model", model);
        floorMesh.draw();

        // -------------------------------------------------------------
        // 2. رسم جدران الغرفة مع ورق الجدران الفاخر آرت ديكو (VIP Wallpaper)
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

        shader.setBool("useTexture", true);
        shader.setVec4("objectColor", glm::vec4(1.0f));

        // -------------------------------------------------------------
        // 3. رسم الملصق الفني الممزق في الجدار الخلفي (The Torn Celebrity Poster)
        // معلق على الجدار الخلفي ويظهر خلفه ورق الجدران بوضوح تام من الزاوية الممزقة
        // -------------------------------------------------------------
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, posterTextureId);
        shader.setInt("texture_diffuse1", 0);

        model = glm::mat4(1.0f);
        // نضعه على بعد ميليمترات طفيفة من الجدار الخلفي (-7.95) ليكون في مجال رؤية اللاعب تماماً
        model = glm::translate(model, glm::vec3(-2.6f, 4.4f, -roomDepth * 0.5f + 0.05f));
        shader.setMat4("model", model);
        tornPosterMesh.draw();

        // -------------------------------------------------------------
        // 4. رسم قطع الديكور الثلاث الأساسية (The 3 Diverse Celebrity Decor Pieces)
        // -------------------------------------------------------------

        // --- قطعة الديكور 1: قاعدة فاخرة + تمثال الأوسكار الذهبي ثلاثي الأبعاد (Assimp 3D Model) ---
        // موقعها بجانب الطاولة في المنظور الأمامي بوضوح تام
        glm::vec3 oscarPos(-3.3f, 2.2f, -1.8f);

        // أ) رسم القاعدة الخشبية المصقولة
        glBindTexture(GL_TEXTURE_2D, floorTextureId);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(oscarPos.x, 1.075f, oscarPos.z));
        shader.setMat4("model", model);
        oscarPedestalMesh.draw();

        // غطاء القاعدة الذهبي الفاخر
        shader.setBool("useTexture", false);
        shader.setVec4("objectColor", glm::vec4(0.85f, 0.70f, 0.22f, 1.0f));
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(oscarPos.x, 2.15f, oscarPos.z));
        shader.setMat4("model", model);
        oscarPedestalCapMesh.draw();

        // ب) رسم مجسم تمثال الأوسكار الذهبي ثلاثي الأبعاد باستخدام Assimp
        if (oscar3DModel) {
            shader.setBool("useTexture", true);
            shader.setVec4("objectColor", glm::vec4(1.0f));
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(oscarPos.x, oscarPos.y, oscarPos.z));
            model = glm::scale(model, glm::vec3(1.15f));
            shader.setMat4("model", model);
            oscar3DModel->Draw(shader);
        }

        // --- قطعة الديكور 2: خزانة المشاهير للأسطوانات البلاتينية والجوائز (Celebrity VIP Showcase) ---
        // بجانب الجدار الأيمن للغرفة بأبعاد واقعية وإكساء فاخر بدون أي تشوه
        // أ) جسم الخزانة الخشبي
        shader.setBool("useTexture", true);
        shader.setVec4("objectColor", glm::vec4(1.0f));
        glBindTexture(GL_TEXTURE_2D, floorTextureId);

        glm::vec3 showcasePos(roomWidth * 0.5f - 0.45f, 2.1f, -2.2f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, showcasePos);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", model);
        showcaseFrameMesh.draw();

        // ب) الواجهة الأمامية للخزانة (الأسطوانات البلاتينية والذهبية وجوائز المشاهير)
        glBindTexture(GL_TEXTURE_2D, bookshelfTextureId);
        model = glm::mat4(1.0f);
        model = glm::translate(model, showcasePos + glm::vec3(-0.33f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", model);
        showcaseFrontMesh.draw();

        // --- قطعة الديكور 3: نجمة ممر الشهرة وأشعة الأضواء الذهبية (Walk of Fame Star Crest) ---
        // معلقة في أعلى الجدار الخلفي فوق الطاولة
        glm::vec3 starPos(0.0f, 5.6f, -roomDepth * 0.5f + 0.12f);

        // أولاً: رسم شعاعي الأضواء الذهبية المتقاطعين
        shader.setBool("useTexture", false);
        shader.setVec4("objectColor", glm::vec4(0.92f, 0.78f, 0.25f, 1.0f)); // ذهبي لماع

        // الشعاع الأول (+45 درجة)
        model = glm::mat4(1.0f);
        model = glm::translate(model, starPos + glm::vec3(0.0f, 0.0f, -0.04f));
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        shader.setMat4("model", model);
        beamBladeMesh.draw();

        // الشعاع الثاني (-45 درجة)
        model = glm::mat4(1.0f);
        model = glm::translate(model, starPos + glm::vec3(0.0f, 0.0f, -0.04f));
        model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        shader.setMat4("model", model);
        beamBladeMesh.draw();

        // ثانياً: رسم نجمة المشاهير الذهبية في المنتصف
        shader.setBool("useTexture", true);
        shader.setVec4("objectColor", glm::vec4(1.0f));
        glBindTexture(GL_TEXTURE_2D, shieldTextureId);

        model = glm::mat4(1.0f);
        model = glm::translate(model, starPos);
        shader.setMat4("model", model);
        starCrestMesh.draw();

        // -------------------------------------------------------------
        // 5. رسم قطعة الديكور الإضافية: السجادة الحمراء وأعمدة الحبال المخملية (VIP Red Carpet)
        // -------------------------------------------------------------
        // أ) إطار السجادة الذهبي
        shader.setBool("useTexture", false);
        shader.setVec4("objectColor", glm::vec4(0.85f, 0.70f, 0.22f, 1.0f));
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.003f, -4.6f));
        shader.setMat4("model", model);
        redCarpetBorderMesh.draw();

        // ب) السجادة الحمراء الملكية
        shader.setVec4("objectColor", glm::vec4(0.72f, 0.07f, 0.12f, 1.0f));
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.006f, -4.6f));
        shader.setMat4("model", model);
        redCarpetMesh.draw();

        // ج) أعمدة الحواجز النحاسية الذهبية (6 Stanchions)
        shader.setVec4("objectColor", glm::vec4(0.92f, 0.78f, 0.28f, 1.0f)); // لمعان النحاس الذهبي

        float stanchionXs[2] = {-1.35f, 1.35f};
        float stanchionZs[3] = {-2.0f, -4.4f, -6.8f};

        for (float sx : stanchionXs) {
            for (float sz : stanchionZs) {
                // القاعدة الدائرية الثقيلة
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(sx, 0.04f, sz));
                shader.setMat4("model", model);
                stanchionBaseMesh.draw();

                // العمود الأسطواني النحاسي
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(sx, 0.58f, sz));
                shader.setMat4("model", model);
                stanchionPostMesh.draw();

                // الكرة الذهبية العلوية
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(sx, 1.18f, sz));
                shader.setMat4("model", model);
                stanchionBallMesh.draw();
            }
        }

        // د) حبال المخمل الحمراء المنحنية بأناقة بين الأعمدة
        shader.setVec4("objectColor", glm::vec4(0.68f, 0.06f, 0.10f, 1.0f)); // مخمل أحمر غامق
        model = glm::mat4(1.0f);
        shader.setMat4("model", model);

        velvetRopeLeft1.draw();
        velvetRopeLeft2.draw();
        velvetRopeRight1.draw();
        velvetRopeRight2.draw();

        shader.setBool("useTexture", true);
        shader.setVec4("objectColor", glm::vec4(1.0f));
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
        velvetRopeRight1.cleanup();
        velvetRopeRight2.cleanup();
        if (oscar3DModel) {
            delete oscar3DModel;
            oscar3DModel = nullptr;
        }
    }
};
