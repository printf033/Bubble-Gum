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
            std::cerr << "Animation not found: " << animName << std::endl;
        curAnim_ = &animations_.at(animName);
        std::clog << "Set animation: " << animName
                  << ", duration: " << curAnim_->getDuration()
                  << ", ticks/s: " << curAnim_->getTicksPerSecond() << std::endl;
    }
    void updateAnimation(Shader &shader, double deltaTime = 0.0)
    {
        assert(curAnim_ != nullptr);
        if (curTick_ < curAnim_->getDuration())
        {
            calculateFinalTransform(getRootHierarchy(), glm::mat4(1.0f));
            curTick_ += deltaTime * curAnim_->getTicksPerSecond();
            GLuint blockIndex = glGetProgramResourceIndex(shader.getID(), GL_SHADER_STORAGE_BLOCK, "BoneTrans");
            if (blockIndex == GL_INVALID_INDEX)
                std::cerr << "Shader storage block BoneMatrices not found!\n";
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
        auto paiScene = importer.ReadFile(path,
                                          aiProcess_LimitBoneWeights |
                                              aiProcess_JoinIdenticalVertices |
                                              aiProcess_ConvertToLeftHanded);
        assert(paiScene != nullptr &&
               !(paiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) &&
               paiScene->mRootNode != nullptr);
        animations_.reserve(paiScene->mNumAnimations);
        for (unsigned int i = 0; i < paiScene->mNumAnimations; ++i)
        {
            auto paiAnimation = paiScene->mAnimations[i];
            animations_.emplace(paiAnimation->mName.C_Str(), Animation(paiAnimation, *this));
            ///////////////////////////////////////////////////////////////
            std::clog << paiAnimation->mName.C_Str() << std::endl; // 报菜名
            ///////////////////////////////////////////////////////////////
        }
        finalTransforms_.resize(getBonesLoaded().size(), glm::mat4(1.0f));
        glGenBuffers(1, &SSBO_);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     finalTransforms_.size() * sizeof(glm::mat4),
                     finalTransforms_.data(),
                     GL_DYNAMIC_DRAW);
        bindingIndex_ = nextBindingPoint_++;
    }
    void calculateFinalTransform(const Hierarchy *node, glm::mat4 parentTransform)
    {
        assert(node != nullptr);
        if (curAnim_ != nullptr && curAnim_->getKeyFrames().contains(node->name))
            parentTransform *= curAnim_->getKeyFrames().at(node->name).interpolate(curTick_);
        finalTransforms_[node->id] = parentTransform * node->offset;
        for (const auto &child : node->children)
            calculateFinalTransform(child, parentTransform);
    }
};
GLuint Animator::nextBindingPoint_ = 0;
#endif