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
#include "camera.hpp"
#include "shader.hpp"
#include "model.hpp"
#include "animator.hpp"
#include "reactor.hpp"
#include "cachedThreadPool.hpp"
#include "syncQueue.hpp"
#include "player.hpp"

void setReactorThread(pthread_t &);

/////////////////////////////////////////////////////
int SCR_WIDTH = 1000;
int SCR_HEIGHT = 800;
Camera camera((float)SCR_WIDTH / (float)SCR_HEIGHT);
float lastX = 0.0f;
float lastY = 0.0f;
Player me;
SyncQueue<NetPlayer> qu(100);
static bool isContinue = true;
/////////////////////////////////////////////////////

int main()
{
    mylog::LogMessage::_setTerminalColorful();
    // mylog::FileManager::_setBasename("log");
    // mylog::Logger::_setOutputFunc(mylog::AsyncHelper::_outputFunc_async_file);
    // mylog::Logger::_setFlushFunc(mylog::AsyncHelper::_flushFunc_async_file);

    pthread_t tid;
    std::jthread t1(setReactorThread, std::ref(tid));

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "BBG", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height)
                                   { glViewport(0, 0, width, height); });
    glfwSetScrollCallback(window, [](GLFWwindow *window, double xoffset, double yoffset)
                          { camera.processMouseScroll(yoffset); });
    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos)
                             {
								float xoffset = xpos - lastX;
								float yoffset = lastY - ypos;
								lastX = xpos;
								lastY = ypos;
								camera.processMouseMovement(xoffset, yoffset); });
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
        std::unordered_map<std::string, Animator> animators;
        shaders.emplace("static", Shader(std::filesystem::current_path() / "../opengl/static_vs.glsl",
                                         std::filesystem::current_path() / "../opengl/static_fs.glsl"));
        shaders.emplace("anim", Shader(std::filesystem::current_path() / "../opengl/anim_vs.glsl",
                                       std::filesystem::current_path() / "../opengl/anim_fs.glsl"));
        animators.emplace("sphere", Animator(std::filesystem::current_path() / "../resrc/obj/sphere/sphere.fbx", "骨架|Action"));
        animators.emplace("ring", Animator(std::filesystem::current_path() / "../resrc/obj/ring/ring.fbx", "骨架|Action"));
        animators.emplace("cube", Animator(std::filesystem::current_path() / "../resrc/obj/cube/cube.fbx", "骨架|Action"));
        double deltaTime = 0.0;
        double lastTime = 0.0;
        while (!glfwWindowShouldClose(window))
        {
            double curTime = glfwGetTime();
            deltaTime = curTime - lastTime;
            lastTime = curTime;
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                camera.processKeyboard(Movement::FORWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                camera.processKeyboard(Movement::BACKWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                camera.processKeyboard(Movement::LEFT, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                camera.processKeyboard(Movement::RIGHT, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                me.processKeyboard(Movement::FORWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                me.processKeyboard(Movement::BACKWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                me.processKeyboard(Movement::LEFT, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                me.processKeyboard(Movement::RIGHT, deltaTime);
            glClearColor(0.7f, 0.7f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shaders.at("anim").use();
            shaders.at("anim").setMat4("model", me.getGlobalTrans());
            shaders.at("anim").setMat4("view", camera.updateView());
            shaders.at("anim").setMat4("projection", camera.updateProjection());
            animators.at("cube").updateAnimation(shaders.at("anim"), deltaTime);
            animators.at("sphere").updateAnimation(shaders.at("anim"), deltaTime);
            while (!qu.empty_r())
            {
                NetPlayer other;
                qu.take_r(other);
                shaders.at("static").use();
                shaders.at("static").setMat4("model", other.globalTrans);
                shaders.at("static").setMat4("view", camera.updateView());
                shaders.at("static").setMat4("projection", camera.updateProjection());
                animators.at("ring").draw(shaders.at("static"));
            }
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    isContinue = false;
    pthread_kill(tid, SIGUSR1);
}

void setReactorThread(pthread_t &tid)
{
    tid = pthread_self();
    Reactor rc("0.0.0.0", 4444);
    CachedThreadPool::_getInstance().excute([&] // 不要用成员变量，不然要加锁
                                            {
        while (isContinue)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            Json::Value jv;
            jv["uuid"] = me.getUUIDv4();
            jv["globalTrans"] = Converter::convertMatrix2String(me.getGlobalTrans());
            Json::FastWriter jfw;
            std::string data(jfw.write(jv));
            rc.send("255.255.255.255", 4445, data);
            // ////////////////////////////////////////////
            // LOG_DEBUG<<"is Countine ??? "<<isContinue;
            // ////////////////////////////////////////////
        } });
    rc.start();
}