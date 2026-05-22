#pragma once

#include "core/Input.h"
#include "core/Camera.h"
#include "scene/entity/Fish1.h"

class Scene
{
public:
    Scene();

    bool init();
    void update(float deltaTime, const Input& input);
    void shutdown();

    Fish& getFish();
    Camera& getCamera();

private:
    Fish fish;
    Camera camera;
};