#include <cmath>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "movement.hpp"

#ifndef CAMERA_HPP
#define CAMERA_HPP

class Camera
{
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
    // +X为右 +Y为上 -Z为前
    Camera(float aspect, glm::vec3 position)
        : position_(position),
          front_(glm::vec3(0.0f, 0.0f, -1.0f)),
          up_(glm::vec3(0.0f, 1.0f, 0.0f)),
          right_(glm::vec3(1.0f, 0.0f, 0.0f)),
          yaw_(-90.0f),
          pitch_(0.0f),
          movementSensitivity_(3.0f),
          mouseSensitivity_(0.15f),
          scollSensitivity_(1.0f),
          fovy_(60.0f),
          aspect_(aspect) {}
    ~Camera() {}
    Camera(const Camera &) = delete;
    Camera &operator=(const Camera &) = delete;
    Camera(Camera &&other)
        : position_(other.position_),
          front_(other.front_),
          up_(other.up_),
          right_(other.right_),
          yaw_(other.yaw_),
          pitch_(other.pitch_),
          movementSensitivity_(other.movementSensitivity_),
          mouseSensitivity_(other.mouseSensitivity_),
          scollSensitivity_(other.scollSensitivity_),
          fovy_(other.fovy_),
          aspect_(other.aspect_)
    {
        other.position_ = glm::vec3(0.0f);
        other.front_ = glm::vec3(0.0f);
        other.up_ = glm::vec3(0.0f);
        other.right_ = glm::vec3(0.0f);
        other.yaw_ = 0.0f;
        other.pitch_ = 0.0f;
    }
    Camera &operator=(Camera &&other)
    {
        if (this != &other)
            Camera(std::move(other)).swap(*this);
        return *this;
    }
    glm::mat4 updateView() const
    {
        return glm::lookAt(position_, position_ + front_, up_);
    }
    glm::mat4 updateProjection() const
    {
        return glm::perspective(glm::radians(fovy_), aspect_, 0.1f, 100.0f);
    }
    void processKeyboard_GodMode(Movement direction, float deltaTime)
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
    void processKeyboard(Movement direction, float deltaTime)
    {
        float rate = movementSensitivity_ * deltaTime;
        glm::vec3 final(0.0f, 0.0f, 0.0f);
        switch (direction)
        {
        case Movement::FORWARD:
            final.x = front_.x;
            final.z = front_.z;
            position_ += final * rate;
            break;
        case Movement::BACKWARD:
            final.x = front_.x;
            final.z = front_.z;
            position_ -= final * rate;
            break;
        case Movement::LEFT:
            final.x = right_.x;
            final.z = right_.z;
            position_ -= final * rate;
            break;
        case Movement::RIGHT:
            final.x = right_.x;
            final.z = right_.z;
            position_ += final * rate;
            break;
        default:
            break;
        }
    }
    void processMouseMovement(float xoffset, float yoffset)
    {
        yaw_ += xoffset * mouseSensitivity_;
        pitch_ += yoffset * mouseSensitivity_;
        pitch_ = glm::clamp(pitch_, -89.9f, 89.9f);
        glm::vec3 front;
        front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front.y = sin(glm::radians(pitch_));
        front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front_ = glm::normalize(front);
        right_ = glm::normalize(glm::cross(front_, glm::vec3(0.0f, 1.0f, 0.0f))); // def +Y up
        up_ = glm::normalize(glm::cross(right_, front_));
    }
    void processMouseScroll(float yoffset)
    {
        fovy_ -= yoffset * scollSensitivity_;
        fovy_ = glm::clamp(fovy_, 1.0f, 179.0f);
    }

private:
    void swap(Camera &other)
    {
        std::swap(position_, other.position_);
        std::swap(front_, other.front_);
        std::swap(up_, other.up_);
        std::swap(right_, other.right_);
        std::swap(yaw_, other.yaw_);
        std::swap(pitch_, other.pitch_);
        std::swap(movementSensitivity_, other.movementSensitivity_);
        std::swap(mouseSensitivity_, other.mouseSensitivity_);
        std::swap(scollSensitivity_, other.scollSensitivity_);
        std::swap(fovy_, other.fovy_);
        std::swap(aspect_, other.aspect_);
    }
};

#endif
