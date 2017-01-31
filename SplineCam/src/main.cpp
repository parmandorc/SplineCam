#include <iostream>

#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "Input/Input.h"
#include "SplineCam/SplineCam.h"

int main()
{
	// init glfw
	if (!glfwInit())
	{
		std::cerr << "GLFW init failed!" << std::endl;
		return -1;
	}

	// set window configuration
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // only support modern opengl

	// create the window
	GLFWwindow* window = glfwCreateWindow(1024, 768, "SplineCam", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "GLFW failed to create the window!" << std::endl;
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Init glew
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "GLEW init failed!" << std::endl;
		return -1;
	}

	// init SplineCam
	SplineCam splineCam;

	// init input
	Input::SetWindow(window);
	Input::SetListener(static_cast<InputListener*>(&splineCam));
	glfwSetKeyCallback(window, Input::OnKeyCallback);
	glfwSetMouseButtonCallback(window, Input::OnMouseButtonCallback);
	glfwSetScrollCallback(window, Input::OnMouseScrollCallback);
	glfwSetCursorPosCallback(window, Input::OnMouseMoveCallback);

	// main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		splineCam.Update();
		splineCam.Render();

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}



