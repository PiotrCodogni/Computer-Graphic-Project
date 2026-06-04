#include "core/Camera.h"
#include "core/Input.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

Camera::Camera()
{
    target   = glm::vec3(0.0f);
    up       = glm::vec3(0.0f, 1.0f, 0.0f);

    yaw           = glm::pi<float>();
    orbitDistance = 4.0f;
    orbitHeight   = 2.0f;
    rotateSpeed   = 2.0f;

    fov   = 60.0f;
    zNear = 0.1f;
    zFar  = 500.0f;

    position = target + glm::vec3(0.0f, orbitHeight, orbitDistance);
}

void Camera::update(float deltaTime, const Input& input)
{
    // R - obrot kamery w lewo
    if (input.isKeyPressed(GLFW_KEY_R))
        yaw -= rotateSpeed * deltaTime;

    // E - obrot kamery w prawo
    if (input.isKeyPressed(GLFW_KEY_E))
        yaw += rotateSpeed * deltaTime;

    // Q - widok za siebie (reset do pozycji startowej za graczem)
    if (input.isKeyPressed(GLFW_KEY_Q))
        yaw = 0.0f;
}

void Camera::followTarget(const glm::vec3& targetPosition)
{
    target = targetPosition;

    // Oblicz pozycje kamery na orbicie wokol gracza
    float camX = orbitDistance * std::sin(yaw);
    float camZ = orbitDistance * std::cos(yaw);

    position = target + glm::vec3(camX, orbitHeight, camZ);
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const
{
    return glm::perspective(
        glm::radians(fov),
        aspectRatio,
        zNear,
        zFar
    );
}

glm::vec3 Camera::getPosition() const
{
    return position;
}