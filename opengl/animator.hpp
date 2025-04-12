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
    GLuint SSBO_;

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
            /////////////////////////////////////////////////////////////////////////////
            LOG_INFO << paiAnimation->mName.C_Str();
            /////////////////////////////////////////////////////////////////////////////
        }
        finalTransforms_.resize(model_.getBoneLoaded().size(), glm::mat4(1.0f));
        glGenBuffers(1, &SSBO_);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_);
        glBufferData(GL_SHADER_STORAGE_BUFFER, finalTransforms_.size() * sizeof(glm::mat4), finalTransforms_.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO_);
    }
    ~Animator() {}
    bool setCurAnimation(const std::string &animName)
    {
        if (!animations_.contains(animName))
        {
            LOG_ERROR << "Animation not found: " << animName;
            return false;
        }
        curAnim_ = &animations_.at(animName);
        LOG_INFO << "Set animation: " << animName
                 << ", duration: " << curAnim_->getDuration()
                 << ", ticks/s: " << curAnim_->getTicksPerSecond();
        return true;
    }
    void updateAnimation(double deltaTime)
    {
        assert(curAnim_ != nullptr);
        curTick_ += deltaTime * curAnim_->getTicksPerSecond();
        curTick_ = fmod(curTick_, curAnim_->getDuration()); // 循环播放
        calculateFinalTransform(curAnim_->getRootNode(), glm::mat4(1.0f));
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, finalTransforms_.size() * sizeof(glm::mat4), finalTransforms_.data());
    }

private:
    void calculateFinalTransform(const Hierarchy &node, glm::mat4 parentTransform)
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (model_.getBoneLoaded().contains(node.name))
        {
            parentTransform *= curAnim_->getBoneKeyFrame(node.name).interpolate(curTick_);
            finalTransforms_[model_.getBoneLoaded()[node.name].id] = parentTransform * model_.getBoneLoaded()[node.name].offset;
        }
        for (const auto &child : node.children)
            calculateFinalTransform(child, parentTransform);
    }
};

#endif