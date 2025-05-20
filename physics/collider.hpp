#include "animator.hpp"
#include "movement.hpp"

#ifndef COLLIDER_HPP
#define COLLIDER_HPP

// 目前Collider仅用于 本地直接控制的静态模型 检测！
#define COLLIDER_POS_X 0.0f
#define COLLIDER_POS_Y 0.0f
#define COLLIDER_POS_HEIGHT 0.0f
#define COLLIDER_POS_MOVE_SENSITIVITY 5.0f

class Collider : public Animator
{
    bool isMoved_;
    float moveSensitivity_;
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;

public:
    Collider(const std::filesystem::path &path,
             float x = COLLIDER_POS_X,
             float y = COLLIDER_POS_Y,
             float height = COLLIDER_POS_HEIGHT)
        : Animator(path),
          isMoved_(false),
          moveSensitivity_(COLLIDER_POS_MOVE_SENSITIVITY),
          position_(x, height, y),
          front_(0.0f, 0.0f, -1.0f),
          up_(0.0f, 1.0f, 0.0f),
          right_(1.0f, 0.0f, 0.0f) {}
    Collider(const std::filesystem::path &path,
             const std::string &animName,
             float x = COLLIDER_POS_X,
             float y = COLLIDER_POS_Y,
             float height = COLLIDER_POS_HEIGHT)
        : Animator(path, animName),
          isMoved_(false),
          moveSensitivity_(COLLIDER_POS_MOVE_SENSITIVITY),
          position_(x, height, y),
          front_(0.0f, 0.0f, -1.0f),
          up_(0.0f, 1.0f, 0.0f),
          right_(1.0f, 0.0f, 0.0f) {}
    Collider(Animator &&animator,
             float x = COLLIDER_POS_X,
             float y = COLLIDER_POS_Y,
             float height = COLLIDER_POS_HEIGHT)
        : Animator(std::move(animator)),
          isMoved_(false),
          moveSensitivity_(COLLIDER_POS_MOVE_SENSITIVITY),
          position_(x, height, y),
          front_(0.0f, 0.0f, -1.0f),
          up_(0.0f, 1.0f, 0.0f),
          right_(1.0f, 0.0f, 0.0f) {}
    Collider(const std::filesystem::path &path,
             const glm::mat4 &globalMat)
        : Animator(path),
          isMoved_(false),
          moveSensitivity_(COLLIDER_POS_MOVE_SENSITIVITY),
          position_(glm::vec3(globalMat[3][0], globalMat[3][1], globalMat[3][2])),
          front_(0.0f, 0.0f, -1.0f),
          up_(0.0f, 1.0f, 0.0f),
          right_(1.0f, 0.0f, 0.0f) {}
    ~Collider() = default;
    Collider(const Collider &) = delete;
    Collider &operator=(const Collider &) = delete;
    Collider(Collider &&other)
        : Animator(std::move(other)),
          isMoved_(other.isMoved_),
          moveSensitivity_(other.moveSensitivity_),
          position_(other.position_),
          front_(other.front_),
          up_(other.up_),
          right_(other.right_)
    {
        other.isMoved_ = false;
        other.position_ = glm::vec3(0.0f);
        other.front_ = glm::vec3(0.0f);
        other.up_ = glm::vec3(0.0f);
        other.right_ = glm::vec3(0.0f);
        other.moveSensitivity_ = 0.0f;
    }
    Collider &operator=(Collider &&other)
    {
        if (this != &other)
            Collider(std::move(other)).swap(*this);
        return *this;
    }
    bool isMoved() const { return isMoved_; }
    void clearMoved() { isMoved_ = false; }
    glm::vec3 &getPosition() { return position_; }
    /////////////////////////////////test////////////////////////////////////
    glm::mat4 getGlobalMat() const
    {
        return glm::mat4(
            glm::vec4(right_, 0.0f),
            glm::vec4(up_, 0.0f),
            glm::vec4(front_, 0.0f),
            glm::vec4(position_.x, position_.y, position_.z, 1.0f));
    }
    void setViewMove(const glm::mat4 &globalMat)
    {
        front_.x = globalMat[0][2];
        front_.y = globalMat[1][2];
        front_.z = globalMat[2][2];
        up_.x = globalMat[0][1];
        up_.y = globalMat[1][1];
        up_.z = globalMat[2][1];
        right_.x = globalMat[0][0];
        right_.y = globalMat[1][0];
        right_.z = globalMat[2][0];
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
        isMoved_ = true;
    }

private:
    void swap(Collider &other)
    {
        Animator::swap(other);
        std::swap(isMoved_, other.isMoved_);
        std::swap(moveSensitivity_, other.moveSensitivity_);
        std::swap(position_, other.position_);
        std::swap(front_, other.front_);
        std::swap(up_, other.up_);
        std::swap(right_, other.right_);
    }
};

#endif