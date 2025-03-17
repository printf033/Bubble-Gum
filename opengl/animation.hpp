#include <string>
#include <vector>
#include <unordered_map>
#include "bone.hpp"
#include "model.hpp"
#include "converter.hpp"

#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#define ANIMATION_ID 0

struct HierarchyData
{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<HierarchyData> children;
};

class Animation
{
    double duration_;
    double ticksPerSecond_;
    std::vector<Bone> bones_;
    HierarchyData root_;

public:
    Model *pModel_;
    glm::mat4 globalInverseTransform_;

public:
    Animation(Model *model)
        : pModel_(model)
    {
        assert(pModel_ != nullptr);
        Assimp::Importer importer;
        const aiScene *paiScene = importer.ReadFile(pModel_->path_, 0);
        assert(paiScene != nullptr &&
               !(paiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) &&
               paiScene->mRootNode != nullptr);
        aiAnimation *paiAnimation = paiScene->mAnimations[ANIMATION_ID]; // 0号动画
        duration_ = paiAnimation->mDuration;
        ticksPerSecond_ = paiAnimation->mTicksPerSecond;
        aiMatrix4x4 globalTransformation = paiScene->mRootNode->mTransformation;
        globalInverseTransform_ = Converter::convertMatrixToGLMFormat(globalTransformation.Inverse());
        readHierarchyData(root_, paiScene->mRootNode);
        readMissingBones(paiAnimation, *model);
    }
    ~Animation() {}
    Bone *findBone(const std::string &name)
    {
        auto it = std::find_if(bones_.begin(), bones_.end(),
                               [&](const Bone &bone)
                               { return bone.getBoneName() == name; });
        if (it == bones_.end())
            return nullptr;
        else
            return &(*it);
    }
    inline double getTicksPerSecond() const { return ticksPerSecond_; }
    inline double getDuration() const { return duration_; }
    inline const HierarchyData &getRootNode() const { return root_; }

private:
    void readHierarchyData(HierarchyData &dst, const aiNode *src)
    {
        assert(src != nullptr);
        dst.name = src->mName.C_Str();
        dst.transformation = Converter::convertMatrixToGLMFormat(src->mTransformation);
        dst.childrenCount = src->mNumChildren;
        dst.children.reserve(dst.childrenCount);
        for (int i = 0; i < dst.childrenCount; ++i)
        {
            HierarchyData child;
            readHierarchyData(child, src->mChildren[i]);
            dst.children.push_back(child);
        }
    }
    void readMissingBones(const aiAnimation *paiAnimation, Model &model)
    {
        assert(paiAnimation != nullptr);
        for (int i = 0; i < paiAnimation->mNumChannels; ++i)
        {
            std::string boneName = paiAnimation->mChannels[i]->mNodeName.C_Str();
            if (model.boneLoaded_.find(boneName) == model.boneLoaded_.end())
                model.boneLoaded_[boneName] = BoneData{model.boneCount_++,
                                                       glm::mat4(1.0f)};
            //////////////////////////////////////////////////////
            LOG_DEBUG << boneName;
            bones_.push_back(Bone(boneName,
                                  model.boneLoaded_[paiAnimation->mChannels[i]->mNodeName.C_Str()].id,
                                  paiAnimation->mChannels[i]));
        }
    }
};

#endif