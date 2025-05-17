#include <thread>
#include <stop_token>
#include <unordered_map>
#include <list>
#include <filesystem>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <jsoncpp/json/json.h>
#include "Logger.hpp"
#include "reactor.hpp"
#include "shader.hpp"
#include "model.hpp"
#include "animator.hpp"
#include "reactor.hpp"
#include "syncQueue.hpp"
#include "player.hpp"

/////////////////////////////////////////////////////
int SCR_WIDTH = 1000;
int SCR_HEIGHT = 800;
Player player((float)SCR_WIDTH / (float)SCR_HEIGHT,
              glm::vec3(0.0f, 1.7f, 0.0f));
float lastX = 0.0f;
float lastY = 0.0f;
SyncQueue<NetPlayer> qu;
/////////////////////////////////////////////////////

int main()
{
    mylog::LogMessage::_setTerminalColorful();
    // mylog::FileManager::_setBasename("log");
    // mylog::Logger::_setOutputFunc(mylog::AsyncHelper::_outputFunc_async_file);
    // mylog::Logger::_setFlushFunc(mylog::AsyncHelper::_flushFunc_async_file);

    Reactor reactor("0.0.0.0", 6664);
    std::jthread t1([&]
                    { reactor.run(); });

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bubble-Gum", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height)
                                   { glViewport(0, 0, width, height); });
    glfwSetScrollCallback(window, [](GLFWwindow *window, double xoffset, double yoffset)
                          { player.processMouseScroll(yoffset); });
    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos)
                             {
								float xoffset = xpos - lastX;
								float yoffset = lastY - ypos;
								lastX = xpos;
								lastY = ypos;
								player.processMouseMovement(xoffset, yoffset); });
    glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
                       {
						if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
							glfwSetWindowShouldClose(window, true);
						if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
							glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
						if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS)
							glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); });
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        LOG_FATAL << "fail to initialize GLAD";
    }
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    {
        std::unordered_map<std::string, Shader> shaders;
        shaders.emplace("modl", Shader(std::filesystem::current_path() / "../opengl/glsl/modl_vs.glsl", std::filesystem::current_path() / "../opengl/glsl/modl_fs.glsl"));
        shaders.emplace("anim", Shader(std::filesystem::current_path() / "../opengl/glsl/anim_vs.glsl", std::filesystem::current_path() / "../opengl/glsl/anim_fs.glsl"));
        std::unordered_map<std::string, Animator> animators;
        animators.emplace("terrain", Animator(std::filesystem::current_path() / "../resources/terrains/boxes/boxes.fbx"));
        animators.emplace("sphere", Animator(std::filesystem::current_path() / "../resources/objects/sphere/sphere.fbx", "骨架|Action"));
        animators.emplace("ring", Animator(std::filesystem::current_path() / "../resources/objects/ring/ring.fbx", "骨架|Action"));
        animators.emplace("cube", Animator(std::filesystem::current_path() / "../resources/objects/cube/cube.fbx", "骨架|Action"));
        double deltaTime = 0.0;
        double lastTime = 0.0;
        while (!glfwWindowShouldClose(window))
        {
            double curTime = glfwGetTime();
            deltaTime = curTime - lastTime;
            lastTime = curTime;
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                player.processKeyboard(Movement::FORWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                player.processKeyboard(Movement::BACKWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                player.processKeyboard(Movement::LEFT, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                player.processKeyboard(Movement::RIGHT, deltaTime);
            glClearColor(0.7f, 0.7f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shaders.at("modl").use();
            shaders.at("modl").setMat4("model", glm::mat4(1.0f));
            shaders.at("modl").setMat4("view", player.updateView());
            shaders.at("modl").setMat4("projection", player.updateProjection());
            animators.at("terrain").draw(shaders.at("modl"));
            shaders.at("anim").use();
            shaders.at("anim").setMat4("model", player.getGlobalTrans());
            shaders.at("anim").setMat4("view", player.updateView());
            shaders.at("anim").setMat4("projection", player.updateProjection());
            animators.at("sphere").updateAnimation(shaders.at("anim"), deltaTime);
            while (!qu.empty_r())
            {
                NetPlayer other;
                qu.take_r(other);
                shaders.at("anim").use();
                shaders.at("anim").setMat4("model", other.globalTrans);
                shaders.at("anim").setMat4("view", player.updateView());
                shaders.at("anim").setMat4("projection", player.updateProjection());
                animators.at("ring").draw(shaders.at("anim"));
            }
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
