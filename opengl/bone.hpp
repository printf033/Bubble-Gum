#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#ifndef BONE_HPP
#define BONE_HPP

struct KeyPosition
{
    glm::vec3 position;
    double timeStamp;
};
struct KeyRotation
{
    glm::quat orientation;
    double timeStamp;
};
struct KeyScale
{
    glm::vec3 scale;
    double timeStamp;
};

class Bone
{
    std::vector<KeyPosition> positions_;
    std::vector<KeyRotation> rotations_;
    std::vector<KeyScale> scales_;
    int positionsCount_;
    int rotationsCount_;
    int scalesCount_;
    glm::mat4 localTransform_;
    std::string name_;
    int ID_;

public:
    Bone(const std::string &name, const int ID, const aiNodeAnim *channel)
        : name_(name),
          ID_(ID),
          localTransform_(1.0f)
    {
        positionsCount_ = channel->mNumPositionKeys;
        for (int i = 0; i < positionsCount_; ++i)
        {
            aiVector3D position = channel->mPositionKeys[i].mValue;
            double timeStamp = channel->mPositionKeys[i].mTime;
            KeyPosition data;
            data.position = Converter::getGLMVec(position);
            data.timeStamp = timeStamp;
            positions_.push_back(data);
        }
        rotationsCount_ = channel->mNumRotationKeys;
        for (int i = 0; i < rotationsCount_; ++i)
        {
            aiQuaternion orientation = channel->mRotationKeys[i].mValue;
            double timeStamp = channel->mRotationKeys[i].mTime;
            KeyRotation data;
            data.orientation = Converter::getGLMQuat(orientation);
            data.timeStamp = timeStamp;
            rotations_.push_back(data);
        }
        scalesCount_ = channel->mNumScalingKeys;
        for (int i = 0; i < scalesCount_; ++i)
        {
            aiVector3D scale = channel->mScalingKeys[i].mValue;
            double timeStamp = channel->mScalingKeys[i].mTime;
            KeyScale data;
            data.scale = Converter::getGLMVec(scale);
            data.timeStamp = timeStamp;
            scales_.push_back(data);
        }
    }
    ~Bone() {}
    std::string getBoneName() const { return name_; }
    const glm::mat4 &getLocalTransform() const { return localTransform_; }
    void update(float animationTime)
    {
        glm::mat4 translation = interpolatePosition(animationTime);
        glm::mat4 rotation = interpolateRotation(animationTime);
        glm::mat4 scale = interpolateScaling(animationTime);
        localTransform_ = translation * rotation * scale;
    }

private:
    glm::mat4 interpolatePosition(float animationTime)
    {
        if (1 == positionsCount_)
            return glm::translate(glm::mat4(1.0f), positions_[0].position);
        int fra0 = getPositionIndex(animationTime);
        int fra1 = fra0 + 1;
        float scaleFactor = getScaleFactor(positions_[fra0].timeStamp,
                                           positions_[fra1].timeStamp,
                                           animationTime);
        glm::vec3 finalPosition = glm::mix(positions_[fra0].position,
                                           positions_[fra1].position,
                                           scaleFactor);
        return glm::translate(glm::mat4(1.0f), finalPosition);
    }
    glm::mat4 interpolateRotation(float animationTime)
    {
        if (1 == rotationsCount_)
        {
            auto rotation = glm::normalize(rotations_[0].orientation);
            return glm::toMat4(rotation);
        }
        int fra0 = getRotationIndex(animationTime);
        int fra1 = fra0 + 1;
        float scaleFactor = getScaleFactor(rotations_[fra0].timeStamp,
                                           rotations_[fra1].timeStamp,
                                           animationTime);
        glm::quat finalRotation = glm::slerp(rotations_[fra0].orientation,
                                             rotations_[fra1].orientation,
                                             scaleFactor);
        finalRotation = glm::normalize(finalRotation);
        return glm::toMat4(finalRotation);
    }
    glm::mat4 interpolateScaling(float animationTime)
    {
        if (1 == scalesCount_)
            return glm::scale(glm::mat4(1.0f), scales_[0].scale);
        int fra0 = getScaleIndex(animationTime);
        int fra1 = fra0 + 1;
        float scaleFactor = getScaleFactor(scales_[fra0].timeStamp,
                                           scales_[fra1].timeStamp,
                                           animationTime);
        glm::vec3 finalScale = glm::mix(scales_[fra0].scale,
                                        scales_[fra1].scale,
                                        scaleFactor);
        return glm::scale(glm::mat4(1.0f), finalScale);
    }
    int getPositionIndex(float animationTime) const
    {
        for (int i = 1; i < positionsCount_; ++i)
        {
            if (animationTime < positions_[i].timeStamp)
                return i - 1;
        }
        return positionsCount_ - 1;
    }
    int getRotationIndex(float animationTime) const
    {
        for (int i = 1; i < rotationsCount_; ++i)
        {
            if (animationTime < rotations_[i].timeStamp)
                return i - 1;
        }
        return rotationsCount_ - 1;
    }
    int getScaleIndex(float animationTime) const
    {
        for (int i = 1; i < scalesCount_; ++i)
        {
            if (animationTime < scales_[i].timeStamp)
                return i - 1;
        }
        return scalesCount_ - 1;
    }
    float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) const
    {
        float scaleFactor = 0.0f;
        float midWayLength = animationTime - lastTimeStamp;
        float framesDiff = nextTimeStamp - lastTimeStamp;
        scaleFactor = midWayLength / framesDiff;
        return scaleFactor;
    }
};

#endif