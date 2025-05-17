#include <string>
#include <vector>
#include <unordered_map>
#include "keyframe.hpp"
#include "model.hpp"
#include "converter.hpp"

#ifndef ANIMATION_HPP
#define ANIMATION_HPP

class Animation
{
    double duration_;
    double ticksPerSecond_;
    std::unordered_map<std::string, KeyFrame> keyFrames_;

public:
    Animation(aiAnimation *paiAnimation,
              Model &model)
        : duration_(paiAnimation->mDuration),
          ticksPerSecond_(paiAnimation->mTicksPerSecond)
    {
        assert(paiAnimation != nullptr);
        assert(ticksPerSecond_ != 0);
        for (unsigned int i = 0; i < paiAnimation->mNumChannels; ++i)
        {
            auto curBone = paiAnimation->mChannels[i];
            keyFrames_.emplace(curBone->mNodeName.C_Str(), KeyFrame(curBone));
        }
    }
    ~Animation() {}
    Animation(const Animation &) = delete;
    Animation &operator=(const Animation &) = delete;
    Animation(Animation &&other)
        : duration_(other.duration_),
          ticksPerSecond_(other.ticksPerSecond_),
          keyFrames_(std::move(other.keyFrames_))
    {
        other.ticksPerSecond_ = 0;
        other.duration_ = 0;
    }
    Animation &operator=(Animation &&other)
    {
        if (this != &other)
            Animation(std::move(other)).swap(*this);
        return *this;
    }
    inline std::unordered_map<std::string, KeyFrame> &getKeyFrames() { return keyFrames_; }
    inline double getTicksPerSecond() const { return ticksPerSecond_; }
    inline double getDuration() const { return duration_; }

private:
    void swap(Animation &other)
    {
        std::swap(duration_, other.duration_);
        std::swap(ticksPerSecond_, other.ticksPerSecond_);
        std::swap(keyFrames_, other.keyFrames_);
    }
};

#endif