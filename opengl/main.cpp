#include <iostream>
#include <string>
#include <filesystem>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Logger.hpp"
#include "shader.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "animation.hpp"
#include "animator.hpp"

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

Camera camera((float)SCR_WIDTH / (float)SCR_HEIGHT);
float lastX = 0.0f;
float lastY = 0.0f;

void debugOpenGL();

int main()
{
	mylog::Logger::_setLogLevel(mylog::LogLevel::TRACE);
	mylog::LogMessage::_setTerminalColorful();
	// mylog::FileManager::_setBasename("log");
	// mylog::Logger::_setOutputFunc(mylog::AsyncHelper::_outputFunc_async_file);
	// mylog::Logger::_setFlushFunc(mylog::AsyncHelper::_flushFunc_async_file);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // debug
	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "BBG", NULL, NULL);
	glfwMakeContextCurrent(window);
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
			glfwSetWindowShouldClose(window, true); });
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		LOG_FATAL << "Failed to initialize GLAD";
	}
	debugOpenGL();
	{
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		Shader shader(std::filesystem::current_path() / "../opengl/model_vs.glsl",
					  std::filesystem::current_path() / "../opengl/model_fs.glsl");
		Model model(std::filesystem::current_path() / "../resrc/obj/sphere/sphere.fbx");
		// ////////////////////////////////////////////////////////////////////////////////上菜
		// Animator animator(model);
		// animator.setCurAnimation("骨架|Action");
		// ////////////////////////////////////////////////////////////////////////////////
		double deltaTime = 0.0;
		double lastTime = 0.0;
		while (!glfwWindowShouldClose(window))
		{
			double curTime = glfwGetTime();
			deltaTime = curTime - lastTime;
			lastTime = curTime;
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				camera.processKeyboard(CamMovement::FORWARD, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				camera.processKeyboard(CamMovement::BACKWARD, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				camera.processKeyboard(CamMovement::LEFT, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				camera.processKeyboard(CamMovement::RIGHT, deltaTime);
			//animator.updateAnimation(deltaTime);
			glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shader.use();
			shader.setMat4("model", glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
											  0.0f, 0.0f, 1.0f, 0.0f,
											  0.0f, -1.0f, 0.0f, 0.0f,
											  0.0f, 0.0f, 0.0f, 1.0f));
			shader.setMat4("view", camera.updateView());
			shader.setMat4("projection", camera.updateProjection());
			model.draw(shader);
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
void debugOpenGL()
{
	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(
			[](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
			{
				std::cout << "Debug message (" << id << "): " << message << std::endl;
				switch (source)
				{
				case GL_DEBUG_SOURCE_API:
					std::cout << "Source: API";
					break;
				case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
					std::cout << "Source: Window System";
					break;
				case GL_DEBUG_SOURCE_SHADER_COMPILER:
					std::cout << "Source: Shader Compiler";
					break;
				case GL_DEBUG_SOURCE_THIRD_PARTY:
					std::cout << "Source: Third Party";
					break;
				case GL_DEBUG_SOURCE_APPLICATION:
					std::cout << "Source: Application";
					break;
				case GL_DEBUG_SOURCE_OTHER:
					std::cout << "Source: Other";
					break;
				}
				std::cout << std::endl;
				switch (type)
				{
				case GL_DEBUG_TYPE_ERROR:
					std::cout << "Type: Error";
					break;
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
					std::cout << "Type: Deprecated Behaviour";
					break;
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
					std::cout << "Type: Undefined Behaviour";
					break;
				case GL_DEBUG_TYPE_PORTABILITY:
					std::cout << "Type: Portability";
					break;
				case GL_DEBUG_TYPE_PERFORMANCE:
					std::cout << "Type: Performance";
					break;
				case GL_DEBUG_TYPE_MARKER:
					std::cout << "Type: Marker";
					break;
				case GL_DEBUG_TYPE_PUSH_GROUP:
					std::cout << "Type: Push Group";
					break;
				case GL_DEBUG_TYPE_POP_GROUP:
					std::cout << "Type: Pop Group";
					break;
				case GL_DEBUG_TYPE_OTHER:
					std::cout << "Type: Other";
					break;
				}
				std::cout << std::endl;
				switch (severity)
				{
				case GL_DEBUG_SEVERITY_HIGH:
					std::cout << "Severity: high";
					break;
				case GL_DEBUG_SEVERITY_MEDIUM:
					std::cout << "Severity: medium";
					break;
				case GL_DEBUG_SEVERITY_LOW:
					std::cout << "Severity: low";
					break;
				case GL_DEBUG_SEVERITY_NOTIFICATION:
					std::cout << "Severity: notification";
					break;
				}
				std::cout << std::endl;
				std::cout << std::endl;
			},
			nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
}