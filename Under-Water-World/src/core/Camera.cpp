#include "core/Camera.h"
#include "core/Input.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cmath>

Camera::Camera()
{
    target   = glm::vec3(0.0f);

    // Kwaternion tożsamościowy = brak obrotu (kamera patrzy wzdłuż -Z)
    orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    orbitDistance = 4.0f;
    yawSpeed     = 2.0f;
    pitchSpeed   = 1.5f;

    fov   = 60.0f;
    zNear = 0.1f;
    zFar  = 5000.0f;

    // Ustawiamy poczatkowa orientacje kamery:
    // lekko pochylona w dol (~25 stopni), patrzac od tylu na rybke
    glm::quat pitchQuat = glm::angleAxis(0.44f, glm::vec3(1.0f, 0.0f, 0.0f));
    orientation = pitchQuat; 

    position = target + glm::vec3(0.0f, 2.0f, orbitDistance);
}

void Camera::update(float deltaTime, const Input& input)
{
    
    float yawAngle = 0.0f;

    // Strzalka w lewo - obrot kamery w prawo (wokol rybki)
    if (input.isKeyPressed(GLFW_KEY_LEFT))
        yawAngle += yawSpeed * deltaTime;

    // Strzalka w prawo - obrot kamery w lewo
    if (input.isKeyPressed(GLFW_KEY_RIGHT))
        yawAngle -= yawSpeed * deltaTime;

    if (yawAngle != 0.0f)
    {
        
        glm::quat yawQuat = glm::angleAxis(yawAngle, glm::vec3(0.0f, 1.0f, 0.0f));
       
        orientation = yawQuat * orientation;
    }

    
    float pitchAngle = 0.0f;

    // Strzalka w gore
    if (input.isKeyPressed(GLFW_KEY_UP))
        pitchAngle += pitchSpeed * deltaTime;

    // Strzalka w dol 
    if (input.isKeyPressed(GLFW_KEY_DOWN))
        pitchAngle -= pitchSpeed * deltaTime;

    if (pitchAngle != 0.0f)
    {
       
        float currentPitch = getPitchFromOrientation();
        float newPitch = currentPitch + pitchAngle;
        newPitch = glm::clamp(newPitch, MIN_PITCH, MAX_PITCH);
        pitchAngle = newPitch - currentPitch;

        if (std::abs(pitchAngle) > 0.0001f)
        {
            
            glm::vec3 localRight = orientation * glm::vec3(1.0f, 0.0f, 0.0f);
            glm::quat pitchQuat = glm::angleAxis(pitchAngle, localRight);
            orientation = pitchQuat * orientation;
        }
    }

    // Normalizacja kwaternionu - zapobiega narastaniu bledow numerycznych
    orientation = glm::normalize(orientation);

    // Q - reset orientacji do pozycji startowej za graczem
    if (input.isKeyPressed(GLFW_KEY_Q))
    {
        glm::quat resetOrientation = glm::angleAxis(0.44f, glm::vec3(1.0f, 0.0f, 0.0f));
       
        float t = glm::clamp(3.0f * deltaTime, 0.0f, 1.0f);
        orientation = glm::slerp(orientation, resetOrientation, t);
    }
}

void Camera::followTarget(const glm::vec3& targetPosition)
{
    target = targetPosition;

    // Oblicz pozycje kamery na orbicie wokol gracza
    
    glm::vec3 offset = orientation * glm::vec3(0.0f, 0.0f, orbitDistance);

    position = target + offset;

    // Zabezpieczenie absolutnej pozycji Y, zeby nie wchodzic pod seabed
    if (position.y < -5.0f) {
        position.y = -5.0f;
    }
}

glm::mat4 Camera::getViewMatrix() const
{
    
    glm::mat4 rotationMatrix = glm::mat4_cast(glm::conjugate(orientation));
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -position);
    return rotationMatrix * translationMatrix;
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

glm::vec3 Camera::getForwardDirection() const
{
    
    glm::vec3 forward = orientation * glm::vec3(0.0f, 0.0f, -1.0f);

    forward.y = 0.0f;
    if (glm::length(forward) > 0.001f)
        forward = glm::normalize(forward);
    else
        forward = glm::vec3(0.0f, 0.0f, -1.0f);
    return forward;
}

float Camera::getPitchFromOrientation() const
{
   
    glm::vec3 euler = glm::eulerAngles(orientation);
    return euler.x;
}