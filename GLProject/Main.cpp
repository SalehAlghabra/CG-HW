// ==============================================================================
// Project: Card Game 3D Scene - OpenGL & C++20
// Subject: Computer Graphics (بيانيات الحاسوب 2026)
// Requirements:
//   1. طاولة تحوي 3 أقسام: مخزون اللاعب (Deck)، بطاقات اللاعب الملعوبة، بطاقات الخصم (فارغ).
//   2. حالتان ثابتتان للكاميرا:
//      - الحالة 1: من فوق الطاولة تماماً (البطاقات موضوعة أفقياً).
//      - الحالة 2: من منظور اللاعب (البطاقات تقف طولياً ليراها اللاعب).
//      - الانتقال بينهما بالضغط على زري 1 و 2 مع تحريك سلس (Animation).
//   3. مخزون اللاعب يحوي 5 بطاقات رباعية الأضلاع بصور لشخصيات خيالية مفضلة.
//   4. الضغط على زر G ينقل بطاقة عشوائية من المخزون إلى اللعب مع تحريك (Animation).
//      - الضغط للمرة السادسة لا يفعل شيئاً.
//      - يعمل بنفس الطريقة في كلا المنظورين.
//   5. غرفة واسعة بجدران ورق جدران وأرضية خشبية وتتسع لـ 3 قطع ديكور مختلفة ومميزة.
//      - قطعة ديكور 1: موديل ثلاثي الأبعاد 3D Model (الكأس الأثري الذهبي عبر Assimp).
//      - قطعة ديكور 2: خزانة كتب فخمة ثلاثية الأبعاد (Bookshelf).
//      - قطعة ديكور 3: درع فروسية ملكي وسيوف متقاطعة على الحائط (Knight Shield & Swords).
//   6. ملصق فني (The Starry Night) على أحد الجدران ممزق وناقص من زاوية هندسياً.
// ==============================================================================

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <filesystem>

// OpenGL, Glad, GLFW, GLM
#include "libs/glad_loader_docs.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// LearnOpenGL & Project Modules
#include <learnopengl/shader.h>
#include <learnopengl/model.h>
#include "../OpenGL.SharedModule/src/abstraction/SharedModule.hpp"

#include "MeshBuilder.hpp"
#include "Table.hpp"
#include "Room.hpp"
#include "Card.hpp"
#include "CameraManager.hpp"

// ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// إعدادات النافذة الافتراضية
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 800;

// المتغيرات العامة
CameraManager cameraManager;
Table gameTable;
Room gameRoom;
std::vector<Card> cards;
SimpleMesh cardFrontMesh;
SimpleMesh cardBackMesh;

// حالة إدخال المفاتيح (Debounce لمنع تكرار الضغط في نفس الإطار)
bool keyGPressed = false;
bool key1Pressed = false;
bool key2Pressed = false;
bool keyRPressed = false;

std::string lastActionMessage = "مرحباً! اضغط [G] لسحب بطاقة، و [1] أو [2] لتبديل منظور الكاميرا.";

// دالة رد النداء لتغيير حجم نافذة العرض
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// دالة تنفيذ حركة لعب بطاقة عشوائية عند الضغط على G (المتطلب 4)
void playRandomCard() {
    // جمع كل البطاقات التي ما زالت في المخزون
    std::vector<int> deckIndices;
    for (size_t i = 0; i < cards.size(); ++i) {
        if (cards[i].state == CardState::IN_DECK) {
            deckIndices.push_back(static_cast<int>(i));
        }
    }

    // إذا لم يتبق أي بطاقة في المخزون (الضغط للمرة السادسة فما فوق): لا يحدث شيء
    if (deckIndices.empty()) {
        lastActionMessage = "تم لعب جميع البطاقات الخمس! (الضغط للمرة السادسة لا يفعل شيئاً).";
        std::cout << "[Game] " << lastActionMessage << std::endl;
        return;
    }

    // اختيار بطاقة عشوائية من المتبقي في المخزون
    int randIdx = rand() % deckIndices.size();
    int selectedCard = deckIndices[randIdx];

    // تحديد الفتحة التالية في منطقة اللعب (0 إلى 4)
    int targetSlot = 5 - static_cast<int>(deckIndices.size());

    // إطلاق حركة انتقال البطاقة ثلاثية الأبعاد
    cards[selectedCard].playToSlot(targetSlot, gameTable.playedSlots[targetSlot]);

    lastActionMessage = "تم لعب البطاقة: " + cards[selectedCard].characterName + " إلى الفتحة " + std::to_string(targetSlot + 1);
    std::cout << "[Game] " << lastActionMessage << std::endl;
}

