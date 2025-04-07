#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "model.hpp"
#include "animation.hpp"

#ifndef ANIMATOR_HPP
#define ANIMATOR_HPP

class Animator
{
    Model model_;
    std::unordered_map<std::string, Animation> animations_;
    Animation *curAnim_;
    double curTick_;
    std::vector<glm::mat4> finalTransforms_;

public:
    Animator(Model &model)
        : model_(model),
          curAnim_(nullptr),
          curTick_(0.0)
    {
        Assimp::Importer importer;
        auto paiScene = importer.ReadFile(model_.getPath(), 0);
        assert(paiScene != nullptr &&
               !(paiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) &&
               paiScene->mRootNode != nullptr);
        animations_.reserve(paiScene->mNumAnimations);
        for (unsigned int i = 0; i < paiScene->mNumAnimations; ++i)
        {
            auto paiAnimation = paiScene->mAnimations[i];
            animations_.emplace(paiAnimation->mName.C_Str(), Animation(paiAnimation, paiScene, model_));
        }
    }
    ~Animator() {}
    bool setCurAnimation(const std::string &animName)
    {
        if (!animations_.contains(animName))
            return false;
        curAnim_ = &animations_.at(animName);
        return true;
    }
    void updateAnimation(double deltaTime)
    {
        if (curAnim_ == nullptr)
            return;
        curTick_ += deltaTime * curAnim_->getTicksPerSecond();
        curTick_ = fmod(curTick_, curAnim_->getDuration()); // 循环播放
        calculateFinalTransform(curAnim_->getRootNode(), glm::mat4(1.0f));
    }
    std::vector<glm::mat4> getFinalTransforms() { return finalTransforms_; }

private:
    void calculateFinalTransform(const Hierarchy &node, glm::mat4 parentTransform)
    {
        KeyFrame curBone = curAnim_->getBoneKeyFrame(node.name); // 没有这个骨骼关键帧的话KeyFrame的默认构造assert会拦截
        // 要保证所有需要的骨骼boneloaded都有
        glm::mat4 localTransform = parentTransform * curBone.interpolate(curTick_);
        finalTransforms_[model_.getBoneLoaded()[node.name].id] = localTransform * model_.getBoneLoaded()[node.name].offset;
        for (int i = 0; i < node.children.size(); ++i)
            calculateFinalTransform(node.children[i], localTransform);
    }
};

#endif