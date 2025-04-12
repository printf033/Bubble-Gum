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
        readHierarchy(root_, paiScene->mRootNode);
        readKeyFrames(paiAnimation, model);
    }
    ~Animation() {}
    inline KeyFrame &getBoneKeyFrame(const std::string &boneName) { return keyFrames_.at(boneName); }
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
                model.getBoneLoaded().emplace(boneName,
                                              BoneData{static_cast<int>(model.getBoneLoaded().size()),
                                                       glm::mat4(1.0f) /*推导*/});
            // ///////////////////////////////////////////////////////////////////////////////本动画所有有关的骨骼
            // LOG_DEBUG << boneName << "#" << model.getBoneLoaded()[boneName].id << "#\n"
            //           << model.getBoneLoaded()[boneName].offset[0][0] << '#'
            //           << model.getBoneLoaded()[boneName].offset[1][0] << '#'
            //           << model.getBoneLoaded()[boneName].offset[2][0] << '#'
            //           << model.getBoneLoaded()[boneName].offset[3][0] << "#\n"
            //           << model.getBoneLoaded()[boneName].offset[0][1] << '#'
            //           << model.getBoneLoaded()[boneName].offset[1][1] << '#'
            //           << model.getBoneLoaded()[boneName].offset[2][1] << '#'
            //           << model.getBoneLoaded()[boneName].offset[3][1] << "#\n"
            //           << model.getBoneLoaded()[boneName].offset[0][2] << '#'
            //           << model.getBoneLoaded()[boneName].offset[1][2] << '#'
            //           << model.getBoneLoaded()[boneName].offset[2][2] << '#'
            //           << model.getBoneLoaded()[boneName].offset[3][2] << "#\n"
            //           << model.getBoneLoaded()[boneName].offset[0][3] << '#'
            //           << model.getBoneLoaded()[boneName].offset[1][3] << '#'
            //           << model.getBoneLoaded()[boneName].offset[2][3] << '#'
            //           << model.getBoneLoaded()[boneName].offset[3][3] << '#';
            // ///////////////////////////////////////////////////////////////////////////////本动画所有有关的骨骼
            keyFrames_.emplace(boneName, KeyFrame(curBone));
        }
    }
    void readHierarchy(Hierarchy &node, aiNode *paiNode)
    {
        assert(paiNode != nullptr);
        node.name = paiNode->mName.C_Str();
        node.transformation = Converter::convertMatrixToGLMFormat(paiNode->mTransformation);
        node.children.reserve(paiNode->mNumChildren);
        for (unsigned int i = 0; i < paiNode->mNumChildren; ++i)
        {
            Hierarchy child;
            readHierarchy(child, paiNode->mChildren[i]);
            node.children.push_back(child);
        }
    }
};

#endif