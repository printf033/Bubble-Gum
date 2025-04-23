#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <shared_mutex>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "movement.hpp"

#ifndef PLAYER_HPP
#define PLAYER_HPP

class Player
{
    mutable std::shared_mutex smtx_;
    std::string uuid_v4_;
    glm::mat4 globalTrans_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    float movementSensitivity_;

public:
    Player(const glm::mat4 &globalTrans = glm::mat4(1.0f))
        : uuid_v4_("00000000-0000-0000-0000-000000000000"),
          globalTrans_(globalTrans),
          front_(glm::vec3(0.0f, 0.0f, -1.0f)),
          up_(glm::vec3(0.0f, 1.0f, 0.0f)),
          right_(glm::vec3(1.0f, 0.0f, 0.0f)),
          movementSensitivity_(1.5f)
    {
        std::random_device rd;
        static thread_local std::mt19937 gen;
        if (rd.entropy() == 0.0)
            gen = std::mt19937(static_cast<unsigned>(std::time(nullptr)));
        else
            gen = std::mt19937(rd());
        std::uniform_int_distribution<uint32_t> dist32(0, 0xFFFFFFFF);
        std::uniform_int_distribution<uint16_t> dist16(0, 0xFFFF);
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        ss << std::setw(8) << dist32(gen) << "-";
        ss << std::setw(4) << dist16(gen) << "-";
        ss << std::setw(4) << ((dist16(gen) & 0x0FFF) | 0x4000) << "-"; // version 4
        ss << std::setw(4) << ((dist16(gen) & 0x3FFF) | 0x8000) << "-"; // variant 1 (10xx)
        ss << std::setw(4) << dist16(gen);
        ss << std::setw(4) << dist16(gen);
        ss << std::setw(4) << dist16(gen);
        uuid_v4_ = ss.str();
    }
    ~Player() {}
    std::string getUUIDv4() const
    {
        std::shared_lock<std::shared_mutex> locker(smtx_);
        return uuid_v4_;
    }
    const glm::mat4 &getGlobalTrans() const
    {
        std::shared_lock<std::shared_mutex> locker(smtx_);
        return globalTrans_;
    }
    void setGlobalTrans(const glm::mat4 &globalTrans)
    {
        std::unique_lock<std::shared_mutex> locker(smtx_);
        globalTrans_ = globalTrans;
    }
    void processKeyboard(Movement direction, float deltaTime)
    {
        std::unique_lock<std::shared_mutex> locker(smtx_);
        float rate = movementSensitivity_ * deltaTime;
        switch (direction)
        {
        case Movement::FORWARD:
            globalTrans_ = glm::translate(globalTrans_, front_ * rate);
            break;
        case Movement::BACKWARD:
            globalTrans_ = glm::translate(globalTrans_, -front_ * rate);
            break;
        case Movement::LEFT:
            globalTrans_ = glm::translate(globalTrans_, -right_ * rate);
            break;
        case Movement::RIGHT:
            globalTrans_ = glm::translate(globalTrans_, right_ * rate);
            break;
        default:
            break;
        }
    }
};

struct NetPlayer
{
    std::string uuid;
    glm::mat4 globalTrans;
};

#endif