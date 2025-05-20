#include "animator.hpp"
#include "movement.hpp"

#ifndef COLLIDER_HPP
#define COLLIDER_HPP

class Collider : public Animator
{
    bool isMoved_ = false;
    glm::mat4 globalMat_ = glm::mat4(1.0f);

public:
    Collider(const std::filesystem::path &path)
        : Animator(path) {}
    Collider(const std::filesystem::path &path, const std::string &animName)
        : Animator(path, animName) {}
    Collider(Animator &&animator)
        : Animator(std::move(animator)) {}
    ~Collider() = default;
    Collider(const Collider &) = delete;
    Collider &operator=(const Collider &) = delete;
    Collider(Collider &&other)
        : Animator(std::move(other)),
          isMoved_(other.isMoved_),
          globalMat_(other.globalMat_)
    {
        other.isMoved_ = false;
        other.globalMat_ = glm::mat4(1.0f);
    }
    Collider &operator=(Collider &&other)
    {
        if (this != &other)
            Collider(std::move(other)).swap(*this);
        return *this;
    }
    bool isMoved() const { return isMoved_; }
    void clearMoved() { isMoved_ = false; }
    void setGlobalMat(const glm::mat4 &globalMat)
    {
        globalMat_ = globalMat;
        isMoved_ = true;
    }
    const glm::mat4 &getGlobalMat() const { return globalMat_; }

    /////////////////////////////////test////////////////////////////////////
    float moveSensitivity_ = 5.0f;
    glm::vec3 position_ = glm::vec3(0.0f, .0f, 0.0f);
    glm::vec3 front_ = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 right_ = glm::vec3(1.0f, 0.0f, 0.0f);
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
        globalMat_ = glm::translate(glm::mat4(1.0f), position_);
        isMoved_ = true;
    }

private:
    void swap(Collider &other)
    {
        std::swap(isMoved_, other.isMoved_);
        std::swap(globalMat_, other.globalMat_);
        Animator::swap(other);
    }
};

#endif