#include <GL/glew.h>

#include "core/Application.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


#include <iostream>

Application::Application()
{
}

bool Application::init()
{
    if (!window.init(1824, 1224, "Under Water World"))
    {
        return false;
    }

    // inicjalizacja GLEW
    glewExperimental = GL_TRUE;
    GLenum glewStatus = glewInit();

    if (glewStatus != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        window.shutdown();
        return false;
    }

    glViewport(0, 0, window.getWidth(), window.getHeight());
    glEnable(GL_DEPTH_TEST);

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    // inicjalizacja ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    if (!renderer.init(window.getWidth(), window.getHeight()))
    {
        return false;
    }

    if (!scene.init())
    {
        return false;
    }

    return true;
}

void Application::run()
{
    while (!window.shouldClose())
    {
        time.update();
        float deltaTime = time.getDeltaTime();

        window.pollEvents();
        input.update(window);

        processInput();
        update(deltaTime);

        render();

        // nowa klatka ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        renderUi();

        //render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.swapBuffers();
     
    }
}

void Application::processInput()
{
    if (input.isKeyPressed(GLFW_KEY_ESCAPE))
    {
        window.close();
    }
}

void Application::update(float deltaTime)
{
    scene.update(deltaTime, input);
}


void Application::render()
{
    int width;
    int height;

    glfwGetFramebufferSize(window.getWindow(), &width, &height);

    glViewport(0, 0, width, height);

    renderer.setViewportSize(width, height);

    renderer.beginFrame();
    renderer.render(scene);
    renderer.endFrame();
}

void Application::renderUi()
{
    ImGui::Begin("Scene Controls");

    glm::vec3 fishPosition = scene.getFish().getPosition();

    ImGui::Text(
        "Fish position: %.2f %.2f %.2f",
        fishPosition.x,
        fishPosition.y,
        fishPosition.z
    );

    bool& showSkybox = scene.getShowSkyboxRef();
    ImGui::Checkbox("Show skybox", &showSkybox);

    bool& showShadows = scene.getShowShadowsRef();
    ImGui::Checkbox("Enable shadow mapping", &showShadows);

    ImGui::Text("W-A-S-D - ruch");
    ImGui::Text("Space / Left Shift - gora / dol");
    ImGui::Text("Strzalki lewo/prawo - obrot kamery");
    ImGui::Text("Strzalki gora/dol - kamera gora/dol");
    ImGui::Text("Q - widok do przodu (reset kamery)");
    ImGui::Text("ESC - zamknij");
    
    ImGui::Separator();
    bool isPanic = scene.getFishSchool().isPanicking();
    ImGui::Text("F - tryb paniki lawicy: %s", isPanic ? "WLACZONY" : "Wylaczony");

    ImGui::Separator();
    ImGui::Text("Crystal interaction");
    ImGui::Text("Status: %s", scene.isCrystalActivated() ? "aktywny" : "nieaktywny");
    if (scene.canInteractWithCrystal())
    {
        ImGui::Text("E - aktywuj / zatrzymaj obrot krysztalu");
    }
    else
    {
        ImGui::Text("Podplyn blizej krysztalu, zeby uzyc E");
    }

    ImGui::Separator();
    ImGui::Text("Pearl A13 Controls");

    Pearl& pearl = scene.getPearl();
    glm::vec3& pearlBaseColor = pearl.getBaseColor();
    float& pearlF0 = pearl.getF0();
    float& pearlFresnelPower = pearl.getFresnelPower();
    float& pearlRefractionRatio = pearl.getRefractionRatio();
    float& pearlReflectionStrength = pearl.getReflectionStrength();
    float& pearlRefractionStrength = pearl.getRefractionStrength();

    ImGui::ColorEdit3("Base color", &pearlBaseColor.x);
    ImGui::SliderFloat("F0", &pearlF0, 0.0f, 1.0f);
    ImGui::SliderFloat("Fresnel power", &pearlFresnelPower, 0.1f, 10.0f);
    ImGui::SliderFloat("Refraction ratio", &pearlRefractionRatio, 0.1f, 1.5f);
    ImGui::SliderFloat("Reflection strength", &pearlReflectionStrength, 0.0f, 2.0f);
    ImGui::SliderFloat("Refraction strength", &pearlRefractionStrength, 0.0f, 2.0f);

    if (ImGui::Button("Reset pearl parameters"))
    {
        pearl.resetParameters();
    }


    ImGui::End();
}

void Application::shutdown()
{
    scene.shutdown();
    renderer.shutdown();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    window.shutdown();
}
