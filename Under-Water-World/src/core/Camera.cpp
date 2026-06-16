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

    yaw           = 0.0f;
    orbitDistance = 4.0f;
    orbitHeight   = 2.0f;
    rotateSpeed   = 2.0f;

    fov   = 60.0f;
    zNear = 0.1f;
    zFar  = 5000.0f;

    position = target + glm::vec3(0.0f, orbitHeight, orbitDistance);
}

void Camera::update(float deltaTime, const Input& input)
{
    // Strzalka w lewo - obrot kamery w prawo
    if (input.isKeyPressed(GLFW_KEY_LEFT))
        yaw += rotateSpeed * deltaTime;

    // Strzalka w prawo - obrot kamery w lewo
    if (input.isKeyPressed(GLFW_KEY_RIGHT))
        yaw -= rotateSpeed * deltaTime;

    // Strzalka w dol - kamera do gory
    if (input.isKeyPressed(GLFW_KEY_DOWN))
        orbitHeight += 3.0f * deltaTime;

    // Strzalka w gore - kamera w dol
    if (input.isKeyPressed(GLFW_KEY_UP))
        orbitHeight -= 3.0f * deltaTime;

    // Zabezpieczenie przed zbytnim oddaleniem kamery
    if (orbitHeight < -10.0f) orbitHeight = -10.0f;
    if (orbitHeight > 15.0f) orbitHeight = 15.0f;

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

    // Zabezpieczenie absolutnej pozycji Y, zeby nie wchodzic pod seabed
    if (position.y < -5.0f) {
        position.y = -5.0f;
    }
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