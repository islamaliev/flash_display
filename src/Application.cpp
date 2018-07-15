#include "Application.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>

namespace Private_Application
{

GLFWwindow* _pWindow = nullptr;

}

namespace flash 
{
    
namespace ui 
{

Application::Application()
{

}

void Application::init(int width, int height, void(*onInitDone)(void))
{
	using namespace Private_Application;
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_pWindow = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
	if (!_pWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
	}
	glfwMakeContextCurrent(_pWindow);

	glewExperimental = GL_TRUE;
	GLenum status = glewInit();
	assert(status == GLEW_OK);

	onInitDone();
}

int Application::run(void(*updateF)(void))
{
	using namespace Private_Application;
	while (isRunning())
	{
		updateF();

		glfwPollEvents();
		glfwSwapBuffers(_pWindow);
	}

	glfwTerminate();
	return 0;
}

bool Application::isRunning() const
{
	using namespace Private_Application;
	return !glfwWindowShouldClose(_pWindow);
}

void Application::stop()
{
	using namespace Private_Application;
	glfwSetWindowShouldClose(_pWindow, GL_TRUE);
}

}
    
}
