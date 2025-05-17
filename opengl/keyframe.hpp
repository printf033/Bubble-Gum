#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/anim.h>
#include "converter.hpp"

#ifndef KEYFRAME_HPP
#define KEYFRAME_HPP

struct KeyPosition
{
    glm::vec3 position;
    double tickStamp;
};
struct KeyRotation
{
    glm::quat orientation;
    double tickStamp;
};
struct KeyScale
{
    glm::vec3 scale;
    double tickStamp;
};

class KeyFrame
{
    std::vector<KeyPosition> positions_;
    std::vector<KeyRotation> rotations_;
    std::vector<KeyScale> scales_;
    int nextPos_;
    int nextRot_;
    int nextScl_;

public:
    KeyFrame(const aiNodeAnim *channel = nullptr)
        : nextPos_(1),
          nextRot_(1),
          nextScl_(1)
    {
        assert(channel != nullptr);
        positions_.reserve(channel->mNumPositionKeys);
        for (unsigned int i = 0; i < channel->mNumPositionKeys; ++i)
        {
            aiVector3D pos = channel->mPositionKeys[i].mValue;
            double tickStamp = channel->mPositionKeys[i].mTime;
            positions_.push_back({Converter::getGLMVec(pos), tickStamp});
        }
        rotations_.reserve(channel->mNumRotationKeys);
        for (unsigned int i = 0; i < channel->mNumRotationKeys; ++i)
        {
            aiQuaternion ort = channel->mRotationKeys[i].mValue;
            double tickStamp = channel->mRotationKeys[i].mTime;
            rotations_.push_back({Converter::getGLMQuat(ort), tickStamp});
        }
        scales_.reserve(channel->mNumScalingKeys);
        for (unsigned int i = 0; i < channel->mNumScalingKeys; ++i)
        {
            aiVector3D scl = channel->mScalingKeys[i].mValue;
            double tickStamp = channel->mScalingKeys[i].mTime;
            scales_.push_back({Converter::getGLMVec(scl), tickStamp});
        }
    }
    ~KeyFrame() {}
    KeyFrame(const KeyFrame &) = delete;
    KeyFrame &operator=(const KeyFrame &) = delete;
    KeyFrame(KeyFrame &&other)
        : positions_(std::move(other.positions_)),
          rotations_(std::move(other.rotations_)),
          scales_(std::move(other.scales_)),
          nextPos_(other.nextPos_),
          nextRot_(other.nextRot_),
          nextScl_(other.nextScl_)
    {
        other.nextPos_ = 0;
        other.nextRot_ = 0;
        other.nextScl_ = 0;
    }
    KeyFrame &operator=(KeyFrame &&other)
    {
        if (this != &other)
            KeyFrame(std::move(other)).swap(*this);
        return *this;
    }
    const glm::mat4 interpolate(double curTick)
    {
        glm::mat4 transformation = glm::mat4(1.0f);
        if (!scales_.empty())
            transformation *= interpolateScaling(curTick);
        if (!rotations_.empty())
            transformation *= interpolateRotation(curTick);
        if (!positions_.empty())
            transformation *= interpolatePosition(curTick);
        return transformation;
    }

private:
    void swap(KeyFrame &other)
    {
        std::swap(positions_, other.positions_);
        std::swap(rotations_, other.rotations_);
        std::swap(scales_, other.scales_);
        std::swap(nextPos_, other.nextPos_);
        std::swap(nextRot_, other.nextRot_);
        std::swap(nextScl_, other.nextScl_);
    }
    glm::mat4 interpolatePosition(double curTick)
    {
        if (positions_.empty())
            return glm::mat4(1.0f);
        int key = getPositionIndex(curTick);
        if (key >= positions_.size() - 1)
            return glm::translate(glm::mat4(1.0f), positions_[key].position);
        double scaleFactor = getScaleFactor(positions_[key].tickStamp,
                                            positions_[key + 1].tickStamp,
                                            curTick);
        /////////////////////////////////////////////////////////////////////////////////
        glm::vec3 finalPosition = glm::mix(positions_[key].position,
                                           positions_[key + 1].position,
                                           scaleFactor);
        return glm::translate(glm::mat4(1.0f), finalPosition);
    }
    glm::mat4 interpolateRotation(double curTick)
    {
        if (rotations_.empty())
            return glm::mat4(1.0f);
        int key = getRotationIndex(curTick);
        if (key >= rotations_.size() - 1)
            return glm::toMat4(glm::normalize(rotations_[key].orientation));
        double scaleFactor = getScaleFactor(rotations_[key].tickStamp,
                                            rotations_[key + 1].tickStamp,
                                            curTick);
        /////////////////////////////////////////////////////////////////////////////////
        glm::quat q1 = rotations_[key].orientation;
        glm::quat q2 = rotations_[key + 1].orientation;
        if (glm::dot(q1, q2) < 0.0f)
            q2 = -q2;
        glm::quat finalRotation = glm::slerp(q1, q2, static_cast<float>(scaleFactor));
        return glm::toMat4(glm::normalize(finalRotation));
    }
    glm::mat4 interpolateScaling(double curTick)
    {
        if (scales_.empty())
            return glm::mat4(1.0f);
        int key = getScaleIndex(curTick);
        if (key >= scales_.size())
            return glm::scale(glm::mat4(1.0f), scales_[key].scale);
        double scaleFactor = getScaleFactor(scales_[key].tickStamp,
                                            scales_[key + 1].tickStamp,
                                            curTick);
        /////////////////////////////////////////////////////////////////////////////////
        glm::vec3 finalScale = glm::mix(scales_[key].scale,
                                        scales_[key + 1].scale,
                                        scaleFactor);
        return glm::scale(glm::mat4(1.0f), finalScale);
    }
    int getPositionIndex(double curTick)
    {
        if (curTick >= positions_[nextPos_].tickStamp &&
            nextPos_ < positions_.size())
            nextPos_ += 1;
        return nextPos_ - 1;
    }
    int getRotationIndex(double curTick)
    {
        if (curTick >= rotations_[nextRot_].tickStamp &&
            nextRot_ < rotations_.size())
            nextRot_ += 1;
        return nextRot_ - 1;
    }
    int getScaleIndex(double curTick)
    {
        if (curTick >= scales_[nextScl_].tickStamp &&
            nextScl_ < scales_.size())
            nextScl_ += 1;
        return nextScl_ - 1;
    }
    double getScaleFactor(double lastTickStamp, double nextTickStamp, double curTick) const
    {
        double midWayLength = curTick - lastTickStamp;
        double framesDiff = nextTickStamp - lastTickStamp;
        return midWayLength / framesDiff;
    }
};

#endif