// إعادة تشغيل الجولة وجمع البطاقات إلى المخزون (للتجربة السهلة)
void resetGame() {
    for (size_t i = 0; i < cards.size(); ++i) {
        cards[i].state = CardState::IN_DECK;
        cards[i].slotIndex = -1;
        cards[i].moveProgress = 0.0f;
        glm::vec3 dPos = gameTable.deckAreaCenter;
        dPos.y += static_cast<float>(i) * 0.005f;
        dPos.z -= static_cast<float>(i) * 0.015f;
        cards[i].currentPos = dPos;
        cards[i].startPos = dPos;
        cards[i].targetPos = dPos;
    }
    lastActionMessage = "تمت إعادة ضبط اللعبة وعودة البطاقات الخمس إلى المخزون.";
    std::cout << "[Game] " << lastActionMessage << std::endl;
}

// معالجة مدخلات لوحة المفاتيح
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // زر 1: الانتقال إلى المنظور العلوي فوق الطاولة (المتطلب 2)
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (!key1Pressed) {
            cameraManager.switchToState(1);
            lastActionMessage = "الانتقال إلى الحالة 1: منظور علوي فوق الطاولة (تستوي البطاقات أفقياً).";
            key1Pressed = true;
        }
    } else {
        key1Pressed = false;
    }

    // زر 2: الانتقال إلى منظور اللاعب (المتطلب 2)
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (!key2Pressed) {
            cameraManager.switchToState(2);
            lastActionMessage = "الانتقال إلى الحالة 2: منظور اللاعب (تقف البطاقات طولياً).";
            key2Pressed = true;
        }
    } else {
        key2Pressed = false;
    }

    // زر G: لعب بطاقة عشوائية مع تحريك (المتطلب 4)
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        if (!keyGPressed) {
            playRandomCard();
            keyGPressed = true;
        }
    } else {
        keyGPressed = false;
    }

    // زر R: إعادة تعيين (اختياري للمساعدة في الفحص)
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (!keyRPressed) {
            resetGame();
            keyRPressed = true;
        }
    } else {
        keyRPressed = false;
    }
}

std::string resolvePath(const std::string& relPath) {
    if (std::filesystem::exists(relPath)) return relPath;
    if (std::filesystem::exists("build/" + relPath)) return "build/" + relPath;
    if (std::filesystem::exists("GLProject/" + relPath)) return "GLProject/" + relPath;
    return relPath;
}

