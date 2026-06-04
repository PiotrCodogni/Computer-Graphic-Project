#include "scene/Scene.h"

Scene::Scene()
{
}

bool Scene::init()
{
    if (!fish.init(
        "assets/models/fish1.glb",
        "assets/texture/fish1.png",
        "shaders/Fish.vert",
        "shaders/Fish.frag"
    ))
    {
        return false;
    }

    return true;
}

void Scene::update(float deltaTime, const Input& input)
{
    fish.update(deltaTime, input);
    camera.followTarget(fish.getPosition());
}

void Scene::shutdown()
{
    fish.shutdown();
}

Fish& Scene::getFish()
{
    return fish;
}

Camera& Scene::getCamera()
{
    return camera;
}