#include "core/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
    target = glm::vec3(0.0f, 0.0f, 0.0f);

    offset = glm::vec3(1.0f, 1.0f, 2.0f);

    position = target + offset;

    up = glm::vec3(0.0f, 1.0f, 0.0f);

    fov = 60.0f;
    zNear = 0.1f;
    zFar = 100.0f;
}

void Camera::followTarget(const glm::vec3& targetPosition)
{
    target = targetPosition;
    position = target + offset;
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(
        position,
        target,
        up
    );
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