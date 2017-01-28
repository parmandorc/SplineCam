#include <iostream>

#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"

int main()
{
	// init GLFW and GLEW
	if (!glfwInit())
	{
		std::cerr << "GLFW init failed!" << std::endl;
		return -1;
	}

	glewExperimental = GL_TRUE;

	if (!glewInit())
	{
		std::cerr << "GLEW init failed!" << std::endl;
		return -1;
	}

	// set window configuration
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // only support modern opengl

	// create the window
	GLFWwindow* window = glfwCreateWindow(800, 600, "SplineCam", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "GLFW failed to create the window!" << std::endl;
		return -1;
	}

	glfwMakeContextCurrent(window);

	// main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}
