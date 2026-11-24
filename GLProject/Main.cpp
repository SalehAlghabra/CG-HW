// ==============================================================================
// Project: Card Game 3D Scene - OpenGL & C++20 (Celebrity VIP Lounge Edition)
// Subject: Computer Graphics (بيانيات الحاسوب 2026)
// Requirements:
//   1. طاولة تحوي 3 أقسام: مخزون اللاعب (Deck)، بطاقات اللاعب الملعوبة، بطاقات الخصم (فارغ).
//   2. حالتان ثابتتان للكاميرا:
//      - الحالة 1: من فوق الطاولة تماماً (البطاقات موضوعة أفقياً).
//      - الحالة 2: من منظور اللاعب (البطاقات تقف طولياً ليراها اللاعب).
//      - الانتقال بينهما بالضغط على زري 1 و 2 مع تحريك سلس (Animation).
//   3. مخزون اللاعب يحوي 5 بطاقات رباعية الأضلاع بصور المشاهير المحبوبين:
//      (Miranda Kerr, Dexter Morgan, Jason Statham, Jack Sparrow, Mahlagha Jaberi)
//      مع إكساء خلفي فخم وموحد لجميع البطاقات، وتبقى البطاقات غير المسحوبة مسطحة على الطاولة دائماً.
//   4. الضغط على زر G ينقل بطاقة عشوائية من المخزون إلى اللعب مع تدويرها 180 درجة لتواجه اللاعب.
//      - الضغط للمرة السادسة لا يفعل شيئاً.
//      - يعمل بنفس الطريقة في كلا المنظورين.
//   5. غرفة واسعة بجدران ورق جدران فاخر وأرضية خشبية مصقولة وتتسع لقطع ديكور سمة المشاهير:
//      - قطعة ديكور 1: تمثال الأوسكار الذهبي ثلاثي الأبعاد 3D Model عبر Assimp على قاعدة فاخرة.
//      - قطعة ديكور 2: خزانة المشاهير للأسطوانات البلاتينية والجوائز (Celebrity VIP Showcase).
//      - قطعة ديكور 3: نجمة ممر الشهرة وأشعة الأضواء الذهبية (Walk of Fame Star Crest).
//      - قطعة ديكور إضافية مميزة: السجادة الحمراء وأعمدة الحبال المخملية (VIP Red Carpet & Stanchions).
//   6. ملصق فني ممزق وناقص من زاوية هندسياً لسمة المشاهير على الجدار الخلفي وتظهر خلفه خلفية الجدار.
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

std::string lastActionMessage = "مرحباً بك في صالة المشاهير! اضغط [G] لسحب بطاقة وقلبها، و [1] أو [2] لتبديل الكاميرا.";

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
        lastActionMessage = "تم لعب جميع بطاقات المشاهير الخمس! (الضغط للمرة السادسة لا يفعل شيئاً).";
        std::cout << "[Game] " << lastActionMessage << std::endl;
        return;
    }

    // اختيار بطاقة عشوائية من المتبقي في المخزون
    int randIdx = rand() % deckIndices.size();
    int selectedCard = deckIndices[randIdx];

    // تحديد الفتحة التالية في منطقة اللعب (0 إلى 4)
    int targetSlot = 5 - static_cast<int>(deckIndices.size());

    // إطلاق حركة انتقال وتدوير البطاقة ثلاثية الأبعاد
    cards[selectedCard].playToSlot(targetSlot, gameTable.playedSlots[targetSlot]);

    lastActionMessage = "تم سحب بطاقة: " + cards[selectedCard].characterName + " وقلبها لتواجه اللاعب في الفتحة " + std::to_string(targetSlot + 1);
    std::cout << "[Game] " << lastActionMessage << std::endl;
}

