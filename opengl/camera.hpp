#include <cmath>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef CAMERA_HPP
#define CAMERA_HPP

enum class CamMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
{
    glm::vec3 defUp_;
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    float yaw_;
    float pitch_;
    float movementSpeed_;
    float mouseSensitivity_;
    float zoom_;

public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 defUp = glm::vec3(0.0f, 1.0f, 0.0f))
        : position_(position),
          defUp_(defUp),
          yaw_(-90.0f),
          pitch_(0.0f),
          front_(glm::vec3(0.0f, 0.0f, -1.0f)),
          movementSpeed_(2.5f),
          mouseSensitivity_(0.1f),
          zoom_(45.0f)
    {
        updateCameraVectors();
    }
    ~Camera() {}
    glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(position_, position_ + front_, up_);
    }
    void processKeyboard(CamMovement direction, float deltaTime)
    {
        float rate = movementSpeed_ * deltaTime;
        if (direction == CamMovement::FORWARD)
            position_ += front_ * rate;
        if (direction == CamMovement::BACKWARD)
            position_ -= front_ * rate;
        if (direction == CamMovement::LEFT)
            position_ -= right_ * rate;
        if (direction == CamMovement::RIGHT)
            position_ += right_ * rate;
    }
    void processMouseMovement(float xoffset, float yoffset)
    {
        yaw_ += xoffset * mouseSensitivity_;
        pitch_ += yoffset * mouseSensitivity_;
        pitch_ = glm::clamp(pitch_, -89.0f, 89.0f);
        updateCameraVectors();
    }
    void processMouseScroll(float yoffset)
    {
        zoom_ -= yoffset;
        zoom_ = glm::clamp(zoom_, -10.0f, 100.0f);
    }
    float getZoom() const { return zoom_; }

private:
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front.y = sin(glm::radians(pitch_));
        front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front_ = glm::normalize(front);
        right_ = glm::normalize(glm::cross(front_, defUp_)); // 标准速度，越接近0越慢
        up_ = glm::normalize(glm::cross(right_, front_));
    }
};

#endif
