#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "movement.hpp"

#ifndef CAMERA_HPP
#define CAMERA_HPP

#define CAMERA_VIEW_ASPECT (16.0f / 9.0f)
#define CAMERA_FIELD_OF_VIEW_VERTICAL 60.0f
#define CAMERA_POS_MOVE_SENSITIVITY 5.0f
#define CAMERA_VIEW_MOVE_SENSITIVITY 0.15f
#define CAMERA_VIEW_ZOOM_SENSITIVITY 3.0f

class Camera
{
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    float yaw_;
    float pitch_;
    float cursorX_;
    float cursorY_;
    float moveSensitivity_;
    float viewSensitivity_;
    float zoomSensitivity_;
    float fovy_;
    float aspect_;

public:
    Camera(float x, float y, float height)
        : position_(x, height, y),
          front_(0.0f, 0.0f, -1.0f),
          up_(0.0f, 1.0f, 0.0f),
          right_(1.0f, 0.0f, 0.0f),
          yaw_(-90.0f),
          pitch_(0.0f),
          cursorX_(0.0f),
          cursorY_(0.0f),
          moveSensitivity_(CAMERA_POS_MOVE_SENSITIVITY),
          viewSensitivity_(CAMERA_VIEW_MOVE_SENSITIVITY),
          zoomSensitivity_(CAMERA_VIEW_ZOOM_SENSITIVITY),
          fovy_(CAMERA_FIELD_OF_VIEW_VERTICAL),
          aspect_(CAMERA_VIEW_ASPECT) {}
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
          cursorX_(other.cursorX_),
          cursorY_(other.cursorY_),
          moveSensitivity_(other.moveSensitivity_),
          viewSensitivity_(other.viewSensitivity_),
          zoomSensitivity_(other.zoomSensitivity_),
          fovy_(other.fovy_),
          aspect_(other.aspect_)
    {
        other.position_ = glm::vec3(0.0f);
        other.front_ = glm::vec3(0.0f);
        other.up_ = glm::vec3(0.0f);
        other.right_ = glm::vec3(0.0f);
        other.yaw_ = 0.0f;
        other.pitch_ = 0.0f;
        other.cursorX_ = 0.0f;
        other.cursorY_ = 0.0f;
        other.moveSensitivity_ = 0.0f;
        other.viewSensitivity_ = 0.0f;
        other.zoomSensitivity_ = 0.0f;
        other.fovy_ = 0.0f;
        other.aspect_ = 0.0f;
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
    void processPosMove_levitate(Movement direction, float deltaTime)
    {
        float rate = moveSensitivity_ * deltaTime;
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
        case Movement::UP:
            position_ += up_ * rate;
            break;
        case Movement::DOWN:
            position_ -= up_ * rate;
            break;
        default:
            break;
        }
    }
    void processPosMove(Movement direction, float deltaTime)
    {
        float rate = moveSensitivity_ * deltaTime;
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
        case Movement::UP:
            final.y = 1.0f;
            position_ += final * rate;
            break;
        case Movement::DOWN:
            final.y = 1.0f;
            position_ -= final * rate;
            break;
        default:
            break;
        }
    }
    void processViewMove(float xpos, float ypos)
    {
        float xoffset = xpos - cursorX_;
        float yoffset = cursorY_ - ypos;
        cursorX_ = xpos;
        cursorY_ = ypos;
        yaw_ += xoffset * viewSensitivity_;
        pitch_ += yoffset * viewSensitivity_;
        pitch_ = glm::clamp(pitch_, -89.9f, 89.9f);
        glm::vec3 front;
        front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front.y = sin(glm::radians(pitch_));
        front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front_ = glm::normalize(front);
        right_ = glm::normalize(glm::cross(front_, glm::vec3(0.0f, 1.0f, 0.0f))); // def +Y up
        up_ = glm::normalize(glm::cross(right_, front_));
    }
    void processViewZoom(float yoffset)
    {
        fovy_ -= yoffset * zoomSensitivity_;
        fovy_ = glm::clamp(fovy_, 10.0f, 120.0f);
    }
    glm::mat4 getGlobalMat() const
    {
        return glm::mat4(
            glm::vec4(right_, 0.0f),
            glm::vec4(up_, 0.0f),
            glm::vec4(front_, 0.0f),
            glm::vec4(position_.x, position_.y, position_.z, 1.0f));
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
        std::swap(cursorX_, other.cursorX_);
        std::swap(cursorY_, other.cursorY_);
        std::swap(moveSensitivity_, other.moveSensitivity_);
        std::swap(viewSensitivity_, other.viewSensitivity_);
        std::swap(zoomSensitivity_, other.zoomSensitivity_);
        std::swap(fovy_, other.fovy_);
        std::swap(aspect_, other.aspect_);
    }
};

#endif
