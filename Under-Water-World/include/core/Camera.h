#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

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
    glm::vec3 getForwardDirection() const;

private:
    glm::vec3 position;
    glm::vec3 target;

    // Orientacja kamery przechowywana jako kwaternion
    // Eliminuje gimbal lock i zapewnia plynna interpolacje obrotow
    glm::quat orientation;
    float currentPitch;

    float orbitDistance; // odleglosc od gracza
    float yawSpeed;     // predkosc obrotu poziomego 
    float pitchSpeed;   // predkosc obrotu pionowego 

    float fov;
    float zNear;
    float zFar;

    
    static constexpr float MIN_PITCH = -0.85f;  
    static constexpr float MAX_PITCH =  0.85f;  

    
    float getPitchFromOrientation() const;
};