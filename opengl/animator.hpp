#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "model.hpp"
#include "animation.hpp"

#ifndef ANIMATOR_HPP
#define ANIMATOR_HPP

class Animator : public Model
{
    std::unordered_map<std::string, Animation> animations_;
    Animation *curAnim_;
    double curTick_;
    std::vector<glm::mat4> finalTransforms_;
    GLuint SSBO_;
    GLuint bindingIndex_;
    static GLuint nextBindingPoint_;

public:
    Animator(const std::filesystem::path &path)
        : Model(path),
          curAnim_(nullptr),
          curTick_(0.0),
          SSBO_(0),
          bindingIndex_(0)
    {
        readAnimations(path);
    }
    Animator(const std::filesystem::path &path,
             const std::string &animName)
        : Model(path),
          curAnim_(nullptr),
          curTick_(0.0),
          bindingIndex_(0)
    {
        readAnimations(path);
        setCurAnimation(animName);
    }
    Animator(Model &&model)
        : Model(std::move(model)),
          curAnim_(nullptr),
          curTick_(0.0),
          bindingIndex_(0)
    {
        readAnimations(getPath());
    }
    Animator(Model &&model, const std::string &animName)
        : Model(std::move(model)),
          curAnim_(nullptr),
          curTick_(0.0),
          bindingIndex_(0)
    {
        readAnimations(getPath());
        setCurAnimation(animName);
    }
    ~Animator()
    {
        glDeleteBuffers(1, &SSBO_);
        curAnim_ = nullptr;
    }
    Animator(const Animator &) = delete;
    Animator &operator=(const Animator &) = delete;
    Animator(Animator &&other)
        : Model(std::move(other)),
          animations_(std::move(other.animations_)),
          curAnim_(other.curAnim_),
          curTick_(other.curTick_),
          finalTransforms_(std::move(other.finalTransforms_)),
          SSBO_(other.SSBO_),
          bindingIndex_(other.bindingIndex_)
    {
        other.curAnim_ = nullptr;
        other.curTick_ = 0.0;
        other.SSBO_ = 0;
    }
    Animator &operator=(Animator &&other)
    {
        if (this != &other)
            Animator(std::move(other)).swap(*this);
        return *this;
    }
    void setCurAnimation(const std::string &animName)
    {
        if (!animations_.contains(animName))
            LOG_ERROR << "Animation not found: " << animName;
        curAnim_ = &animations_.at(animName);
        LOG_INFO << "Set animation: " << animName
                 << ", duration: " << curAnim_->getDuration()
                 << ", ticks/s: " << curAnim_->getTicksPerSecond();
    }
    void updateAnimation(Shader &shader, double deltaTime)
    {
        assert(curAnim_ != nullptr);
        if (curTick_ < curAnim_->getDuration())
        {
            calculateFinalTransform(curAnim_->getRootNode(), glm::mat4(1.0f));
            curTick_ += deltaTime * curAnim_->getTicksPerSecond();
            GLuint blockIndex = glGetProgramResourceIndex(shader.getID(), GL_SHADER_STORAGE_BLOCK, "BoneTrans");
            if (blockIndex == GL_INVALID_INDEX)
                LOG_ERROR << "Shader storage block BoneMatrices not found!";
            else
                glShaderStorageBlockBinding(shader.getID(), blockIndex, bindingIndex_);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex_, SSBO_);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, finalTransforms_.size() * sizeof(glm::mat4), finalTransforms_.data());
        }
        draw(shader);
    }

private:
    void swap(Animator &other)
    {
        std::swap(animations_, other.animations_);
        std::swap(curAnim_, other.curAnim_);
        std::swap(curTick_, other.curTick_);
        std::swap(finalTransforms_, other.finalTransforms_);
        std::swap(SSBO_, other.SSBO_);
    }
    void readAnimations(const std::filesystem::path &path)
    {
        Assimp::Importer importer;
        auto paiScene = importer.ReadFile(path, 0);
        assert(paiScene != nullptr &&
               !(paiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) &&
               paiScene->mRootNode != nullptr);
        animations_.reserve(paiScene->mNumAnimations);
        for (unsigned int i = 0; i < paiScene->mNumAnimations; ++i)
        {
            auto paiAnimation = paiScene->mAnimations[i];
            animations_.emplace(paiAnimation->mName.C_Str(), Animation(paiAnimation, paiScene, *this));
            LOG_INFO << paiAnimation->mName.C_Str(); // 报菜名
        }
        finalTransforms_.resize(getBoneLoaded().size(), glm::mat4(1.0f));
        glGenBuffers(1, &SSBO_);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     finalTransforms_.size() * sizeof(glm::mat4),
                     finalTransforms_.data(),
                     GL_DYNAMIC_DRAW);
        bindingIndex_ = nextBindingPoint_++;
    }
    void calculateFinalTransform(const Hierarchy &node, glm::mat4 parentTransform)
    {
        //////////////////////////////////////////////////////////////////////改正
        if (getBoneLoaded().contains(node.name))
        {
            parentTransform *= curAnim_->getBoneKeyFrame(node.name).interpolate(curTick_);
            finalTransforms_[getBoneLoaded()[node.name].id] = parentTransform * getBoneLoaded()[node.name].offset;
            // //////////////////////////////////////////////////////////
            // LOG_DEBUG << "当前节点名称#" << node.name
            //           << " id#" << getBoneLoaded()[node.name].id
            //           << " 变换矩阵#\n"
            //           << finalTransforms_[getBoneLoaded()[node.name].id][0][0] << '#'
            //           << finalTransforms_[getBoneLoaded()[node.name].id][1][0] << '#'
            //           << finalTransforms_[getBoneLoaded()[node.name].id][2][0] << '#'
            //           << finalTransforms_[getBoneLoaded()[node.name].id][3][0] << "#\n"
            //           << finalTransforms_[getBoneLoaded()[node.name].id][0][1] << '#'
            //           << finalTransforms_[getBoneLoaded()[node.name].id][1][1] << '#'
            //           << finalTransforms_[getBoneLoaded()[node.name].id][2][1] << '#'
            //           << finalTransforms_[getBoneLoaded()[node.name].id][3][1] << "#\n"
            //           << finalTransforms_[getBoneLoaded()[node.name].id][0][2] << '#'
            //           << finalTransforms_[getBoneLoaded()[node.name].id][1][2] << '#'
            //           << finalTransforms_[getBoneLoaded()[node.name].id][2][2] << '#'
            //           << finalTransforms_[getBoneLoaded()[node.name].id][3][2] << "#\n"
            //           << finalTransforms_[getBoneLoaded()[node.name].id][0][3] << '#'
            //           << finalTransforms_[getBoneLoaded()[node.name].id][1][3] << '#'
            //           << finalTransforms_[getBoneLoaded()[node.name].id][2][3] << '#'
            //           << finalTransforms_[getBoneLoaded()[node.name].id][3][3] << '#';
            // //////////////////////////////////////////////////////////
        }
        for (const auto &child : node.children)
            calculateFinalTransform(child, parentTransform);
    }
};
GLuint Animator::nextBindingPoint_ = 0;
#endif