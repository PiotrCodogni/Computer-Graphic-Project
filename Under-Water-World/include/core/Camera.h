#pragma once

#include <glm/glm.hpp>

class Input;

class Camera
{
public:
    Camera();

    void followTarget(const glm::vec3& targetPosition);
    void update(float deltaTime, const Input& input);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    glm::vec3 getPosition() const;

private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

    float yaw;           // kat orbity poziomej [radiany]
    float orbitDistance; // odleglosc od gracza
    float orbitHeight;   // wysokosc kamery nad graczem
    float rotateSpeed;   // predkosc obrotu [rad/s]

    float fov;
    float zNear;
    float zFar;
};