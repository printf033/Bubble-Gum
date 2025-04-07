#include <string>
#include <vector>
#include <unordered_map>
#include "keyframe.hpp"
#include "model.hpp"
#include "converter.hpp"

#ifndef ANIMATION_HPP
#define ANIMATION_HPP

struct Hierarchy
{
    std::string name;
    glm::mat4 transformation;
    std::vector<Hierarchy> children;
};

class Animation
{
    double duration_;
    double ticksPerSecond_;
    std::unordered_map<std::string, KeyFrame> keyFrames_;
    Hierarchy root_;

public:
    Animation(aiAnimation *paiAnimation,
              const aiScene *paiScene,
              Model &model)
        : duration_(paiAnimation->mDuration),
          ticksPerSecond_(paiAnimation->mTicksPerSecond)
    {
        assert(paiAnimation != nullptr);
        assert(paiScene != nullptr);
        assert(ticksPerSecond_ != 0);
        readKeyFrames(paiAnimation, model);
        readHierarchy(root_, paiScene->mRootNode);
    }
    ~Animation() {}
    inline KeyFrame getBoneKeyFrame(const std::string &boneName) { return keyFrames_[boneName]; }
    inline double getTicksPerSecond() const { return ticksPerSecond_; }
    inline double getDuration() const { return duration_; }
    inline const Hierarchy &getRootNode() const { return root_; }

private:
    void readKeyFrames(aiAnimation *paiAnimation, Model &model)
    {
        assert(paiAnimation != nullptr);
        for (unsigned int i = 0; i < paiAnimation->mNumChannels; ++i)
        {
            auto curBone = paiAnimation->mChannels[i];
            std::string boneName(curBone->mNodeName.C_Str());
            if (!model.getBoneLoaded().contains(boneName))
                model.getBoneLoaded()[boneName] = BoneData{static_cast<int>(model.getBoneLoaded().size()),
                                                           /*推导*/};
            LOG_DEBUG << boneName; ///////////////////////////////////本动画所有有关的骨骼
            keyFrames_[boneName] = KeyFrame(curBone);
        }
    }
    void readHierarchy(Hierarchy &hierarchy, aiNode *paiNode)
    {
        assert(paiNode != nullptr);
        hierarchy.name = paiNode->mName.C_Str();
        hierarchy.transformation = Converter::convertMatrixToGLMFormat(paiNode->mTransformation);
        hierarchy.children.reserve(paiNode->mNumChildren);
        for (unsigned int i = 0; i < paiNode->mNumChildren; ++i)
        {
            Hierarchy child;
            readHierarchy(child, paiNode->mChildren[i]);
            hierarchy.children.push_back(child);
        }
    }
};

#endif