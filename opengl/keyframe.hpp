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
    glm::mat4 curTransform_;

public:
    KeyFrame(const aiNodeAnim *channel = nullptr) : curTransform_(1.0f)
    {
        assert(channel != nullptr);
        positions_.reserve(channel->mNumPositionKeys);
        for (unsigned int i = 0; i < channel->mNumPositionKeys; ++i)
        {
            aiVector3D position = channel->mPositionKeys[i].mValue;
            double tickStamp = channel->mPositionKeys[i].mTime;
            positions_.push_back({Converter::getGLMVec(position), tickStamp});
        }
        rotations_.reserve(channel->mNumRotationKeys);
        for (unsigned int i = 0; i < channel->mNumRotationKeys; ++i)
        {
            aiQuaternion orientation = channel->mRotationKeys[i].mValue;
            double tickStamp = channel->mRotationKeys[i].mTime;
            rotations_.push_back({Converter::getGLMQuat(orientation), tickStamp});
        }
        scales_.reserve(channel->mNumScalingKeys);
        for (unsigned int i = 0; i < channel->mNumScalingKeys; ++i)
        {
            aiVector3D scale = channel->mScalingKeys[i].mValue;
            double tickStamp = channel->mScalingKeys[i].mTime;
            scales_.push_back({Converter::getGLMVec(scale), tickStamp});
        }
    }
    ~KeyFrame() {}
    const glm::mat4 &interpolate(double curTick)
    {
        glm::mat4 translation = interpolatePosition(curTick);
        glm::mat4 rotation = interpolateRotation(curTick);
        glm::mat4 scale = interpolateScaling(curTick);
        curTransform_ = translation * rotation * scale;
        return curTransform_;
    }

private:
    glm::mat4 interpolatePosition(double curTick)
    {
        if (1 == positions_.size())
            return glm::translate(glm::mat4(1.0f), positions_[0].position);
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
        if (1 == rotations_.size())
            return glm::toMat4(glm::normalize(rotations_[0].orientation));
        int key = getRotationIndex(curTick);
        if (key == rotations_.size() - 1)
            return glm::toMat4(glm::normalize(rotations_[key].orientation));
        double scaleFactor = getScaleFactor(rotations_[key].tickStamp,
                                            rotations_[key + 1].tickStamp,
                                            curTick);
        glm::quat finalRotation = glm::slerp(rotations_[key].orientation,
                                             rotations_[key + 1].orientation,
                                             static_cast<float>(scaleFactor));
        return glm::toMat4(glm::normalize(finalRotation));
    }
    glm::mat4 interpolateScaling(double curTick)
    {
        if (1 == scales_.size())
            return glm::scale(glm::mat4(1.0f), scales_[0].scale);
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
    int getPositionIndex(double curTick) const
    {
        for (unsigned int i = 1; i < positions_.size(); ++i)
            if (curTick < positions_[i].tickStamp)
                return i - 1;
        return positions_.size() - 1;
    }
    int getRotationIndex(double curTick) const
    {
        for (unsigned int i = 1; i < rotations_.size(); ++i)
            if (curTick < rotations_[i].tickStamp)
                return i - 1;
        return rotations_.size() - 1;
    }
    int getScaleIndex(double curTick) const
    {
        for (unsigned int i = 1; i < scales_.size(); ++i)
            if (curTick < scales_[i].tickStamp)
                return i - 1;
        return scales_.size() - 1;
    }
    double getScaleFactor(double lastTickStamp, double nextTickStamp, double curTick) const
    {
        double midWayLength = curTick - lastTickStamp;
        double framesDiff = nextTickStamp - lastTickStamp;
        return midWayLength / framesDiff;
    }
};

#endif