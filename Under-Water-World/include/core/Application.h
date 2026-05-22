#pragma once

#include "core/Window.h" 
#include "core/Time.h"
#include "core/Input.h"
#include "scene/Fish.h"
#include "scene/Scene.h"
#include "graphics/Renderer.h"


class Application
{
public:
    Application();

    bool init();
    void run();
    void shutdown();

private:
    void processInput();
    void update(float deltaTime);
    void render();
    void renderUi();

private:
    Window window;
    Time time;
    Input input;

    Scene scene;
    Renderer renderer;
};