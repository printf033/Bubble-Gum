#include <vector>
#include <glm/glm.hpp>
#include "animation.hpp"

#ifndef ANIMATOR_HPP
#define ANIMATOR_HPP

#define MAX_BONE_MATRICES 300

class Animator
{
    std::vector<glm::mat4> finalBoneMatrices_;
    double currentTime_;
    Animation *pAnimation_;

public:
    Animator(Animation *animation)
        : currentTime_(0.0),
          pAnimation_(animation)
    {
        finalBoneMatrices_.reserve(MAX_BONE_MATRICES);
        for (int i = 0; i < MAX_BONE_MATRICES; ++i)
            finalBoneMatrices_.push_back(glm::mat4(1.0f));
    }
    ~Animator() {}
    void updateAnimation(float deltaTime)
    {
        if (pAnimation_ != nullptr)
        {
            currentTime_ += deltaTime * pAnimation_->getTicksPerSecond();
            currentTime_ = fmod(currentTime_, pAnimation_->getDuration());
            //calculateBoneTransform(&pAnimation_->getRootNode(), pAnimation_->globalInverseTransform_);
        }
    }
    std::vector<glm::mat4> getFinalBoneMatrices() { return finalBoneMatrices_; }

private:
    void calculateBoneTransform(const HierarchyData *node, glm::mat4 &parentTransform)
    {
        glm::mat4 nodeTransform = node->transformation;
        Bone *pBone = pAnimation_->findBone(node->name);
        if (nullptr != pBone)
        {
            pBone->update(currentTime_);
            nodeTransform = pBone->getLocalTransform();
            ///////////////////////////////////////////////
            LOG_ERROR << pBone->getBoneName();
        }
        else
        {
            ///////////////////////////////////////////////
            LOG_ERROR << "missing bone!!!!!!!!!! " << node->name;
        }
        glm::mat4 globalTransformation = parentTransform * nodeTransform;
        auto boneLoaded = pAnimation_->pModel_->boneLoaded_;
        if (boneLoaded.find(node->name) != boneLoaded.end())
        {
            glm::mat4 offset = boneLoaded[node->name].offset;
            finalBoneMatrices_[boneLoaded[node->name].id] = globalTransformation * offset;
        }
        for (int i = 0; i < node->childrenCount; ++i)
            calculateBoneTransform(&node->children[i], globalTransformation);
    }
};

#endif