#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <shared_mutex>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"
#include "movement.hpp"

#ifndef PLAYER_HPP
#define PLAYER_HPP

class Player : public Camera // 必须在主线程
{
    // mutable std::shared_mutex smtx_;
    std::string uuid_v4_;
    bool isFocus_ = true;

private:
    Player(float aspect, float x, float y, float height)
        : Camera(aspect, x, y, height), uuid_v4_(genUUIDv4()) {}
    ~Player() = default;
    Player(const Player &) = delete;
    Player &operator=(const Player &) = delete;
    Player(Player &&) = delete;
    Player &operator=(Player &&) = delete;
    std::string genUUIDv4()
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
        return ss.str();
    }

public:
    static Player &getInstance()
    {
        static Player instance((16.0f / 9.0f), .0f, .0f, 1.7f); // 默认
        return instance;
    }
    std::string getUUIDv4() const { return uuid_v4_; }
    bool &getFocus() { return isFocus_; }
    void initCurrentWindowInput(GLFWwindow *window)
    {
        glfwMakeContextCurrent(window);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height)
                                       { glViewport(0, 0, width, height); });
        glfwSetScrollCallback(window, [](GLFWwindow *window, double xoffset, double yoffset)
                              { getInstance().processViewZoom(yoffset); });
        glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos)
                                 { getInstance().processViewMove(xpos, ypos); });
        glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
                           {
                               if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                                   glfwSetWindowShouldClose(window, true);
                               if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS)
                               {
                                   if (getInstance().getFocus())
                                   {
                                       glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                                       getInstance().getFocus() = false;
                                   }
                                   else
                                   {
                                       glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                                       getInstance().getFocus() = true;
                                   }
                               }
                               //
                           });
    }
    void processKeyboard(GLFWwindow *window, float deltaTime)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            Player::getInstance().processPosMove(Movement::FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            Player::getInstance().processPosMove(Movement::BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            Player::getInstance().processPosMove(Movement::LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            Player::getInstance().processPosMove(Movement::RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            Player::getInstance().processPosMove(Movement::UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            Player::getInstance().processPosMove(Movement::DOWN, deltaTime);
    }
};

struct NetPlayer
{
    std::string uuid = "00000000-0000-0000-0000-000000000000";
    glm::mat4 globalMat = glm::mat4(1.0f);
};

#endif