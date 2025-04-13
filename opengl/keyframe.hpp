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
            ///////////////////////////////////////////////////////////////////////////////////
            LOG_DEBUG << "key#" << i << " pos#\n"
                      << positions_.back().position[0] << '#'
                      << positions_.back().position[1] << '#'
                      << positions_.back().position[2] << '#';
            ///////////////////////////////////////////////////////////////////////////////////
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
    const glm::mat4 interpolate(double curTick)
    {
        glm::mat4 transformation = glm::mat4(1.0f);
        if (!scales_.empty())
            transformation *= interpolateScaling(curTick);
        if (!rotations_.empty())
            transformation *= interpolateRotation(curTick);
        if (!positions_.empty())
            transformation *= interpolatePosition(curTick);
        ///////////////////////////////////////////////////////////////////////////////////
        LOG_DEBUG << "模型变换矩阵#\n"
                  << transformation[0][0] << '#'
                  << transformation[1][0] << '#'
                  << transformation[2][0] << '#'
                  << transformation[3][0] << "#\n"
                  << transformation[0][1] << '#'
                  << transformation[1][1] << '#'
                  << transformation[2][1] << '#'
                  << transformation[3][1] << "#\n"
                  << transformation[0][2] << '#'
                  << transformation[1][2] << '#'
                  << transformation[2][2] << '#'
                  << transformation[3][2] << "#\n"
                  << transformation[0][3] << '#'
                  << transformation[1][3] << '#'
                  << transformation[2][3] << '#'
                  << transformation[3][3] << '#';
        ///////////////////////////////////////////////////////////////////////////////////
        return transformation;
    }

private:
    glm::mat4 interpolatePosition(double curTick)
    {
        if (positions_.empty())
            return glm::mat4(1.0f);
        int key = getPositionIndex(curTick);
        if (key == positions_.size() - 1)
            return glm::translate(glm::mat4(1.0f), positions_[key].position);
        double scaleFactor = getScaleFactor(positions_[key].tickStamp,
                                            positions_[key + 1].tickStamp,
                                            curTick);
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
        if (key == rotations_.size() - 1)
            return glm::toMat4(glm::normalize(rotations_[key].orientation));
        double scaleFactor = getScaleFactor(rotations_[key].tickStamp,
                                            rotations_[key + 1].tickStamp,
                                            curTick);
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
        if (key == scales_.size())
            return glm::scale(glm::mat4(1.0f), scales_[key].scale);
        double scaleFactor = getScaleFactor(scales_[key].tickStamp,
                                            scales_[key + 1].tickStamp,
                                            curTick);
        glm::vec3 finalScale = glm::mix(scales_[key].scale,
                                        scales_[key + 1].scale,
                                        scaleFactor);
        return glm::scale(glm::mat4(1.0f), finalScale);
    }
    int getPositionIndex(double curTick)
    {
        if (nextPos_ == positions_.size() - 1) // 循环播放
            nextPos_ = 1;
        if (curTick >= positions_[nextPos_].tickStamp &&
            nextPos_ < positions_.size())
            nextPos_ += 1;
        //////////////////////////////////////////////////////////////////////
        LOG_TRACE << nextPos_ << " curTick:" << curTick << " tickStamp:" << positions_[nextPos_].tickStamp;
        //////////////////////////////////////////////////////////////////////
        return nextPos_ - 1;
    }
    int getRotationIndex(double curTick)
    {
        if (nextRot_ == rotations_.size() - 1)
            nextRot_ = 0;
        if (curTick >= rotations_[nextRot_].tickStamp &&
            nextRot_ < rotations_.size())
            nextRot_ += 1;
        return nextRot_ - 1;
    }
    int getScaleIndex(double curTick)
    {
        if (nextScl_ == scales_.size() - 1)
            nextScl_ = 0;
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