int main(int argc, char* argv[]) {
    // 1. ضبط مسار التشغيل الحالي ليطابق مسار الملف التنفيذي بأمان تام
    try {
        if (argc > 0 && argv[0] != nullptr) {
            std::filesystem::path p(argv[0]);
            if (std::filesystem::exists(p)) {
                std::filesystem::current_path(std::filesystem::canonical(p).parent_path());
            } else {
                std::filesystem::path absP = std::filesystem::absolute(p);
                if (std::filesystem::exists(absP.parent_path())) {
                    std::filesystem::current_path(absP.parent_path());
                }
            }
        }
    } catch (...) {
        // الاحتفاظ بالمسار الحالي في حال حدوث أي استثناء
    }

    srand(static_cast<unsigned int>(time(nullptr)));

    // 2. تهيئة مكتبة GLFW وإعدادات نافذة OpenGL 3.3 Core
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // Anti-aliasing

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Card Game Scene - Computer Graphics 2026", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 3. تحميل دوال OpenGL عبر GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return -1;
    }

    // 4. تفعيل اختبار العمق ومزج الشفافية (Depth testing & Alpha Blending)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);

    // 5. تهيئة واجهة ImGui للتحكم والمعلومات
    setup_ImGUI(window);

    // 6. تحميل الشيدرات (Vertex & Fragment Shaders)
    Shader sceneShader(resolvePath("shaders/vs/scene.vs").c_str(), resolvePath("shaders/fs/scene.fs").c_str());

    // 7. تحميل الإكساءات (Textures)
    std::cout << "[Init] Loading textures..." << std::endl;
    unsigned int texTable       = load_texture(resolvePath("assets/textures/table_wood.png").c_str());
    unsigned int texFloor       = load_texture(resolvePath("assets/textures/floor_wood.png").c_str());
    unsigned int texWallpaper   = load_texture(resolvePath("assets/textures/wallpaper.png").c_str());
    unsigned int texPoster      = load_texture(resolvePath("assets/textures/poster_starry_night.png").c_str());
    unsigned int texBookshelf   = load_texture(resolvePath("assets/textures/bookshelf.png").c_str());
    unsigned int texShield      = load_texture(resolvePath("assets/textures/shield.png").c_str());
    unsigned int texCardBack    = load_texture(resolvePath("assets/textures/card_back.png").c_str());

    // إكساءات بطاقات الشخصيات الخيالية الخمس (المتطلب 3)
    unsigned int texGeralt      = load_texture(resolvePath("assets/textures/card_geralt.png").c_str());
    unsigned int texGandalf     = load_texture(resolvePath("assets/textures/card_gandalf.png").c_str());
    unsigned int texBatman      = load_texture(resolvePath("assets/textures/card_batman.png").c_str());
    unsigned int texSpiderman   = load_texture(resolvePath("assets/textures/card_spiderman.png").c_str());
    unsigned int texGoku        = load_texture(resolvePath("assets/textures/card_goku.png").c_str());

    // 8. تهيئة مجسمات الطاولة والغرفة والديكور
    gameTable.init(texTable);
    gameRoom.init(texFloor, texWallpaper, texPoster, texBookshelf, texShield, resolvePath("assets/models/chalice.obj"));

    // مجسمات البطاقات (وجه أمامي وخلفي)
    cardFrontMesh = MeshBuilder::createCardFront(0.72f, 1.08f);
    cardBackMesh  = MeshBuilder::createCardBack(0.72f, 1.08f);

    // 9. إنشاء البطاقات الخمس في منطقة المخزون (Deck)
    struct CharacterCardInfo {
        std::string name;
        unsigned int textureId;
    };
    std::vector<CharacterCardInfo> cardData = {
        {"Geralt of Rivia (The Witcher)", texGeralt},
        {"Gandalf the Grey (Lord of the Rings)", texGandalf},
        {"Batman (DC Comics)", texBatman},
        {"Spider-Man (Marvel)", texSpiderman},
        {"Son Goku (Dragon Ball)", texGoku}
    };

    cards.clear();
    for (size_t i = 0; i < cardData.size(); ++i) {
        glm::vec3 dPos = gameTable.deckAreaCenter;
        dPos.y += static_cast<float>(i) * 0.005f; // تفادي تداخل الأوجه
        dPos.z -= static_cast<float>(i) * 0.015f; // تنظيم لطيف في كومة المخزون
        cards.emplace_back(cardData[i].name, cardData[i].textureId, texCardBack, dPos);
    }

    std::cout << "[Init] Scene setup complete! Starting render loop..." << std::endl;

    // توقيت الإطارات
    float lastFrameTime = static_cast<float>(glfwGetTime());

    // =========================================================================
    // حلقة التصيير الرئيسية (Main Render Loop)
    // =========================================================================
    while (!glfwWindowShouldClose(window)) {
        float currentFrameTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // معالجة المدخلات
        processInput(window);

        // تحديث حركة الكاميرا والبطاقات
        cameraManager.update(deltaTime);
        for (auto& card : cards) {
            card.update(deltaTime);
        }

        // مسح مخزن اللون ومخزن العمق
        glClearColor(0.06f, 0.07f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // حساب مصفوفات العرض والإسقاط
        int displayW, displayH;
        glfwGetFramebufferSize(window, &displayW, &displayH);
        float aspect = (displayH > 0) ? (float)displayW / (float)displayH : 1.6f;

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        glm::mat4 view = cameraManager.getViewMatrix();

        // تمرير المتغيرات العامة للشيدر (إضاءة، كاميرا، مصفوفات)
        sceneShader.use();
        sceneShader.setMat4("projection", projection);
        sceneShader.setMat4("view", view);
        sceneShader.setVec3("viewPos", cameraManager.currentPos);

        // إضاءة الغرفة (ثريا دافئة معلقة فوق الطاولة)
        sceneShader.setVec3("lightPos", glm::vec3(0.0f, 6.2f, 0.2f));
        sceneShader.setVec3("lightColor", glm::vec3(1.0f, 0.95f, 0.88f));
        sceneShader.setFloat("ambientStrength", 0.60f);

        // 1. رسم الغرفة الواسعة + ورق الجدران + الملصق الممزق + قطع الديكور الثلاث
        gameRoom.render(sceneShader);

        // 2. رسم الطاولة الخشبية بأقسامها الثلاثة (المخزون، اللعب، الخصم)
        gameTable.render(sceneShader);

        // 3. رسم البطاقات الخمس مع تطبيق مصفوفات التحريك والوقوف/التسطيح
        float standProgress = cameraManager.getCardStandProgress();
        for (auto& card : cards) {
            card.render(sceneShader, cardFrontMesh, cardBackMesh, standProgress);
        }

        // =====================================================================
        // واجهة ImGui الرسومية للتحكم وعرض حالة الوظيفة (HUD Overlay)
        // =====================================================================
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(16, 16), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(420, 310), ImGuiCond_FirstUseEver);
        ImGui::Begin("Card Game Controls & Inspector", nullptr, ImGuiWindowFlags_NoCollapse);

        ImGui::TextColored(ImVec4(0.9f, 0.8f, 0.2f, 1.0f), "Computer Graphics 2026 - Card Game");
        ImGui::Separator();

        // عرض وضع الكاميرا الحالي
        if (cameraManager.targetState == 1) {
            ImGui::Text("Camera State: [1] Top-Down View (Flat Cards)");
        } else {
            ImGui::Text("Camera State: [2] Player Perspective (Standing Cards)");
        }

        // أزرار تبديل الكاميرا
        if (ImGui::Button("State 1: Top-Down (Key 1)")) {
            cameraManager.switchToState(1);
            lastActionMessage = "تم اختيار المنظور العلوي [1] (تسطيح البطاقات).";
        }
        ImGui::SameLine();
        if (ImGui::Button("State 2: Player View (Key 2)")) {
            cameraManager.switchToState(2);
            lastActionMessage = "تم اختيار منظور اللاعب [2] (وقوف البطاقات).";
        }

        ImGui::Separator();

        // حساب عدد البطاقات المتبقية والمحركة
        int inDeckCount = 0;
        int playedCount = 0;
        for (const auto& c : cards) {
            if (c.state == CardState::IN_DECK) inDeckCount++;
            else playedCount++;
        }

        ImGui::Text("Cards in Deck: %d / 5", inDeckCount);
        ImGui::Text("Cards Played:  %d / 5", playedCount);

        // زر لعب بطاقة عشوائية
        if (ImGui::Button("Play Random Card (Key G)", ImVec2(220, 28))) {
            playRandomCard();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset (Key R)", ImVec2(100, 28))) {
            resetGame();
        }

        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.4f, 0.9f, 0.4f, 1.0f), "Status: %s", lastActionMessage.c_str());

        ImGui::Separator();
        ImGui::Text("Controls Guide:");
        ImGui::BulletText("[1]: Top-Down camera (cards lie flat)");
        ImGui::BulletText("[2]: Player perspective (cards stand upright)");
        ImGui::BulletText("[G]: Play random card to table (Animation)");
        ImGui::BulletText("[R]: Reset all cards back to deck");
        ImGui::BulletText("[ESC]: Close window");

        ImGui::End();

        // رسم واجهة ImGui فوق المشهد ثلاثي الأبعاد
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // تبديل المخازن ومعالجة الأحداث
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // تنظيف الموارد
    std::cout << "[Cleanup] Cleaning up resources..." << std::endl;
    cardFrontMesh.cleanup();
    cardBackMesh.cleanup();
    gameTable.cleanup();
    gameRoom.cleanup();

    glDeleteTextures(1, &texTable);
    glDeleteTextures(1, &texFloor);
    glDeleteTextures(1, &texWallpaper);
    glDeleteTextures(1, &texPoster);
    glDeleteTextures(1, &texBookshelf);
    glDeleteTextures(1, &texShield);
    glDeleteTextures(1, &texCardBack);
    glDeleteTextures(1, &texGeralt);
    glDeleteTextures(1, &texGandalf);
    glDeleteTextures(1, &texBatman);
    glDeleteTextures(1, &texSpiderman);
    glDeleteTextures(1, &texGoku);

    shutdown_ImGui();
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "[Shutdown] Program exited normally." << std::endl;
    return 0;
}
