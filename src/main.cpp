// STL headers
#include <iostream>

// Project headers
#include "user.h"

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	static_cast<CUser*>(glfwGetWindowUserPointer(window))->handleKey(key, action);
}

int main(void)
{
	if (!glfwInit())
	{
		std::cout << "glfwInit";
		return EXIT_FAILURE;
	}

	GLFWwindow* window = glfwCreateWindow(640, 480, "Hello, GLFW World!", NULL, NULL);
	if (!window)
	{
		std::cout << "glfwCreateWindow";

		glfwTerminate();
		return EXIT_FAILURE;
	}

	CUser user;
	user.loadFile("assets/tests/triangulation.obj");

	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, &user);
	glfwSetKeyCallback(window, keyCallback);

	while (!glfwWindowShouldClose(window))
	{	
		glfwSwapBuffers(window);
		glfwPollEvents();
		glfwGetInputMode(window, GLFW_CURSOR);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}