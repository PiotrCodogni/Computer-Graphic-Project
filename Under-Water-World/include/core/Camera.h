#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    Camera();

    void followTarget(const glm::vec3& targetPosition);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    glm::vec3 getPosition() const;

private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

    glm::vec3 offset;

    float fov;
    float zNear;
    float zFar;
};