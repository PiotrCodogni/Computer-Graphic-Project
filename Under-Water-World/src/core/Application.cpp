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
    if (!window.init(1000, 1000, "Under Water World"))
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

    if (!renderer.init())
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

        // render ImGui
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

    ImGui::Text("W-A-S-D - move");
    ImGui::Text("Space-Left_Shift move");
    ImGui::Text("ESC - close app");


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