#include <cmath>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "movement.hpp"

#ifndef CAMERA_HPP
#define CAMERA_HPP

class Camera
{
    glm::vec3 defUp_;
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    float yaw_;
    float pitch_;
    float movementSensitivity_;
    float mouseSensitivity_;
    float scollSensitivity_;
    float fovy_;
    float aspect_;

public:
    // +X为右 +Y为上 +Z为前
    Camera(float aspect,
           glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f),
           glm::vec3 defUp = glm::vec3(0.0f, 1.0f, 0.0f))
        : position_(position),
          defUp_(defUp),
          yaw_(-90.0f),
          pitch_(0.0f),
          front_(glm::vec3(0.0f, 0.0f, -1.0f)),
          up_(glm::vec3(0.0f, 1.0f, 0.0f)),
          right_(glm::vec3(1.0f, 0.0f, 0.0f)),
          movementSensitivity_(3.0f),
          mouseSensitivity_(0.15f),
          scollSensitivity_(1.0f),
          fovy_(60.0f),
          aspect_(aspect)
    {
    }
    ~Camera() {}
    glm::mat4 updateView() const
    {
        return glm::lookAt(position_, position_ + front_, up_);
    }
    glm::mat4 updateProjection() const
    {
        return glm::perspective(glm::radians(fovy_), aspect_, 0.1f, 100.0f);
    }
    void processKeyboard(Movement direction, float deltaTime)
    {
        float rate = movementSensitivity_ * deltaTime;
        switch (direction)
        {
        case Movement::FORWARD:
            position_ += front_ * rate;
            break;
        case Movement::BACKWARD:
            position_ -= front_ * rate;
            break;
        case Movement::LEFT:
            position_ -= right_ * rate;
            break;
        case Movement::RIGHT:
            position_ += right_ * rate;
            break;
        default:
            break;
        }
    }
    void processMouseMovement(float xoffset, float yoffset)
    {
        yaw_ += xoffset * mouseSensitivity_;
        pitch_ += yoffset * mouseSensitivity_;
        pitch_ = glm::clamp(pitch_, -89.0f, 89.0f);
        glm::vec3 front;
        front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front.y = sin(glm::radians(pitch_));
        front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front_ = glm::normalize(front);
        right_ = glm::normalize(glm::cross(front_, defUp_));
        up_ = glm::normalize(glm::cross(right_, front_));
    }
    void processMouseScroll(float yoffset)
    {
        fovy_ -= yoffset * scollSensitivity_;
        fovy_ = glm::clamp(fovy_, 1.0f, 179.0f);
    }
};

#endif