// إعادة تشغيل الجولة وجمع البطاقات إلى المخزون (مقلوبة ومستوية على الطاولة)
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
    lastActionMessage = "تمت إعادة ضبط اللعبة وعودة بطاقات المشاهير إلى المخزون (مسطحة ومقلوبة على الطاولة).";
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
            lastActionMessage = "الانتقال إلى الحالة 1: منظور علوي فوق الطاولة (تستوي البطاقات الملعوبة أفقياً).";
            key1Pressed = true;
        }
    } else {
        key1Pressed = false;
    }

    // زر 2: الانتقال إلى منظور اللاعب (المتطلب 2)
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (!key2Pressed) {
            cameraManager.switchToState(2);
            lastActionMessage = "الانتقال إلى الحالة 2: منظور اللاعب (تقف البطاقات الملعوبة طولياً لتواجه اللاعب).";
            key2Pressed = true;
        }
    } else {
        key2Pressed = false;
    }

    // زر G: لعب بطاقة عشوائية مع تدويرها وتحليقها (المتطلب 4)
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        if (!keyGPressed) {
            playRandomCard();
            keyGPressed = true;
        }
    } else {
        keyGPressed = false;
    }

    // زر R: إعادة تعيين اللعبة
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
    // 1. ضبط مسار التشغيل الحالي ليطابق مسار الملف التنفيذي بأمان
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

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Celebrity VIP Lounge - Computer Graphics 2026", NULL, NULL);
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

    // 7. تحميل الإكساءات (Celebrity VIP Lounge Textures)
    std::cout << "[Init] Loading textures..." << std::endl;
    unsigned int texTable       = load_texture(resolvePath("assets/textures/table_wood.png").c_str());
    unsigned int texFloor       = load_texture(resolvePath("assets/textures/floor_wood.png").c_str());
    unsigned int texWallpaper   = load_texture(resolvePath("assets/textures/wallpaper.png").c_str());

    // إكساء الملصق الفني الممزق في الجدار الخلفي
    std::string posterPath = resolvePath("assets/textures/poster_celebrity.png");
    if (!std::filesystem::exists(posterPath)) posterPath = resolvePath("assets/textures/poster_starry_night.png");
    unsigned int texPoster      = load_texture(posterPath.c_str());

    // إكساء خزانة المشاهير للأسطوانات البلاتينية
    std::string showcasePath = resolvePath("assets/textures/showcase_celebrity.png");
    if (!std::filesystem::exists(showcasePath)) showcasePath = resolvePath("assets/textures/bookshelf.png");
    unsigned int texBookshelf   = load_texture(showcasePath.c_str());

    // إكساء نجمة ممر الشهرة للمشاهير
    std::string starPath = resolvePath("assets/textures/celebrity_star.png");
    if (!std::filesystem::exists(starPath)) starPath = resolvePath("assets/textures/shield.png");
    unsigned int texShield      = load_texture(starPath.c_str());

    // الإكساء الخلفي الفخم الموحد لجميع البطاقات
    unsigned int texCardBack    = load_texture(resolvePath("assets/textures/card_back.png").c_str());

    // إكساءات بطاقات المشاهير الخمس
    std::string mirandaPath = resolvePath("assets/textures/Miranda2.jpg");
    if (!std::filesystem::exists(mirandaPath)) mirandaPath = resolvePath("assets/textures/Miranda.jpg");
    unsigned int texMiranda     = load_texture(mirandaPath.c_str());
    unsigned int texDexter      = load_texture(resolvePath("assets/textures/Dexter.jpg").c_str());
    unsigned int texStatham     = load_texture(resolvePath("assets/textures/Jason Statham.jpg").c_str());
    unsigned int texJackSparrow = load_texture(resolvePath("assets/textures/Jack Sparrow.png").c_str());
    unsigned int texMahlagha    = load_texture(resolvePath("assets/textures/Mahlagha Jaberi.jpg").c_str());

    // 8. تهيئة مجسمات الطاولة والغرفة والديكور
    gameTable.init(texTable);

    std::string modelFile = resolvePath("assets/models/oscar_trophy.obj");
    if (!std::filesystem::exists(modelFile)) modelFile = resolvePath("assets/models/chalice.obj");
    gameRoom.init(texFloor, texWallpaper, texPoster, texBookshelf, texShield, modelFile);

    // مجسمات البطاقات (وجه أمامي وخلفي)
    cardFrontMesh = MeshBuilder::createCardFront(0.72f, 1.08f);
    cardBackMesh  = MeshBuilder::createCardBack(0.72f, 1.08f);

    // 9. إنشاء بطاقات المشاهير الخمس في منطقة المخزون (Deck)
    struct CharacterCardInfo {
        std::string name;
        unsigned int textureId;
    };
    std::vector<CharacterCardInfo> cardData = {
        {"Miranda Kerr (Supermodel)", texMiranda},
        {"Dexter Morgan (Michael C. Hall)", texDexter},
        {"Jason Statham (Action Star)", texStatham},
        {"Captain Jack Sparrow (Johnny Depp)", texJackSparrow},
        {"Mahlagha Jaberi (Fashion Icon)", texMahlagha}
    };

    cards.clear();
    for (size_t i = 0; i < cardData.size(); ++i) {
        glm::vec3 dPos = gameTable.deckAreaCenter;
        dPos.y += static_cast<float>(i) * 0.005f; // تفادي تداخل الأوجه
        dPos.z -= static_cast<float>(i) * 0.015f; // تنظيم لطيف في كومة المخزون
        cards.emplace_back(cardData[i].name, cardData[i].textureId, texCardBack, dPos);
    }

    std::cout << "[Init] Celebrity VIP Lounge setup complete! Starting render loop..." << std::endl;

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
        glClearColor(0.05f, 0.06f, 0.09f, 1.0f);
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

        // إضاءة صالة المشاهير (ثريا هوليوود الدافئة فوق الطاولة)
        sceneShader.setVec3("lightPos", glm::vec3(0.0f, 6.2f, 0.2f));
        sceneShader.setVec3("lightColor", glm::vec3(1.0f, 0.94f, 0.85f));
        sceneShader.setFloat("ambientStrength", 0.65f);

        // 1. رسم صالة المشاهير + ورق الجدران الفاخر + الملصق الممزق + قطع الديكور الأربعة
        gameRoom.render(sceneShader);

        // 2. رسم طاولة المشاهير الفاخرة بأقسامها الثلاثة (المخزون، اللعب، الخصم)
        gameTable.render(sceneShader);

        // 3. رسم بطاقات المشاهير الخمس (المخزون يبقى مسطحاً ومقلوباً، والمسحوبة تدور 180 درجة وتواجه اللاعب)
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
        ImGui::SetNextWindowSize(ImVec2(440, 320), ImGuiCond_FirstUseEver);
        ImGui::Begin("Celebrity VIP Lounge & Inspector", nullptr, ImGuiWindowFlags_NoCollapse);

        ImGui::TextColored(ImVec4(0.95f, 0.82f, 0.25f, 1.0f), "Computer Graphics 2026 - Celebrity VIP Edition");
        ImGui::Separator();

        // عرض وضع الكاميرا الحالي
        if (cameraManager.targetState == 1) {
            ImGui::Text("Camera State: [1] Top-Down View (Flat Cards on Table)");
        } else {
            ImGui::Text("Camera State: [2] Player Perspective (Standing Cards Face Player)");
        }

        // أزرار تبديل الكاميرا
        if (ImGui::Button("State 1: Top-Down (Key 1)")) {
            cameraManager.switchToState(1);
            lastActionMessage = "تم اختيار المنظور العلوي [1] (تسطيح البطاقات).";
        }
        ImGui::SameLine();
        if (ImGui::Button("State 2: Player View (Key 2)")) {
            cameraManager.switchToState(2);
            lastActionMessage = "تم اختيار منظور اللاعب [2] (وقوف البطاقات لتواجه اللاعب).";
        }

        ImGui::Separator();

        // حساب عدد البطاقات المتبقية والمحركة
        int inDeckCount = 0;
        int playedCount = 0;
        for (const auto& c : cards) {
            if (c.state == CardState::IN_DECK) inDeckCount++;
            else playedCount++;
        }

        ImGui::Text("Undrawn Cards in Deck: %d / 5 (Always flat face-down)", inDeckCount);
        ImGui::Text("Played Celebrity Cards: %d / 5", playedCount);

        // زر لعب بطاقة عشوائية
        if (ImGui::Button("Play Random Card (Key G)", ImVec2(230, 28))) {
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
        ImGui::BulletText("[1]: Top-Down camera (cards lie flat on table)");
        ImGui::BulletText("[2]: Player perspective (drawn cards stand facing player)");
        ImGui::BulletText("[G]: Play card (lifts, flips 180 deg to face player)");
        ImGui::BulletText("[R]: Reset all cards back to deck (flat face-down)");
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
    glDeleteTextures(1, &texMiranda);
    glDeleteTextures(1, &texDexter);
    glDeleteTextures(1, &texStatham);
    glDeleteTextures(1, &texJackSparrow);
    glDeleteTextures(1, &texMahlagha);

    shutdown_ImGui();
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "[Shutdown] Program exited normally." << std::endl;
    return 0;
}
