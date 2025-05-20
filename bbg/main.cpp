#include <thread>
#include <stop_token>
#include <unordered_map>
#include <list>
#include <filesystem>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <jsoncpp/json/json.h>
#include "logger.hpp"
#include "reactor.hpp"
#include "shader.hpp"
#include "model.hpp"
#include "animator.hpp"
#include "reactor.hpp"
#include "syncQueue.hpp"
#include "player.hpp"
#include "ground.hpp"

/////////////////////////////////////////////////////
SyncQueue<NetPlayer> npQue;
/////////////////////////////////////////////////////

int main()
{
    mylog::LogMessage::setTerminalVivid();
    // mylog::Logger::setOutputFunc(mylog::AsyncHelper::outputFunc_async_file);
    // mylog::Logger::setFlushFunc(mylog::AsyncHelper::flushFunc_async_file);

    Reactor reactor("0.0.0.0", 6664);
    std::jthread t1([&]
                    { reactor.run(); });

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    int SCR_WIDTH = 1600;
    int SCR_HEIGHT = 900;
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bubble-Gum", nullptr, nullptr);
    Player::getInstance().initCurrentWindowInput(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        LOG_FATAL << "GLAD failed!";
    }

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    {
        ///////////////////////////////////////////////////////////////////////////////
        Shader shader_static(std::filesystem::current_path() / "../opengl/glsl/modl_vs.glsl", std::filesystem::current_path() / "../opengl/glsl/modl_fs.glsl");
        Shader shader_dynamic(std::filesystem::current_path() / "../opengl/glsl/anim_vs.glsl", std::filesystem::current_path() / "../opengl/glsl/anim_fs.glsl");
        Ground ground(std::filesystem::current_path() / "../resources/terrains/boxes/boxes.fbx");
        ground.addCollider("sphere", std::filesystem::current_path() / "../resources/objects/sphere/sphere.fbx");
        ground.addCollider("ring", std::filesystem::current_path() / "../resources/objects/ring/ring.fbx");
        ground.addCollider("cube", std::filesystem::current_path() / "../resources/objects/cube/cube.fbx");
        ground.addCollider("monkey", std::filesystem::current_path() / "../resources/objects/monkey/monkey.fbx");
        ///////////////////////////////////////////////////////////////////////////////
        double deltaTime = 0.0;
        double lastTime = 0.0;
        while (!glfwWindowShouldClose(window))
        {
            double curTime = glfwGetTime();
            deltaTime = curTime - lastTime;
            lastTime = curTime;
            Player::getInstance().processKeyboard(window, deltaTime);
            ground.detectNcorrect(); // detect collision and correct
            glm::mat4 view = Player::getInstance().updateView();
            glm::mat4 projection = Player::getInstance().updateProjection();
            glClearColor(0.7f, 0.7f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            auto dynamicShade = [&view, &projection, &deltaTime, &shader_dynamic](Animator &animator, const glm::mat4 &globalMat = glm::mat4(1.0f))
            {
                shader_dynamic.use();
                shader_dynamic.setMat4("model", globalMat);
                shader_dynamic.setMat4("view", view);
                shader_dynamic.setMat4("projection", projection);
                animator.updateAnimation(shader_dynamic, deltaTime);
            };
            auto staticShade = [&view, &projection, &shader_static](Model &model, const glm::mat4 &globalMat = glm::mat4(1.0f))
            {
                shader_static.use();
                shader_static.setMat4("model", globalMat);
                shader_static.setMat4("view", view);
                shader_static.setMat4("projection", projection);
                model.draw(shader_static);
            };
            ///////////////////////////////////////////////////////////////////////////////
            staticShade(ground);
            // dynamicShade(ground.getCollider("cube"));
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                ground.getCollider("sphere").processPosMove(Movement::FORWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                ground.getCollider("sphere").processPosMove(Movement::BACKWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                ground.getCollider("sphere").processPosMove(Movement::LEFT, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                ground.getCollider("sphere").processPosMove(Movement::RIGHT, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                ground.getCollider("sphere").processPosMove(Movement::UP, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
                ground.getCollider("sphere").processPosMove(Movement::DOWN, deltaTime);
            staticShade(ground.getCollider("sphere"), ground.getCollider("sphere").getGlobalMat()); // test
            while (!npQue.empty_r())
            {
                NetPlayer other;
                npQue.take_r(other);
                dynamicShade(ground.getCollider("ring"), other.globalMat);
            }
            ///////////////////////////////////////////////////////////////////////////////
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    reactor.stop();
    if (t1.joinable())
        t1.join();
    return 0;
}
